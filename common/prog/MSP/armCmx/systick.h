/*******************************************************************************
 *  Copyright (c) 2017 BSH Hausgeraete GmbH,
 *  Carl-Wery-Str. 34, 81739 Munich, Germany, www.bsh-group.de
 *
 *  All rights reserved. This program and the accompanying materials
 *  are protected by international copyright laws.
 *  Please contact copyright holder for licensing information.
 *
 *******************************************************************************
 *  PROJECT          Generic SW
 *  COMP_ABBREV      SysTick
 ******************************************************************************/


#ifndef SYSTICK_H
#define SYSTICK_H

#ifdef __cplusplus
 extern "C" {
#endif


/******************************************************************************/
/* DOCUMENTATION                                                              */
/******************************************************************************/
/** \file
 *
 *  \brief    Public definitions and declarations for unit SysTick.
 *
 */


/******************************************************************************/
/* INCLUDES                                                                   */
/******************************************************************************/
#include "bsh_stdinc.h"
#include <stdint.h> 


/******************************************************************************/
/* PUBLIC FUNCTION DECLARATIONS                                               */
/******************************************************************************/

/** \brief    Function for configuring SysTick timer.
 *
 *  \details  This function initialize SysTick based on actual system clock and
 *            desired period. Most common period for SysTick is 1 ms. When the
 *            core clock changes, SysTick needs to be configured for new clock
 *            settings to preserve functionality.
 *
 *  \param    sysClock_Hz:  Actual core clock frequency in hertz
 *  \param    period_ms:    desired SysTick period in milliseconds
 *
 *  \retval   return number of ticks if configuration was successful, otherwise -1
 */
extern int32_t SysTick_Configure(uint32_t sysClock_Hz, uint16_t period_ms);


/** \brief    Function for getting actual period of SysTick.
 *
 *  \retval   actual period of SysTick timer in milliseconds
 */
extern uint16_t SysTick_GetPeriod(void);


#ifdef __cplusplus
}
#endif

#endif // SYSTICK_H
