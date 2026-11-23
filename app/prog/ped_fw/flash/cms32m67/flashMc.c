/*******************************************************************************
 *  Copyright (c) 2023 BSH Hausgeraete GmbH,
 *  Carl-Wery-Str. 34, 81739 Munich, Germany, www.bsh-group.de
 *
 *  All rights reserved. This program and the accompanying materials
 *  are protected by international copyright laws.
 *  Please contact copyright holder for licensing information.
 *
 *******************************************************************************
 *  PROJECT          FLASH
 ******************************************************************************/

/******************************************************************************/
/* DOCUMENTATION                                                              */
/******************************************************************************/
/** \file     flashMc.c
 *  \brief    Implementation of the CMS32M67xx processor specific flash driver.
 *
 *  \details  The CMS32M67xx flash consists of 512 byte sectors. One sector can
 *            be erased and 4 byte words can be programmed. The access itself is
 *            performed by the vendor flash routines (EraseSector, ProgramPage,
 *            flashRead).
 *
 *  \note     The functions of this file are the counterpart of the generic
 *            flash library (flash.c), they have to be provided by the
 *            application.
 */

/******************************************************************************/
/* INCLUDES                                                                   */
/******************************************************************************/
#include "libtypes.h"
#include "segmentdef.h"
#include "utility/utility.h"
#include "processor.h"
#include "FlashMcXS.h"
#include "Flash.h"
#include "FlashMc.h"
#include "hsup.h"
#if defined(DEBUG) && (DEBUG != 0U)
#include "pedAssert.h"
#endif

//lint -e818  Pointer parameter could be declared as pointing to const
//lint -e661 potential out of bounds pointer access

/******************************************************************************/
/* PRIVATE DEFINITIONS                                                        */
/******************************************************************************/
#ifndef DOXY_DEVELOPERS_DOC
#define FLASH_SECTOR_SIZE               (512u)
#define FLASH_MAIN_START                ((uint32_t)ROM1_PHY_START)
#define FLASH_MAIN_END                  ((uint32_t)ROM1_PHY_END)
#define FLASH_UNWRITTEN_BYTE            (0xFFU)
#define FLASH_INVALID_ADDRESS           (0xFFFFFFFFuL)
#endif

/******************************************************************************/
/* PUBLIC DATA DEFINITIONS                                                    */
/******************************************************************************/

tStatusCode FLASH_tFunctionStatus = NO_ERRORS;

/******************************************************************************/
/* PRIVATE DATA DEFINITIONS                                                   */
/******************************************************************************/
#ifndef DOXY_DEVELOPERS_DOC

/* PROTOTYPES */
static bool FLASH_isMainMemory(const tAddress *address, uint16_t size);
static bool FLASH_writeKernel(tAddress *address, tWriteData *data, uint16_t size);
#endif

/******************************************************************************/
/* PRIVATE FUNCTION DEFINITIONS                                               */
/******************************************************************************/
#ifndef DOXY_DEVELOPERS_DOC

static bool FLASH_isMainMemory(const tAddress *address, uint16_t size)
{
    return ( ((((uint32_t)address) >= FLASH_MAIN_START) &&
              ((((uint32_t)address + (uint32_t)size) - 1UL) <= FLASH_MAIN_END)) != 0U ) ? true : false;
}


static bool FLASH_writeKernel(tAddress *address, tWriteData *data, uint16_t size)
{
    uint8_t buffer[FLASH_SECTOR_SIZE];
    uint16_t index;
    uint32_t startAddress = (uint32_t)address;

    if(size > FLASH_SECTOR_SIZE)
    {
        return false;
    }

    for(index = 0U; index < size; index++)
    {
        buffer[index] = data[index];
    }

    /* The vendor routine programs one byte at a time by writing to the flash
     * memory, it returns 0 if the operation was successful */
    return (ProgramPage(startAddress, (uint32_t)size, buffer) == 0) ? true : false;
}
#endif //DOXY_DEVELOPERS_DOC

/******************************************************************************/
/* PUBLIC FUNCTION DEFINITIONS                                                */
/******************************************************************************/

/** \brief    Enables the flash access
 *
 *  \return   true - successful
 */
bool FLASH_bEnableAccessMc(void)
{
    /* the CMS32M67xx flash interface is always accessible, the protection is
     * done by the configuration keys inside the vendor routines */
    FLASH_tFunctionStatus = NO_ERRORS;

    return true;
}


/** \brief    Disables the flash access
 *
 *  \return   true - successful
 */
bool FLASH_bDisableAccessMc(void)
{
    return true;
}


/** \brief    Writes data to the flash memory
 *
 *  \param    address - start address in the flash memory
 *  \param    data    - data to be written
 *  \param    size    - number of bytes to be written
 *
 *  \return   true - successful
 */
bool FLASH_bWriteMc(tAddress *address, tWriteData *data, uint16_t size)
{
    bool retVal = false;

    if(FLASH_isMainMemory(address, size))
    {
        _FLASH_resetWatchdogTimer();
        HSUP_vDisableInt();

        _FLASH_vOperationStart(FLASH_STATE_PROGRAM);
        retVal = FLASH_writeKernel(address, data, size);
        _FLASH_vOperationFinished(FLASH_STATE_PROGRAM);

        HSUP_vEnableInt();
    }
    else
    {
        /* address out of range */
        FLASH_tFunctionStatus = ADDRESS_OUT_OF_RANGE;
    }

    return retVal;
}


