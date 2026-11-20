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
    \brief      SH30F specific configuration template for timer library.
*
*   \details    Used TIM0 Basic 16 bit timer .
*/

/******************************************************************************/
/* INCLUDES                                                                   */
/******************************************************************************/
#include "bsh_stdinc.h"
#include "Libtypes.h"
#include "timerlib.h"
#include "processor.h"
#ifdef MCAL_MPCM_INCLUDED
    #include "mcal/mpcm.h"
#endif
#include "math/mathDiv.h"

/* How to set CORE_CLOCK and PCLOCK, PCLOCK1 -> in the build process see common_config.mak */



/* DEFINITIONS ****************************************************************************************/



// Used TIMER 0


BOOL _TIM_bGetInitConfig(void)
{
    uint32_t timer_psc;

    #ifdef MCAL_MPCM_INCLUDED
    uint32_t Periph_Clock_MHz = MTDIV_div_32_32(MPCM_getClockFreq(MPCM_CLOCK_PCLK0), 1000000u); ///< Peripheral clock (APB0 bus)

    timer_psc= Periph_Clock_MHz -1U;  //calculation with MPCM
    #else
    timer_psc= PCLOCK - 1U;          //calculation without MPCM
    #endif

    TIM_InitTypeDef TIM_InitStructure;
    TIM_StructInit(&TIM_InitStructure);
    RCC_APB0PeriphClockOnOff(RCC_APB0_TIM0, SH_ON);
    TIM_InitStructure.TIM_Prescaler = timer_psc;
    TIM_InitStructure.TIM_Period = 65535U;
    TIM_InitStructure.TIM_CLKSource = TIM_CLKSource_PCLK;
    TIM_InitStructure.TIM_OPMode = TIM_OPMode_Continue;
    TIM_InitStructure.TIM_TCMode = TIM_TCMode_Disable;
    TIM_InitStructure.TIM_ECFFilter = TIM_FLITER_TIMER_NONE;
    TIM_Init(TIM0, &TIM_InitStructure);
    TIM_INTConfig(TIM0, SH_ENABLE);
    TIM_OnOff(TIM0, SH_ON);

    return TRUE;
}

// Axivion Next Line MisraC2012-8.3: Return type at definition differs from type at declaration
uint16_t TIM_uiGetCircleMicroSeconds(void)
{
    return (uint16_t) (TIM0->TCNT);
}

#ifdef MCAL_MPCM_INCLUDED
// Axivion Next Line MisraC2012-8.13: Parameter can be declared as pointer/reference to const
void TIM_MpcmEventCallback(void *obj, uint32_t flags, const struct MCAL_EventResponse *eventResponse)
{
    (void) obj;
    (void) flags;

    if(MCAL_isEvent(eventResponse, MCAL_EVENT_CLOCK_CHANGE_PRE))
    {
        /* disable timer until pmode is changed */
        TIM0->CR.V32 &= (~TIM_CR_STR_Msk);
    }
    else if(MCAL_isEvent(eventResponse, MCAL_EVENT_CLOCK_CHANGE_POST))
    {
        /* configure and re-enable timer */
        (void)_TIM_bGetInitConfig();
    }
    else
    {
        /* Nothing to do */
    }
}
#endif
