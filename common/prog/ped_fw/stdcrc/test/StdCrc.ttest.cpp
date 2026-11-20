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



/******************************************************************************/
/* INCLUDES                                                                   */
/******************************************************************************/
#include <stdint.h>
#include "../StdCrc.h"
#ifndef __NOT_CC__ // no lint please
#include <tau/tau.h>


const uint16_t CRC_test_ccittStartValue = 0xFFFF;
const uint16_t CRC_test_ccittAnyStartValue = 0x4711;
const uint8_t  CRC_test_buffer[18]{ 0x31,0x32,0x33,0x34,0x35,0x36,0x37,0x38,0x39,0x31,0x32,0x33,0x34,0x35,0x36,0x37,0x38,0x39 };
const uint8_t  CRC_test_anyData = 0x55;
const uint8_t  CRC_test_evenAnyData = 0xAA;


const uint32_t CRC_test_ethernetStartValue = 0xFFFFFFFF;
const uint32_t CRC_test_ethernetAnyStartValue = 0x47110815;



TEST(StdCrcCcitt, CRC_Ccitt_Init)
{
    CHECK_EQ(CRC_Ccitt_Init(),CRC_test_ccittStartValue);
}


TEST(StdCrcCcitt, CRC_Ccitt_UpdateByte_0)
{
    uint16_t crc = CRC_Ccitt_Init();
    CHECK_EQ(CRC_Ccitt_UpdateByte(crc,CRC_test_buffer[0]),0xC782);
}

TEST(StdCrcCcitt, CRC_Ccitt_UpdateByte_1)
{
    (void)CRC_Ccitt_Init();
    CHECK_EQ(CRC_Ccitt_UpdateByte(CRC_test_ccittAnyStartValue,CRC_test_anyData),0x2373);
}


TEST(StdCrcCcitt, CRC_Ccitt_UpdateByteFast_0)
{
    uint16_t crc = CRC_Ccitt_Init();
    CHECK_EQ(CRC_Ccitt_UpdateByteFast(crc,CRC_test_buffer[0]),0xC782);
}

TEST(StdCrcCcitt, CRC_Ccitt_UpdateByteFast_1)
{
    (void)CRC_Ccitt_Init();
    CHECK_EQ(CRC_Ccitt_UpdateByteFast(CRC_test_ccittAnyStartValue,CRC_test_anyData),0x2373);
}

TEST(StdCrcCcitt, CRC_Ccitt_Update_StdTest)
{
    (void)CRC_Ccitt_Init();
    CHECK_EQ(CRC_Ccitt_Update(CRC_test_ccittStartValue, 9, CRC_test_buffer),0x29B1);
}

TEST(StdCrcCcitt, CRC_Ccitt_Update_StdTest_unalligned)
{
    (void)CRC_Ccitt_Init();
    CHECK_EQ(CRC_Ccitt_Update(CRC_test_ccittStartValue, 9, &CRC_test_buffer[9]),0x29B1);
}

TEST(StdCrcCcitt, CRC_Ccitt_Finish_0)
{
    (void)CRC_Ccitt_Init();
    CHECK_EQ(CRC_Ccitt_Finish(CRC_test_anyData),CRC_test_anyData);
}


TEST(StdCrcCcitt, CRC_Ccitt_Finish_1)
{
    (void)CRC_Ccitt_Init();
    CHECK_EQ(CRC_Ccitt_Finish(CRC_test_evenAnyData),CRC_test_evenAnyData);
}

TEST(StdCrcCcitt, CRC_Ccitt_Calc_StdTest)
{
    CHECK_EQ(CRC_Ccitt_Calc(9, CRC_test_buffer),0x29B1);
}

TEST(StdCrcCcitt, CRC_Ccitt_Calc_StdTest_unalligned)
{
    CHECK_EQ(CRC_Ccitt_Calc(9, &CRC_test_buffer[9]),0x29B1);
}





TEST(StdCrcEthernet, CRC_Ethernet_Init)
{
    CHECK_EQ(CRC_Ethernet_Init(),CRC_test_ethernetStartValue);
}


TEST(StdCrcEthernet, CRC_Ethernet_UpdateByte_0)
{
    uint32_t crc = CRC_Ethernet_Init();
    CHECK_EQ(CRC_Ethernet_UpdateByte(crc,CRC_test_buffer[0]),(uint32_t)~0x83DCEFB7);
}


TEST(StdCrcEthernet, CRC_Ethernet_UpdateByte_1)
{
    (void)CRC_Ethernet_Init();
    uint32_t crc = CRC_Ethernet_UpdateByte(CRC_test_ethernetAnyStartValue,CRC_test_anyData);
    CHECK_EQ(crc,0x55AD36A7);
}


TEST(StdCrcEthernet, CRC_Ethernet_Update_StdTest)
{
    (void)CRC_Ethernet_Init();
    CHECK_EQ(CRC_Ethernet_Update(CRC_test_ethernetStartValue, 9, CRC_test_buffer),(uint32_t)~0xCBF43926);
}

TEST(StdCrcEthernet, CRC_Ethernet_Update_StdTest_unalligned)
{
    (void)CRC_Ethernet_Init();
    CHECK_EQ(CRC_Ethernet_Update(CRC_test_ethernetStartValue, 9, &CRC_test_buffer[9]),(uint32_t)~0xCBF43926);
}

TEST(StdCrcEthernet, CRC_Ethernet_Finish_0)
{
    (void)CRC_Ethernet_Init();
    CHECK_EQ(CRC_Ethernet_Finish(CRC_test_anyData),(uint32_t)~CRC_test_anyData);
}


TEST(StdCrcEthernet, CRC_Ethernet_Finish_1)
{
    (void)CRC_Ethernet_Init();
    CHECK_EQ(CRC_Ethernet_Finish(CRC_test_evenAnyData),(uint32_t)~CRC_test_evenAnyData);
}

TEST(StdCrcEthernet, CRC_Ethernet_Calc_StdTest)
{
    CHECK_EQ(CRC_Ethernet_Calc(9, CRC_test_buffer),0xCBF43926);
}

TEST(StdCrcEthernet, CRC_Ethernet_Calc_StdTest_unalligned)
{
    CHECK_EQ(CRC_Ethernet_Calc(9, &CRC_test_buffer[9]),0xCBF43926);
}

#endif