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
 *  COMP_ABBREV      ---
 ******************************************************************************/

/******************************************************************************/
/* DOCUMENTATION                                                              */
/******************************************************************************/
/** \file main.c
    Generic Main routine to be used by all projects.
*/

/******************************************************************************/
/* INCLUDES                                                                   */
/******************************************************************************/
#include "bsh_stdinc.h"
#include "basic.h"
/*lint -esym(9046,main) 'main' is typographically ambiguous with respect to 'Main' when ignoring case [MISRA 2012 Directive 4.5, advisory] */

/**
   Generic main function
\remarks
   - This function should not be modified in it architecture
   - Functions calls BASIC_xxx can be optionally implemented in case
     the default implementation is not suitable.
   - This function calls the three basic interfaces provided by the framework in an appropriate order:
     -# BASIC_vConfigPlatform()
     -# BASIC_vUsrInitAppl()
     -# BASIC_vUsrSchedTasks()
*/
int main(void) //lint !e970 main must return int
{
  #ifndef NO_PLATFORM_INIT
    BASIC_vConfigPlatform();        /* Configure Platform */
  #endif
    BASIC_vUsrInitAppl();           /* Initialize User Application */

    for (;;)
    {
        BASIC_vUsrSchedTasks();     /* Schedule User Tasks */
    }
}
