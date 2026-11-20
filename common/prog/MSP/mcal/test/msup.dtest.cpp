/*******************************************************************************
*  Copyright (c) 2024 BSH Hausgeraete GmbH,
*  Carl-Wery-Str. 34, 81739 Munich, Germany, www.bsh-group.de
*  are protected by international copyright laws.
*
*  All rights reserved. This program and the accompanying materials
*  Please contact copyright holder for licensing information.
*******************************************************************************/

#include <doctest/doctest.h>
/* Your mocking includes here */

namespace msup_test
{
    // include the implementation of 'msup' into the current namespace
    #include "mcal/msup.c"

    TEST_SUITE("msup test suit")
    {
        /* do here the common stuff e.g. initialization
         * it is executed before each test case.        */
        TEST_CASE("MSUP_handleTimerExt: Check...")
        {   //void MSUP_handleTimerExt(struct MSUP_TimeoutHandle *handle)
            //MSUP_handleTimerExt(handle);
            CHECK(true);
        }

        TEST_CASE("MSUP_startTimeMeasureExt: Check...")
        {   //void MSUP_startTimeMeasureExt(struct MSUP_TimeoutHandle *handle)
            //MSUP_startTimeMeasureExt(handle);
            CHECK(true);
        }

        TEST_CASE("MSUP_setTimeOutExt: Check...")
        {   //void MSUP_setTimeOutExt(struct MSUP_TimeoutHandle *handle, uint32_t timeout)
            //MSUP_setTimeOutExt(handle, timeout);
            CHECK(true);
        }

        TEST_CASE("MSUP_getMeasuredTimeExt: Check...")
        {   //uint32_t MSUP_getMeasuredTimeExt(struct MSUP_TimeoutHandle *handle)
            //uint32_t ret = MSUP_getMeasuredTimeExt(handle);
            CHECK(true);
        }

        TEST_CASE("MSUP_isTimeOutExt: Check...")
        {   //bool MSUP_isTimeOutExt(struct MSUP_TimeoutHandle *handle)
            //bool ret = MSUP_isTimeOutExt(handle);
            CHECK(true);
        }

        TEST_CASE("MSUP_delay: Check...")
        {   //void MSUP_delay(uint16_t delay)
            //MSUP_delay(delay);
            CHECK(true);
        }

        TEST_CASE("MSUP_handleTimer: Check...")
        {   //void MSUP_handleTimer(void)
            //MSUP_handleTimer();
            CHECK(true);
        }

        TEST_CASE("MSUP_startTimeMeasure: Check...")
        {   //void MSUP_startTimeMeasure(void)
            //MSUP_startTimeMeasure();
            CHECK(true);
        }

        TEST_CASE("MSUP_setTimeOut: Check...")
        {   //void MSUP_setTimeOut(uint32_t timeout)
            //MSUP_setTimeOut(timeout);
            CHECK(true);
        }

        TEST_CASE("MSUP_getMeasuredTime: Check...")
        {   //uint32_t MSUP_getMeasuredTime(void)
            //uint32_t ret = MSUP_getMeasuredTime();
            CHECK(true);
        }

        TEST_CASE("MSUP_isTimeOut: Check...")
        {   //bool MSUP_isTimeOut(void)
            //bool ret = MSUP_isTimeOut();
            CHECK(true);
        }

        TEST_CASE("HSUP_disableInt: Check...")
        {   //void HSUP_disableInt(void)
            //HSUP_disableInt();
            CHECK(true);
        }

        TEST_CASE("HSUP_enableInt: Check...")
        {   //void HSUP_enableInt(void)
            //HSUP_enableInt();
            CHECK(true);
        }

        TEST_CASE("MSUP_getHighByte: Check...")
        {   //uint8_t MSUP_getHighByte(uint16_t value)
            //uint8_t ret = MSUP_getHighByte(value);
            CHECK(true);
        }

        TEST_CASE("MSUP_getLowByte: Check...")
        {   //uint8_t MSUP_getLowByte(uint16_t value)
            //uint8_t ret = MSUP_getLowByte(value);
            CHECK(true);
        }

        TEST_CASE("MSUP_setHighByte: Check...")
        {   //void MSUP_setHighByte(uint16_t *value1, uint8_t value2)
            //MSUP_setHighByte(value1, value2);
            CHECK(true);
        }

        TEST_CASE("MSUP_setLowByte: Check...")
        {   //void MSUP_setLowByte(uint16_t *value1, uint8_t value2)
            //MSUP_setLowByte(value1, value2);
            CHECK(true);
        }

        TEST_CASE("MSUP_getHighWord: Check...")
        {   //uint32_t MSUP_getHighWord(uint32_t value)
            //uint32_t ret = MSUP_getHighWord(value);
            CHECK(true);
        }

        TEST_CASE("MSUP_getLowWord: Check...")
        {   //uint32_t MSUP_getLowWord(uint32_t value)
            //uint32_t ret = MSUP_getLowWord(value);
            CHECK(true);
        }

        TEST_CASE("MSUP_setHighWord: Check...")
        {   //void MSUP_setHighWord(uint32_t *value1, uint16_t value2)
            //MSUP_setHighWord(value1, value2);
            CHECK(true);
        }

        TEST_CASE("MSUP_setLowWord: Check...")
        {   //void MSUP_setLowWord(uint32_t *value1, uint16_t value2)
            //MSUP_setLowWord(value1, value2);
            CHECK(true);
        }

        TEST_CASE("MSUP_generateReset: Check...")
        {   //void MSUP_generateReset(void)
            //MSUP_generateReset();
            CHECK(true);
        }

    }
}

