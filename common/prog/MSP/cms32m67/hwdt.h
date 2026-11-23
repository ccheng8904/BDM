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
/** \file     hwdt.h
 *  \brief    This file contains the exported definitions and interfaces for the
 *            implementation of the HAL WDT of the CMS32M67xx.
 *
 *            The watchdog of the CMS32M67xx is configured by the user option
 *            byte 0xC0 (WDTON, WDCS2..0, WINDOW1..0, WDTINT, WDSTBYON) inside
 *            system_CMS32M67xx.c. This module provides the service (trigger)
 *            of the watchdog only.
 */

#ifndef HWDT_H
#define HWDT_H

#ifdef __cplusplus
extern "C" {
#endif

/******************************************************************************/
/* INCLUDES                                                                   */
/******************************************************************************/
#include "LibTypes.h"
#include "processor.h"


/******************************************************************************/
/* PUBLIC DEFINITIONS                                                         */
/******************************************************************************/

/* Watchdog timer type enumeration */
typedef enum {HWDT_USE_WWDG, HWDT_USE_IWDG} wdgType_t;

/* Values of the service (trigger) key of the CMS32M67xx watchdog */
#define HWDT_KEY_TO_TRIGGER             0xACU

/* The watchdog overflow time is defined by the option byte 0xC0
 * (WDCS2..0: 2^6/fIL ... 2^16/fIL, fIL = 15kHz) */
#define HWDT_IWDT_MAX_RELOAD_VALUE      0x1FFFU
#define HWDT_IWDT_WINDOW_MAX_VALUE      0x1FFFU

/******************************************************************************/
/* PUBLIC TYPE DEFINITIONS                                                    */
/******************************************************************************/

typedef struct _T_HWDT_Config {
   wdgType_t bWatchdogType;
   uint8_t   ucWdtPrescaler;
   uint16_t  uiWindowUpperLimit;
   uint16_t  uiWdtUpdateValue;
} T_HWDT_Config;

/******************************************************************************/
/* PUBLIC FUNCTION DECLARATIONS                                               */
/******************************************************************************/

/** Initialize WDT
 *
 *  \note The watchdog is enabled and configured by the user option byte, this
 *        function only stops the watchdog while the debugger halts the core
 *        and triggers the watchdog once.
 */
extern void HWDT_vInit(uint8_t ucIndex);


/** Re-triggers the processor internal watchdog.
 */
/*Axivion Disable MisraC2012-8.6 : Type, object or function without definition */
/*
   Note 9004: object/function 'MWDT_trigger(void)' previously declared [MISRA 2012 Rule 8.5, mandatory]
*/
//lint -save -e9004
extern void HWDT_vTrigger(void);
//lint -restore -e9004
/* Axivion Enable Style MisraC2012-8.6 */

#ifdef __cplusplus
}
#endif

#endif  // HWDT_H
