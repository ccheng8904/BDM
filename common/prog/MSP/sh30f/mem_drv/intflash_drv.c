/*******************************************************************************
 *  Copyright (c) 2023 BSH Hausgeraete GmbH,
 *  Carl-Wery-Str. 34, 81739 Munich, Germany, www.bsh-group.de
 *
 *  All rights reserved. This program and the accompanying materials
 *  are protected by international copyright laws.
 *  Please contact copyright holder for licensing information.
 *
 *******************************************************************************
 *  PROJECT          MCU Framework
 *  PLATFORM         SH30F9X71
 ******************************************************************************/


/******************************************************************************/
/* DOCUMENTATION                                                              */
/******************************************************************************/
/** \file     intflash_drv.c
 *
 *  \ingroup  msp
 *
 *  \brief    Implementation of flash driver for SH30F9X71.
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
static void DRVIF_unlockFlash(void);
static void DRVIF_lockFlash(void);
static void DRVIF_setPageNumber(uint32_t addr);
static void DRVIF_cacheReset(void);
static void DRVIF_clearPGMWindow(void);
static void DRVIF_setPGMWindow(uint32_t CounterInitValue, uint32_t UpperCounterValue);

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

#define FLASH_ACR_UNLOCK_KEY    (uint32_t)0x5AA5U
static uint8_t DRV_LVRState = 0u;
    
/******************************************************************************/
/* PUBLIC DATA DEFINITIONS                                                    */
/******************************************************************************/
#ifndef FWU_MEM_DRV_READ_ONLY
const struct MEMDRV_driver_s DRVIF_driver =
{
    .allowInterrupts     = false,
    .timeOutPwr2         = MEMDRV_TMO_8S,  // > 40ms x 128 blocks
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
    /* Axivion Disable Style MisraC2012-17.7 : Missing declaration for function definition */
    __disable_irq();
    /* Axivion Enable Style MisraC2012-17.7 */ 

    /* Get LVR enable value */
    DRV_LVRState = (uint8_t)SYSCFG->PWRCR.BIT.LVREN;
    /* Disable low vlotage reset */
    SYSCFG->PWRCR.V32 &= ~SYSCFG_PWRCR_LVREN_Msk;
    DRVIF_unlockFlash();
    DRVIF_resetFlash();
}

void DRVIF_deinitFlash(void)
{
    DRVIF_resetFlash();
    DRVIF_lockFlash();
    /* Restore default LVR settings */
    if (DRV_LVRState == 0x01u)
    {
        SYSCFG->PWRCR.V32 |= SYSCFG_PWRCR_LVREN_Msk;
    }
    __enable_irq();
}

uint32_t DRVIF_write(uint32_t addr, uint32_t size, const uint8_t* buffer)
{

    if(!COMA_isAligned(addr)){return 0U;}
    if((addr > ROM1_PHY_END)){return 0U;}

    uint32_t tmpreg = FLASH->ACR.V32;
    //disable the pre read
    FLASH->ACR.V32 = ((FLASH_ACR_UNLOCK_KEY << FLASH_ACR_LOCK_Pos) | (tmpreg & FLASH_ACR_LATENCY_Msk));

    DRVIF_setPGMWindow(SE_ERS_TIME, SE_ERS_TIME);
    /* word program, page program, start flag */
    FLASH->CR.V32 = ((FLASH_CR_PSIZE_WD << FLASH_CR_PSIZE_Pos) \
                    | (FLASH_CR_CMD_MPG << FLASH_CR_CMD_Pos)   \
                    | ((uint32_t)1U << FLASH_CR_STRT_Pos));

    COMA_copyToWrUnit(size, buffer, addr);
    while ((FLASH->SR.V32 & FLASH_FLAG_BSY) != 0U){}
    /* reset flag*/
    FLASH->CR.V32 = 0U;
    DRVIF_clearPGMWindow();

    //enable the pre read
    tmpreg = ((FLASH_ACR_UNLOCK_KEY << FLASH_ACR_LOCK_Pos) | tmpreg);
    FLASH->ACR.V32 = tmpreg;
    
    /* Cache reset */
    DRVIF_cacheReset();

    return sizeof(COMA_wrUnit_t); /* return written size */
}


uint32_t DRVIF_erase(uint32_t addr, uint32_t size)
{
    (void)size;
    if(!COMA_isBlockAddrValid(&DRVIF_flashSegment, addr)){return 0u;}
    DRVIF_setPGMWindow(SE_ERS_TIME, SE_ERS_TIME);
    DRVIF_setPageNumber(addr);
    /* sector erase*/
    FLASH->CR.V32 = (FLASH_CR_CMD_MSE << FLASH_CR_CMD_Pos) | ((uint32_t)0x01U << FLASH_CR_STRT_Pos);
    while ((FLASH->SR.V32 & FLASH_FLAG_BSY) != 0U){}
    /* reset flag*/
    FLASH->CR.V32 = 0U;
    DRVIF_clearPGMWindow();
    /* Cache reset */
    DRVIF_cacheReset();
    return DRVIF_flashSegment.blockSize;
}

