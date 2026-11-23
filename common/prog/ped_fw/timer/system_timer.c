/*******************************************************************************
 *  Copyright (c) 2017 BSH Hausgeraete GmbH,
 *  Carl-Wery-Str. 34, 81739 Munich, Germany, www.bsh-group.de
 *
 *  All rights reserved. This program and the accompanying materials
 *  are protected by international copyright laws.
 *  Please contact copyright holder for licensing information.
 *
 *******************************************************************************
 *  PROJECT          System Timer, Generic SW
 *  COMP_ABBREV      STIM
 ******************************************************************************/


/******************************************************************************/
/* DOCUMENTATION                                                              */
/******************************************************************************/
/** \file
 *
 *  \brief    Implementation of STIM.
 *
 *  \details  System timer library serves for single or periodic time
 *            measurement and callback execution with resolution down to one
 *            millisecond. System timer library contains two types of execution
 *            handlers, interrupt handler and scheduler handler.
 *
 *            Scheduler handler is used for processing of less accurate timers.
 *            Accuracy of these timers is given by time period of execution of
 *            this handler. If accuracy of these timers is not sufficient, then
 *            interrupt based timers (see next section) should be used instead.
 *
 *            Interrupt handler is called from hardware timer interrupt service
 *            routine with period of integer value in milliseconds. This
 *            handler is used for base time update and for processing of
 *            accurate interrupt base timers (including callback execution).
 *            For all ARM Cortex-M micro controllers, SysTick timer is used by
 *            default for this purpose.
 *
 *            Interrupt based timers should only be used if really necessary,
 *            and respective callback functions should be kept as light as
 *            possible. Otherwise they can have impact on other real-time
 *            functions (UART/DBus handling, RTOS scheduling, ...)
 *
 */


/******************************************************************************/
/* INCLUDES                                                                   */
/******************************************************************************/
#include "system_timer.h"
#include "processor.h"
#ifdef SYSTEM_TIMER_SYSTICK
    #include "systick.h"
#else
    #include "system_timer_cfg.h"
#endif
#ifdef MCAL_MPCM_INCLUDED
    #include "mcal/mpcm.h"
#endif
#include "timerlib.h"
//#include <assert.h>

/******************************************************************************/
/* PRIVATE DEFINITIONS                                                        */
/******************************************************************************/

/* Definitions for time calculations from ms to us */
#define FIXED_POINT_SHIFT   25u
#define TICKS_PER_MS        1000u


/******************************************************************************/
/* PRIVATE DATA DEFINITIONS                                                   */
/******************************************************************************/

/* Pointer to the linked list of timers handled in interrupts */
static struct STIM_Timer *STIM_interruptTimerList = NULL;

/* Pointer to the linked list of timers handled in scheduler */
static struct STIM_Timer *STIM_schedulerTimerList = NULL;

/* Elapsed time variable since system timer initialization */
static volatile uint32_t STIM_tick = 0U;


/******************************************************************************/
/* PRIVATE FUNCTION DECLARATIONS AND PRIVATE MACRO FUNCTION DEFINITIONS       */
/******************************************************************************/

/** \brief    Function registering timer instance into lined list of timers.
 *
 *  \param    timer:        pointer to timer instance to be added to lined list
 *  \param    processingType:   enumeration type for setting processing type
 */
static int32_t STIM_RegisterTimer(struct STIM_Timer *timer, enum STIM_Processing processingType);


/** \brief    Function processing timer instances.
 *
 *  \param    timerPtr:     pointer to first timer instance in timer lined list
 *  \param    timeDiff_ms:  time elapsed since last processing
 */
static void STIM_ProcessTimers(struct STIM_Timer *timerPtr, uint16_t timeDiff_ms);


/** \brief    Function for checking if target time of timer is set.
 *
 *  \param    timer: pointer to timer instance
 *
 *  \return   true, if target time is set
 */
static bool STIM_IsTargetTimeSet(const struct STIM_Timer *timer)
{
    return timer->targetTime_ms != 0U;
}


/** \brief    Function for checking if the timer is triggered.
 *
 *  \param    timer: pointer to timer instance
 *
 *  \return   true, if the timer is triggered
 */
static bool STIM_IsTimerTriggered(const struct STIM_Timer *timer)
{
    return timer->currentTime_ms >= timer->targetTime_ms;
}


