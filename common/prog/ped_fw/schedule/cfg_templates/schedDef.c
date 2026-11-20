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
 *  \brief    Application specific definitions and declarations for scheduler
 *
 */


/**************************************************************************************************/
/* INCLUDES                                                                                       */
/**************************************************************************************************/
#include "bsh_stdinc.h"
#include "schedule.h"
#include "schedDef.h"
#include "schpriv.h"
#include "utility/utility.h"

#ifdef DBUS2_INCLUDED
#include "dbus/bal.h"
#include "dbus/dbuspresentation.h"
#include "dbusdll.h"
#endif //DBUS2_INCLUDED

#include "timer/timerlib.h"
#ifdef CCSS
#include "CCSS_SyStTask.h"
#endif
#ifdef CCSS_DOM_REF
#include "CCDOM_DomainRefImpl.h"
#endif
#ifdef FEATURE_FUNCTIONAL_SAFETY_ENABLED
#include "FuncSafe/fSafe.h"      // for handle task's
#include "FuncSafe/fsafeXs.h"    // needed for DEP_xxx
#endif
#ifdef STACK_MONITOR_ENABLED
#include "stack_monitor/stack_monitor.h"
#endif
#ifdef DBUSCAN_INCLUDED
#include "devices/dbuscan/dbuscan_drv.h"
#endif

/**************************************************************************************************/
/* LOCAL DATA                                                                                     */
/**************************************************************************************************/
// Note: MAKE SURE, that there are no circular dependencies !!!!

/* 9034: MISRA 2012 Rule 10.3, required "Expression assigned to a narrower or different essential type"
 * 641:  Converting enum 'T_SCH_EnumTaskIds' to 'int
 * It is for sure that enum fits into uchar, should be solved into unit headers
 */
/*lint -save -e9034 -e641 */
#ifdef DBUS2_INCLUDED
static const TtaskId BAL_ptDep[]  = {DEPLIB_BAL  SCH_END_MARK};
static const TtaskId DBPL_ptDep[] = {DEPLIB_DBPL SCH_END_MARK};
static const TtaskId DLL_ptDep[]  = {DEPLIB_DLL  SCH_END_MARK};
#endif //DBUS2_INCLUDED
static const TtaskId TIM_ptDep[]  = {DEP_TIM     SCH_END_MARK};
#ifdef DBUSCAN_INCLUDED
static const TtaskId DBC_ptDep[]  = {DEP_DBC     SCH_END_MARK};
#endif
#ifdef STACK_MONITOR_ENABLED
static const TtaskId STK_ptDep[]  = {DEP_STK     SCH_END_MARK};
#endif
#ifdef CCSS
static const TtaskId CCSS_ptDep[] = {DEP_CCSS    SCH_END_MARK};
#endif
#ifdef CCSS_DOM_REF
static const TtaskId CCDOM_ptDep[] ={DEP_CCDOM   SCH_END_MARK};
#endif
#if !defined(TH_FSCOM) && defined(FEATURE_FUNCTIONAL_SAFETY_ENABLED)
#ifdef FUNCTIONAL_SAFETY_REGISTER_CHECK_ACTIVE
static const TtaskId FSF_REG_ptDep[] = { DEP_REG SCH_END_MARK };       //!< FSF REG task dependency structure
#endif
#ifdef FUNCTIONAL_SAFETY_RAM_CHECK_ACTIVE
static const TtaskId FSF_RAM_ptDep[] = { DEP_FSF SCH_END_MARK };       ///< dependency list of module #MOD_RAM
#endif
#ifdef FUNCTIONAL_SAFETY_ROM_CHECK_ACTIVE
static const TtaskId FSF_ROM_ptDep[] = { DEP_ROM SCH_END_MARK };       ///< dependency list of module #MOD_ROM
#endif
#ifdef FUNCTIONAL_SAFETY_STACK_CHECK_ACTIVE
static const TtaskId FSF_STACK_ptDep[] = { DEP_STACK SCH_END_MARK };   ///< dependency list of module #MOD_STACK
#endif
#ifdef FUNCTIONAL_SAFETY_ISR_CHECK_ACTIVE
static const TtaskId FSF_ISR_ptDep[] = { DEP_ISR SCH_END_MARK };       ///< dependency list of module #MOD_ISR
#endif
#ifdef FUNCTIONAL_SAFETY_SYS_CLK_CHECK_ACTIVE
static const TtaskId FSF_FSCLKptDep[] = { DEP_FSCLK SCH_END_MARK };
#endif
#endif
#ifdef FEATURE_FUNCTIONAL_SAFETY_ENABLED
static const TtaskId FSF_ptDep[] = { DEP_FSF SCH_END_MARK };
#endif
/*lint -restore -e9034 -e641 */