bool DRVIF_isBusy(void)
{
    return ((FLASH->SR.V32 & FLASH_FLAG_BSY) != 0U) ? true : false ;
}


bool DRVIF_isError(void)
{
    uint32_t ERROR_FLAG = FLASH_FLAG_WRPERR | FLASH_FLAG_FLSERR | FLASH_FLAG_PGPERR | FLASH_FLAG_PGWERR | FLASH_FLAG_STAERR | FLASH_FLAG_OPERR;
    return ((FLASH->SR.V32 & ERROR_FLAG) != 0U) ? true : false;
}


void DRVIF_resetFlash(void)
{
    /* Clear pending flags (if any) */
    FLASH->SR.V32 |= ((uint32_t)(FLASH_FLAG_EOP | FLASH_FLAG_OPERR | FLASH_FLAG_WRPERR \
                    | FLASH_FLAG_STAERR | FLASH_FLAG_PGPERR \
                    | FLASH_FLAG_PGWERR | FLASH_FLAG_FLSERR) << 16);
    /* all control register are set to 0 */
    FLASH->CR.V32 = 0U;
}


bool DRVIF_isAddrRangeValid(uint32_t addr, uint32_t range)
{
    return COMA_isAddrRangeValid(&DRVIF_flashSegment, addr, range);
}


uint32_t DRVIF_checkErase(uint32_t addr, uint32_t size)
{
    //After erasing, the value is not 0xff, but 0x00
    const COMA_wrUnit_t * ptr = COMA_addrToWrUnit(addr);
    COMA_wrUnit_t  eraseValue = (COMA_wrUnit_t)(0U);
    for (uint32_t i=0u; i< (size/sizeof(COMA_wrUnit_t)); i++)
    {
        if (ptr[i] != eraseValue){return 0;}
    }
    return size;
}

/**
  * @brief  Resets the Instruction Cache and the Data Cache.
  * @retval None
  */
static void DRVIF_cacheReset(void)
{
    uint32_t tmpreg = FLASH->ACR.V32;
    /* Set CRST bit to Reset Cache  */
    FLASH->ACR.V32 |= ((FLASH_ACR_UNLOCK_KEY << FLASH_ACR_LOCK_Pos) | ((uint32_t)0x01U << FLASH_ACR_CRST_Pos));
    FLASH->ACR.V32 = (tmpreg | (FLASH_ACR_UNLOCK_KEY << FLASH_ACR_LOCK_Pos));
}

/**
  * @brief  Clears the programming window timer values.
  * @param  None
  * @retval None
  */
static void DRVIF_clearPGMWindow(void)
{
    /* Clears the window counters */
    FLASH->CNTR = 0x0U;
    FLASH->UPCNTR = 0x0U;
    FLASH->CNTCR = 0x0U;
}

/**
  * @brief  Sets the programming window timer values.
  * @param  CounterInitValue: specifies the watchdog counter initial value.
  * @param  UpperCounterValue: specifies the upper limit value.
  * @retval None
  */
static void DRVIF_setPGMWindow(uint32_t CounterInitValue, uint32_t UpperCounterValue)
{
    /* Sets the initial value of counter */
    FLASH->CNTR = CounterInitValue;
    /* Sets the upper limit value of counter */
    FLASH->UPCNTR = UpperCounterValue;
    /* Enable the downcounter */
    FLASH->CNTCR = 0x01U;
}

/******************************************************************************/
/* LOCAL FUNCTION DEFINITIONS                                                 */
/******************************************************************************/
/** Prepares the flash memory for writing */
static void DRVIF_unlockFlash(void)
{
    /* If main flash area is locked*/
    if (FLASH->CR.BIT.MNLCK)
    {
        /* Authorize the FLASH Registers access */
        FLASH->MKYR = FLASH_MAIN_KEY;
        /* The multiple unlock key unlocks the flash interface for write or erase operations until the block is locked. */
        FLASH->MKYR = FLASH_MULTI_OP_KEY;
    }
}


/** lock the flash memory */
static void DRVIF_lockFlash(void)
{
    /* Set the LOCK Bit to lock the FLASH Registers access */
    FLASH->CR.BIT.MNLCK = 0x01U;
}

/** delivers the page number for current address
 *  \param addr: physical address
 *  \return related page number for input address
 */
static void DRVIF_setPageNumber(uint32_t addr)
{
    FLASH->CR1 = (uint16_t)((addr-DRVIF_flashSegment.startAddr)/PAGE_SIZE);
}
