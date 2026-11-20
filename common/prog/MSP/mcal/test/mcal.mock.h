/*******************************************************************************
*  Copyright (c) 2024 BSH Hausgeraete GmbH,
*  Carl-Wery-Str. 34, 81739 Munich, Germany, www.bsh-group.de
*
*  All rights reserved. This program and the accompanying materials
*  are protected by international copyright laws.
*  Please contact copyright holder for licensing information.
*
*******************************************************************************/

#ifndef MCAL_MOCK_H
#define MCAL_MOCK_H

#include <doctest/doctest.h>
#include <trompeloeil/doctest/trompeloeil.hpp>

namespace mcal_m
{
    #include "mcal/mcal.h"

    class mcal_mock_class
    {
    public:
        MAKE_MOCK1(MCAL_checkReady               , MCAL_Error        ( MCAL_Status status ));
        MAKE_MOCK1(MCAL_getEventData_uint8       , uint8_t           ( const MCAL_EventResponse *eventResponse ));
        MAKE_MOCK1(MCAL_getEventData_uint16      , uint16_t          ( const MCAL_EventResponse *eventResponse ));
        MAKE_MOCK1(MCAL_getEventData_uint32      , uint32_t          ( const MCAL_EventResponse *eventResponse ));
        MAKE_MOCK1(MCAL_getEventData_int8        , int8_t            ( const MCAL_EventResponse *eventResponse ));
        MAKE_MOCK1(MCAL_getEventData_int16       , int16_t           ( const MCAL_EventResponse *eventResponse ));
        MAKE_MOCK1(MCAL_getEventData_int32       , int32_t           ( const MCAL_EventResponse *eventResponse ));
        MAKE_MOCK1(MCAL_getEventData_error       , MCAL_Error        ( const MCAL_EventResponse *eventResponse ));
        MAKE_MOCK3(MCAL_initCallback             , void              ( MCAL_Callback_t *callback, MCAL_CallbackFunction_t callbackFunction, void *objPtr ));
        MAKE_MOCK4(MCAL_invokeCallbacks          , void              ( const MCAL_Callback_t *callback, MCAL_Module eventSource, MCAL_Event eventType, const MCAL_EventData *eventData ));
        MAKE_MOCK4(MCAL_invokeChannelCallbacks   , void              ( const MCAL_Callback_t *callback, MCAL_Module eventSource, uint8_t channelNum, const MCAL_EventData *eventData ));
        MAKE_MOCK3(MCAL_enableEvent              , void              ( MCAL_Callback_t **cbList, MCAL_Callback_t *callback, MCAL_Event event ));
        MAKE_MOCK3(MCAL_enableChannelEvent       , void              ( MCAL_Callback_t **cbList, MCAL_Callback_t *callback, uint8_t channelNum ));
        MAKE_MOCK2(MCAL_disableEvent             , void              ( MCAL_Callback_t *callback, MCAL_Event event ));
        MAKE_MOCK2(MCAL_disableChannelEvent      , void              ( MCAL_Callback_t *callback, uint8_t channelNum ));
        MAKE_MOCK3(MCAL_executeCallbacks         , int32_t           ( const STDCB_Callback *callback, MCAL_Event event, int32_t eventData ));
        MAKE_MOCK3(MCAL_executeChannelCallbacks  , int32_t           ( const STDCB_Callback *callback, uint8_t channelNum, int32_t eventData ));
    };

    extern mcal_mock_class mcal_mock;
}

using namespace mcal_m;

#endif  // MCAL_MOCK_H