// Note: same order as enumeration #T_SCH_EnumTaskIds in schedDef.h !!!
const TtaskId* const SCH_pptDependencyListPtrTable[NUMBER_OF_MODULES] =
{
#ifdef DBUS2_INCLUDED
    BAL_ptDep,
    DBPL_ptDep,
    DLL_ptDep,
#endif //DBUS2_INCLUDED
    TIM_ptDep,
#ifdef CCSS
    CCSS_ptDep,
#endif
    // place here your task dependencies
#ifdef DBUSCAN_INCLUDED
    DBC_ptDep,
#endif
#ifdef STACK_MONITOR_ENABLED
    STK_ptDep,
#endif
#ifdef CCSS_DOM_REF
    CCDOM_ptDep,
#endif
#ifdef FUNCTIONAL_SAFETY_REGISTER_CHECK_ACTIVE
    FSF_REG_ptDep,
#endif
#ifdef FUNCTIONAL_SAFETY_RAM_CHECK_ACTIVE
    FSF_RAM_ptDep,
#endif
#ifdef FUNCTIONAL_SAFETY_ROM_CHECK_ACTIVE
    FSF_ROM_ptDep,
#endif
#ifdef FUNCTIONAL_SAFETY_STACK_CHECK_ACTIVE
    FSF_STACK_ptDep,
#endif
#ifdef FUNCTIONAL_SAFETY_ISR_CHECK_ACTIVE
    FSF_ISR_ptDep,
#endif
#ifdef FUNCTIONAL_SAFETY_SYS_CLK_CHECK_ACTIVE
    FSF_FSCLKptDep,
#endif
#ifdef FEATURE_FUNCTIONAL_SAFETY_ENABLED
    FSF_ptDep,
#endif
};

// Note: same order as enumeration #T_SCH_EnumTaskIds in schedDef.h !!!
const TpfTaskHandler SCH_ppfTaskHandlerTable[] =
{
#ifdef DBUS2_INCLUDED
    BAL_HandleTask,
    DBPL_HandleTask,
    DLL_HandleTask,
#endif //DBUS2_INCLUDED
    TIM_bHandleTask16,  /* TIM_bHandleTask if no 16Bit timer used */
#ifdef CCSS
    CCSS_RunSyStTask,
#endif
    // place here your project task handler
#ifdef DBUSCAN_INCLUDED
    DBCDRV_HandleTask,
#endif
#ifdef STACK_MONITOR_ENABLED
    STK_HandleTask,
#endif
#ifdef CCSS_DOM_REF
    CCDOM_RunDomRefTask,
#endif
#ifdef FUNCTIONAL_SAFETY_REGISTER_CHECK_ACTIVE
    FSF_ucRegHandleTask,
#endif
#ifdef FUNCTIONAL_SAFETY_RAM_CHECK_ACTIVE
    FSF_ucRamHandleTask,
#endif
#ifdef FUNCTIONAL_SAFETY_ROM_CHECK_ACTIVE
    FSF_ucRomHandleTask,
#endif
#ifdef FUNCTIONAL_SAFETY_STACK_CHECK_ACTIVE
    FSF_ucStackHandleTask,
#endif
#ifdef FUNCTIONAL_SAFETY_ISR_CHECK_ACTIVE
    FSF_ucIsrHandleTask,
#endif
#ifdef FUNCTIONAL_SAFETY_SYS_CLK_CHECK_ACTIVE
    FSCLK_u8HandleTask,
#endif
#ifdef FEATURE_FUNCTIONAL_SAFETY_ENABLED
    FSF_ucComHandleTask,
#endif
};

