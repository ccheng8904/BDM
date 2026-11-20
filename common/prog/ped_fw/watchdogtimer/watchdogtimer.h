/*******************************************************************************
 *  Copyright (c) 2017 BSH Hausgeraete GmbH,
 *  Carl-Wery-Str. 34, 81739 Munich, Germany, www.bsh-group.de
 *
 *  All rights reserved. This program and the accompanying materials
 *  are protected by international copyright laws.
 *  Please contact copyright holder for licensing information.
 *
 *******************************************************************************
 *  PROJECT          Watchdogtimer GED Framework (Generic SW)
 *  COMP_ABBREV      WDT
 *  AUTHOR           Manoj Khatri 
 *  CREATED          14.10.2019
 ******************************************************************************/


#ifndef WATCHDOGTIMER_H
#define WATCHDOGTIMER_H

#include "bsh_stdinc.h" 
#include "LibTypes.h"
#include "timerlib.h"

#define DEP_WDT MOD_TIM,   ///< dependencies of library module

extern uint8_t WDT_bHandleTask(void);

static inline uint8_t WDT_handleTask(void)
{
#ifdef WATCHDOG_INCLUDED
    return WDT_bHandleTask();
#else
    return TASK_TERMINATE;
#endif
}




#endif

