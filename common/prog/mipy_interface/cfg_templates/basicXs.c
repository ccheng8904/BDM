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

#include "schedule/schedDef.h"


#include "hsup.h"

#include "mipy_interface/mipy.h"


void BASIC_vUsrInitAppl (void)
{
   /* Do any pre initialization here: ...*/
    SCH_vSetMainStateTable(MainTaskTbl); // register tasks with allotted priorities
   BASIC_vEnableAllInterrupts();

   MIPY_bInitTask();

}


void BASIC_vUsrSchedTasks (void)
{   /** delays are required to give mor CPU time to ISRs */ 
    (void)TIM_bHandleTask16();
    (void)MIPY_bHandleTask();
    HWDT_vTrigger();
}
