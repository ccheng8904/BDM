/*******************************************************************************
 *  Copyright (c) 2017 BSH Hausgeraete GmbH,
 *  Carl-Wery-Str. 34, 81739 Munich, Germany, www.bsh-group.de
 *
 *  All rights reserved. This program and the accompanying materials
 *  are protected by international copyright laws.
 *  Please contact copyright holder for licensing information.
 *
 *******************************************************************************
 *  PROJECT          Generic SW / Boot-Loader
 ******************************************************************************/


/******************************************************************************/
/* DOCUMENTATION                                                              */
/******************************************************************************/
/** \file
 *
 *  \ingroup  firmware_update
 *
 *  \brief    Implementation of unit BLmal (Boot-Loader Memory Abstraction).
 *
 *  \details  Provides interfaces for memory abstraction required for RAM-Loader.
 *            The correct driver will be selected depending on specified address
 */


/******************************************************************************/
/* INCLUDES                                                                   */
/******************************************************************************/
#include "bsh_stdinc.h"
#include <stdint.h>
#include "BLmal.h"
#include "firmware_update/mem_drv/mem_drv.h"
#define COMA_UNIT
#include "firmware_update/mem_drv/com_mal.h"

#ifdef MCAL_INCLUDED
  #include "BLcomMcalMap.h"
#else
  #include "BLcomMap.h"
#endif

/******************************************************************************/
/* PRIVATE FUNCTION DECLARATIONS                                              */
/******************************************************************************/
static uint8_t BLMAL_getDriverIndex(uint32_t addr, uint32_t size);
static uint32_t BLMAL_write(uint32_t addr, uint32_t size, const uint8_t* buffer, uint8_t driverIndex);
static uint32_t BLMAL_erase(uint32_t addr, uint8_t driverIndex);
static bool BLMAL_isInternalMemory(uint8_t driverIndex);
static bool BLMAL_isBusy(uint8_t driverIndex);
static bool BLMAL_isError(uint8_t driverIndex);
static void BLMAL_reset(uint8_t driverIndex);
static uint32_t BLMAL_getPhyAddr(uint32_t addr, uint8_t driverIndex);


/******************************************************************************/
/* PRIVATE DATA DEFINITIONS                                                   */
/******************************************************************************/
static const uint8_t BLMAL_noDriver = 0xFFu;


/******************************************************************************/
/* PUBLIC FUNCTION DEFINITIONS                                                */
/******************************************************************************/
void BLMAL_init(void)
{
    for(uint8_t i = 0; MEMDRV_driverTable[i].driver != NULL; i++)
    {// call all supported driver initializers 
        MEMDRV_driverTable[i].driver->init();
    }
}


bool BLMAL_writeBuffer(uint32_t addr, uint32_t size, const uint8_t* buffer)
{
    int32_t sizeToWrite = (int32_t)size;
    uint8_t driverIndex = BLMAL_getDriverIndex(addr,size);
    
    if(driverIndex == BLMAL_noDriver) {return false;} // no valid driver found
    
    BLMAL_reset(driverIndex);
    while(sizeToWrite > 0)
    {// Write a couple of bytes (usually smallest write unit, defined by driver)
        uint32_t writtenBytes = BLMAL_write(addr,(uint32_t)sizeToWrite,buffer,driverIndex);
        if (writtenBytes == 0u)
        {
            BLMAL_reset(driverIndex);
            return false; //writing failed
        }
        else
        {// atomic write succeed, prepare next write
            sizeToWrite -= (int32_t)writtenBytes;
            addr        += writtenBytes;/*lint !e9016 "pointer arithmetic not allowed MisraC2012 18.4 advisory" */
            buffer      += writtenBytes;/*lint !e9016 "pointer arithmetic not allowed MisraC2012 18.4 advisory" */
        }
    }
    BLMAL_reset(driverIndex);
    return true;
}


bool BLMAL_eraseBlock(uint32_t addr)
{
    uint8_t driverIndex = BLMAL_getDriverIndex(addr,0u);
    
    if(driverIndex == BLMAL_noDriver) {return false;} // no valid driver found
    
    BLMAL_reset(driverIndex);
    
    return ((BLMAL_erase(addr,driverIndex) == 0u) ? false : true);
}


bool BLMAL_readBuffer(uint32_t addr, uint32_t size, uint8_t* buffer)
{
    uint8_t driverIndex = BLMAL_getDriverIndex(addr,size);

    if(driverIndex == BLMAL_noDriver) {return false;} // no valid driver found

    if(BLMAL_isInternalMemory(driverIndex))
    {
        return COMA_read(addr,size,buffer);
    }
    else
    {
        return MEMDRV_driverTable[driverIndex].driver->read(addr,size,buffer);
    }
}


