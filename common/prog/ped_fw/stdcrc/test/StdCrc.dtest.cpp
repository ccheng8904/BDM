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

#include "doctest/doctest.h"
#include "../StdCrc.h"
#include "../CrcCom.h"



TEST_SUITE("CRC")
{
    const uint16_t CRC_test_ccittStartValue = 0xFFFF;
    const uint16_t CRC_test_ccittAnyStartValue = 0x4711;
    const uint8_t  CRC_test_buffer[18]{ 0x31,0x32,0x33,0x34,0x35,0x36,0x37,0x38,0x39,0x31,0x32,0x33,0x34,0x35,0x36,0x37,0x38,0x39 };
    const uint8_t  CRC_test_anyData = 0x55;
    const uint8_t  CRC_test_evenAnyData = 0xAA;

    const uint32_t CRC_test_ethernetStartValue = 0xFFFFFFFF;
    const uint32_t CRC_test_ethernetAnyStartValue = 0x47110815;

    TEST_CASE("CRC_Ccitt interfaces")
    {
        uint16_t crc = CRC_Ccitt_Init(); // executed for each subcase
    
        SUBCASE("CRC_Ccitt_Init: Check the start value")
        {
            CHECK(crc == CRC_test_ccittStartValue);
        }
        SUBCASE("CRC_Ccitt_UpdateByte: Check update with standard start value")
        {
            CHECK(CRC_Ccitt_UpdateByte(crc,CRC_test_buffer[0]) == 0xC782);
        }
        SUBCASE("CRC_Ccitt_UpdateByte: Check update with random start value and data")
        {
            CHECK(CRC_Ccitt_UpdateByte(CRC_test_ccittAnyStartValue,CRC_test_anyData) == 0x2373);
        }
        SUBCASE("CRC_Ccitt_UpdateByteFast: Check update with standard start value")
        {
            CHECK(CRC_Ccitt_UpdateByteFast(crc,CRC_test_buffer[0]) == 0xC782);
        }
        SUBCASE("CRC_Ccitt_UpdateByteFast: Check update with random start value and data")
        {
            CHECK(CRC_Ccitt_UpdateByteFast(CRC_test_ccittAnyStartValue,CRC_test_anyData) == 0x2373);
        }

        SUBCASE("CRC_Ccitt_Update: Check with standard pattern '123456789'")
        {
            CHECK(CRC_Ccitt_Update(crc, 9, CRC_test_buffer) == 0x29B1);
        }

        SUBCASE("CRC_Ccitt_Update: Check with standard pattern unaligned '123456789'")
        {
            CHECK(CRC_Ccitt_Update(crc, 9, &CRC_test_buffer[9]) == 0x29B1);
        }

        SUBCASE("CRC_Ccitt_Finish: For CCITT finish does not change CRC")
        {
            CHECK(CRC_Ccitt_Finish(CRC_test_anyData) == CRC_test_anyData);
        }

        SUBCASE("CRC_Ccitt_Finish: For CCITT finish does not change CRC, another CRC input")
        {
            CHECK(CRC_Ccitt_Finish(CRC_test_evenAnyData) == CRC_test_evenAnyData);
        }

        SUBCASE("CRC_Ccitt_Calc: Check with standard pattern '123456789'")
        {
            CHECK(CRC_Ccitt_Calc(9, CRC_test_buffer) == 0x29B1);
        }

        SUBCASE("CRC_Ccitt_Calc: Check with standard pattern unaligned '123456789'")
        {
            CHECK(CRC_Ccitt_Calc(9, &CRC_test_buffer[9]) == 0x29B1);
        }

        SUBCASE("CRC_CcittCalcByte8BitMachine: Check with standard pattern '123456789'")
        {   // Hint this API interface is a special for 8Bit machines, but for sure it might be tested on any machine
            for (size_t i = 0; i < 9; i++)
            {
                crc = CRC_CcittCalcByte8BitMachine(crc, CRC_test_buffer[i]);
            }
            
            CHECK(crc == 0x29B1);
        }


    }
    
    TEST_CASE("CRC_Ethernet interfaces")
    {
        uint32_t crc = CRC_Ethernet_Init(); // executed for each subcase
        
        SUBCASE("CRC_Ethernet_Init: Check the start value")
        {
            CHECK(crc == CRC_test_ethernetStartValue);
        }
        SUBCASE("CRC_Ethernet_UpdateByte: Check update with standard start value")
        {
            CHECK(CRC_Ethernet_UpdateByte(crc,CRC_test_buffer[0]) == (uint32_t)~0x83DCEFB7UL);
        }
        
        SUBCASE("CRC_Ethernet_Update: Check with standard pattern '123456789'")
        {
            CHECK(CRC_Ethernet_Update(CRC_test_ethernetStartValue, 9, CRC_test_buffer) ==(uint32_t)~0xCBF43926);
        }
        
        SUBCASE("CRC_Ethernet_Calc: Check with standard pattern '123456789'")
        {
            CHECK(CRC_Ethernet_Calc(9, CRC_test_buffer) == 0xCBF43926);
        }
    }

}
