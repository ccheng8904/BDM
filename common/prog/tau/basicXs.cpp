/*******************************************************************************
 *   Copyright (c) 2024 BSH Hausgeraete GmbH,
 *   Carl-Wery-Str. 34, 81739 Munich, Germany, www.bsh-group.de
 *
 *   All rights reserved. This program and the accompanying materials
 *   are protected by international copyright laws.
 *   Please contact copyright holder for licensing information.
 *
 ********************************************************************************
 *   PROJECT          Generic SW
 *   MODULE-PREFIX    BASIC
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

#ifndef __NOT_CC__ // no lint please
#include "tau/tau.h"
TAU_NO_MAIN() // defines global variables for Tau
#endif

/**************************************************************************************************/
/* GLOBAL FUNCTION DEFINITIONS                                                                    */
/**************************************************************************************************/
void BASIC_vUsrInitAppl (void)
{
    BASIC_vEnableAllInterrupts();
#ifndef __NOT_CC__ // no lint please
    (void)tau_main(0, NULL); 
#endif
}


void BASIC_vUsrSchedTasks (void)
{
    HWDT_vTrigger(); //no scheduling, trigger WDT only
}


TEST(dymmyTest, dummy)
{
    CHECK_EQ(1,1); // fails as 4 != 5
}