/*******************************************************************************
 *  Copyright (c) 2020 BSH Hausgeraete GmbH,
 *  Carl-Wery-Str. 34, 81739 Munich, Germany, www.bsh-group.de
 *
 *  All rights reserved. This program and the accompanying materials
 *  are protected by international copyright laws.
 *  Please contact copyright holder for licensing information.
 *
 *******************************************************************************
 *  PROJECT          Generic SW
 *  COMP_ABBREV      PRG
 ******************************************************************************/


#ifndef PRGUSRTSK_H
#define PRGUSRTSK_H

#ifdef __cplusplus
extern "C" {
#endif

/******************************************************************************/
/* INCLUDES                                                                   */
/******************************************************************************/
#include "bsh_stdinc.h" 
#include "LibTypes.h"

/******************************************************************************/
/* PUBLIC FUNCTION DECLARATIONS                                               */
/******************************************************************************/

/**
 *  \brief    Function PRG_bUserHandleTask(void) is a user specific function in Programmer.
 *            This function is called periodically inside a simple endless loop in the Programmer.
 *
 *  \param    none
 *
 *  \return   TASK_INITIALIZED(true) or TASK_NOT_INITIALIZED (false)
 *            Return value is ignored in the Programmer's scheduler.
 *
 *  \details  TASK_TERMINATE is not valid for Programmer user task handler
 */
extern bool PRG_bUserHandleTask(void);


#ifdef __cplusplus
}
#endif

#endif // PRGUSRTSK_H
