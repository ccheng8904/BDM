/*******************************************************************************
 *  Copyright (c) 2017 BSH Hausgeraete GmbH,
 *  Carl-Wery-Str. 34, 81739 Munich, Germany, www.bsh-group.de
 *
 *  All rights reserved. This program and the accompanying materials
 *  are protected by international copyright laws.
 *  Please contact copyright holder for licensing information.
 *
 *******************************************************************************
 *  PROJECT          System Timer, Generic SW
 *  COMP_ABBREV      STIM
 ******************************************************************************/

#ifndef SYSTEM_TIMER_CFG_H
#define SYSTEM_TIMER_CFG_H

#ifdef __cplusplus
extern "C" {
#endif


/******************************************************************************/
/* DOCUMENTATION                                                              */
/******************************************************************************/

/** \file
 *
 *  \brief    Public definitions and declarations for Intel system timer configuration.
 */

/******************************************************************************/
/* INCLUDES                                                                   */
/******************************************************************************/
#include "bsh_stdinc.h"
#include <stdint.h>


/******************************************************************************/
/* PUBLIC FUNCTION DECLARATIONS                                               */
/******************************************************************************/

/** \brief    System timer base timer configuration.
 *
 *  \param    period_ms: period of timer interrupt in milliseconds.
 *
 *  \retval   required by STIM_InitModule and is always 0
 */
extern int32_t STIM_HwConfigure(uint16_t period_ms);


#ifdef __cplusplus
}
#endif

#endif // SYSTEM_TIMER_CFG_H
