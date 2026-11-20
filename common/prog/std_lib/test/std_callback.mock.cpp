/*******************************************************************************
*  Copyright (c) 2024 BSH Hausgeraete GmbH,
*  Carl-Wery-Str. 34, 81739 Munich, Germany, www.bsh-group.de
*
*  All rights reserved. This program and the accompanying materials
*  are protected by international copyright laws.
*  Please contact copyright holder for licensing information.
*
*******************************************************************************/

#include "std_callback.mock.h"


namespace std_callback_m
{
    std_callback_mock_class std_callback_mock;
    bool STDCB_InitCallback_mock = true;

    int32_t STDCB_InitCallback(struct STDCB_Callback *cbPtr, STDCB_CallbackFunction_t funcPtr, void *objPtr, uint32_t eventMask)
    {
        if(STDCB_InitCallback_mock)
        {
            return std_callback_mock.STDCB_InitCallback(cbPtr, funcPtr, objPtr, eventMask);
        }
        else
        {
            return std_callback_test::STDCB_InitCallback((std_callback_test::STDCB_Callback*)cbPtr, funcPtr, objPtr, eventMask);
        }
    }

    int32_t STDCB_AttachCallback(struct STDCB_Callback **cbList, struct STDCB_Callback *cbPtr)
    {
        return std_callback_mock.STDCB_AttachCallback(cbList, cbPtr);
    }

    int32_t STDCB_DetachCallback(struct STDCB_Callback **cbList, const struct STDCB_Callback *cbPtr)
    {
        return std_callback_mock.STDCB_DetachCallback(cbList, cbPtr);
    }

    int32_t STDCB_ExecuteCallbacks(const struct STDCB_Callback *cbPtr, uint32_t eventFlags, int32_t eventData)
    {
        return std_callback_mock.STDCB_ExecuteCallbacks(cbPtr, eventFlags, eventData);
    }

}

