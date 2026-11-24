/*******************************************************************************
*   Copyright (c) 2017 BSH Hausgeraete GmbH,
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


#ifndef <$UNIT_NAME>_RD_STUB_H
#define <$UNIT_NAME>_RD_STUB_H


/******************************************************************************/
/* DOCUMENTATION                                                              */
/******************************************************************************/
/** \file
*
*   \brief    Function call redirection for dynamic SW unit test
*             for all public functions defined in <$UnitName>.c.
*
*/


/******************************************************************************/
/* PUBLIC STUB FUNCTION DECLARATIONS resp.                                    */
/* REDIRECTION OF FUNCTION CALLS TO STUB CALLS                                */
/******************************************************************************/
#if ! defined (<$UNIT_NAME>_UNIT)

    // Isolate units that are under test, i.e.
    //  1)  if any other (='foreign') translation is under test and if this unit shall be stubbed: prevent the other (='foreign') unit from using (=calling) this unit by stubbing this unit's interface
    #if (defined (UNIT_UNDER_TEST)) && (defined (FOR_UNIT_UNDER_TEST_STUB_<$UNIT_NAME>))
        #define APPLY_STUBBING_<$UNIT_NAME>
    #endif
    // 2)  if this unit is under test: prevent this unit from being used (=called) by other (='foreign') translation units by stubbing this unit's interface
    //  with the exception the stubbing calls to this unit is exceptionally suspended (to allow SW-SW-integration tests)
    #if (defined (<$UNIT_NAME>_UNDER_TEST)) && (! defined (SUSPEND_STUBBING_CALLS_TO_<$UNIT_NAME>))
        #define APPLY_STUBBING_<$UNIT_NAME>
    #endif

    #if defined (APPLY_STUBBING_<$UNIT_NAME>)
        <$StubRedirection>
    #endif

#endif


#endif // <$UNIT_NAME>_STUB_H
