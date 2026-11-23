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
 *  \brief    CMS32M67xx specific implementation of the Boot Manager shared data.
 *
 *  \details  The revision identifier of the CMS32M67xx is read from the
 *            address 0x000000C4, the reset cause is provided by the reset flag
 *            register (RST->RESF):
 *
 *             - LVIRF  low voltage detection reset
 *             - IAWRF  illegal access reset
 *             - WDTRF  watchdog reset
 *             - SYSRF  software/system reset
 *
 *            A power on reset is recognised by the fact that none of the reset
 *            flags is set after the device was powered up.
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
/* PRIVATE DEFINITIONS                                                        */
/******************************************************************************/

/* all reset source flags of the CMS32M67xx reset flag register */
#define BMDAT_RESET_FLAG_MASK   (RST_RESF_LVIRF_Msk | RST_RESF_IAWRF_Msk | \
                                 RST_RESF_WDTRF_Msk | RST_RESF_SYSRF_Msk)

/******************************************************************************/
/* PUBLIC FUNCTION DEFINITIONS                                                */
/******************************************************************************/

bool BMDAT_isPowerOnReset(void)
{
    /* after a power on reset no reset source flag is set, after any other
     * reset source the corresponding flag is set by the hardware */
    return (((RST->RESF & BMDAT_RESET_FLAG_MASK) == 0u) ? true : false);
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