// Taskhandler for main State
#if ((! defined (UNIT_TEST)) || (defined (UNIT_TEST) && defined (SCHEDULE_APPLICATION_TASKS))) && !defined(VARIANT_TESTSERVER)
//lint -esym(765,MainTaskTbl) external symbol 'MainTaskTbl' could be made static [MISRA 2012 Rule 8.7, advisory], it cannot
const TschMainTaskTable MainTaskTbl[] =
{
#ifdef DBUS2_INCLUDED
    {(TtaskId)MOD_BAL,      SCH_PRIOR_HI},
    {(TtaskId)MOD_DBPL,     SCH_PRIOR_HI},
    {(TtaskId)MOD_DLL,      SCH_PRIOR_LO},
#endif //DBUS2_INCLUDED
    {(TtaskId)MOD_TIM,      SCH_PRIOR_HI},
#ifdef CCSS
    {(TtaskId)MOD_CCSS,     SCH_PRIOR_LO},
#endif
    // place here your project task priorities
#ifdef DBUSCAN_INCLUDED
    {(TtaskId)MOD_DBC,      SCH_PRIOR_LO},
#endif
#ifdef STACK_MONITOR_ENABLED
    {(TtaskId)MOD_STK,      SCH_PRIOR_LO},
#endif
#ifdef CCSS_DOM_REF
    {(TtaskId)MOD_CCDOM,    SCH_PRIOR_LO},
#endif
#ifdef FUNCTIONAL_SAFETY_REGISTER_CHECK_ACTIVE
    {(TtaskId)MOD_REG,      SCH_PRIOR_HI},          ///< Functional Safety - register test
#endif
#ifdef FUNCTIONAL_SAFETY_RAM_CHECK_ACTIVE
    {(TtaskId)MOD_RAM,      SCH_PRIOR_HI},          ///< Functional Safety - ram test
#endif
#ifdef FUNCTIONAL_SAFETY_ROM_CHECK_ACTIVE
    {(TtaskId)MOD_ROM,      SCH_PRIOR_HI},          ///< Functional Safety - rom test
#endif
#ifdef FUNCTIONAL_SAFETY_STACK_CHECK_ACTIVE
    {(TtaskId)MOD_STACK,    SCH_PRIOR_HI},          ///< Functional Safety - stack test
#endif
#ifdef FUNCTIONAL_SAFETY_ISR_CHECK_ACTIVE
    {(TtaskId)MOD_ISR,      SCH_PRIOR_HI},
#endif
#ifdef FUNCTIONAL_SAFETY_SYS_CLK_CHECK_ACTIVE
    {(TtaskId)MOD_CLK,      SCH_PRIOR_HI},
#endif
#ifdef FEATURE_FUNCTIONAL_SAFETY_ENABLED
    {(TtaskId)MOD_FSF,      SCH_PRIOR_HI},
#endif
    {SCH_END_MARK, SCH_PRIOR_HI}
};

#else
// attention: don't place project tasks in this table because this table is only used for unit testing i.e. it only schedules timer and DBUS tasks
const TschMainTaskTable MainTaskTbl[] = {
#ifdef DBUS2_INCLUDED
    {(TtaskId)MOD_BAL,      SCH_PRIOR_HI},
    {(TtaskId)MOD_DBPL,     SCH_PRIOR_HI},
    {(TtaskId)MOD_DLL,      SCH_PRIOR_LO},
#endif //DBUS2_INCLUDED
    {(TtaskId)MOD_TIM,      SCH_PRIOR_HI},
    {SCH_END_MARK, SCH_PRIOR_HI}
};
#endif


// Fixed definitions, external defined here but used in library
uint8_t SCH_pucPriorityTable[((TtaskId)NUMBER_OF_MODULES  +7U) /8U];
uint8_t SCH_pucDisableTable[((TtaskId)NUMBER_OF_MODULES   +7U) /8U];
uint8_t SCH_pucInitStateTable[((TtaskId)NUMBER_OF_MODULES +7U) /8U];

