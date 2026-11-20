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
#include <trompeloeil/doctest/trompeloeil.hpp>
/* Your mocking includes here
   NOTE: The include order is important! lowest level need to be included first.
         Cross include dependencies creates troubles!!*/

using trompeloeil::_;

namespace std_callback_test
{
    #include "std_lib/std_callback.h"

    /************************** local mocks ***********************************
     *  Use macro MOCKI(function-name)(..) to mock internal static interfaces.
     *  Use macro MOCKE(..)(..) to mock interfaces of the unit.
     *  Mocking of static internal interface should be prevented. Use it to break
     *  endless loops and to reduce high complexity.
     */
    class
    {
      // MAKE_MOCK1(XXX_isConvReady      , bool        ( const XXX_Handle *handle ));
    }int_mock;

    class
    {
      // MAKE_MOCK2(XXX_readSample       , uint16_t    ( XXX_Handle *handle, const XXX_Channel *channel ));
    }ext_mock;

    /**************************************************************************/

    // include the implementation of 'std_callback' into the current namespace
    #include "std_lib/std_callback.c"

    TEST_SUITE("std_callback")
    {
        trompeloeil::sequence seq;
        /* do here the common stuff e.g. declarations needed in the following test cases*/

        TEST_CASE("STDCB_InitCallback")
        {
            SUBCASE("Check....")
            {   //int32_t STDCB_InitCallback(struct STDCB_Callback *cbPtr, STDCB_CallbackFunction_t funcPtr, void *objPtr, uint32_t eventMask)
                //int32_t ret = STDCB_InitCallback(cbPtr, funcPtr, objPtr, eventMask);
                CHECK(true);
            }
        }

        TEST_CASE("STDCB_AttachCallback")
        {
            SUBCASE("Check....")
            {   //int32_t STDCB_AttachCallback(struct STDCB_Callback **cbList, struct STDCB_Callback *cbPtr)
                //int32_t ret = STDCB_AttachCallback(cbList, cbPtr);
                CHECK(true);
            }
        }

        TEST_CASE("STDCB_DetachCallback")
        {
            SUBCASE("Check....")
            {   //int32_t STDCB_DetachCallback(struct STDCB_Callback **cbList, const struct STDCB_Callback *cbPtr)
                //int32_t ret = STDCB_DetachCallback(cbList, cbPtr);
                CHECK(true);
            }
        }

        TEST_CASE("STDCB_ExecuteCallbacks")
        {
            SUBCASE("Check....")
            {   //int32_t STDCB_ExecuteCallbacks(const struct STDCB_Callback *cbPtr, uint32_t eventFlags, int32_t eventData)
                //int32_t ret = STDCB_ExecuteCallbacks(cbPtr, eventFlags, eventData);
                CHECK(true);
            }
        }

        TEST_CASE("STDCB_EnableEvents")
        {
            SUBCASE("Check....")
            {   //void STDCB_EnableEvents(struct STDCB_Callback *cbPtr, uint32_t eventMaskToEnable)
                //STDCB_EnableEvents(cbPtr, eventMaskToEnable);
                CHECK(true);
            }
        }

        TEST_CASE("STDCB_DisableEvents")
        {
            SUBCASE("Check....")
            {   //void STDCB_DisableEvents(struct STDCB_Callback *cbPtr, uint32_t eventMaskToDisable)
                //STDCB_DisableEvents(cbPtr, eventMaskToDisable);
                CHECK(true);
            }
        }

    }
}

