/*******************************************************************************
*   Copyright (c) 2023 BSH Hausgeraete GmbH,
*   Carl-Wery-Str. 34, 81739 Munich, Germany, www.bsh-group.de
*
*   All rights reserved. This program and the accompanying materials
*   are protected by international copyright laws.
*   Please contact copyright holder for licensing information.
*
********************************************************************************
*   PROJECT          Generic SW
*   PROCESSOR        CMS32M67xx
*******************************************************************************/


/******************************************************************************/
/* DOCUMENTATION                                                              */
/******************************************************************************/
/** \file    hwdt.c
*   \brief   Implementation of the HAL WDT for the CMS32M67xx.
*/

/******************************************************************************/
/* INCLUDES                                                                   */
/******************************************************************************/
#include "bsh_stdinc.h"
#include "hwdt.h"


/******************************************************************************/
/* PUBLIC FUNCTION DEFINITIONS                                                */
/******************************************************************************/

void HWDT_vInit(uint8_t ucIndex)
{
    (void)ucIndex;

    /* The watchdog is configured by the user option byte 0xC0, therefore only
     * the debug behaviour is defined here (stop the watchdog when the core is
     * halted) */
    WDT_DISABLE_DEBUG();

    /* service the watchdog once */
    HWDT_vTrigger();
}


void HWDT_vTrigger(void)
{
    WDT_Restart();
}
