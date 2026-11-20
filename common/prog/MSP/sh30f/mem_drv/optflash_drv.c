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
 *  PLATFORM         GD32F33
 ******************************************************************************/


/******************************************************************************/
/* DOCUMENTATION                                                              */
/******************************************************************************/
/** \file     optflash_drv.c
 *
 *  \ingroup  msp
 *
 *  \brief    Implementation of option bytes flash driver for GD32F33.
 *
 */


/******************************************************************************/
/* INCLUDES                                                                   */
/******************************************************************************/
#include "bsh_stdinc.h"
#include <stdint.h>
#include "processor.h"
#include "firmware_update/mem_drv/optflash_drv.h"
#include "firmware_update/mem_drv/mem_drv.h"
#include "hsup.h"
/******************************************************************************/
/* LOCAL TYPE DEFINITIONS                                                     */
/******************************************************************************/
/* write unit needed in common include com_mal.h */
typedef uint16_t COMA_wrUnit_t; ///< write unit for option bytes
#include "firmware_update/mem_drv/com_mal.h"



/******************************************************************************/
/* LOCAL FUNCTION DECLARATIONS                                                */
/******************************************************************************/
static void DRVOF_unlockFlash(void);
static void DRVOF_lockFlash(void);
static bool DRVOF_isSizeAligned(uint32_t size);
static bool DRVOF_isOptByteValid(const uint8_t* buffer);

/******************************************************************************/
/* LOCAL DATA DEFINITIONS                                                     */
/******************************************************************************/
//lint -esym(9003,COMA_*,DRVOF_*) block scope does not increase maintainability here


static const struct COMA_memSeg_s DRVOF_flashSeg =
{   //whole option byte area
    .blockSize = ROM3_PHY_SIZE,
    .startAddr = ROM3_PHY_START, // first option byte address
    .endAddr   = ROM3_PHY_END  // last option byte address
};


static const uint32_t DRVOF_flashKey1=0x45670123u;  ///< first Flash Key
static const uint32_t DRVOF_flashKey2=0xCDEF89ABu;  ///< second Flash Key


static const uint32_t DRVOF_errMask= FLASH_SR_WRPRT_MASK | FLASH_SR_PGERR_MASK;

static const uint32_t DRVOF_cmdMask=
    FLASH_CR_STRT_MASK | FLASH_CR_OPTER_MASK | FLASH_CR_OPTPG_MASK |
    FLASH_CR_MER_MASK  | FLASH_CR_PER_MASK   | FLASH_CR_PG_MASK;


/******************************************************************************/
/* PUBLIC DATA DEFINITIONS                                                    */
/******************************************************************************/
const struct MEMDRV_driver_s DRVOF_driver =
{
    .allowInterrupts     = false,
    .timeOutPwr2         = MEMDRV_TMO_2S,
    .writeBlockSize      = sizeof(COMA_wrUnit_t),
    .init                = DRVOF_initFlash,
    .deinit              = DRVOF_deinitFlash,    
    .isAddrRangeValid    = DRVOF_isAddrRangeValid,
    .reset               = DRVOF_resetFlash,
    .isBusy              = DRVOF_isBusy,
    .isError             = DRVOF_isError,
    .write               = DRVOF_write,
    .erase               = DRVOF_erase,
    .read                = NULL,
    .checkErase          = DRVOF_checkErase
};

/******************************************************************************/
/* PUBLIC FUNCTION DEFINITIONS                                                */
/******************************************************************************/
void DRVOF_initFlash(void)
{
    DRVOF_unlockFlash();
    DRVOF_resetFlash();
}


void DRVOF_deinitFlash(void)
{
    DRVOF_resetFlash();
    DRVOF_lockFlash();
}


uint32_t DRVOF_write(uint32_t addr, uint32_t size, const uint8_t* buffer)
{
    if( (!COMA_isAligned(addr))   ||   /* aligned?           */
        (!DRVOF_isSizeAligned(size))||  /* multiple of 2?     */
        (!DRVOF_isOptByteValid(buffer)) /* complement valid ? */
      )
    {
        return 0; /* data cannot be written */
    }
    FLASH_CR |= FLASH_CR_OPTPG_MASK; /* enable option byte write */
    COMA_copyToWrUnit(size, buffer, addr);

    return sizeof(COMA_wrUnit_t); /* return written size */
}


uint32_t DRVOF_erase(uint32_t addr, uint32_t size)
{
    (void)(size);
    if(!COMA_isBlockAddrValid(&DRVOF_flashSeg, addr)){return 0u;}
    FLASH_CR |= FLASH_CR_OPTER_MASK;    /* enables option byte erase */
    FLASH_CR |= FLASH_CR_STRT_MASK;     /* starts the erase */
    return DRVOF_flashSeg.blockSize;
    
}


bool DRVOF_isBusy(void)
{
    return (((FLASH_SR & FLASH_SR_BSY_MASK) != 0u) ? true : false);
}


bool DRVOF_isError(void)
{
    return (((FLASH_SR & DRVOF_errMask) != 0u) ? true : false);
}


void DRVOF_resetFlash(void)
{
    FLASH_SR |=  DRVOF_errMask; /* all error handling status register will be set to 1 */
    FLASH_CR &= ~DRVOF_cmdMask; /* all control register are set to 0 */
}


bool DRVOF_isAddrRangeValid(uint32_t addr, uint32_t range)
{
    return COMA_isAddrRangeValid(&DRVOF_flashSeg, addr, range);
}


uint32_t DRVOF_checkErase(uint32_t addr, uint32_t size)
{
    (void)addr;
    (void)size;
    return 0u; // force an erase in any case
}

/******************************************************************************/
/* LOCAL FUNCTION DEFINITIONS                                                 */
/******************************************************************************/
/** Prepares the flash memory for writing */
static void DRVOF_unlockFlash(void)
{
    FLASH_KEYR   = DRVOF_flashKey1;
    FLASH_KEYR   = DRVOF_flashKey2;
    FLASH_OPTKEYR= DRVOF_flashKey1;
    FLASH_OPTKEYR= DRVOF_flashKey2;
}


/** lock the option flash memory */
static void DRVOF_lockFlash(void)
{
    FLASH_CR |= FLASH_CR_LOCK_MASK;
    FLASH_CR &= ~FLASH_CR_OPTWRE_MASK;
}


/** Checks if size is multiple of write unit (8)
 *  \param size: number of bytes to write
 *  \return true if size is multiple of 8, false otherwise 
 */
static bool DRVOF_isSizeAligned(uint32_t size)
{
    const uint32_t sizeMask = (sizeof(COMA_wrUnit_t)-1u);
    
    return (((size & sizeMask) == 0u ) ? true : false);
    
}


/** Checks if next option bytes to write has correct complement
 *  \param buffer: pointer to data buffer
 *  \return true if complement is correct, false otherwise 
 */
static bool DRVOF_isOptByteValid(const uint8_t* buffer)
{
    return ((buffer[0] == (uint8_t)~buffer[1]) ? true : false);
}
