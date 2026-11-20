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
 *  COMP_ABBREV      SysTick
 ******************************************************************************/


/******************************************************************************/
/* DOCUMENTATION                                                              */
/******************************************************************************/
/** \file
 *
 *  \brief    Public definitions and declarations for unit SysTick.
 *
 */

/******************************************************************************/
/* INCLUDES                                                                   */
/******************************************************************************/
#include "systick.h"
#include "processor.h"
#include "math/mathDiv.h"


/******************************************************************************/
/* PRIVATE DATA DEFINITIONS                                                   */
/******************************************************************************/

/* Variable for storing actual SysTick period */
static uint16_t SysTick_period = 0;

/******************************************************************************/
/* PUBLIC FUNCTION DEFINITIONS                                                */
/******************************************************************************/

uint16_t SysTick_GetPeriod(void)
{
    return SysTick_period;
}


int32_t SysTick_Configure(uint32_t sysClock_Hz, uint16_t period_ms)
{
    uint32_t ticks;
    
    // Update period variable
    SysTick_period = period_ms;
    // Calculate number of ticks for one period
    ticks = (MTDIV_div_32_32((uint32_t)sysClock_Hz,1000U)*period_ms)-1U;
    // Check if value is valid
    if(ticks > SysTick_LOAD_RELOAD_Msk)
    {
        return -1;
    }
    
    // Configure SysTick
    SysTick->LOAD  = ticks;                         /* set reload register */
    SysTick->VAL   = 0U;                            /* Load the SysTick Counter Value */
    SysTick->CTRL  = SysTick_CTRL_CLKSOURCE_Msk |
                     SysTick_CTRL_TICKINT_Msk   |
                     SysTick_CTRL_ENABLE_Msk;       /* Enable SysTick IRQ and SysTick Timer */

    return (int32_t)ticks;
}