/** \brief    Function returns time difference in ms since last function call.
 *
 *  Time difference is calculated based on difference of actual timestamp and
 *  timestamp updated from last function call. For calculating time difference
 *  in milliseconds, fixed point arithmetic multiplication is used:
 *  timeDiff_ms = (timeDiff_us * ceil(2^25 / 1000)) / 2^25
 *  This calculation is precise up to 60 ms with 25 bit fixedPointShift.
 *  Maximum returned elapsed time is 64 ms.
 *
 *  \param    timeStamp: pointer static timestamp value for time keeping
 *
 *  \return   millisecond time difference since last update
 */
static uint16_t STIM_GetElapsedTimeMs(uint16_t *timeStamp)
{
    const uint8_t fixedPointShift = FIXED_POINT_SHIFT;
    const uint16_t ticksPerMs = TICKS_PER_MS;
    const uint32_t msMulConst = (((uint32_t)1u << FIXED_POINT_SHIFT) / TICKS_PER_MS) + 1u;
    
    uint16_t elapsedTime_us = TIM_uiGetCircleMicroSeconds() - *timeStamp;
    uint16_t elapsedTime_ms = (uint16_t)((elapsedTime_us * msMulConst) >> fixedPointShift); 
    *timeStamp += (uint16_t)(elapsedTime_ms * ticksPerMs);
    
    return elapsedTime_ms;
}


/** \brief    Function executing callback functions of timer instance.
 *
 *  \param    timer:    pointer to timer instance
 *  \param    status:   actual timer status flags
 */
static void STIM_ExecuteCallbacks(const struct STIM_Timer *timer, uint8_t status)
{
   (void) STDCB_ExecuteCallbacks(timer->cbList, (uint32_t)status, 0);
}


#ifdef MCAL_MPCM_INCLUDED
/** \brief    Callback function that is executed when clock frequency is changed
 *
 *  \param    objPtr:       callback object pointer - NULL
 *  \param    eventFlags:   MPCM status flags - MPCM_STATUS_CLOCK_POST
 *  \param    eventData:    integer number of new power mode
 *  \return   always 0
 */
/*lint -e818 Pointer parameter could be declared as pointing to const */
static void STIM_MPCM_CallbackFunction(void *objPtr, uint32_t eventFlags, const struct MCAL_EventResponse *eventResponse)
{
    (void)eventFlags;
    (void)*eventResponse;

    if(objPtr == NULL)
    {
        uint32_t coreClock_Hz;
        coreClock_Hz = MPCM_getClockFreq(MPCM_CORE_CLOCK);
        (void)SysTick_Configure(coreClock_Hz, SYSTEM_TIMER_PERIOD);
    }
}
#endif

/******************************************************************************/
/* PUBLIC FUNCTION DEFINITIONS                                                */
/******************************************************************************/

int32_t STIM_InitModule(void)
{
#ifdef SYSTEM_TIMER_SYSTICK
    uint32_t coreClock_Hz;
    
  #ifdef MCAL_MPCM_INCLUDED
    /* Initialize callback and attach it to clock change event */
    static MCAL_Callback_t STIM_OnClockChangeCB;
    MCAL_initCallback(&STIM_OnClockChangeCB, STIM_MPCM_CallbackFunction, NULL);
    (void)MPCM_enableEvent(&STIM_OnClockChangeCB, MCAL_EVENT_CLOCK_CHANGE_POST);
    /* Get actual core clock frequency */
    coreClock_Hz = MPCM_getClockFreq(MPCM_CORE_CLOCK);
  #else
    #ifdef CLOCK_IN_KHZ
        coreClock_Hz = CORE_CLOCK * 1000U;
    #else
        coreClock_Hz = CORE_CLOCK * 1000000U;
    #endif
  #endif
    
    return SysTick_Configure(coreClock_Hz, SYSTEM_TIMER_PERIOD);
#else
    return STIM_HwConfigure(SYSTEM_TIMER_PERIOD);
#endif
}


// alias for STM32 HAL drivers
#ifdef SYSTEM_TIMER_SYSTICK
extern uint32_t HAL_GetTick(void);
uint32_t HAL_GetTick(void)
{
    return STIM_tick;
}
#endif
uint32_t STIM_GetTick(void)
{
    return STIM_tick;
}


void STIM_InterruptHandler(uint16_t interruptPeriod_ms)
{
    (void)interruptPeriod_ms;
    
    static uint16_t interruptTimeStamp = 0u;
    uint16_t elapsedTime_ms = STIM_GetElapsedTimeMs(&interruptTimeStamp);
    
    if(elapsedTime_ms > 0U)
    {
        /* Update circular millisecond timer */
        STIM_tick += elapsedTime_ms;
        /* Process IRQ timers */
        STIM_ProcessTimers(STIM_interruptTimerList, elapsedTime_ms);
    }
}


