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
 *  \ingroup  msp
 *
 *  \brief    Implementation for external serial flash driver.
 *
 */


/******************************************************************************/
/* INCLUDES                                                                   */
/******************************************************************************/
#include "bsh_stdinc.h"
#include <stdint.h>
#include "processor.h"
#include "firmware_update/mem_drv/extflash_drv.h"
#include "firmware_update/mem_drv/mem_drv.h"
#include "devices/spi_flash/abs_spi_flash.h"
/******************************************************************************/
/* LOCAL DEFINITIONS                                                          */
/******************************************************************************/
#define DRVSF_SECTOR_SIZE  0x1000U
#define DRVSF_BLOCK_SIZE   0x10000U

/******************************************************************************/
/* LOCAL TYPE DEFINITIONS                                                     */
/******************************************************************************/
/* write unit needed in common include com_mal.h */
/*Axivion Next Line MisraC2012-5.6 : "Unique identifier" will be solved with C++ templates in future */
typedef uint8_t COMA_wrUnit_t;
#include "firmware_update/mem_drv/com_mal.h"

/******************************************************************************/
/* LOCAL FUNCTION DECLARATIONS                                                */
/******************************************************************************/
static void DRVSF_initSegments(void);

/******************************************************************************/
/* LOCAL DATA DEFINITIONS                                                     */
/******************************************************************************/
//lint -esym(9003,*,DRVSF_*) block scope does not increase maintainability here


static struct COMA_memSeg_s DRVSF_flashSegment;
static const uint32_t DRVSF_BlockSize  = DRVSF_BLOCK_SIZE;
static const uint32_t DRVSF_SectorSize = DRVSF_SECTOR_SIZE;


/******************************************************************************/
/* PUBLIC DATA DEFINITIONS                                                    */
/******************************************************************************/
const struct MEMDRV_driver_s DRVSF_driver =
{
    .allowInterrupts     = false,
    .timeOutPwr2         = MEMDRV_TMO_8S,  // > 40ms x 128 blocks
    .writeBlockSize      = sizeof(COMA_wrUnit_t),
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
    ABSSF_init();
    DRVSF_initSegments();
    
}


uint32_t DRVSF_write(uint32_t addr, uint32_t size, const uint8_t* buffer)
{
    return ABSSF_write(addr, size, buffer);
}


uint32_t DRVSF_erase(uint32_t addr, uint32_t size)
{
    if(!COMA_isBlockAddrValid(&DRVSF_flashSegment, addr)){return 0u;}
    // addr is at least aligned to 4k sector
    if(((addr % DRVSF_BlockSize) == 0u) && (size >= DRVSF_BlockSize))
    {   // addr 64k aligned and size at leat 64k
        ABSSF_eraseBlock64(addr);
        return DRVSF_BlockSize;
    }
    else
    {   // erase at least a sector (4k)
        ABSSF_eraseSector(addr);
        return DRVSF_SectorSize;
    }
}


bool DRVSF_isBusy(void)
{
    return ABSSF_isBusy();
}


bool DRVSF_isError(void)
{   // no error states detectable by serial external flash
    return false;
}


bool DRVSF_isAddrRangeValid(uint32_t addr, uint32_t range)
{
    return COMA_isAddrRangeValid(&DRVSF_flashSegment, addr, range);
}


uint32_t DRVSF_checkErase(uint32_t addr, uint32_t size)
{
    (void)addr;
    (void)size;
    return 0u; // check takes more time than an erase
}


bool DRVSF_read(uint32_t addr, uint32_t size, uint8_t* buffer)
{
    ABSSF_read(addr, size, buffer);
    return true; // no error detection for read
}

void DRVSF_deinitFlash(void){} // nothing to do
void DRVSF_resetFlash(void) {} // nothing to do
/******************************************************************************/
/* LOCAL FUNCTION DEFINITIONS                                                 */
/******************************************************************************/

/** initializes the memory segment structure */
static void DRVSF_initSegments(void)
{   
    DRVSF_flashSegment.blockSize = DRVSF_SECTOR_SIZE;
    DRVSF_flashSegment.startAddr = 0x00;
    DRVSF_flashSegment.endAddr   = ABSSF_getMemorySize() - 1u;
}
