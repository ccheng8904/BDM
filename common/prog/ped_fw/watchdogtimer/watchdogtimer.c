/*******************************************************************************
 *  Copyright (c) 2017 BSH Hausgeraete GmbH,
 *  Carl-Wery-Str. 34, 81739 Munich, Germany, www.bsh-group.de
 *
 *  All rights reserved. This program and the accompanying materials
 *  are protected by international copyright laws.
 *  Please contact copyright holder for licensing information.
 *
 *******************************************************************************
 *  PROJECT          Watchdogtimer  GED Framework (Generic SW)
 *  COMP_ABBREV      WDT
 *  AUTHOR           Manoj Khatri 
 *  CREATED          14.10.2019
 ******************************************************************************/

/******************************************************************************/
/* DOCUMENTATION                                                              */
/******************************************************************************/
/** \file
 *
 *  \brief  Implementation of watchdogtimer
 *
 */


/**************************************************************************************************/
/* INCLUDES                                                                                       */
/**************************************************************************************************/
#include "bsh_stdinc.h"
#include <stdint.h>
#include "watchdogtimer.h"
#include "hwdt.h"
#include "LibDefines.h"


/* Global Functions*/

uint8_t WDT_bHandleTask(void)
{
    HWDT_vTrigger();
    return TASK_INITIALISED; //always true
}


