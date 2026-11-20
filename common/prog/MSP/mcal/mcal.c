/*******************************************************************************
 *  Copyright (c) 2017 BSH Hausgeraete GmbH,
 *  Carl-Wery-Str. 34, 81739 Munich, Germany, www.bsh-group.de
 *
 *  All rights reserved. This program and the accompanying materials
 *  are protected by international copyright laws.
 *  Please contact copyright holder for licensing information.
 *
 *******************************************************************************
 *  PROJECT          MCAL
 *  COMP_ABBREV      MCAL
 ******************************************************************************/


/******************************************************************************/
/* DOCUMENTATION                                                              */
/******************************************************************************/
/** \file
 *
 *  \ingroup  MCAL
 *
 *  \brief    Implementation of MCAL
 */


/******************************************************************************/
/* INCLUDES                                                                   */
/******************************************************************************/
#include "BSH_stdinc.h"
#include "mcal.h"

#include "mcal_assert.h"


/******************************************************************************/
/* PUBLIC FUNCTION DEFINITIONS                                                */
/******************************************************************************/


void MCAL_initCallback(MCAL_Callback_t *callback, MCAL_CallbackFunction_t callbackFunction, void *objPtr)
{
    MCAL_assert(callback != NULL);
    /*lint -e{929} "cast from pointer to pointer [MISRA 2012 Rule 11.3, 11.5 required]" */
    /*Axivion Next Line MisraC2012-11.1 : "Conversion between incompatible function pointer types", Tested to work correctly */
    (void)STDCB_InitCallback(callback, (STDCB_CallbackFunction_t)callbackFunction, objPtr, 0u);
}


static struct MCAL_EventData MCAL_checkEventData(const struct MCAL_EventData *eventData)
{
    struct MCAL_EventData retEventData;

    if(eventData != NULL)
    {
        retEventData = *eventData;
    }
    else
    {
        retEventData.dataType = MCAL_DATA_TYPE_NONE;
    }

    return retEventData;
}


void MCAL_invokeCallbacks(const MCAL_Callback_t *callback, enum MCAL_Module eventSource, enum MCAL_Event eventType, const struct MCAL_EventData *eventData)
{
    struct MCAL_EventResponse eventResponse;
    uint32_t eventFlags;

    eventFlags = MCAL_getEventMask(eventType);

    eventResponse.eventSource = eventSource;
    eventResponse.eventType = eventType;
    eventResponse.eventData = MCAL_checkEventData(eventData);

    /*lint -e{923} "cast from pointer to pointer [MISRA 2012 Rule 11.1, 11.4, 11.6 required]" */
    (void)STDCB_ExecuteCallbacks(callback, eventFlags, (int32_t)&eventResponse);
    /*lint -e{550} "Symbol 'eventResponse' not accessed", Yes it is */
}


void MCAL_invokeChannelCallbacks(const MCAL_Callback_t *callback, enum MCAL_Module eventSource, uint8_t channelNum, const struct MCAL_EventData *eventData)
{
    struct MCAL_EventResponse eventResponse;
    uint32_t eventFlags;

    eventFlags = MCAL_getChannelEventMask(channelNum);

    eventResponse.eventSource = eventSource;
    eventResponse.eventType = MCAL_EVENT_CHANNEL;
    eventResponse.eventData = MCAL_checkEventData(eventData);

    /*lint -e{923} "cast from pointer to pointer [MISRA 2012 Rule 11.1, 11.4, 11.6 required]" */
    (void)STDCB_ExecuteCallbacks(callback, eventFlags, (int32_t)&eventResponse);
    /*lint -e{550} "Symbol 'eventResponse' not accessed", Yes it is */
}


void MCAL_enableEvent(MCAL_Callback_t **cbList, MCAL_Callback_t *callback, enum MCAL_Event event)
{
    uint32_t eventMask;

    MCAL_assert(cbList != NULL);
    MCAL_assert(callback != NULL);

    eventMask = MCAL_getEventMask(event);
    
    /* Return value can be ignored as it only returns error when cbList is NULL */
    (void) STDCB_AttachCallback(cbList, callback);
    STDCB_EnableEvents(callback, eventMask);
}


