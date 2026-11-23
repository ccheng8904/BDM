/*******************************************************************************
 *  Copyright (c) 2023 BSH Hausgeraete GmbH,
 *  Carl-Wery-Str. 34, 81739 Munich, Germany, www.bsh-group.de
 *
 *  All rights reserved. This program and the accompanying materials
 *  are protected by international copyright laws.
 *  Please contact copyright holder for licensing information.
 *
 *******************************************************************************
 *  PROJECT          MCU Framework
 *  COMP_ABBREV      TIM
 ******************************************************************************/


/******************************************************************************/
/* DOCUMENTATION                                                              */
/******************************************************************************/
/** \file       timerMcXS.c
    \brief      CMS32M67xx specific configuration template for timer library.
*
*   \details    Uses TMR0, the 32 bit reload timer of the CMS32M67xx. The
*               timer is clocked by the main clock and serves the 1us time
*               base of the timer library.
*/

/******************************************************************************/
/* INCLUDES                                                                   */
/******************************************************************************/
#include "bsh_stdinc.h"
#include "Libtypes.h"
#include "timerlib.h"
#include "processor.h"
#include <stdint.h>

/* How to set CORE_CLOCK and PCLOCK, PCLOCK1 -> in the build process see common_config.mak */



/* DEFINITIONS ****************************************************************************************/

/* The 32 bit reload timer counts the main clock cycles, the upper 16 bit are
 * used as microsecond circle. */
#define TIM_CIRCLE_SHIFT        0U

BOOL _TIM_bGetInitConfig(void)
{
    /* the timer runs at the core clock, each tick is one clock cycle */
    CGC_PER11PeriphClockCmd(CGC_PER11Periph_TIMER01, ENABLE);

    /* 16 bit mode is not used, the timer counts continuously */
    TMR_ConfigClk(TMR0, TMR_CLK_DIV_1);
    TMR_ConfigRunMode(TMR0, TMR_COUNT_CONTINUONS_MODE, TMR_BIT_16_MODE);
    TMR_DisableOneShotMode(TMR0);
    TMR_SetPeriod(TMR0, 0xFFFFU);
    TMR_Start(TMR0);

    return TRUE;
}

// Axivion Next Line MisraC2012-8.3: Return type at definition differs from type at declaration
uint16_t TIM_uiGetCircleMicroSeconds(void)
{
    return (uint16_t)(TMR_GetTimingData(TMR0));
}

#ifdef MCAL_MPCM_INCLUDED
/* The timer is clocked by the main clock, the reload value does not have to be
 * recalculated when the clock is changed. The callback is required by the
 * timer library interface. */
// Axivion Next Line MisraC2012-8.13: Parameter can be declared as pointer/reference to const
void TIM_MpcmEventCallback(void *obj, uint32_t flags, const struct MCAL_EventResponse *eventResponse)
{
    (void) obj;
    (void) flags;
    (void) eventResponse;
}
#endif