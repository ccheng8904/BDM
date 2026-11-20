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
 *  COMP_ABBREV      MCAL
 ******************************************************************************/


#ifndef MCAL_H
#define MCAL_H

#if defined(__cplusplus) && !defined(MOCK_IT)
extern "C" {
#endif //__cplusplus


/******************************************************************************/
/* DOCUMENTATION                                                              */
/******************************************************************************/
/** \file
 *
 *  \ingroup  MCAL
 *
 *  \brief    Public definitions and declarations for unit MCAL.
 *
 */


/******************************************************************************/
/* INCLUDES                                                                   */
/******************************************************************************/
#include "BSH_stdinc.h"
#include <stdint.h>
#include <stdbool.h>
#include "std_lib/std_callback.h"

#include "mcal_types.h"


/******************************************************************************/
/* PUBLIC INLINE FUNCTION DEFINITIONS                                         */
/******************************************************************************/

static inline enum MCAL_Module MCAL_getEventSource(const struct MCAL_EventResponse *eventResponse)
{
    return eventResponse->eventSource;
}


static inline enum MCAL_Event MCAL_getEventType(const struct MCAL_EventResponse *eventResponse)
{
    return eventResponse->eventType;
}


static inline const struct MCAL_EventData *MCAL_getEventData(const struct MCAL_EventResponse *eventResponse)
{
    return &eventResponse->eventData;
}


static inline enum MCAL_DataType MCAL_getEventDataType(const struct MCAL_EventResponse *eventResponse)
{
    return eventResponse->eventData.dataType;
}


/** \brief    Functions for setting event data.
 *
 *  \details  These functions are intended to be used when passing event data using callback.
 * 
 */
static inline void MCAL_setEventData_uint8(struct MCAL_EventData *eventData, uint8_t data)
{
    eventData->dataType = MCAL_DATA_TYPE_UINT8;
    eventData->data.uint8 = data;
}

static inline void MCAL_setEventData_uint16(struct MCAL_EventData *eventData, uint16_t data)
{
    eventData->dataType = MCAL_DATA_TYPE_UINT16;
    eventData->data.uint16 = data;
}

static inline void MCAL_setEventData_uint32(struct MCAL_EventData *eventData, uint32_t data)
{
    eventData->dataType = MCAL_DATA_TYPE_UINT32;
    eventData->data.uint32 = data;
}

static inline void MCAL_setEventData_int8(struct MCAL_EventData *eventData, int8_t data)
{
    eventData->dataType = MCAL_DATA_TYPE_INT8;
    eventData->data.int8 = data;
}

static inline void MCAL_setEventData_int16(struct MCAL_EventData *eventData, int16_t data)
{
    eventData->dataType = MCAL_DATA_TYPE_INT16;
    eventData->data.int16 = data;
}

static inline void MCAL_setEventData_int32(struct MCAL_EventData *eventData, int32_t data)
{
    eventData->dataType = MCAL_DATA_TYPE_INT32;
    eventData->data.int32 = data;
}

static inline void MCAL_setEventData_error(struct MCAL_EventData *eventData, enum MCAL_Error error)
{
    eventData->dataType = MCAL_DATA_TYPE_ERROR_VALUE;
    eventData->data.error = error;
}


/** \brief    Functions for simple checking of status.
 *
 *  \param    status: status to check
 *  \return   false if status does not correspond.
 */
static inline bool MCAL_isInitialized(enum MCAL_Status status)
{
    return (status != MCAL_STATUS_RESET) ? true : false;
}

static inline bool MCAL_isReady(enum MCAL_Status status)
{
    return (status == MCAL_STATUS_READY) ? true : false;
}

static inline bool MCAL_isDisabled(enum MCAL_Status status)
{
    return (status == MCAL_STATUS_DISABLED) ? true : false;
}

static inline bool MCAL_isBusy(enum MCAL_Status status)
{
    return (status == MCAL_STATUS_BUSY) ? true : false;
}



/** \brief    Functions for advanced simple checking of status.
 *
 *  \details  Difference to simple checking is, that these function also return
 *            error value. These functions can be used together with MCAL_RETURN_ON_ERROR
 *            and MCAL_BREAK_ON_ERROR macros to return from function or break from loop
 *            on error
 * 
 *  \param    status: status to check
 *  \return   MCAL_OK if status correspond, specific error value otherwise.
 */
static inline enum MCAL_Error MCAL_checkInitialized(enum MCAL_Status status)
{
    return (MCAL_isInitialized(status)) ? MCAL_OK : MCAL_ERROR_NOT_INITIALIZED;
}

extern enum MCAL_Error MCAL_checkReady(enum MCAL_Status status);


/******************************************************************************/
/* PUBLIC FUNCTION DECLARATIONS                                               */
/******************************************************************************/

/** \brief    Functions for getting event data from event response.
 *
 *  \details  These functions are intended to be used inside callbacks to get data from event.
 * 
 *  \return   depends on specific interface
 */
extern uint8_t MCAL_getEventData_uint8(const struct MCAL_EventResponse *eventResponse);
extern uint16_t MCAL_getEventData_uint16(const struct MCAL_EventResponse *eventResponse);
extern uint32_t MCAL_getEventData_uint32(const struct MCAL_EventResponse *eventResponse);
extern int8_t MCAL_getEventData_int8(const struct MCAL_EventResponse *eventResponse);
extern int16_t MCAL_getEventData_int16(const struct MCAL_EventResponse *eventResponse);
extern int32_t MCAL_getEventData_int32(const struct MCAL_EventResponse *eventResponse);
extern enum MCAL_Error MCAL_getEventData_error(const struct MCAL_EventResponse *eventResponse);


/** \brief    Function for casting enum MCAL_Event to error mask.
 *
 *  \details  This function is used as to convert MCAL events to event flags
 *            used by STDCB module.
 */
static inline uint32_t MCAL_getEventMask(enum MCAL_Event event)
{
    return (uint32_t) event;
}


/** \brief    Function for converting channel number to channel event mask/flag.
 *
 *  \details  This function is used as to convert channel number to event flags
 *            used by STDCB module.
 */
static inline uint32_t MCAL_getChannelEventMask(uint8_t channelNum)
{
    return (uint32_t)0x01u << channelNum;
}


/** \brief    Function for checking if specific MCAL event is pending.
 *
 *  \details  This function can be used inside callback to query if specific
 *            event triggered actual callback, based on eventFlags parameter.
 */
static inline bool MCAL_isEvent(const struct MCAL_EventResponse *eventResponse, enum MCAL_Event event)
{
    return (eventResponse->eventType == event) ? true : false;
}

extern void MCAL_initCallback(MCAL_Callback_t *callback, MCAL_CallbackFunction_t callbackFunction, void *objPtr);

extern void MCAL_invokeCallbacks(const MCAL_Callback_t *callback, enum MCAL_Module eventSource, enum MCAL_Event eventType, const struct MCAL_EventData *eventData);

extern void MCAL_invokeChannelCallbacks(const MCAL_Callback_t *callback, enum MCAL_Module eventSource, uint8_t channelNum, const struct MCAL_EventData *eventData);

extern void MCAL_enableEvent(MCAL_Callback_t **cbList, MCAL_Callback_t *callback, enum MCAL_Event event);

extern void MCAL_enableChannelEvent(MCAL_Callback_t **cbList, MCAL_Callback_t *callback, uint8_t channelNum);

extern void MCAL_disableEvent(MCAL_Callback_t *callback, enum MCAL_Event event);

extern void MCAL_disableChannelEvent(MCAL_Callback_t *callback, uint8_t channelNum);

extern bool MCAL_isEventEnabled(MCAL_Callback_t **cbList, uint32_t eventMask);

// Obsolete functions
extern int32_t MCAL_executeCallbacks(const struct STDCB_Callback *callback, enum MCAL_Event event, int32_t eventData);

extern int32_t MCAL_executeChannelCallbacks(const struct STDCB_Callback *callback, uint8_t channelNum, int32_t eventData);

#if defined(__cplusplus) && !defined(MOCK_IT)
}
#endif // __cplusplus

/******************************************************************************/
/* PUBLIC CLASS DECLARATIONS                                                  */
/******************************************************************************/
#ifdef __cplusplus

namespace MCAL
{
    class Callback
    {
        private:
            MCAL_Callback_t callback;

        public:
            Callback(void)
            {
            }

            Callback(MCAL_CallbackFunction_t funcPtr, void *objPtr = nullptr)
            {
                MCAL_initCallback(&this->callback, funcPtr, objPtr);
            }

            void init(MCAL_CallbackFunction_t funcPtr, void *objPtr = nullptr)
            {
                MCAL_initCallback(&this->callback, funcPtr, objPtr);
            }

            MCAL_Callback_t *getCallback(void)
            {
                return &callback;
            }

            // void enableEvent(enum MCAL_Event event)
            // {
            //     this->enableEvents(MCAL_getEventMask(event));
            // }

            // void disableEvent(enum MCAL_Event event)
            // {
            //     this->disableEvents(MCAL_getEventMask(event));
            // }
    };
}

#endif // __cplusplus

#endif // MCAL_H
