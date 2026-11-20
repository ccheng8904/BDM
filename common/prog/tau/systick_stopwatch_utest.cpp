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

#include "systick_stopwatch.c"
#ifndef __NOT_CC__ // no lint please
#include <tau/tau.h>


TEST(systick_stopwatch, STSW_ClockInit)
{
    STSW_ClockInit();
    CHECK_EQ(SysTick->LOAD,STSW_tick_reload);
    CHECK_NE(SysTick->VAL,0u); // right after init it should be near reload value
    CHECK_EQ(SysTick->CTRL, SysTick_CTRL_CLKSOURCE_Msk | SysTick_CTRL_ENABLE_Msk | SysTick_CTRL_TICKINT_Msk);
}

TEST(systick_stopwatch, STSW_getTime)
{
    STSW_ClockInit();
    HSUP_vDelay(100);
    uint32_t time = (uint32_t)STSW_getTime();
    // test the expected range 100.000ns +/- 8%
    CHECK_GT(time,  92000UL); 
    CHECK_LT(time, 108000UL);
}

TEST(systick_stopwatch, STSW_getTime_overflow)
{   //overflow after 262ms = 262.000.000ns
    STSW_ClockInit();
    for(uint32_t i=0; i< 300; i++)
    {
        HSUP_vDelay(1000);
    }
    uint32_t time = STSW_getTime();
    // test the expected range 300ms +/- 8%
    CHECK_GT(time, 270000000UL); 
    CHECK_LT(time, 330000000UL);
}

TEST(systick_stopwatch, STSW_getTime_SomeOverflows)
{
    STSW_ClockInit();
    for(uint32_t i=0; i< 2000; i++)
    {
        HSUP_vDelay(1000);
    }
    uint32_t time = STSW_getTime();
    // test the expected range 2.0s +/- 8%
    CHECK_GT(time, 1900000000UL); 
    CHECK_LT(time, 2100000000UL);
}



TEST(systick_stopwatch, STSW_getTime_No_Overflow)
{
    STSW_ClockInit();
    for(uint32_t i=0; i< 100; i++)
    {
        HSUP_vDelay(1000);
    }
    uint32_t time = STSW_getTime();
    // test the expected range 100.000ns +/- 8%
    CHECK_GT(time,  92000000UL); 
    CHECK_LT(time, 108000000UL);
}


TEST(systick_stopwatch, STSW_getStopTime_0)
{
    struct STSW_timeStruct elapsed_time;
    SysTick->CTRL = SysTick_CTRL_CLKSOURCE_Msk ;
    SysTick->VAL  = STSW_tick_reload;
    HSUP_vDelay(10);
    SysTick->VAL  = STSW_tick_reload;
    STSW_time = 12345678UL;
    uint32_t time = STSW_getTime();
    elapsed_time = STSW_getStopTime(0);
    CHECK_EQ(time, 12345678UL);
    CHECK_EQ(elapsed_time.ns, 600UL);
    CHECK_EQ(elapsed_time.us, 345UL);
    CHECK_EQ(elapsed_time.ms, 12UL);
    CHECK_EQ(elapsed_time.s,  0UL);
}



#endif