/*******************************************************************************
 *  Copyright (c) 2017 BSH Hausgeraete GmbH,
 *  Carl-Wery-Str. 34, 81739 Munich, Germany, www.bsh-group.de
 *
 *  All rights reserved. This program and the accompanying materials
 *  are protected by international copyright laws.
 *  Please contact copyright holder for licensing information.
 *
 *******************************************************************************
 *  PROJECT          PP PED Framework
 *  COMP_ABBREV      HSUP
 ******************************************************************************/

#ifndef HSUPMC_H
#define HSUPMC_H

#ifdef __cplusplus
extern "C" {
#endif

/******************************************************************************/
/* DOCUMENTATION                                                              */
/******************************************************************************/
/** \file
 *
 *  \brief    Exported definitions and interfaces of specific part of HAL SUP.
 *
 */

/******************************************************************************/
/* INCLUDES                                                                   */
/******************************************************************************/
#include "bsh_stdinc.h" 
#include <stdint.h>
#include "processor.h"


/** reverse the bit order of a 32 Bit argument.

\param Val: value that shall be mirrored\n
 \b type: uint16_t\n

\return: Mirrored result\n
 \b type: uint16_t\n

*/
extern uint16_t HSUP_u16Mirror(uint16_t Val);

/** reverse the bit order of a 32 Bit argument.

\param Val: value that shall be mirrored\n
 \b type: uint32_t\n

\return: Mirrored result\n
 \b type: uint32_t\n

*/
extern uint32_t HSUP_u32Mirror(uint32_t Val);


/** Performs a NOP operation of the processor.

\b description: \n
 This function performs a NOP operation of the processor.

\param none

\return none
*/
/*lint -esym(9026, HSUP_*) function-like macro*/
#ifdef __NOT_CC__
#define HSUP_vNop() do { } while(0)
#else
#define HSUP_vNop() do {__asm("NOP");} while(0)
#endif


extern void HSUP_DelayClk(uint16 delay, uint32 clock);
#define HSUP_vDelay(delay)  HSUP_DelayClk(delay, CORE_CLOCK)


/** Enters the low power mode (with lowest power consumption)


\b description: \n
 This function enters the low power mode (with lowest power consumption). It will return
 as soon the condition(s) for leaving the low power mode are fulfilled.

\note This function is not re-entrant.

\param none

\return none
*/
extern void HSUP_vEnterLowPowerMode(void);


#ifdef __cplusplus
}
#endif

#endif