void MCAL_enableChannelEvent(MCAL_Callback_t **cbList, MCAL_Callback_t *callback, uint8_t channelNum)
{
    uint32_t eventMask;

    MCAL_assert(cbList != NULL);
    MCAL_assert(callback != NULL);

    eventMask = MCAL_getChannelEventMask(channelNum);
    
    /* Return value can be ignored as it only returns error when cbList is NULL */
    (void) STDCB_AttachCallback(cbList, callback);
    STDCB_EnableEvents(callback, eventMask);
}


void MCAL_disableEvent(MCAL_Callback_t *callback, enum MCAL_Event event)
{
    uint32_t eventMask = MCAL_getEventMask(event);
    STDCB_DisableEvents(callback, eventMask);
}


void MCAL_disableChannelEvent(MCAL_Callback_t *callback, uint8_t channelNum)
{
    uint32_t eventMask = MCAL_getChannelEventMask(channelNum);
    STDCB_DisableEvents(callback, eventMask);
}


uint8_t MCAL_getEventData_uint8(const struct MCAL_EventResponse *eventResponse)
{
    MCAL_assert(MCAL_getEventDataType(eventResponse) == MCAL_DATA_TYPE_UINT8);
    return eventResponse->eventData.data.uint8;
}


uint16_t MCAL_getEventData_uint16(const struct MCAL_EventResponse *eventResponse)
{
    MCAL_assert(MCAL_getEventDataType(eventResponse) == MCAL_DATA_TYPE_UINT16);
    return eventResponse->eventData.data.uint16;
}


uint32_t MCAL_getEventData_uint32(const struct MCAL_EventResponse *eventResponse)
{
    MCAL_assert(MCAL_getEventDataType(eventResponse) == MCAL_DATA_TYPE_UINT32);
    return eventResponse->eventData.data.uint32;
}


int8_t MCAL_getEventData_int8(const struct MCAL_EventResponse *eventResponse)
{
    MCAL_assert(MCAL_getEventDataType(eventResponse) == MCAL_DATA_TYPE_INT8);
    return eventResponse->eventData.data.int8;
}


int16_t MCAL_getEventData_int16(const struct MCAL_EventResponse *eventResponse)
{
    MCAL_assert(MCAL_getEventDataType(eventResponse) == MCAL_DATA_TYPE_INT16);
    return eventResponse->eventData.data.int16;
}


int32_t MCAL_getEventData_int32(const struct MCAL_EventResponse *eventResponse)
{
    MCAL_assert(MCAL_getEventDataType(eventResponse) == MCAL_DATA_TYPE_INT32);
    return eventResponse->eventData.data.int32;
}


enum MCAL_Error MCAL_getEventData_error(const struct MCAL_EventResponse *eventResponse)
{
    MCAL_assert(MCAL_getEventDataType(eventResponse) == MCAL_DATA_TYPE_ERROR_VALUE);

    // Check if event data contains error value (additional to assert)
    if(eventResponse->eventData.dataType != MCAL_DATA_TYPE_ERROR_VALUE)
    {
        return MCAL_ERROR_INVALID;
    }
    else
    {
        return eventResponse->eventData.data.error;
    }
}


enum MCAL_Error MCAL_checkReady(enum MCAL_Status status)
{
    if(status == MCAL_STATUS_READY)
    {
        return MCAL_OK;
    }
    else if(status == MCAL_STATUS_BUSY)
    {
        return MCAL_ERROR_BUSY;
    }
    else if(status == MCAL_STATUS_RESET)
    {
        return MCAL_ERROR_NOT_INITIALIZED;
    }
    else if(status == MCAL_STATUS_DISABLED)
    {
        return MCAL_ERROR_NOT_READY;
    }
    else
    {
        return MCAL_ERROR;
    }
}


bool MCAL_isEventEnabled(MCAL_Callback_t **cbList, uint32_t eventMask)
{
    MCAL_Callback_t *cb = *cbList;

    while(cb != NULL)
    {
        if((cb->eventMask & eventMask) != 0u)
        {
            return true;
        }
        cb = cb->_nextCallback;
    }

    return false;
}
