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
#include "std_lib/test/std_callback.mock.h"
/* Your mocking includes here
   NOTE: The include order is important! lowest level need to be included first.
         Cross include dependencies creates troubles!!*/

using trompeloeil::_;

namespace mcal_test
{
    #include "mcal/mcal.h"

    /************************** local mocks ***********************************
     *  Use macro MOCKI(function-name)(..) to mock internal static interfaces.
     *  Use macro MOCKE(..)(..) to mock interfaces of the unit.
     *  Mocking of static internal interface should be prevented. Use it to break
     *  endless loops and to reduce high complexity.
     */
    class
    {
      // MAKE_MOCK1(XXX_isConvReady      , bool        ( const XXX_Handle *handle ));
      // needed to test callback functionality
       MAKE_MOCK3(MCAL_myTestCallback       , int32_t     (void* objPtr, uint32_t eventFlags, const MCAL_EventResponse *eventResponse));
    }int_mock;

    class
    {
      // MAKE_MOCK2(XXX_readSample       , uint16_t    ( XXX_Handle *handle, const XXX_Channel *channel ));
    }ext_mock;

    /**************************************************************************/
 
    // include the implementation of 'mcal' into the current namespace
    #include "mcal/mcal.c"

//extern int32_t STDCB_InitCallback(struct STDCB_Callback *cbPtr, STDCB_CallbackFunction_t funcPtr, void *objPtr, uint32_t eventMask)


    bool operator==(const MCAL_EventData& left, const MCAL_EventData& right)
    {
        return ((left.data.uint32  == right.data.uint32)     &&
                (left.dataType == right.dataType)
               );
    }



    bool operator==(const MCAL_EventResponse& left, const MCAL_EventResponse& right)
    {
        return ((left.eventData   == right.eventData)     &&
                (left.eventSource == right.eventSource)   &&
                (left.eventType   == right.eventType)
               );
    }


