/*******************************************************************************
 *  Copyright (c) 2024 BSH Hausgeraete GmbH,
 *  Carl-Wery-Str. 34, 81739 Munich, Germany, www.bsh-group.de
 *
 *  All rights reserved. This program and the accompanying materials
 *  are protected by international copyright laws.
 *  Please contact copyright holder for licensing information.
 *
 *******************************************************************************
 *  PROJECT          Generic SW Test
 ******************************************************************************/

#include <doctest/doctest.h>
#include "hsup.h"

/* Test strategy of HSUP:
 * There are a few interfaces which tests the intel implementation (HSUP_xxProcToBigEndian and HSUP_xxcMirror)
 * HSUP_xxcMirror uses equal implementation than CortexM0/M0+/M23
 * The test for byte/word get and set functions are checking the generic implementation.
 * Other functions like HSUP_vEnableInt/HSUP_vDisableInt and HSUP_vGenerateReset are provided as mocks for other tests
 */
TEST_SUITE("hsup")
{   
    TEST_CASE("HSUP_ulProcToBigEndian")
    {   // This tests the intel implementation only
        CHECK(HSUP_ulProcToBigEndian(0x11223344) == 0x44332211);
    }
    TEST_CASE("HSUP_uiProcToBigEndian")
    {   // This tests the intel implementation only
        CHECK(HSUP_uiProcToBigEndian(0x1122) == 0x2211);
    }
    TEST_CASE("HSUP_ucMirror")
    {   // Intel implementation, but it is the same than CortexM0/M23
        SUBCASE("HSUP_ucMirror(0)")
        {
            CHECK(HSUP_ucMirror(0) == 0);
        }
        SUBCASE("HSUP_ucMirror(0xFF)")
        {
            CHECK(HSUP_ucMirror(0xFF) == 0xFF);
        }
        SUBCASE("HSUP_ucMirror(42)")
        {
            CHECK(HSUP_ucMirror(42) == 84);
        }
        SUBCASE("HSUP_ucMirror(84)")
        {
            CHECK(HSUP_ucMirror(84) == 42);
        }

    }
    TEST_CASE("HSUP_u16Mirror")
    {   // Intel implementation, but it is the same than CortexM0/M23
        SUBCASE("HSUP_u16Mirror(0)")
        {
            CHECK(HSUP_u16Mirror(0) == 0);
        }
        SUBCASE("HSUP_u16Mirror(0xFFFF)")
        {
            CHECK(HSUP_u16Mirror(0xFFFF) == 0xFFFF);
        }
        SUBCASE("HSUP_u16Mirror(0x1234)")
        {
            CHECK(HSUP_u16Mirror(0x1234) == 0x2C48);
        }
        SUBCASE("HSUP_u16Mirror(0x2C48)")
        {
            CHECK(HSUP_u16Mirror(0x2C48) == 0x1234);
        }

    }
    TEST_CASE("HSUP_u32Mirror")
    {   // Intel implementation, but it is the same than CortexM0/M23
        SUBCASE("HSUP_u32Mirror(0)")
        {
            CHECK(HSUP_u32Mirror(0) == 0);
        }
        SUBCASE("HSUP_u32Mirror(0xFFFFFFFF)")
        {
            CHECK(HSUP_u32Mirror(0xFFFFFFFF) == 0xFFFFFFFF);
        }
        SUBCASE("HSUP_u32Mirror(0x12345678)")
        {
            CHECK(HSUP_u32Mirror(0x12345678) == 0x1E6A2C48);
        }
        SUBCASE("HSUP_u32Mirror(0x1E6A2C48)")
        {
            CHECK(HSUP_u32Mirror(0x1E6A2C48) == 0x12345678);
        }

    }

/* the following code tests the generic implementation of HSUP */

    TEST_CASE("HSUP_ucGetHighByte")
    {
        SUBCASE("HSUP_ucGetHighByte(0x1234)")
        {
            CHECK(HSUP_ucGetHighByte(0x1234) == 0x12);
        }
        SUBCASE("HSUP_ucGetHighByte(0x0034)")
        {
            CHECK(HSUP_ucGetHighByte(0x0034) == 0x00);
        }
    }

    TEST_CASE("HSUP_ucGetLowByte")
    {
        SUBCASE("HSUP_ucGetLowByte(0x1234)")
        {
            CHECK(HSUP_ucGetLowByte(0x1234) == 0x34);
        }
        SUBCASE("HSUP_ucGetLowByte(0x0034)")
        {
            CHECK(HSUP_ucGetLowByte(0x3400) == 0x00);
        }
    }

    TEST_CASE("HSUP_vSetHighByte")
    {
        SUBCASE("HSUP_vSetHighByte(0x1234, 0)")
        {
            uint16_t value = 0x1234;
            HSUP_vSetHighByte(&value, 0x00);
            CHECK(value == 0x34);
        }
        SUBCASE("HSUP_vSetHighByte(0x9876, 0x55)")
        {
            uint16_t value = 0x9876;
            HSUP_vSetHighByte(&value, 0x55);
            CHECK(value == 0x5576);
        }
    }
    TEST_CASE("HSUP_vSetLowByte")
    {
        SUBCASE("HSUP_vSetLowByte(0x1234, 0)")
        {
            uint16_t value = 0x1234;
            HSUP_vSetLowByte(&value, 0x00);
            CHECK(value == 0x1200);
        }
        SUBCASE("HSUP_vSetLowByte(0x9876, 0x55)")
        {
            uint16_t value = 0x9876;
            HSUP_vSetLowByte(&value, 0x55);
            CHECK(value == 0x9855);
        }
    }
    
    TEST_CASE("HSUP_uiGetHighWord")
    {
        SUBCASE("HSUP_uiGetHighWord(0x12345678)")
        {
            CHECK(HSUP_uiGetHighWord(0x12345678) == 0x1234);
        }
        SUBCASE("HSUP_uiGetHighWord(0x34)")
        {
            CHECK(HSUP_uiGetHighWord(0x34) == 0x00);
        }
    }
    TEST_CASE("HSUP_uiGetLowWord")
    {
        SUBCASE("HSUP_uiGetLowWord(0x12345678)")
        {
            CHECK(HSUP_uiGetLowWord(0x12345678) == 0x5678);
        }
        SUBCASE("HSUP_uiGetLowWord(0x3412)")
        {
            CHECK(HSUP_uiGetLowWord(0x3412) == 0x3412);
        }
    }
    TEST_CASE("HSUP_vSetHighWord")
    {
        SUBCASE("HSUP_vSetHighWord(0x12345678, 0)")
        {
            uint32_t value = 0x12345678;
            HSUP_vSetHighWord(&value, 0x00);
            CHECK(value == 0x5678);
        }
        SUBCASE("HSUP_vSetHighWord(0x98765432, 0x55)")
        {
            uint32_t value = 0x98765432;
            HSUP_vSetHighWord(&value, 0xAA);
            CHECK(value == 0x00AA5432);
        }
    }

    TEST_CASE("HSUP_vSetLowWord")
    {
        SUBCASE("HSUP_vSetLowWord(0x12345678, 0)")
        {
            uint32_t value = 0x12345678;
            HSUP_vSetLowWord(&value, 0x00);
            CHECK(value == 0x12340000);
        }
        SUBCASE("HSUP_vSetLowWord(0x98765432, 0xAA)")
        {
            uint32_t value = 0x98765432;
            HSUP_vSetLowWord(&value, 0xAA);
            CHECK(value == 0x987600AA);
        }
    }




}