/******************************************************************************/
/* PRIVATE FUNCTION DEFINITIONS                                               */
/******************************************************************************/

/** \brief    Search for a valid driver for given address range.
 *
 *  \details  Depending on a given address and range this interface returns
 *            the index of an according driver.
 *            and that has to be terminated by an empty line.
 *
 *  \param    addr: address to check
 *  \param    size: checks if also end of the range is inside supported
 *                  memory.
 *  \return   The index of the memory driver inside \sa MEMDRV_driverTable,
              or \sa BLMAL_noDriver when no driver found.
 */
static uint8_t BLMAL_getDriverIndex(uint32_t addr, uint32_t size)
{
    for(uint8_t i = 0; MEMDRV_driverTable[i].driver != NULL; i++)
    {
        uint32_t phyAddr = BLMAL_getPhyAddr(addr, i);
        if (MEMDRV_driverTable[i].bp2_offset_addr > addr)
        {   // virtual address smaller than offset address --> check next driver entry
            continue;
        }
        if (MEMDRV_driverTable[i].driver->isAddrRangeValid(phyAddr, size))
        {
            return i;
        }
    }
    return BLMAL_noDriver;
}


/** \brief writes some bytes form buffer to flash,
 *         and returns the number of written bytes.
 *  
 *  \note This interface does not write all the bytes out of buffer,
 *        The driver/flash defines number of bytes.
 *
 *  \param addr: start address of block to write
 *  \param size: length of data to write
 *  \param buffer: data buffer to write
 *  \param driverIndex: index for according driver
 *  \return number of written bytes
 */
static uint32_t BLMAL_write(uint32_t addr, uint32_t size, const uint8_t* buffer, uint8_t driverIndex)
{
    uint32_t phyAddr      = BLMAL_getPhyAddr(addr, driverIndex);
    uint32_t writtenBytes = MEMDRV_driverTable[driverIndex].driver->write(phyAddr,size,buffer);
    
    while (BLMAL_isBusy(driverIndex))
    {// wait until write process has been finished
        BLCOM_triggerWdg();
    }
    if(BLMAL_isError(driverIndex))
    {
        writtenBytes = 0;
    }
    return writtenBytes;
}



/** \brief erases a single flash block for given start address.
 *  
 *  \param addr: start address of block to erase
 *  \param driverIndex: index for according driver
 *  \return number of erased bytes
 */
static uint32_t BLMAL_erase(uint32_t addr, uint8_t driverIndex)
{   
    uint32_t phyAddr     = BLMAL_getPhyAddr(addr, driverIndex);
    // erase a block of at least one byte
    uint32_t erasedBytes = MEMDRV_driverTable[driverIndex].driver->erase(phyAddr,1u);
    
    while (BLMAL_isBusy(driverIndex))
    {// wait until write process has been finished
        BLCOM_triggerWdg();
    }
    if(BLMAL_isError(driverIndex))
    {
        erasedBytes = 0;
    }
    return erasedBytes;
}


/** \brief returns true if according memory is internal (memory mapped)
 * 
 *  \param driverIndex: index of the driver to check
 */
static bool BLMAL_isInternalMemory(uint8_t driverIndex)
{
    return (MEMDRV_driverTable[driverIndex].driver->read == NULL);
}


/** \brief check if selected memory driver is busy.
 *
 *  \param driverIndex: index of the driver to check
 *  \return true when driver is busy, else false
 */
static bool BLMAL_isBusy(uint8_t driverIndex)
{
    return MEMDRV_driverTable[driverIndex].driver->isBusy();
}


/** \brief check if an error happened for selected memory driver.
 *
 *  \param driverIndex: index of the driver to check
 *  \return true when error happened, else false
 */
static bool BLMAL_isError(uint8_t driverIndex)
{
    return MEMDRV_driverTable[driverIndex].driver->isError();
}


/** \brief resets the selected memory driver.
 *  \details Interface need to be called after error happened.
 *
 *  \param driverIndex: index of the driver to check
 */
static void BLMAL_reset(uint8_t driverIndex)
{
    MEMDRV_driverTable[driverIndex].driver->reset();
}


/** \brief Calculates a physical address out of virtual address.
 *  \note calling interface need to check if bp2 offset address is smaller than virtual address.
 *
 *  \param addr: virtual address (used in bp2 protocol)
 *  \param driverIndex: index of the driver to check
 */
static uint32_t BLMAL_getPhyAddr(uint32_t addr, uint8_t driverIndex)
{
    return(uint32_t)(addr - MEMDRV_driverTable[driverIndex].bp2_offset_addr);
}
