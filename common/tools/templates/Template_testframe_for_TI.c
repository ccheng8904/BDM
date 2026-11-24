/*******************************************************************************
*   Copyright (c) 2016 BSH Hausgeraete GmbH,
*   Carl-Wery-Str. 34, 81739 Munich, Germany, www.bsh-group.de
*
*   All rights reserved. This program and the accompanying materials
*   are protected by international copyright laws.
*   Please contact copyright holder for licensing information.
*
********************************************************************************
*   PROJECT          <$PROJECT_NAME>
*   COMP_ABBREV      <$UNIT_PREFIX>
*******************************************************************************/


/******************************************************************************/
/* DOCUMENTATION                                                              */
/******************************************************************************/
/** \file
*
*   \brief    Testframe for <$UnitName>.c.
*
*   \details  This testframe contains the reduced test instrumentation
*             for dynamic SW unit test. It includes <$UnitName>.c
*             which contains the test instrumentation as it was necessary
*             before introducing testframes.
*             This translation unit is meant to be used
*             instead of <$UnitName>.c
*             if building the project for unit testing.
*/


/******************************************************************************/
/* include MkDefines is absolutely necessary for diverse non-compiler tools   */
/* esp. for static (PC-lint, Axivion) and dynamic (Axivion) SW unit test      */
/******************************************************************************/
#include "bsh_stdinc.h"


/******************************************************************************/
/* TEST INSTRUMENTATION 1/3                                                   */
/******************************************************************************/
#if (defined (UNIT_TEST)) && (! defined (DEACTIVATE_TEST_INSTRUMENTATION_FOR_<$UNIT_NAME>))

    #define <$UNIT_NAME>_UNIT

    #if defined (<$UNIT_NAME>_UNDER_TEST)
        #define UNIT_UNDER_TEST // this unit is under test
    #endif

#endif

/******************************************************************************/
/* UNIT-SPECIFIC CONFIGURATION: FOR EXAMPLE SUPPRESS STUB-REDIRECTION         */
/******************************************************************************/
#include "<$UnitName>_ut_config.h"

/******************************************************************************/
/* REDIRECTION OF FUNCTIONS THAT ARE INCLUDED TO STUB-FUNCTIONS               */
/******************************************************************************/
#include "All_Stub_rd.h"

/******************************************************************************/
/* INCLUDE TESTFRAMED UNIT                                                    */
/******************************************************************************/
#if (defined (UNIT_TEST)) && (! defined (NO_CFILE_INCLUDE_IN_TESTFRAME_FOR_<$UNIT_NAME>))
    #include "<$UnitName>.c"
#else
    #if (defined (UNIT_TEST))
        #include "<$UnitName>_Stub.c"
        #if defined (UNIT_UNDER_TEST)
            #include "<$UnitName>_IntStub.c"
        #endif
    #endif
#endif