/** \brief    Writes one byte to the flash memory
 *
 *  \param    address - address in the flash memory
 *  \param    data    - byte to be written
 *
 *  \return   true - successful
 */
bool FLASH_bWriteByteMc(tAddress *address, uint8_t data)
{
    uint8_t au8Data[1];

    au8Data[0] = data;

    return FLASH_bWriteMc(address, au8Data, (uint16_t)sizeof(au8Data));
}


/** \brief    Reads data from the flash memory
 *
 *  \param    address - start address in the flash memory
 *  \param    data    - buffer for the read data
 *  \param    size    - number of bytes to be read
 *
 *  \return   true - successful
 */
bool FLASH_bReadMc(tAddress *address, tReadData *data, uint16_t size)
{
    uint16_t counter;
    bool retVal = false;

    if(FLASH_isMainMemory(address, size))
    {
        for(counter = 0U; counter < size; counter++)
        {
            data[counter] = address[counter];
        }
        retVal = true;
    }
    else
    {
        /* address out of range */
        FLASH_tFunctionStatus = ADDRESS_OUT_OF_RANGE;
    }

    return retVal;
}


/** \brief    Erases one sector of the flash memory
 *
 *  \param    address - address inside the sector to be erased
 *
 *  \return   true - successful
 */
bool FLASH_bEraseMc(tAddress *address)
{
    bool retVal = false;
    uint32_t sectorAddress;

    if(FLASH_isMainMemory(address, 1U))
    {
        sectorAddress = ((uint32_t)address) & ~((uint32_t)FLASH_SECTOR_SIZE - 1U);

        _FLASH_resetWatchdogTimer();
        HSUP_vDisableInt();

        _FLASH_vOperationStart(FLASH_STATE_ERASE);
        retVal = (EraseSector(sectorAddress) == 0) ? true : false;
        _FLASH_vOperationFinished(FLASH_STATE_ERASE);

        HSUP_vEnableInt();

        if(!retVal)
        {
            FLASH_tFunctionStatus = ERASE_HW_ERROR;
        }
    }
    else
    {
        /* address out of range */
        FLASH_tFunctionStatus = ADDRESS_OUT_OF_RANGE;
    }

    return retVal;
}


/** \brief    Erases the complete main memory area
 *
 *  \return   true - successful
 */
bool FLASH_eraseAll(void)
{
    bool retVal = false;

    _FLASH_resetWatchdogTimer();
    HSUP_vDisableInt();

    _FLASH_vOperationStart(FLASH_STATE_ERASE);
    retVal = (EraseChip(FLASH_MAIN_START) == 0) ? true : false;
    _FLASH_vOperationFinished(FLASH_STATE_ERASE);

    HSUP_vEnableInt();

    if(!retVal)
    {
        FLASH_tFunctionStatus = ERASE_HW_ERROR;
    }

    return retVal;
}


/** \brief    Reads the lock bit status of the given flash block
 *
 *  \param    address  - an address from the desired memory block
 *  \param    isLocked - pointer to the result status
 *
 *  \return   true - test successful
 */
bool FLASH_readLockStatus(tAddress *address, bool *isLocked)
{
    bool retVal = false;

    if((isLocked != (bool *)NULL) && (FLASH_isMainMemory(address, 1U)))
    {
        /* The CMS32M67xx does not provide a page wise write protection,
         * the protection is done by the block protect mode of the option
         * bytes (see user manual) */
        *isLocked = false;
        retVal = true;
    }
    else
    {
        FLASH_tFunctionStatus = ADDRESS_OUT_OF_RANGE;
    }

    return retVal;
}


/** \brief    Gets the start address of the given flash block
 *
 *  \param    blockNumber - number of the desired memory block
 *
 *  \return   start address of the block
 */
uint32_t FLASH_getStartAddress(uint16_t blockNumber)
{
    if(blockNumber < FLASH_blockCount)
    {
        return (ROM1_PHY_START + ((uint32_t)blockNumber * (uint32_t)FLASH_blockSize));
    }
    else
    {
        return FLASH_INVALID_ADDRESS;
    }
}


/** \brief    Gets the last address of the given flash block
 *
 *  \param    blockNumber - number of the desired memory block
 *
 *  \return   end address of the block
 */
uint32_t FLASH_getEndAddress(uint16_t blockNumber)
{
    if(blockNumber < FLASH_blockCount)
    {
        return ((uint32_t)FLASH_getStartAddress(blockNumber) + (uint32_t)FLASH_blockSize - 1UL);
    }
    else
    {
        return FLASH_INVALID_ADDRESS;
    }
}


/** \brief    Gets the number of the flash page for the requested address
 *
 *  \param    address - address of the flash memory
 *
 *  \return   page (sector) number
 */
uint8_t FLASH_getPageNumber(tAddress *address)
{
    uint8_t result = UINT8_MAX;
    uint32_t blockAddress;

    if(FLASH_isMainMemory(address, 1U))
    {
        blockAddress = ((uint32_t)address) - (uint32_t)ROM1_PHY_START;
        result = (uint8_t)(blockAddress / (uint32_t)FLASH_blockSize);
    }
    else
    {
        /* out of memory */
#if defined(DEBUG) && (DEBUG != 0x00u)
        _ASSERT_vOnFailure ((const uint8_t*)(__FILE__), __LINE__);
#endif
    }

    return result;
}
