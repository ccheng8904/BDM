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


/******************************************************************************/
/* DOCUMENTATION                                                              */
/******************************************************************************/
/** \file
 *
 *  \brief Public application specific definitions and declarations for scheduler
 *
 */


#ifndef SCHEDEF_AUTO_H
#define SCHEDEF_AUTO_H

#ifdef __cplusplus
extern "C" {
#endif

/******************************************************************************/
/* INCLUDES                                                                   */
/******************************************************************************/
#include "bsh_stdinc.h"
#include "schedule/schedule.h"


/** Enumeration of modules used by application
  *
  \note Enumeration must be in same order than definitions in #SCH_pptDependencyListPtrTable and #SCH_ppfTaskHandlerTable
 */
typedef enum _T_SCH_EnumTaskIds
{
<$foreach(var,$(scheduler_tasks),macro("    MOD_$1,\n",$(var.name)))>
    /* USER CODE BEGIN DependencyList */
    
    /* USER CODE END DependencyList */
    NUMBER_OF_MODULES   ///< number of modules handled by scheduler
} T_SCH_EnumTaskIds;

///< table of tasks, use #SCH_vSetMainStateTable() to set this task state
extern const TschMainTaskTable MainTaskTbl[];

/* USER CODE BEGIN UserTaskTableDeclarations */

/* USER CODE END */


#ifdef __cplusplus
}
#endif

#endif