void STIM_SchedulerHandleTask(void)
{
    static uint16_t schedulerTimeStamp = 0u;
    uint16_t elapsedTime_ms = STIM_GetElapsedTimeMs(&schedulerTimeStamp);
    
    if(elapsedTime_ms > 0U)
    {
        /* Process Scheduler timers */
        STIM_ProcessTimers(STIM_schedulerTimerList, elapsedTime_ms);
    }
}


int32_t STIM_InitTimer(struct STIM_Timer *timer, enum STIM_Processing processingType,
    STIM_Time_t targetTime_ms, enum STIM_Mode timerMode, bool immediateStart, struct STDCB_Callback *callback)
{
    //assert(timer); //Currently disabled, because of LINT issues
    
    int32_t retVal = -1;
    
    if(timer != NULL)
    {
        retVal = STIM_RegisterTimer(timer, processingType);
        
        // If timer was successfully registered, initialize him
        if(retVal >= 0)
        {
            STIM_SetTargetTime(timer, targetTime_ms);
            
            STIM_SetTimerMode(timer, timerMode);
            
            timer->cbList = callback;
            
            if(immediateStart)
            {
                STIM_EnableTimer(timer);
            }
        }
    }
    return retVal;
}


/******************************************************************************/
/* PRIVATE FUNCTION DEFINITIONS                                               */
/******************************************************************************/

static void STIM_ProcessTimers(struct STIM_Timer *timerPtr, uint16_t timeDiff_ms)
{
    STIM_Time_t timerTime;
    uint8_t timerStatus;
    
    /* no processing needed if timeDiff_ms is 0 */
    if(timeDiff_ms == 0U)
    {
        return;
    }
    
    /* process until last (NULL) timer pointer is reached */
    while(timerPtr != NULL)
    {
        if(timerPtr->enabled == true)
        {
            // Clear status and calculate new time
            timerStatus = 0U;
            timerTime = timerPtr->currentTime_ms + timeDiff_ms;
            
            // Check for timer overflow
            if(timerPtr->currentTime_ms > timerTime)
            {
                timerStatus |= STIM_STATUS_OVERFLOW;
                STIM_DisableTimer(timerPtr);
            }
            timerPtr->currentTime_ms = timerTime;
            
            if(STIM_IsTargetTimeSet(timerPtr))
            {
                // Check if timer is triggered
                if(STIM_IsTimerTriggered(timerPtr))
                {
                    timerStatus |= STIM_STATUS_TRIGGERED;
                    
                    // If timer mode is single, stop timer
                    if(timerPtr->mode == STIM_MODE_SINGLE)
                    {
                        STIM_DisableTimer(timerPtr);
                    }
                    else
                    {
                        // Decrement timer by target time
                        timerPtr->currentTime_ms -= timerPtr->targetTime_ms;
                        
                        // Check again for for missed trigger
                        if(STIM_IsTimerTriggered(timerPtr))
                        {
                            timerStatus |= STIM_STATUS_MISSED;
                        }
                    }
                }
            }
            
            // In case status is not 0, execute callbacks
            if(timerStatus != 0U)
            {
                timerPtr->status |= timerStatus;
                STIM_ExecuteCallbacks(timerPtr, timerStatus);
            }
        }
        
        // set pointer to next timer instance
        timerPtr = timerPtr->nextTimer;
    }
}


static int32_t STIM_RegisterTimer(struct STIM_Timer *timer, enum STIM_Processing processingType)
{
    struct STIM_Timer **timerList;
    struct STIM_Timer *timerPtr;
    
    timerList = (processingType == STIM_PROCESSING_INTERRUPT) ? &STIM_interruptTimerList : &STIM_schedulerTimerList;
    
    if(*timerList == NULL)
    {
        *timerList = timer;
        timer->nextTimer = NULL;
    }
    else
    {
        // Check for same timer instance pointer
        // AXIVION Next Line MisraC2012-14.2 : Infinite loop must be of form 'for ( ; ; )'
        for(timerPtr = *timerList; /* No condition */; timerPtr = timerPtr->nextTimer)
        {
            if(timerPtr == timer)
            {
                return -1;
            }
            
            /* Break at the end of list */
            if(timerPtr->nextTimer == NULL)
            {
                break;
            }
        }
        
        // Add timer instance to the end of list
        timerPtr->nextTimer = timer;
        timer->nextTimer = NULL;
    }
    return 0;
}
