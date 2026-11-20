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


#ifndef SCHEDEF_H
#define SCHEDEF_H

#ifdef __cplusplus
extern "C" {
#endif

/**************************************************************************************************/
/* INCLUDES                                                                                       */
/**************************************************************************************************/
#include "bsh_stdinc.h"
#include "schedule/schedule.h"

#ifdef SCHEDULE_CONFIG_AUTO
#include LD_STRINGIZE(schedule/VARIANT/schedDef_auto.h)
#else
/*lint -esym(769,_T_SCH_EnumTaskIds*) global enumeration constant not referenced */

/** Enumeration of modules used by application
 *
  \note Enumeration must be in same order than definitions in #SCH_pptDependencyListPtrTable and #SCH_ppfTaskHandlerTable
 */
typedef enum _T_SCH_EnumTaskIds {
#ifdef DBUS2_INCLUDED
    MOD_BAL,            ///< module ID of dbus bus layer
    MOD_DBPL,           ///< module ID of dbus presentation layer
    MOD_DLL,            ///< module ID of dbus data link layer
#endif //DBUS2_INCLUDED
    MOD_TIM,            ///< module ID of timer library
#ifdef CCSS
    MOD_CCSS,           ///< module ID of SystemStates Taskhandler
#endif
    // place here your projects tasks
#ifdef DBUSCAN_INCLUDED
    MOD_DBC,             ///< module ID of task handler for DBusCAN chip
#endif
#ifdef STACK_MONITOR_ENABLED
    MOD_STK,             ///< module ID of task handler for Stack Monitor
#endif
#ifdef CCSS_DOM_REF
    MOD_CCDOM,          ///< module ID of SystemStates Domain Reference implementation. to be replaced with an application specific implementation.
#endif
#ifdef FUNCTIONAL_SAFETY_REGISTER_CHECK_ACTIVE
    MOD_REG,            ///< Functional Safety - register test
#endif
#ifdef FUNCTIONAL_SAFETY_RAM_CHECK_ACTIVE
    MOD_RAM,            ///< Functional Safety - ram test
#endif
#ifdef FUNCTIONAL_SAFETY_ROM_CHECK_ACTIVE
    MOD_ROM,            ///< Functional Safety - rom test
#endif
#ifdef FUNCTIONAL_SAFETY_STACK_CHECK_ACTIVE
    MOD_STACK,          ///< Functional Safety - stack test
#endif
#ifdef FUNCTIONAL_SAFETY_ISR_CHECK_ACTIVE
    MOD_ISR,            ///< Functional Safety - ISR test
#endif
#ifdef FUNCTIONAL_SAFETY_SYS_CLK_CHECK_ACTIVE
    MOD_CLK,            ///< Functional Safety - SYS clock test
#endif
#ifdef FEATURE_FUNCTIONAL_SAFETY_ENABLED
    MOD_FSF,
#endif
    NUMBER_OF_MODULES   ///< number of modules handled by scheduler
} T_SCH_EnumTaskIds;

///< table of tasks, use #SCH_vSetMainStateTable() to set this task state
/* Axivion Next Line MisraC2012-8.11: "Array declaration with unknown size" */
extern const TschMainTaskTable MainTaskTbl[]; /*lint !e526 symbol 'MainTaskTbl' is not defined (false positive for lintplus)*/

#endif /* SCHEDULE_AUTO */

#ifdef __cplusplus
}
#endif

#endif
