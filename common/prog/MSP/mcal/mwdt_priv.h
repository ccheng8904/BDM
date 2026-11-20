/*******************************************************************************
 *  Copyright (c) 2021 BSH Hausgeraete GmbH,
 *  Carl-Wery-Str. 34, 81739 Munich, Germany, www.bsh-group.de
 *
 *  All rights reserved. This program and the accompanying materials
 *  are protected by international copyright laws.
 *  Please contact copyright holder for licensing information.
 *
 *******************************************************************************
 *  PROJECT          Generic SW
 *  COMP_ABBREV      MWDT
 ******************************************************************************/


#ifndef MWDT_PRIV_H
#define MWDT_PRIV_H

#ifdef __cplusplus
extern "C" {
#endif


/******************************************************************************/
/* DOCUMENTATION                                                              */
/******************************************************************************/
/** \file
 *
 *  \ingroup  MCAL
 *
 *  \brief    Public definitions and declarations for unit MWDT.
 *
 */


/******************************************************************************/
/* INCLUDES                                                                   */
/******************************************************************************/

#include "bsh_stdinc.h"
#include "mcal/mcal.h"


/******************************************************************************/
/* PRIVATE FUNCTION DECLARATIONS                                              */
/******************************************************************************/

/** Function to calculate the deviation in percent between actual and required time for watchdog
 *  \param actualTimeUs: actual time in us based on calculated from a watchdog registers
 *  \param requiredTimeMs: the required time in ms to be achieved from user configuration
 *  \return positive integer in percent. Returns the deviation in percent between actual and required time
 */
extern uint16_t MWDT_getPrecisionInPercent(uint32_t actualTimeUs, uint16_t requiredTimeMs);


#ifdef __cplusplus
}
#endif

#endif // MWDT_PRIV_H
