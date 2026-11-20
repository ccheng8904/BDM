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
 *  COMP_ABBREV      HWDT
 ******************************************************************************/
#ifndef HWDT_H
#define HWDT_H

#ifdef __cplusplus
extern "C" {
#endif

/******************************************************************************/
/* DOCUMENTATION                                                              */
/******************************************************************************/
/** \file     hwdt.h
 *  \brief    This file contains the exported definitions and interfaces for the implementation of the HAL WDT.

\section WDTintro Introduction

The HAL WDT function supports the handling of the watchdog

\section WDTifc1 Interface Package 1

<TABLE>
<TR><TD><b>Interface</TD><TD>Description</b></TD></TR>
<TR><TD>HWDT_vInit(uchar ucIndex)</TD><TD>Initialise the WDT </TD></TR>
<TR><TD>HWDT_vTrigger()</TD><TD>Re-triggers the processor internal watchdog.</TD></TR>
</TABLE>
*/

/******************************************************************************/
/* INCLUDES                                                                   */
/******************************************************************************/
#include "LibTypes.h"
#include "processor.h"


/*
Example of initialization and usage in the application :

1. Configuration structure (by default located in hwdt01xs.c)

******************************* IWDG structure *******************************
const T_HWDT_Config HWDT_atConfig[] = { HWDT_USE_IWDG                  -> bWatchdogType
                                        HWDT_IWDT_DIVIDER_32,          -> ucWdtPrescaler(0x03U)
                                        HWDT_IWDT_WINDOW_MAX_VALUE,    -> uiWindowUpperLimit(0x0FFFU)
                                        HWDT_IWDT_MAX_RELOAD_VALUE };  -> uiWdtUpdateValue(0x0FFFU)
   t_iwdg = LSI(40kHz)^-1 * 2^ucWdtPrescaler * uiWdtUpdateValue
   0.82s = 1/40000 * 8 * 4095
   26.2s = 1/40000 * 256 * 4095
******************************* WWDG structure *******************************
const T_HWDT_Config HWDT_atConfig[] = { HWDT_USE_WWDG             -> bWatchdogType
                                        HWDT_COUNTER_CLOCK_DIV_8, -> ucWdtPrescaler(0x03U)
                                        HWDT_WINDOW_MAX_VALUE,    -> uiWindowUpperLimit(0x7FU)
                                        HWDT_MAX_RELOAD_VALUE };  -> uiWdtUpdateValue(0x7FU)
  t_wwdg = Pclk(48MHz)^-1 * 4096 * 2^ucWdtPrescaler * (ucWdtUpdateValue(0x7F)+1)
  87.38 ms = (1/48MHz) * 4096 * 2^3 * (127+1)


2. Initialization and use
.... some code

   HWDT_vInit(0);   - Initialization of the WDT

.... some code
   for(;;) {
   HWDT_vTrigger(); - trigger WDT
   }
*/

/******************************************************************************/
/* PUBLIC DEFINITIONS                                                         */
/******************************************************************************/

/**
\b description: \n
 These defines are used to fill Run Time Configuration structures.\n
\n
*/

/* Watchdog timer type enumeration */
typedef enum {HWDT_USE_WWDG, HWDT_USE_IWDG} wdgType_t;

// Defines for IWDT (Independent watchdog)
#define HWDT_IWDT_DIVIDER_4            0x00U        // 0.40seconds
#define HWDT_IWDT_DIVIDER_8            0x01U        // 0.82sec
#define HWDT_IWDT_DIVIDER_16           0x02U        // 1.64s
#define HWDT_IWDT_DIVIDER_32           0x03U        // 3.28s
#define HWDT_IWDT_DIVIDER_64           0x04U        // 6.55s
#define HWDT_IWDT_DIVIDER_128          0x05U        // 13.1s
#define HWDT_IWDT_DIVIDER_256          0x06U        // 26.2s

#define HWDT_IWDT_MAX_RELOAD_VALUE     0x0FFFU
#define HWDT_IWDT_WINDOW_MAX_VALUE     0x0FFFU
#define HWDT_IWDT_KEY_TO_TRIGGER       0xAAAAU
#define HWDT_IWDT_KEY_TO_UNLOCK_REG    0x5555U
#define HWDT_IWDT_KEY_TO_START_WDT     0xCCCCU

// Defines for WWDT (Window watchdog)
#define HWDT_COUNTER_CLOCK_DIV_0       0x00U
#define HWDT_COUNTER_CLOCK_DIV_2       0x01U
#define HWDT_COUNTER_CLOCK_DIV_4       0x02U
#define HWDT_COUNTER_CLOCK_DIV_8       0x03U


#define HWDT_MAX_RELOAD_VALUE     0x007FU
#define HWDT_WINDOW_MAX_VALUE     0x007FU

/******************************************************************************/
/* PUBLIC TYPE DEFINITIONS                                                    */
/******************************************************************************/

/*=====================================================================================
    THWDT_Config
=====================================================================================*/

/**
\b description: \n
 This type defines the configuration set of the HAL WDT for the Run Time Configuration.\n
 The configuration is dependent on the parameter bWatchdogType. This parameter tells if \n
 Window watchdog or independent watchdog will be used.\n
\n
******************* WWDG (System window watchdog) configuration *******************\n
\n
\b elements for WWDG (HWDT_USE_WWDG parameter used): \n
\n
 \b ucWdtPrescaler:\n
  \b description: \n
  \b type: uint8_t\n
\n
              HWDT_COUNTER_CLOCK_DIV_0 : CK Counter Clock (PCLK1 div 4096) div 1\n
              HWDT_COUNTER_CLOCK_DIV_2 : CK Counter Clock (PCLK1 div 4096) div 2\n
              HWDT_COUNTER_CLOCK_DIV_4 : CK Counter Clock (PCLK1 div 4096) div 4\n
              HWDT_COUNTER_CLOCK_DIV_8 : CK Counter Clock (PCLK1 div 4096) div 8\n
\n
 \b uiWindowUpperLimit: set the upper limit for the counter upload time span\n
  \b description: \n
  \b type: uint16_t\n
  \b range: 0x40 .. 0x7F\n
 \b uiWdtUpdateValue: set the value by which the counter will be updated\n
  \b description: \n
  \b type: uint16_t\n
  \b range: 0x40 .. 0x7F\n
\n
******************** IWDG (Independent watchdog) configuration ********************\n
\n
\b elements for IWDG (HWDT_USE_IWDG parameter used): \n
\n
 \b ucWdtPrescaler:\n
  \b description: \n
  \b type: uint8_t\n
\n
               HWDT_IWDT_DIVIDER_4   : LSI (40 kHz) / 4   \n
               HWDT_IWDT_DIVIDER_8   : LSI (40 kHz) / 8   \n
               HWDT_IWDT_DIVIDER_16  : LSI (40 kHz) / 16  \n
               HWDT_IWDT_DIVIDER_32  : LSI (40 kHz) / 32  \n
               HWDT_IWDT_DIVIDER_64  : LSI (40 kHz) / 64  \n
               HWDT_IWDT_DIVIDER_128 : LSI (40 kHz) / 128 \n
               HWDT_IWDT_DIVIDER_256 : LSI (40 kHz) / 256 \n
\n
 \b uiWindowUpperlimit: set the upper limit for the counter upload time span\n
  \b description: \n
  \b type: uint16_t\n
  \b range: 0x0 .. 0x0FFF\n
 \b uiWdtUpdateValue: set the value by which the counter will be updated.\n
  \b description: \n
  \b type: uint16_t\n
  \b range: 0x0FFF\n
\n
*/


typedef struct _T_HWDT_Config {
   wdgType_t bWatchdogType;
   uint8_t ucWdtPrescaler;
   uint16_t uiWindowUpperLimit;
   uint16_t uiWdtUpdateValue;
} T_HWDT_Config;

/******************************************************************************/
/* PUBLIC FUNCTION DECLARATIONS                                               */
/******************************************************************************/


/*=====================================================================================
    HWDT_vInit
=====================================================================================*/
/** Initialize WDT

\b package: \n
 HWDTPackage1\n

\note
 */

extern void HWDT_vInit(uint8_t ucIndex);


/*=====================================================================================
    HWDT_vTrigger
=====================================================================================*/
/** Re-triggers the processor internal watchdog.

\b package: \n
 HWDTPackage1\n

\b description: \n
 This function re-triggers the processor internal watchdog. \n
 \n
\param  none\n
 \n
\return none
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
