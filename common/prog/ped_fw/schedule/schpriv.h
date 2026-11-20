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
 *  COMP_ABBREV      SCH
 ******************************************************************************/
#ifndef SCHPRIV_H
#define SCHPRIV_H

#ifdef __cplusplus
extern "C" {
#endif


/******************************************************************************/
/* DOCUMENTATION                                                              */
/******************************************************************************/
/** \file
 *
 *  \brief    Private definitions and declarations of schedule.
 *
 */


/******************************************************************************/
/* INCLUDES                                                                   */
/******************************************************************************/
#include "bsh_stdinc.h"
#include <stdbool.h>
#include <stdint.h>



/******************************************************************************/
/* PUBLIC DATA DECLARATIONS                                                   */
/******************************************************************************/

/** table of task handler

This Table has to be defined by application.\n
The schedule library handles up to 127 tasks, for more tasks use schedu16.lib.
\note Same order as enumeration #SCH_EnumTaskIds in schedDef.h !!!

*/
extern const TpfTaskHandler SCH_ppfTaskHandlerTable[];


/**  Pointer table to dependency list of each task

This Table has to be defined by application.\n
The schedule library handles up to 127 tasks, for more tasks use schedu16.lib.
\note Same order as enumeration #SCH_EnumTaskIds in schedDef.h !!!
*/

extern const TtaskId* const SCH_pptDependencyListPtrTable[];


/** Priority table

This table saves the priority of each module (1bit for each task).
For each task there is a corresponding bit. For Module number MOD this is bit MOD%8 in Byte MODE/8\n
The application has to define this in schedDef.c.\n
\b size: (number of tasks+7)/8
*/
extern uint8_t SCH_pucPriorityTable[];


/** Saves the state of task [disbale,enable]

For each task there is a corresponding bit. For Module number MOD this is bit MOD%8 in Byte MODE/8\n
If this bit is set, corresponding task will not called.\n
The application has to define this in schedDef.c.\n
\b size: (number of tasks+7)/8

\note After reset all tasks are enabled.\n 
      A disabled task is disabled until it will be enabled or a reset occurs
*/
extern uint8_t SCH_pucDisableTable[];



/** Init state of all tasks [initilised, not initialised]

For each task there is a corresponding bit. For Module number MOD this is bit MOD%8 in Byte MODE/8\n
If this bit is set, corresponding task is initialised.\n
The application has to define this in schedDef.c.\n
\b size: (number of tasks+7)/8

*/
extern uint8_t SCH_pucInitStateTable[];


/******************************************************************************/
/* PUBLIC FUNCTION DECLARATIONS                                               */
/******************************************************************************/

/** handles the task with ID \p tTaskId

This Function may be overwritten by application.\n
\n
The default function just calls the task, disables the task if it's return value is TASK_TERMINATE.
The File scheduXS.c contains examples how to overwrite this function.
An interesting possibility is to implement time measurement overwriting this function.

\return return value of task\n
\b Type: uint8_t
*/
uint8_t _SCH_ucDoTask(TtaskId tTaskId);



#ifdef __cplusplus
}
#endif

#endif

