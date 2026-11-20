/*******************************************************************************
*  Copyright (c) 2024 BSH Hausgeraete GmbH,
*  Carl-Wery-Str. 34, 81739 Munich, Germany, www.bsh-group.de
*
*  All rights reserved. This program and the accompanying materials
*  are protected by international copyright laws.
*  Please contact copyright holder for licensing information.
*
*******************************************************************************/

#include "mcal.mock.h"

namespace mcal_m
{
    mcal_mock_class mcal_mock;

    enum MCAL_Error MCAL_checkReady(enum MCAL_Status status)
    {
        return mcal_mock.MCAL_checkReady(status);
    }

    uint8_t MCAL_getEventData_uint8(const struct MCAL_EventResponse *eventResponse)
    {
        return mcal_mock.MCAL_getEventData_uint8(eventResponse);
    }

    uint16_t MCAL_getEventData_uint16(const struct MCAL_EventResponse *eventResponse)
    {
        return mcal_mock.MCAL_getEventData_uint16(eventResponse);
    }

    uint32_t MCAL_getEventData_uint32(const struct MCAL_EventResponse *eventResponse)
    {
        return mcal_mock.MCAL_getEventData_uint32(eventResponse);
    }

    int8_t MCAL_getEventData_int8(const struct MCAL_EventResponse *eventResponse)
    {
        return mcal_mock.MCAL_getEventData_int8(eventResponse);
    }

    int16_t MCAL_getEventData_int16(const struct MCAL_EventResponse *eventResponse)
    {
        return mcal_mock.MCAL_getEventData_int16(eventResponse);
    }

    int32_t MCAL_getEventData_int32(const struct MCAL_EventResponse *eventResponse)
    {
        return mcal_mock.MCAL_getEventData_int32(eventResponse);
    }

    enum MCAL_Error MCAL_getEventData_error(const struct MCAL_EventResponse *eventResponse)
    {
        return mcal_mock.MCAL_getEventData_error(eventResponse);
    }

    void MCAL_initCallback(MCAL_Callback_t *callback, MCAL_CallbackFunction_t callbackFunction, void *objPtr)
    {
        mcal_mock.MCAL_initCallback(callback, callbackFunction, objPtr);
    }

    void MCAL_invokeCallbacks(const MCAL_Callback_t *callback, enum MCAL_Module eventSource, enum MCAL_Event eventType, const struct MCAL_EventData *eventData)
    {
        mcal_mock.MCAL_invokeCallbacks(callback, eventSource, eventType, eventData);
    }

    void MCAL_invokeChannelCallbacks(const MCAL_Callback_t *callback, enum MCAL_Module eventSource, uint8_t channelNum, const struct MCAL_EventData *eventData)
    {
        mcal_mock.MCAL_invokeChannelCallbacks(callback, eventSource, channelNum, eventData);
    }

    void MCAL_enableEvent(MCAL_Callback_t **cbList, MCAL_Callback_t *callback, enum MCAL_Event event)
    {
        mcal_mock.MCAL_enableEvent(cbList, callback, event);
    }

    void MCAL_enableChannelEvent(MCAL_Callback_t **cbList, MCAL_Callback_t *callback, uint8_t channelNum)
    {
        mcal_mock.MCAL_enableChannelEvent(cbList, callback, channelNum);
    }

    void MCAL_disableEvent(MCAL_Callback_t *callback, enum MCAL_Event event)
    {
        mcal_mock.MCAL_disableEvent(callback, event);
    }

    void MCAL_disableChannelEvent(MCAL_Callback_t *callback, uint8_t channelNum)
    {
        mcal_mock.MCAL_disableChannelEvent(callback, channelNum);
    }

    int32_t MCAL_executeCallbacks(const struct STDCB_Callback *callback, enum MCAL_Event event, int32_t eventData)
    {
        return mcal_mock.MCAL_executeCallbacks(callback, event, eventData);
    }

    int32_t MCAL_executeChannelCallbacks(const struct STDCB_Callback *callback, uint8_t channelNum, int32_t eventData)
    {
        return mcal_mock.MCAL_executeChannelCallbacks(callback, channelNum, eventData);
    }

}

