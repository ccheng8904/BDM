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
/** \file     flashMcXs.c
 *  \brief    Implementation of CMS32M67xx processor specific extension code for flash routines.
 */
/******************************************************************************/
/* INCLUDES                                                                   */
/******************************************************************************/
#include "bsh_stdinc.h"
#include "LibTypes.h"
#include "FlashMcXS.h"
#include "Flash.h"
#include "hsup.h"
#include "hwdt.h"

/******************************************************************************/
/* PRIVATE DEFINITIONS                                                        */
/******************************************************************************/

#define FLASH_BLOCK_SIZE          512   ///< Number of bytes in flash memory block (sector size)
#define PROTECTED_PAGES_PER_PIN     1   ///< Number of flash pages that share common protect pin
#if defined(CMS32M67XX)
/* 128 kB, sector 512 bytes */
#define FLASH_BLOCKS_COUNT        256   ///< Number of flash memory blocks
#else
#error "Processor derivative not defined, please check compiler options !"
#endif

/******************************************************************************/
/* PUBLIC DATA DEFINITIONS                                                    */
/******************************************************************************/
const uint16_t FLASH_blockCount              = FLASH_BLOCKS_COUNT;       ///< Variable holds number of flash memory blocks
const uint16_t FLASH_blockSize               = FLASH_BLOCK_SIZE;         ///< Variable holds number of bytes in flash memory block
const uint16_t FLASH_protectedPagesPerPin    = PROTECTED_PAGES_PER_PIN;  ///< Variable holds number of flash pages that share common protect pin

/******************************************************************************/
/* PRIVATE FUNCTION DECLARATIONS AND PRIVATE MACRO FUNCTION DEFINITIONS       */
/******************************************************************************/

/******************************************************************************/
/* PUBLIC FUNCTION DEFINITIONS                                                */
/******************************************************************************/

/** \brief    Library callback function to restart the Watchdog timer
 *            in the poll loop of flash ready flag
 * \remarks   This function is used when polling the flash ready bit to avoid
 *            watchdog timer expiration and therefore a reset during flash operation.
 */
void _FLASH_resetWatchdogTimer(void)
{
    /* Note : This is only an implementation example
              which must not match your case. Implement
              this part according to your application
    */
    HWDT_vTrigger();
}

void _FLASH_vOperationStart(T_FLASH_OPSTATE operation)
{
    switch ( operation )
    {
        case FLASH_STATE_PROGRAM :
        {
            break;
        }
        case FLASH_STATE_READ :
        {
            break;
        }
        case FLASH_STATE_ERASE :
        {
            break;
        }
        case FLASH_STATE_OPTIONS:
        {
            break;
        }
        case FLASH_STATE_LOCK_CHECK :
        {
            break;
        }
        default:
        {
            break;
        }
    }
}

void _FLASH_vOperationFinished(T_FLASH_OPSTATE operation)
{
    switch ( operation )
    {
        case FLASH_STATE_PROGRAM :
        {
            break;
        }
        case FLASH_STATE_READ :
        {
            break;
        }
        case FLASH_STATE_ERASE :
        {
            break;
        }
        case FLASH_STATE_OPTIONS :
        {
            break;
        }
        case FLASH_STATE_LOCK_CHECK :
        {
            break;
        }
        default:
        {
            break;
        }
    }
}
