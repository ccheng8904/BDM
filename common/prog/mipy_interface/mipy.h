/*******************************************************************************
*   Copyright (c) 2017 BSH Hausgeraete GmbH,
*   Carl-Wery-Str. 34, 81739 Munich, Germany, www.bsh-group.de
*
*   All rights reserved. This program and the accompanying materials
*   are protected by international copyright laws.
*   Please contact copyright holder for licensing information.
*
********************************************************************************
*   PROJECT          Micropython TestInterface
*   COMP_ABBREV      MIPY
*******************************************************************************/

#ifndef __MIPY_H
#define __MIPY_H

#ifdef __cplusplus
extern "C" {
#endif


/******************************************************************************/
/* DOCUMENTATION                                                              */
/******************************************************************************/
/** \file mipy.h
    Generic header to include for the use of the micropython-interface.
*/


/******************************************************************************/
/* Scheduler Task-Dependencies                                                */
/******************************************************************************/
#define DEPLIB_MIPY (TtaskId)MOD_MIPY, (TtaskId)MOD_TIM, ///< dependencies of library module


/******************************************************************************/
/* INCLUDES                                                                   */
/******************************************************************************/
#include "mipy_interface/testdriver_table_config.h"
#include "testdriver_interface.h"

#ifdef __cplusplus
}
#endif

#endif
