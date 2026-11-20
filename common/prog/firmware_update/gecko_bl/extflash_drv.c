/*******************************************************************************
 *  Copyright (c) 2017 BSH Hausgeraete GmbH,
 *  Carl-Wery-Str. 34, 81739 Munich, Germany, www.bsh-group.de
 *
 *  All rights reserved. This program and the accompanying materials
 *  are protected by international copyright laws.
 *  Please contact copyright holder for licensing information.
 *
 *******************************************************************************
 *  PROJECT          Generic SW
 ******************************************************************************/


/******************************************************************************/
/* DOCUMENTATION                                                              */
/******************************************************************************/
/** \file
 *
 *  \ingroup  firmware_update
 *
 *  \brief    This maps the SiLabs memory driver to MVU Framework.
 *
 */


/******************************************************************************/
/* INCLUDES                                                                   */
/******************************************************************************/
#include "bsh_stdinc.h"
#include <stdint.h>
#include "firmware_update/mem_drv/extflash_drv.h"
#include "firmware_update/mem_drv/mem_drv.h"
/* Note 9018: union '(anonymous)' declared [MISRA 2012 Rule 19.2, advisory] */
/*lint -save -e9018 */
#include "gecko_bl/storage/btl_storage.h"
/*lint -restore -e9018 */
/******************************************************************************/
/* LOCAL DATA DEFINITIONS                                                     */
/******************************************************************************/

static const uint32_t DRVSF_error = 0u;
static const uint32_t DRVSF_pageSize = 0x1000u;
/******************************************************************************/
/* PUBLIC DATA DEFINITIONS                                                    */
/******************************************************************************/
const struct MEMDRV_driver_s DRVSF_driver =
{
    .allowInterrupts     = false,
    .timeOutPwr2         = MEMDRV_TMO_8S,  // > 40ms x 128 blocks
    .writeBlockSize      = sizeof(uint32_t),
    .init                = DRVSF_initFlash,
    .deinit              = DRVSF_deinitFlash,
    .isAddrRangeValid    = DRVSF_isAddrRangeValid,
    .reset               = DRVSF_resetFlash,
    .isBusy              = DRVSF_isBusy,
    .isError             = DRVSF_isError,
    .write               = DRVSF_write,
    .erase               = DRVSF_erase,
    .read                = DRVSF_read,
    .checkErase          = DRVSF_checkErase
};

/******************************************************************************/
/* PUBLIC FUNCTION DEFINITIONS                                                */
/******************************************************************************/
void DRVSF_initFlash(void)
{
    (void)storage_init();
}


uint32_t DRVSF_write(uint32_t addr, uint32_t size, const uint8_t* buffer)
{   /* const casted away : storage_writeRaw defined by SiLabs, const specifier is missing in their interface */
    if(storage_writeRaw(addr, (uint8_t *)buffer, size) == BOOTLOADER_OK)/*lint !e926 !e9005 */
    {
        return size;
    }
    else
    {
        return DRVSF_error; // e.g. alignment not valid
    }
}


uint32_t DRVSF_erase(uint32_t addr, uint32_t size)
{
    (void)size; // Winflash does not set size --> erase one page
    if(storage_eraseRaw(addr, DRVSF_pageSize) == BOOTLOADER_OK)
    {
        return DRVSF_pageSize;
    }
    else
    {
        return DRVSF_error; // e.g. alignment not valid
    }
}


bool DRVSF_isBusy(void)
{
    return storage_isBusy();
}


bool DRVSF_isError(void)
{   // no error states detectable by serial external flash
    return false;
}


bool DRVSF_isAddrRangeValid(uint32_t addr, uint32_t range)
{  /* As SiLabs provides an address check internally only,
      This interface returns true always.
      If needed the interface verifyAddressRange() can be made public
   */
   (void)addr;
   (void)range;
   return true;
}


uint32_t DRVSF_checkErase(uint32_t addr, uint32_t size)
{
    (void)addr;
    (void)size;
    return 0u; // check takes more time than an erase
}


bool DRVSF_read(uint32_t addr, uint32_t size, uint8_t* buffer)
{
    
    if(storage_readRaw(addr, buffer, size) == BOOTLOADER_OK)
    {
        return true;
    }
    else
    {
        return false; // e.g. invalid address
    }
}

void DRVSF_deinitFlash(void){} // nothing to do
void DRVSF_resetFlash(void) {} // nothing to do

