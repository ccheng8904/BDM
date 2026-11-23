/*******************************************************************************
 *  Copyright (c) 2023 BSH Hausgeraete GmbH,
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
/** \file     intflash_drv.c
 *
 *  \ingroup  msp
 *
 *  \brief    Implementation of the flash driver for CMS32M67xx.
 *
 *  \details  The CMS32M67xx flash is organised in 512 byte sectors that can be
 *            erased, words (4 byte aligned) can be programmed. The access is
 *            performed by the vendor flash routines.
 *
 */

/******************************************************************************/
/* INCLUDES                                                                   */
/******************************************************************************/
#include "bsh_stdinc.h"
#include <stdint.h>
#include "processor.h"
#include "firmware_update/mem_drv/intflash_drv.h"
#include "firmware_update/mem_drv/mem_drv.h"

/******************************************************************************/
/* LOCAL TYPE DEFINITIONS                                                     */
/******************************************************************************/
/* write unit needed in common include com_simpleFlash.h */
typedef uint32_t COMA_wrUnit_t;
#include "firmware_update/mem_drv/com_mal.h"


/******************************************************************************/
/* LOCAL FUNCTION DECLARATIONS                                                */
/******************************************************************************/
static void DRVIF_disableInterrupts(void);
static void DRVIF_enableInterrupts(void);
static void DRVIF_triggerWatchdog(void);

/******************************************************************************/
/* LOCAL DATA DEFINITIONS                                                     */
/******************************************************************************/
//lint -esym(9003,*,DRVIF_*) block scope does not increase maintainability here

static const struct COMA_memSeg_s DRVIF_flashSegment =
{
    .blockSize = PAGE_SIZE,       ///< block size
    .startAddr = ROM1_PHY_START,  ///< flash start address
    .endAddr   = ROM1_PHY_END     ///< flash end address
};

/******************************************************************************/
/* PUBLIC DATA DEFINITIONS                                                    */
/******************************************************************************/
#ifndef FWU_MEM_DRV_READ_ONLY
const struct MEMDRV_driver_s DRVIF_driver =
{
    .allowInterrupts     = false,
    .timeOutPwr2         = MEMDRV_TMO_8S,  // > 40ms x sector count
    .writeBlockSize      = sizeof(COMA_wrUnit_t),
    .init                = DRVIF_initFlash,
    .deinit              = DRVIF_deinitFlash,
    .isAddrRangeValid    = DRVIF_isAddrRangeValid,
    .reset               = DRVIF_resetFlash,
    .isBusy              = DRVIF_isBusy,
    .isError             = DRVIF_isError,
    .write               = DRVIF_write,
    .erase               = DRVIF_erase,
    .read                = NULL,
    .checkErase          = DRVIF_checkErase
};
#else
const struct MEMDRV_driver_s DRVIF_driver =
{
    .init                = DRVIF_initFlash,
    .deinit              = DRVIF_deinitFlash,
    .isAddrRangeValid    = DRVIF_isAddrRangeValid,
    .read                = NULL,
};
#endif

/******************************************************************************/
/* PUBLIC FUNCTION DEFINITIONS                                                */
/******************************************************************************/
void DRVIF_initFlash(void)
{
    DRVIF_disableInterrupts();

    /* the flash interface is protected by the vendor routines, only the
     * status flags are cleared here */
    FMC->FLSTS = FMC_FLSTS_OVF_Msk | FMC_FLSTS_EVF_Msk;
}

void DRVIF_deinitFlash(void)
{
    DRVIF_resetFlash();

    DRVIF_enableInterrupts();
}

uint32_t DRVIF_write(uint32_t addr, uint32_t size, const uint8_t* buffer)
{
    uint32_t copySize = size;
    uint32_t retVal;

    if(!COMA_isAligned(addr)){return 0U;}
    if((addr > ROM1_PHY_END)){return 0U;}

    (void)copySize;

    /* the vendor routine programs the given number of bytes, it returns 0 on
     * success */
    retVal = (ProgramPage(addr, size, (uint8_t *)buffer) == 0) ? size : 0U;

    DRVIF_triggerWatchdog();

    return retVal;
}


uint32_t DRVIF_erase(uint32_t addr, uint32_t size)
{
    (void)size;

    if(!COMA_isBlockAddrValid(&DRVIF_flashSegment, addr)){return 0u;}

    /* the sector address is aligned inside the vendor routine */
    if(EraseSector(addr) != 0)
    {
        return 0u;
    }

    DRVIF_triggerWatchdog();

    return DRVIF_flashSegment.blockSize;
}

bool DRVIF_isBusy(void)
{
    /* the vendor routines are blocking, therefore the flash is not busy here */
    return false;
}


bool DRVIF_isError(void)
{
    return ((FMC->FLSTS & FMC_FLSTS_EVF_Msk) != 0U) ? true : false;
}


void DRVIF_resetFlash(void)
{
    /* clear all flash status flags */
    FMC->FLSTS = FMC_FLSTS_OVF_Msk | FMC_FLSTS_EVF_Msk;
}


bool DRVIF_isAddrRangeValid(uint32_t addr, uint32_t range)
{
    return COMA_isAddrRangeValid(&DRVIF_flashSegment, addr, range);
}


uint32_t DRVIF_checkErase(uint32_t addr, uint32_t size)
{
    /* after erasing, the value is 0xff */
    const COMA_wrUnit_t * ptr = COMA_addrToWrUnit(addr);
    COMA_wrUnit_t  eraseValue = (COMA_wrUnit_t)(0xFFFFFFFFu);
    uint32_t i;

    for (i = 0u; i < (size / sizeof(COMA_wrUnit_t)); i++)
    {
        if (ptr[i] != eraseValue){return 0;}
    }
    return size;
}

/******************************************************************************/
/* LOCAL FUNCTION DEFINITIONS                                                 */
/******************************************************************************/
static void DRVIF_disableInterrupts(void)
{
    __disable_irq();
}


static void DRVIF_enableInterrupts(void)
{
    __enable_irq();
}


static void DRVIF_triggerWatchdog(void)
{
    /* service the watchdog during longer flash operations */
    FMC->FLSTS = FMC_FLSTS_OVF_Msk | FMC_FLSTS_EVF_Msk;
}