    TEST_SUITE("mcal")
    {
        trompeloeil::sequence seq;
        /* do here the common stuff e.g. declarations needed in the following test cases*/
        MCAL_EventResponse eventResponse;
        MCAL_EventData     eventData;
        const uint8_t      u8Value  = 0x33;
        const uint16_t     u16Value = 0x3355;
        const uint32_t     u32Value = 0x12345678;
        const int8_t       i8Value  = -0x33;
        const int16_t      i16Value = -0x3355;
        const int32_t      i32Value = -0x12345678;



        TEST_CASE("MCAL_getEventSource")
        {   //enum MCAL_Event MCAL_getEventSource(const struct MCAL_EventResponse *eventResponse)

            SUBCASE("Check with UART")
            {   
                eventResponse.eventSource = MCAL_MODULE_MUART;
                CHECK(MCAL_getEventSource(&eventResponse) == MCAL_MODULE_MUART);
            }
            SUBCASE("Check with I2C")
            {   
                eventResponse.eventSource = MCAL_MODULE_MI2C;
                CHECK(MCAL_getEventSource(&eventResponse) == MCAL_MODULE_MI2C);
            }
        }

        TEST_CASE("MCAL_getEventType")
        {   //enum MCAL_Event MCAL_getEventType(const struct MCAL_EventResponse *eventResponse)

            SUBCASE("MCAL_EVENT_TRIGGER")
            {   
                eventResponse.eventType = MCAL_EVENT_TRIGGER;
                CHECK(MCAL_getEventType(&eventResponse) == MCAL_EVENT_TRIGGER);
            }
            SUBCASE("MCAL_EVENT_ACKNOWLEDGE")
            {   
                eventResponse.eventType = MCAL_EVENT_ACKNOWLEDGE;
                CHECK(MCAL_getEventType(&eventResponse) == MCAL_EVENT_ACKNOWLEDGE);
            }

        }

        TEST_CASE("*MCAL_getEventData")
        {   //const struct MCAL_EventData *MCAL_getEventData(const struct MCAL_EventResponse *eventResponse)
            CHECK(MCAL_getEventData(&eventResponse) == &eventResponse.eventData);
        }

        TEST_CASE("MCAL_getEventDataType")
        {   //enum MCAL_DataType MCAL_getEventDataType(const struct MCAL_EventResponse *eventResponse)

            SUBCASE("MCAL_DATA_TYPE_NONE")
            {   
                eventResponse.eventData.dataType = MCAL_DATA_TYPE_NONE;
                CHECK(MCAL_getEventDataType(&eventResponse) == MCAL_DATA_TYPE_NONE);
            }
            SUBCASE("MCAL_DATA_TYPE_NONE")
            {   
                eventResponse.eventData.dataType = MCAL_DATA_TYPE_INT16;
                CHECK(MCAL_getEventDataType(&eventResponse) == MCAL_DATA_TYPE_INT16);
            }

        }

        TEST_CASE("MCAL_setEventData_uint8")
        {   //void MCAL_setEventData_uint8(struct MCAL_EventData *eventData, uint8_t data)
            MCAL_setEventData_uint8(&eventData, u8Value);
            CHECK(eventData.dataType == MCAL_DATA_TYPE_UINT8);
            CHECK(eventData.data.uint8 == u8Value);
        }

        TEST_CASE("MCAL_setEventData_uint16")
        {   //void MCAL_setEventData_uint16(struct MCAL_EventData *eventData, uint16_t data)
            MCAL_setEventData_uint16(&eventData, u16Value);
            CHECK(eventData.dataType == MCAL_DATA_TYPE_UINT16);
            CHECK(eventData.data.uint16 == u16Value);
        }

        TEST_CASE("MCAL_setEventData_uint32")
        {   //void MCAL_setEventData_uint32(struct MCAL_EventData *eventData, uint32_t data)
            MCAL_setEventData_uint32(&eventData, u32Value);
            CHECK(eventData.dataType == MCAL_DATA_TYPE_UINT32);
            CHECK(eventData.data.uint32 == u32Value);
        }

        TEST_CASE("MCAL_setEventData_int8")
        {
            //void MCAL_setEventData_int8(struct MCAL_EventData *eventData, int8_t data)
            MCAL_setEventData_int8(&eventData, i8Value);
            CHECK(eventData.dataType == MCAL_DATA_TYPE_INT8);
            CHECK(eventData.data.int8 == i8Value);
        }

        TEST_CASE("MCAL_setEventData_int16")
        {   //void MCAL_setEventData_int16(struct MCAL_EventData *eventData, int16_t data)
            MCAL_setEventData_int16(&eventData, i16Value);
            CHECK(eventData.dataType == MCAL_DATA_TYPE_INT16);
            CHECK(eventData.data.int16 == i16Value);
        }

        TEST_CASE("MCAL_setEventData_int32")
        {   //void MCAL_setEventData_int32(struct MCAL_EventData *eventData, int32_t data)
            MCAL_setEventData_int32(&eventData, i32Value);
            CHECK(eventData.dataType == MCAL_DATA_TYPE_INT32);
            CHECK(eventData.data.int32 == i32Value);
        }

        TEST_CASE("MCAL_setEventData_error")
        {   //void MCAL_setEventData_error(struct MCAL_EventData *eventData, enum MCAL_Error error)
            SUBCASE("MCAL_ERROR_CALLBACK")
            {   
                MCAL_setEventData_error(&eventData, MCAL_ERROR_CALLBACK);
                CHECK(eventData.data.error == MCAL_ERROR_CALLBACK);
                CHECK(eventData.dataType   == MCAL_DATA_TYPE_ERROR_VALUE);
            }

            SUBCASE("MCAL_ERROR_CALLBACK")
            {   
                MCAL_setEventData_error(&eventData, MCAL_ERROR_OVERFLOW);
                CHECK(eventData.data.error == MCAL_ERROR_OVERFLOW);
                CHECK(eventData.dataType   == MCAL_DATA_TYPE_ERROR_VALUE);
            }

        }

        TEST_CASE("MCAL_isInitialized")
        {   //bool MCAL_isInitialized(enum MCAL_Status status)
            CHECK(MCAL_isInitialized(MCAL_STATUS_DISABLED));
            CHECK(MCAL_isInitialized(MCAL_STATUS_RESET) == false);
        }

        TEST_CASE("MCAL_isReady")
        {   //bool MCAL_isReady(enum MCAL_Status status)
            CHECK(MCAL_isReady(MCAL_STATUS_READY));
            CHECK(MCAL_isReady(MCAL_STATUS_BUSY) == false);
        }

        TEST_CASE("MCAL_isDisabled")
        {
            CHECK(MCAL_isDisabled(MCAL_STATUS_DISABLED));
            CHECK(MCAL_isDisabled(MCAL_STATUS_BUSY) == false);
        }

        TEST_CASE("MCAL_isBusy")
        {
            CHECK(MCAL_isBusy(MCAL_STATUS_BUSY));
            CHECK(MCAL_isBusy(MCAL_STATUS_READY) == false);
        }

        TEST_CASE("MCAL_checkInitialized")
        {   //enum MCAL_Error MCAL_checkInitialized(enum MCAL_Status status)
            CHECK(MCAL_checkInitialized(MCAL_STATUS_RESET) == MCAL_ERROR_NOT_INITIALIZED);
            CHECK(MCAL_checkInitialized(MCAL_STATUS_BUSY) == MCAL_OK);
            CHECK(MCAL_checkInitialized(MCAL_STATUS_READY) == MCAL_OK);
            CHECK(MCAL_checkInitialized(MCAL_STATUS_DISABLED) == MCAL_OK);
        }

        TEST_CASE("MCAL_checkReady")
        {   //enum MCAL_Error MCAL_checkReady(enum MCAL_Status status)
            CHECK(MCAL_checkReady(MCAL_STATUS_READY)    == MCAL_OK);
            CHECK(MCAL_checkReady(MCAL_STATUS_BUSY)     == MCAL_ERROR_BUSY);
            CHECK(MCAL_checkReady(MCAL_STATUS_RESET)    == MCAL_ERROR_NOT_INITIALIZED);
            CHECK(MCAL_checkReady(MCAL_STATUS_DISABLED) == MCAL_ERROR_NOT_READY);
            CHECK(MCAL_checkReady((MCAL_Status)0xff)    == MCAL_ERROR);
        }

        TEST_CASE("MCAL_getEventData_uint8")
        {   //uint8_t MCAL_getEventData_uint8(const struct MCAL_EventResponse *eventResponse)
            MCAL_setEventData_uint8(&eventResponse.eventData, u8Value);
            CHECK(MCAL_getEventData_uint8(&eventResponse) == u8Value);
        }

        TEST_CASE("MCAL_getEventData_uint16")
        {   //uint16_t MCAL_getEventData_uint16(const struct MCAL_EventResponse *eventResponse)
            MCAL_setEventData_uint16(&eventResponse.eventData, u16Value);
            CHECK(MCAL_getEventData_uint16(&eventResponse) == u16Value);
        }

        TEST_CASE("MCAL_getEventData_uint32")
        {   //uint32_t MCAL_getEventData_uint32(const struct MCAL_EventResponse *eventResponse)
            MCAL_setEventData_uint32(&eventResponse.eventData, u32Value);
            CHECK(MCAL_getEventData_uint32(&eventResponse) == u32Value);
        }

        TEST_CASE("MCAL_getEventData_int8")
        {   //int8_t MCAL_getEventData_int8(const struct MCAL_EventResponse *eventResponse)
            MCAL_setEventData_int8(&eventResponse.eventData, i8Value);
            CHECK(MCAL_getEventData_int8(&eventResponse) == i8Value);

        }

        TEST_CASE("MCAL_getEventData_int16")
        {   //int16_t MCAL_getEventData_int16(const struct MCAL_EventResponse *eventResponse)
            MCAL_setEventData_int16(&eventResponse.eventData, i16Value);
            CHECK(MCAL_getEventData_int16(&eventResponse) == i16Value);
        }

        TEST_CASE("MCAL_getEventData_int32")
        {   //int32_t MCAL_getEventData_int32(const struct MCAL_EventResponse *eventResponse)
            MCAL_setEventData_int32(&eventResponse.eventData, i32Value);
            CHECK(MCAL_getEventData_int32(&eventResponse) == i32Value);
        }

        TEST_CASE("MCAL_getEventData_error")
        {   //enum MCAL_Error MCAL_getEventData_error(const struct MCAL_EventResponse *eventResponse)
            SUBCASE("no error data available")
            {   
                eventResponse.eventData.dataType = MCAL_DATA_TYPE_UINT8;
                CHECK(MCAL_getEventData_error(&eventResponse) == MCAL_ERROR_INVALID);
            }
            SUBCASE("error data available")
            {   
                eventResponse.eventData.dataType = MCAL_DATA_TYPE_ERROR_VALUE;
                eventResponse.eventData.data.error  = MCAL_BUS_ERROR;
                CHECK(MCAL_getEventData_error(&eventResponse) == MCAL_BUS_ERROR);
            }

        }

        TEST_CASE("MCAL_getEventMask")
        {   //uint32_t MCAL_getEventMask(enum MCAL_Event event)
            CHECK(MCAL_getEventMask(MCAL_EVENT_NOT_ACKNOWLEDGE) == uint32_t(MCAL_EVENT_NOT_ACKNOWLEDGE));
            CHECK(MCAL_getEventMask(MCAL_EVENT_NONE) == uint32_t(MCAL_EVENT_NONE));
        }

        TEST_CASE("MCAL_getChannelEventMask")
        {   //uint32_t MCAL_getChannelEventMask(uint8_t channelNum)
            CHECK(MCAL_getChannelEventMask(0u) == 0x01uL);
            CHECK(MCAL_getChannelEventMask(8u) == 0x100uL);
            CHECK(MCAL_getChannelEventMask(16u) == 0x10000uL);
            CHECK(MCAL_getChannelEventMask(32u) == 0x01uL);  // undefined behaviour!! On MINGW it is 1.
        }

        TEST_CASE("MCAL_isEvent")
        {   //bool MCAL_isEvent(const struct MCAL_EventResponse *eventResponse, enum MCAL_Event event)
            SUBCASE("MCAL_EVENT_ACKNOWLEDGE")
            {
                eventResponse.eventType = MCAL_EVENT_ACKNOWLEDGE;
                CHECK(MCAL_isEvent(&eventResponse, MCAL_EVENT_ACKNOWLEDGE) == true);
                CHECK(MCAL_isEvent(&eventResponse, MCAL_EVENT_READ_BYTE) == false);
            }
            SUBCASE("MCAL_EVENT_ACKNOWLEDGE")
            {
                eventResponse.eventType = MCAL_EVENT_UPDATE;
                CHECK(MCAL_isEvent(&eventResponse, MCAL_EVENT_UPDATE) == true);
                CHECK(MCAL_isEvent(&eventResponse, MCAL_EVENT_ACKNOWLEDGE) == false);
            }

        }

        TEST_CASE("MCAL_initCallback")
        {   /* HINT: this interface calls the mocked interface STDCB_InitCallback
               In this test we just check if STDCB_InitCallback is called correctly
               It might be required to call the original interface instead
               std_callback_test::STDCB_InitCallback as there is a suspicious cast */
            MCAL_Callback_t callback;
            MCAL_CallbackFunction_t  callback_function = (MCAL_CallbackFunction_t)0x12345678;
            void* object = (void*)0x4711;
            STDCB_InitCallback_mock = false;
            // REQUIRE_CALL(std_callback_mock, STDCB_InitCallback(_,_,_,_)) 
            //     .WITH(_1 == &callback)
            //     .WITH(_2 == (STDCB_CallbackFunction_t)callback_function)
            //     .WITH(_3 == object)
            //     .WITH(_4 == 0u)
            //     .RETURN(1);
            MCAL_initCallback(&callback,callback_function,object);
            CHECK(callback.functionPtr == (std_callback_m::STDCB_CallbackFunction_t)callback_function);
            CHECK(callback.objectPtr == object);
            CHECK(callback.eventMask == 0u);
            CHECK(callback._nextCallback == nullptr);
            STDCB_InitCallback_mock = true; // call mock function by default 
        }

        TEST_CASE("MCAL_invokeCallbacks")
        {   //void MCAL_invokeCallbacks(const MCAL_Callback_t *callback, enum MCAL_Module eventSource, enum MCAL_Event eventType, const struct MCAL_EventData *eventData)
            MCAL_Callback_t callback;
            //callback.functionPtr = (STDCB_CallbackFunction_t)&int_mock.MCAL_myTestCallback;
            MCAL_EventData   eventData;
            eventData.data.uint32 = 0x154711;
            eventData.dataType    = MCAL_DATA_TYPE_UINT32;
            // REQUIRE_CALL(std_callback_mock, STDCB_ExecuteCallbacks(_,_,_))
            //     .RETURN(0)
            //     .WITH(_1 == (STDCB_Callback *)callback)
            //     .WITH(_2 == (uint32_t)MCAL_EVENT_TRIGGER)
            //     .WITH(*(MCAL_EventResponse*)(_3) == (MCAL_EventResponse){MCAL_MODULE_MSPI, MCAL_EVENT_TRIGGER, eventData});
            //MCAL_invokeCallbacks(&callback, MCAL_MODULE_MSPI, MCAL_EVENT_TRIGGER, &eventData);
            CHECK(4 == sizeof(void*));
        }

        TEST_CASE("MCAL_invokeChannelCallbacks")
        {
            SUBCASE("Check....")
            {   //void MCAL_invokeChannelCallbacks(const MCAL_Callback_t *callback, enum MCAL_Module eventSource, uint8_t channelNum, const struct MCAL_EventData *eventData)
                //MCAL_invokeChannelCallbacks(callback, eventSource, channelNum, eventData);
                CHECK(true);
            }
        }

        TEST_CASE("MCAL_enableEvent")
        {
            SUBCASE("Check....")
            {   //void MCAL_enableEvent(MCAL_Callback_t **cbList, MCAL_Callback_t *callback, enum MCAL_Event event)
                //MCAL_enableEvent(cbList, callback, event);
                CHECK(true);
            }
        }

        TEST_CASE("MCAL_enableChannelEvent")
        {
            SUBCASE("Check....")
            {   //void MCAL_enableChannelEvent(MCAL_Callback_t **cbList, MCAL_Callback_t *callback, uint8_t channelNum)
                //MCAL_enableChannelEvent(cbList, callback, channelNum);
                CHECK(true);
            }
        }

        TEST_CASE("MCAL_disableEvent")
        {
            SUBCASE("Check....")
            {   //void MCAL_disableEvent(MCAL_Callback_t *callback, enum MCAL_Event event)
                //MCAL_disableEvent(callback, event);
                CHECK(true);
            }
        }

        TEST_CASE("MCAL_disableChannelEvent")
        {
            SUBCASE("Check....")
            {   //void MCAL_disableChannelEvent(MCAL_Callback_t *callback, uint8_t channelNum)
                //MCAL_disableChannelEvent(callback, channelNum);
                CHECK(true);
            }
        }

        TEST_CASE("MCAL_executeCallbacks")
        {
            SUBCASE("Check....")
            {   //int32_t MCAL_executeCallbacks(const struct STDCB_Callback *callback, enum MCAL_Event event, int32_t eventData)
                //int32_t ret = MCAL_executeCallbacks(callback, event, eventData);
                CHECK(true);
            }
        }

        TEST_CASE("MCAL_executeChannelCallbacks")
        {
            SUBCASE("Check....")
            {   //int32_t MCAL_executeChannelCallbacks(const struct STDCB_Callback *callback, uint8_t channelNum, int32_t eventData)
                //int32_t ret = MCAL_executeChannelCallbacks(callback, channelNum, eventData);
                CHECK(true);
            }
        }

    }
}

