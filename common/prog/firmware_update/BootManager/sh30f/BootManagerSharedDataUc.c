/*******************************************************************************
 *  Copyright (c) 2023 BSH Hausgeraete GmbH,
 *  Carl-Wery-Str. 34, 81739 Munich, Germany, www.bsh-group.de
 *
 *  All rights reserved. This program and the accompanying materials
 *  are protected by international copyright laws.
 *  Please contact copyright holder for licensing information.
 *
 *******************************************************************************
 *  PROJECT          Boot-Manager
 ******************************************************************************/


/******************************************************************************/
/* DOCUMENTATION                                                              */
/******************************************************************************/
/** \file     BootManagerSharedDataUc.c
 *
 *  \ingroup  BootManager
 *
 *  \brief    GD32E23 specific implementation of for Boot Manager shared data.
 *
 */


/******************************************************************************/
/* INCLUDES                                                                   */
/******************************************************************************/
#include "BSH_stdinc.h"
#include <stdbool.h>
#include "BootManagerSharedData.h"
#include "BootManagerSharedDataUc.h"
#include "processor.h"


/******************************************************************************/
/* PUBLIC FUNCTION DEFINITIONS                                                */
/******************************************************************************/

bool BMDAT_isPowerOnReset(void)
{
    if((RCC->RSTSTR.V32 & RCC_RSTSTR_PORSTF_Msk) != 0u)
    {
        // clear the reset flag
        RCC_REGS_UNLOCK();
        RCC->RSTCLR.V32 |= (uint32_t)RCC_RSTCLR_PORSTFC_Msk;
        RCC_REGS_LOCK();

        return true;
    }
    return false;
}

void BMDAT_setDataInitializedFlag(void)
{
    BMDAT_setMagicPattern(BMDAT_magicPattern);
}

void BMDAT_resetDataInitializedFlag(void)
{
    BMDAT_setMagicPattern(BMDAT_startAppAfterReset);
}


bool BMDAT_isDataInitialized(void)
{
    return ((BMDAT_getMagicPattern() == BMDAT_magicPattern) ? true : false);
}
