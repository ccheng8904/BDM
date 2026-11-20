/*******************************************************************************
 *  Copyright (c) 2017 BSH Hausgeraete GmbH,
 *  Carl-Wery-Str. 34, 81739 Munich, Germany, www.bsh-group.de
 *
 *  All rights reserved. This program and the accompanying materials
 *  are protected by international copyright laws.
 *  Please contact copyright holder for licensing information.
 *
 *******************************************************************************
 *  PROJECT          PP PED-Framework
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
 *  \brief  Exported definitions and interfaces of specific part of HAL SUP.
 *
 */

/******************************************************************************/
/* INCLUDES                                                                   */
/******************************************************************************/
#include "bsh_stdinc.h" 
#include <stdint.h>


/** \brief reverse the bit order of a 16 Bit argument.
 *
 *  \param Val: value that shall be mirrored\n
 *
 *  \return Mirrored result\n
 *
 */
extern uint16_t HSUP_u16Mirror(uint16_t Val);

/** \brief reverse the bit order of a 32 Bit argument.
 *
 *  \param Val: value that shall be mirrored\n
 *
 *  \return Mirrored result\n
 *
 */
extern uint32_t HSUP_u32Mirror(uint32_t Val);


/** \brief Performs a NOP operation of the processor.
 * 
 *  Hint: Mock implementation for intel
 */
extern void HSUP_vNop(void);


/** \brief Performs a delay.
 *
 *  Hint: Mock implementation for intel
 *  \param delay in µs
 *
 */
extern void HSUP_vDelay(uint16_t delay);


/** \brief Enters the low power mode (with lowest power consumption)
 * 
 * This function enters the low power mode (with lowest power consumption). It will return
 * as soon the condition(s) for leaving the low power mode are fulfilled.
 * Hint: Mock implementation for intel
 *
 */
extern void HSUP_vEnterLowPowerMode(void);


#ifdef __cplusplus
}
#endif

#endif
