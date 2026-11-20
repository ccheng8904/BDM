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
 *  COMP_ABBREV      LPAN
 ******************************************************************************/


/******************************************************************************/
/* DOCUMENTATION                                                              */
/******************************************************************************/
/** \file
 *
 *  \brief Application specific implementation / configuration
 *         for logpan library.
 * 
 *  \note This is a minimal implementation of configuration without any function
 *        Check for configuration examples to make your own implementation
 *
 */

/******************************************************************************/
/* INCLUDES                                                                   */
/******************************************************************************/
#include "bsh_stdinc.h"
#include <stdint.h>
#include <stdbool.h>
#include "logpan.h"
#include "logpPriv.h"

/******************************************************************************/
/* PRIVATE DEFINITIONS                                                        */
/******************************************************************************/
#define LPAN_NUMBER_OF_GRIPS  6u

/******************************************************************************/
/* PUBLIC VARIABLES                                                           */
/******************************************************************************/
uint8_t LPAN_aucGripState[LPAN_NUMBER_OF_GRIPS];
const uint8_t LPAN_ucNumberOfGrips = LPAN_NUMBER_OF_GRIPS;

/******************************************************************************/
/* PUBLIC FUNCTION DEFINITIONS                                                */
/******************************************************************************/
bool _LPAN_bIsKeyMask(uint8_t ucGripId)
{
    (void)ucGripId;
    return false;
}


bool _LPAN_bIsTimerDown(uint8_t ucGripId)
{
    (void)ucGripId;
    return false;
}


void _LPAN_vLoadTimer(uint8_t ucGripId, uint8_t ucTimeId)
{
    (void)ucGripId;
    (void)ucTimeId;
}


uint8_t _LPAN_ucMapGripEvent(uint8_t ucGripId, uint8_t ucPhysEvent)
{
    (void)ucGripId;
    (void)ucPhysEvent;
    return LPAN_NO_EVENT;
}
