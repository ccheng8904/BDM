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

#ifndef SCHEDULE_H
#define SCHEDULE_H

#ifdef __cplusplus
extern "C" {
#endif


/******************************************************************************/
/* DOCUMENTATION                                                              */
/******************************************************************************/
/** \file
 *
 *  \brief    Public definitions and declarations of scheduler library
 *
 *  \details \section intro 1. Introduction
 *   The job of the scheduler is to call the task handlers of each module.
 *   The scheduler replaces the call of task handlers in main loop. Using the scheduler the modules are not responsible for run-up dependencies.
 *   Before a task handler is called the first time after run-up the scheduler make sure that all used modules are initialized.
 *   The scheduler is able to handle several task tables. Each task to schedule is an entry in this table and will be called frequently from scheduler.
 *   This tables may be exchanged on runtime to handle different states (e.g. service mode, normal mode, error state).
 *   Each task has three attributes:\n
 *   - Mode: enabled/disabled
 *   - State: initialized/ not initialized
 *   - Priority: High/ Low
 *   
 *   The state and priority may be changed dynamically.
 *   The state will be set to initialised once a task handler returns TASK_INITIALISED or TASK_TERMINATE.
 *   
 *   
 *   Each module which has to be called from scheduler has to define its dependencies. When there are no dependencies (see example LED) an empty define has to be placed in the module header.
 *   A module with module token XXX depend on timer (token TIM) has to define the following dependency in the header of module:
 *   #define DEP_XXX MOD_TIM,
 *   
 *   
 *   \section depend 2. Dependencies
 *   Each module which will be scheduled has an unambiguous identifier MOD_XXX.
 *   This identifiers are not known for the modules they will be defined in the enumeration in shedDef.h. 
 *   The schedule dependencies are not the same than include dependencies. Just dependencies of modules with task handlers have to be defined.
 *   Each module has exactly one dependency definition in its header. When a module has no definition of dependencies temporary this definition can be placed in schedDef.c
 *   
 *   \note   Care should be taken with placement of comma. A not empty definition must terminated with a comma.
 *   
 *   \b Examples:
 *   \code
 *   #define DEP_PHYP   MOD_TIM,
 *   #define DEP_LED
 *   #define DEP_DBPL   MOD_DLL, MOD_TIM,
 *   \endcode
 *   
 *   \note To use the feature of overlapping memory (compiler optimizing), the dependencies shall be sorted alphabetically.\n
 *   Extra care should be taken with circular dependencies which cannot be handled by the scheduler.
 */


/******************************************************************************/
/* INCLUDES                                                                   */
/******************************************************************************/
#include "bsh_stdinc.h"
#include <stdbool.h>
#include <stdint.h>
#include "LibTypes.h"



/******************************************************************************/
/* PUBLIC DEFINITIONS                                                         */
/******************************************************************************/


#define SCH_PRIOR_HI 1 ///< For tasks with high priority, such tasks are called each call of #SCH_bHandleTask()
#define SCH_PRIOR_LO 0 ///< For tasks with low priority, each call of #SCH_bHandleTask() one of them is called

#define SCH_NO_TIMER_TASK_TABLE  ((TschTimerTaskTable*)0)///< zero pointer from type #TschTimerTaskTable


#ifdef schedu16
#define SCH_END_MARK ((TtaskId)0x7FFFU) ///< termination of dependency array

#else
#define SCH_END_MARK ((TtaskId)0x7FU) ///< termination of dependency array

#endif




/******************************************************************************/
/* PUBLIC TYPE DEFINITIONS                                                    */
/******************************************************************************/
typedef uint8_t (*TpfTaskHandler)(void);///<  type of pointer to task handler, which returns status of corresponding module

#ifdef schedu16
typedef uint16_t TtaskId;///<type for task ID [0..32767]

#else
typedef uint8_t TtaskId;///<type for task ID [0..127]

#endif

/** \struct _TschMainTaskTable

Each system shall define at least one main task table (an array of this type). This table contains for each task to be called one entry.
*/
typedef struct _TschMainTaskTable
{
#ifdef schedu16
    const TbitField  TaskId:15; ///< ID of taskhandler to be called [0..2047]
#else
    const TbitField  TaskId:7;  ///< ID of taskhandler to be called [0..127]
#endif
    const TbitField  Priority:1;///< priority of taskhandler [#SCH_PRIOR_HI, #SCH_PRIOR_LO]
} TschMainTaskTable;///<Type for main task table(s)


/******************************************************************************/
/* GLOBAL FUNCTION DECLARATIONS                                               */
/******************************************************************************/
/** Handles the main task table

All high and one low priority task will be called. The next call all high and the next low priority task is called.\n
This function shall be called in endless main loop.

\return reserved\n
 \b Type: bool
*/
bool SCH_bHandleTask(void);


/** Sets the main state table
\param ptMainTbl pointer to main task table\n
   \b Type: TschMainTaskTable*\n
\return None

\note This function should be called from main, outside call hierarchy of SCH_bHandleTask()
*/
void SCH_vSetMainStateTable(const TschMainTaskTable* ptMainTbl);


/** Disable/Enable a task

\param tTaskId Id of task to enable/disbale\n
   \b Type: TtaskId\n
\param bOnOff specify if task has to switch on or off\n
   \b Type: bool\n
   \b Range: [FALSE=OFF, all other =ON]

\note  A Module which was not initialised before it was disabled will be never initialised.
       Disabling a module will \b not reset the "initialised-Bit", so scheduler will \b not stop calling modules which are depend on this module.
*/
void SCH_vSwitchModule(TtaskId tTaskId, bool bOnOff);



/** Sets the priority of a module

\param tTaskId Id of task to enable/disbale\n
   \b Type: TtaskId\n
\param bPriority specify the priority of task with ID tTaskId\n
   \b Type: bool\n
   \b Range: [SCH_PRIOR_LO=low priority, SCH_PRIOR_HIGH (and all others) = high priority]
*/
void SCH_vSetPriority(TtaskId tTaskId, bool bPriority);


/** Gets the init status of a module

\param tTaskId Id of task \n
   \b Type: TtaskId\n
\return reserved\n
   \b Type: bool
*/
bool SCH_bGetInitModuleStatus(TtaskId tTaskId);


/** Returns TRUE if all modules in current task table are initialized, otherwise FALSE

\return reserved\n
   \b Type: bool
*/
bool SCH_bGetInitMainTaskTableStatus(void);

#ifdef __cplusplus
}
#endif

#endif

