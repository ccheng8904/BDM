/*******************************************************************************
 *  Copyright (c) 2024 BSH Hausgeraete GmbH,
 *  Carl-Wery-Str. 34, 81739 Munich, Germany, www.bsh-group.de
 *
 *  All rights reserved. This program and the accompanying materials
 *  are protected by international copyright laws.
 *  Please contact copyright holder for licensing information.
 *
 *******************************************************************************
 *  PROJECT          Generic SW
 ******************************************************************************/


/******************************************************************************/
/* DOCUMENTATION                                                              */
/******************************************************************************/
/** \file
 *
 *  \brief    Implementation of system tick stopwatch STSW for tau.
 *
 */


/******************************************************************************/
/* INCLUDES                                                                   */
/******************************************************************************/
#include "bsh_stdinc.h"
#include <stdint.h>
#include "systick_stopwatch.h"
#include "hsup.h"
#include "processor.h"

/******************************************************************************/
/* PRIVATE DEFINITIONS                                                        */
/******************************************************************************/


/******************************************************************************/
/* PRIVATE FUNCTION DECLARATIONS                                              */
/******************************************************************************/
static bool STSW_isUnhandledOverflow(uint32_t ticks);


/******************************************************************************/
/* PRIVATE DATA DEFINITIONS                                                   */
/******************************************************************************/
static const uint32_t STSW_tick_reload = 0xFFFFFF; ///< reload value of systick timer
static uint64_t STSW_time;                         ///< time in ns since start
static uint32_t STSW_SysTickDisableInt = SysTick_CTRL_CLKSOURCE_Msk | SysTick_CTRL_ENABLE_Msk;
static uint32_t STSW_SysTickEnableInt  = SysTick_CTRL_CLKSOURCE_Msk | SysTick_CTRL_ENABLE_Msk | SysTick_CTRL_TICKINT_Msk;

/******************************************************************************/
/* PUBLIC FUNCTION DEFINITIONS                                                */
/******************************************************************************/

void STSW_ClockInit(void)
{
    STSW_time = 0;
    SysTick->CTRL = 0;
    SysTick->LOAD = STSW_tick_reload;
    SysTick->VAL  = 0;
    SysTick->CTRL = STSW_SysTickEnableInt;
    NVIC_EnableIRQ(SysTick_IRQn); 
}

uint64_t STSW_getTime(void)
{
    SysTick->CTRL  = STSW_SysTickDisableInt;
    uint32_t ticks = STSW_tick_reload;
    ticks -= SysTick->VAL;
    if(STSW_isUnhandledOverflow(ticks))
    {
        STSW_InterruptHandler();
    }
    uint64_t time  = STSW_time; // cumulated overflow 
    time += (uint32_t)(((uint32_t)((ticks * 100UL) / CORE_CLOCK)) * 10UL);
    SysTick->CTRL = STSW_SysTickEnableInt;
    return time;
}


struct STSW_timeStruct STSW_getStopTime(uint64_t StartTime)
{
    const uint64_t mil = 1000UL;
    struct STSW_timeStruct result = {0,0,0,0};
    uint64_t elapsed = STSW_getTime() - StartTime;
    result.ns = (uint16_t)(elapsed % mil);
    elapsed  /= mil; // us
    result.us = (uint16_t)(elapsed % mil);
    elapsed  /= mil; // ms
    result.ms = (uint16_t)(elapsed % mil);
    result.s  = (uint32_t)(elapsed / mil);
    return result;
}



void STSW_InterruptHandler(void)
{   
    const uint32_t periode = ((0x1000000UL * 100UL) / CORE_CLOCK) * 10UL;
    STSW_time += periode;
    SysTick->CTRL; // read int flag by reading it
}


/******************************************************************************/
/* PRIVATE FUNCTION DEFINITIONS                                               */
/******************************************************************************/

static bool STSW_isUnhandledOverflow(uint32_t ticks)
{
    if (SysTick->CTRL & SysTick_CTRL_COUNTFLAG_Msk) // unhandled interrupt 
    {   // check if flag has been set before/after timer value has been read
        return (ticks < (uint32_t)(STSW_tick_reload/2UL));
    }
    return false;
}
