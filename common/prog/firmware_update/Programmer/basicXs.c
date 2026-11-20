/*******************************************************************************
*   Copyright (c) 2016 BSH Hausgeraete GmbH,
*   Carl-Wery-Str. 34, 81739 Munich, Germany, www.bsh-group.de
*
*   All rights reserved. This program and the accompanying materials
*   are protected by international copyright laws.
*   Please contact copyright holder for licensing information.
*
********************************************************************************
*   PROJECT          Generic SW / Firmware Update
*******************************************************************************/

/******************************************************************************/
/* DOCUMENTATION                                                              */
/******************************************************************************/
/** \file
    \brief Implementation of application specific functions declared in basic.h
*/

/******************************************************************************/
/* INCLUDES                                                                   */
/******************************************************************************/
#include "bsh_stdinc.h"
#include "basic/basic.h"
#include "hwdt.h"
#include "dbus/bal.h"
#include "dbus/dbuspresentation.h"
#include "dbusdll.h"
#include "timer/timerlib.h"
#include "map_mem_access_interface.h"
#include "hsup.h"
#include "firmware_update/mem_drv/mem_drv.h"
#include "firmware_update/mal/mal_command.h"
#include "firmware_update/Programmer/prog_user_task.h"

#ifdef STACK_MONITOR_ENABLED
#include "stack_monitor/stack_monitor.h"
#endif

void BASIC_vUsrInitAppl (void)
{
   /* Do any pre initialization here: ...*/
   BASIC_vEnableAllInterrupts();

   #ifdef UNIT_TEST
       TM_vInit();
   #endif
   MAL_CMD_init();
   MAP_MemAccess_Init();
}


void BASIC_vUsrSchedTasks (void)
{   /** delays are required to give more CPU time to ISRs */ 
    (void)TIM_bHandleTask16();
    (void)DLL_HandleTask();
    HSUP_vDelay(0xFFU);
    (void)BAL_HandleTask();
    HSUP_vDelay(0xFFU);
    (void)DBPL_HandleTask();
    HSUP_vDelay(0xFFU);
    MEMDRV_schedule();
    HWDT_vTrigger();
    (void)PRG_bUserHandleTask(); // Handle function for user specific code
#ifdef STACK_MONITOR_ENABLED
    (void)STK_HandleTask();
#endif
}
