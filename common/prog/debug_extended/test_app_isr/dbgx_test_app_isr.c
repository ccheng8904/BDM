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
 *  COMP_ABBREV      DBGXTS
 ******************************************************************************/


/******************************************************************************/
/* DOCUMENTATION                                                              */
/******************************************************************************/
/** \file
 *
 *  \brief    Test implementation of DBGXTS.
 *
 *  \details  Test of the api layer of the debug extended module.
 */


/******************************************************************************/
/* INCLUDES                                                                   */
/******************************************************************************/
#include "dbgx_test_app_isr.h"

#include "BSH_stdinc.h"

#include "mcal/mcal_includes.h"
#include "api_cfg.h"

/******************************************************************************/
/* PRIVATE DATA DEFINITIONS                                                   */
/******************************************************************************/


/******************************************************************************/
/* PRIVATE FUNCTION DECLARATIONS                                              */
/******************************************************************************/

static struct MTIM_Handle DBGXTS_TimerHandle;

const struct MTIM_TimerConfig DBGXTS_TimerConfig =
{
    32u,                        // Prescaler
    1000u,                      // Reload value
    MTIM_TIMER_DIRECTION_UP,
    MTIM_TIMER_MODE_TIMER
};

/* Timer channel mapping to physical timer unit, channel and pin (example mapping for STM32 MCUs) */
const struct MTIM_Channel *DBGXTS_CompareChannel   = &MTIM1_CH1;
uint32_t DBGXTS_TimerCount = 0;

static MCAL_Callback_t DBGXTS_TimerCallback;

char DBGXTS_InterruptText [26] = "Here is the interrupt !!!";

/******************************************************************************/
/* PRIVATE FUNCTION DEFINITIONS                                                */
/******************************************************************************/


/* Callback function for update event of timer */
void DBGXTS_TimerCallbackFunction(void *objPtr, uint32_t eventFlags, const struct MCAL_EventResponse *eventResponse)
{
    DBGX_logStr_NonFiltered(DBGXTS_InterruptText);
}

uint8_t DBGXTS_handleTask( void )
{
    static uint8_t DBGXTS_TaskState = TASK_NOT_INITIALISED;

    if (DBGXTS_TaskState == TASK_INITIALISED)
    {
        DBGXTS_TimerCount = MTIM_readTimerCount(&DBGXTS_TimerHandle);
    }
    else if (DBGXTS_TaskState == TASK_NOT_INITIALISED)
    {
        DBGX_init();
        (void)MTIM_initTimer(&DBGXTS_TimerHandle, &MTIM1, &DBGXTS_TimerConfig);
        /* Configure Channel 0 as compare - does not need external pin */
        (void)MTIM_initChannel(&DBGXTS_TimerHandle, DBGXTS_CompareChannel, &MTIM_CHANNEL_COMPARE);
        MCAL_initCallback(&DBGXTS_TimerCallback, DBGXTS_TimerCallbackFunction, NULL);
        (void)MTIM_enableTimerEvent(&DBGXTS_TimerHandle, &DBGXTS_TimerCallback);
        DBGXTS_TaskState = TASK_INITIALISED;
    }
    else if (DBGXTS_TaskState == TASK_TERMINATE)
    {
        // Do nothing
    }
    else
    {
        // Do nothing
    }

    return (DBGXTS_TaskState);
}


