/*******************************************************************************
*  Copyright (c) 2024 BSH Hausgeraete GmbH,
*  Carl-Wery-Str. 34, 81739 Munich, Germany, www.bsh-group.de
*
*  All rights reserved. This program and the accompanying materials
*  are protected by international copyright laws.
*  Please contact copyright holder for licensing information.
*
*******************************************************************************/

#include <doctest/doctest.h>
/* include your required mock headers here. Lowest SW layer first!*/
<$priv_mock_inc>

using trompeloeil::_;

namespace <$unit_name>_test
{   

    /*----- START: mocking own internal (static) and external interface functions (optional!) *****/
    /* Note: you may delete this block (start to end) if no internal or external mocks are needed */
    #include "../<$unit_header>"
    /* To mock internal functions the following macros are used in the code under test:
     * MOCKI(function-name)(paramA, paramB,...)
     * The parameters are the same as in the original function.
     * For own external functions the macro MOCKE(function-name)(paramA, paramB,...) is used.
     */

    class
    {/* no. params function name                   return type         parameter list */
<$int_mock_example>
    }int_mock;

    class
    {/* no. params function name                   return type         parameter list */
<$ext_mock_example>
    }ext_mock;

    /*---- END: mocking own functions ------------------------------------------------------------*/




    #include "../<$unit_name>.c"

    void test_setup(void)
    {
        /* Optional interface 
           Do here here everything to prepare the tests.
           E.g. common variable initialization.
           Note: The final test result shall be independent of the test order
                 This is why you should prepare each test case.
        */
    }   


    TEST_SUITE("<$unit_name>") // the test suite for the unit <$unit_name>
    {
        // here you can declare variables and constants you need in all test cases within this test suite.

<$TEST_CASES>

    } // TEST_SUITE("<$unit_name>")
} // namespace <$unit_name>_test

