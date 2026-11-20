/*******************************************************************************
 *  Copyright (c) 2017 BSH Hausgeraete GmbH,
 *  Carl-Wery-Str. 34, 81739 Munich, Germany, www.bsh-group.de
 *
 *  All rights reserved. This program and the accompanying materials
 *  are protected by international copyright laws.
 *  Please contact copyright holder for licensing information.
 *
 *******************************************************************************
 *  PROJECT          your project
 ******************************************************************************/


/******************************************************************************/
/* DOCUMENTATION                                                              */
/******************************************************************************/
/** \file
 *
 *  \brief    template for memory driver.
 *         !! For implementation consult: firmware_update/mem_drv/mem_drv.h !!
 *         !! and firmware_update/README.md (chapter 3.1)                   !!
 *
 */


/******************************************************************************/
/* INCLUDES                                                                   */
/******************************************************************************/
#include "bsh_stdinc.h"
#include <stdint.h>
#include "processor.h"
#include "firmware_update/mem_drv/mem_drv.h"

/******************************************************************************/
/* LOCAL DEFINITIONS                                                          */
/******************************************************************************/
#define DRVMD_WRITE_BLOCK_SIZE      0x8u     ///< minimal block to write

/******************************************************************************/
/* LOCAL FUNCTION DECLARATIONS                                                */
/******************************************************************************/
static void DRVMD_unlockFlash(void);
static void DRVMD_lockFlash(void);


/******************************************************************************/
/* LOCAL DATA DEFINITIONS                                                     */
/******************************************************************************/
/** minimal block to write */
static const uint32_t DRVMD_writeBlockSize = DRVMD_WRITE_BLOCK_SIZE;

/** minimal block to erase */
static const uint32_t DRVMD_eraseBlockSize = 0x100u;

static const uint32_t DRVMD_startAddress = MY_START_ADDRESS;
static const uint32_t DRVMD_endAddress   = MY_END_ADDRESS;

/******************************************************************************/
/* LOCAL FUNCTION DECLARATIONS                                                */
/******************************************************************************/
static bool DRVMD_isAligned(uint32_t addr);


/******************************************************************************/
/* PUBLIC DATA DEFINITIONS                                                    */
/******************************************************************************/
const struct MEMDRV_driver_s DRVMD_driver =
{
    .allowInterrupts     = true,           // for external memory usually true
    .timeOutPwr2         = MEMDRV_TMO_8S,  // time out for erase complete memory
    .writeBlockSize      = DRVMD_WRITE_BLOCK_SIZE, // smallest writeable block
    .init                = DRVMD_initFlash,
    .deinit              = DRVMD_deinitFlash,
    .isAddrRangeValid    = DRVMD_isAddrRangeValid,
    .reset               = DRVMD_resetFlash,
    .isBusy              = DRVMD_isBusy,
    .isError             = DRVMD_isError,
    .write               = DRVMD_write,
    .erase               = DRVMD_erase,
    .read                = DRVMD_read,   // should be NULL for internal memory
    .checkErase          = DRVMD_checkErase
};

/******************************************************************************/
/* PUBLIC FUNCTION DEFINITIONS                                                */
/******************************************************************************/
void DRVMD_initFlash(void)
{
    DRVMD_unlockFlash();
    DRVMD_resetFlash();
}


void DRVMD_deinitFlash(void)
{
    DRVMD_resetFlash();
    DRVMD_lockFlash();
}


uint32_t DRVMD_write(uint32_t addr, uint32_t size, const uint8_t* buffer)
{
    if(!DRVMD_isAligned(addr)){return 0u;}
    uint32_t writtenBytes;
    /* Write some bytes. It is recommanded not to write a lot of bytes
       It is not required to write all the bytes, the driver itself decides how much.
       The upper layer is informed by the return value how many bytes have been written.
       Note: returning zero is handled as error in upper layer!
     */
    return writtenBytes; /* return written size */
}


uint32_t DRVMD_erase(uint32_t addr, uint32_t size)
{
    (void)size;
    /* erase smallest block
       Note: Returning zero will handled as an error in upper layer!
       Note: When size=0 is given, erase the smallest possible block!
     */
    return DRVMD_eraseBlockSize;
}


bool DRVMD_isBusy(void)
{   // return whether memory is busy or not 
    return false;
}


bool DRVMD_isError(void)
{   // return if an error has been occurred
    return false;
}


void DRVMD_resetFlash(void)
{
    // reset commands and errors, if possible for this memory
}


bool DRVMD_isAddrRangeValid(uint32_t addr, uint32_t range)
{
    const uint32_t blockMask = (uint32_t)(DRVMD_eraseBlockSize-1u);
    return ((
        ((blockMask & addr) == 0u)   && /* check block alignment */
        (addr  >= DRVMD_startAddress)  &&
        (addr  <= DRVMD_endAddress)
         ) ? true : false);
}


bool DRVMD_read(uint32_t addr, uint32_t size, uint8_t* buffer)
{
    /* Read function need to be implemented for external memory. 
       For internal memory you can use COMA_read() interface */
    return true;
}

uint32_t DRVMD_checkErase(uint32_t addr, uint32_t size)
{   /* when not implemented erase will be performed
       in any case (proposed for external memory) */
    return false;
}

/******************************************************************************/
/* LOCAL FUNCTION DEFINITIONS                                                 */
/******************************************************************************/
/** Prepares the memory for writing / erasing */
static void DRVMD_unlockMem(void)
{
    /* do what ever required to unlock memory
       Might also be empty when not supported. */
}


/** locks the memory */
static void DRVMD_lockMem(void)
{
    /* do what ever required to lock memory
       Might also be empty when not supported. */
}

static bool DRVMD_isAligned(uint32_t addr)
{   // using a masks prevent a division (which is not provided by all platforms)
    const uint32_t WrBlockMask = (uint32_t)(DRVMD_writeBlockSize-1u);
    return ((WrBlockMask & addr) == 0u) ? true : false;
}