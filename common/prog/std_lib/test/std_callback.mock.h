/*******************************************************************************
*  Copyright (c) 2024 BSH Hausgeraete GmbH,
*  Carl-Wery-Str. 34, 81739 Munich, Germany, www.bsh-group.de
*
*  All rights reserved. This program and the accompanying materials
*  are protected by international copyright laws.
*  Please contact copyright holder for licensing information.
*
*******************************************************************************/

#ifndef STD_CALLBACK_MOCK_H
#define STD_CALLBACK_MOCK_H

#include <doctest/doctest.h>
#include <trompeloeil/doctest/trompeloeil.hpp>

namespace std_callback_test
{
    #include "std_lib/std_callback.h"
    // allow second include of std_callback.h in mock namespace
    #undef STD_CALLBACK_H
}



namespace std_callback_m
{
    #include "std_lib/std_callback.h"

    extern bool STDCB_InitCallback_mock;

    class std_callback_mock_class
    {
    public:
        MAKE_MOCK4(STDCB_InitCallback            , int32_t           ( STDCB_Callback *cbPtr, STDCB_CallbackFunction_t funcPtr, void *objPtr, uint32_t eventMask ));
        MAKE_MOCK2(STDCB_AttachCallback          , int32_t           ( STDCB_Callback **cbList, STDCB_Callback *cbPtr ));
        MAKE_MOCK2(STDCB_DetachCallback          , int32_t           ( STDCB_Callback **cbList, const STDCB_Callback *cbPtr ));
        MAKE_MOCK3(STDCB_ExecuteCallbacks        , int32_t           ( const STDCB_Callback *cbPtr, uint32_t eventFlags, int32_t eventData ));
    };

    extern std_callback_mock_class std_callback_mock;
}

using namespace std_callback_m;

#endif  // STD_CALLBACK_MOCK_H
