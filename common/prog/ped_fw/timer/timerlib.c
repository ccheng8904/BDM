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
 *  COMP_ABBREV      TIM
 ******************************************************************************/


/******************************************************************************/
/* DOCUMENTATION                                                              */
/******************************************************************************/
/** \file
 *
 *  \brief    Implementation of timer library.
 *
 */


/******************************************************************************/
/* INCLUDES                                                                   */
/******************************************************************************/
#include "bsh_stdinc.h"
#include "timerlib.h"

#ifdef SYSTEM_TIMER_ENABLED
    #include "system_timer.h"
#endif

#ifdef MCAL_MPCM_INCLUDED
    #include "mcal/mpcm.h"
#endif
/**************************************************************************************************/
/* LOCAL DEFINITIONS                                                                              */
/**************************************************************************************************/
#define TIM_8BIT  0x00u ///< 8 bit timer types
#define TIM_16BIT 0x01u ///< 16 bit timer types

/* ALIGNMENT shall be defined to 1,2 or 4! */
#ifndef ALIGNMENT
    #error "Define 'ALIGNMENT' to either 1,2,4"
#else
    #if (ALIGNMENT != 1) && (ALIGNMENT != 2) && (ALIGNMENT != 4)
        #error "Define 'ALIGNMENT' to either 1,2,4"
    #endif
#endif


/**************************************************************************************************/
/* LOCAL TYPE DEFINITIONS                                                                         */
/**************************************************************************************************/

typedef enum _T_TIMstateEnum{
    TIM_INIT, ///< Init task state
    TIM_WORK  ///< Working task state
} T_TIMstateEnum;///< task state enumeration type


/******************************************************************************/
/* PRIVATE FUNCTION DECLARATIONS                                              */
/******************************************************************************/
static void TIM_vHandleTimer(void);     ///< Handles all 8-Bit timers
static void TIM_vHandleTimer16(void);   ///< Handles all 8 and 16-Bit timers
static void TIM_vInitTimer(void);       ///< Initialises 8-Bit timers
static void TIM_vInitTimer16(void);     ///< Initialises 8 and 16-Bit timers
/* Warning 522: highest operation, function 'TIM_vHandleTimer16', lacks side effects [MISRA 2012 Rule 2.2, advisory] */
/*lint -esym(522,TIM_vHandleTimer16) false positive! */
/******************************************************************************/
/* PRIVATE DATA DEFINITIONS                                                   */
/******************************************************************************/
static T_TIMstateEnum TIM_tTaskState = TIM_INIT; ///< timer task state variable

#ifdef MCAL_MPCM_INCLUDED
/** MPCM clock change Callback instance */
static MCAL_Callback_t TimerMpcmCb;
#endif

/******************************************************************************/
/* PUBLIC FUNCTION DEFINITIONS                                                */
/******************************************************************************/

BOOL TIM_bHandleTask(void)
{
    if (TIM_tTaskState == TIM_WORK)
    {
        if (_TIM_bIsBaseTimerDown())
        {
            TIM_vHandleTimer();  //handles all timers
        }
#ifdef SYSTEM_TIMER_ENABLED
        STIM_SchedulerHandleTask();
#endif

    }
    else
    {
        if (_TIM_bGetInitConfig())
        {
            TIM_vInitTimer();
#ifdef SYSTEM_TIMER_ENABLED
            (void)STIM_InitModule();
#endif

#ifdef MCAL_MPCM_INCLUDED
            MCAL_initCallback(&TimerMpcmCb, TIM_MpcmEventCallback, NULL);
            (void)MPCM_enableEvent(&TimerMpcmCb, MCAL_EVENT_CLOCK_CHANGE_ANY);
#endif
            TIM_tTaskState = TIM_WORK;
        }
        else
        {
            return TASK_NOT_INITIALISED;
        }
    }
    return TASK_INITIALISED;//allways true
}


BOOL TIM_bHandleTask16(void)
{
    if (TIM_tTaskState == TIM_WORK)
    {
        if (_TIM_bIsBaseTimerDown())
        {
            TIM_vHandleTimer16();  //handles all timers
        }
#ifdef SYSTEM_TIMER_ENABLED
        STIM_SchedulerHandleTask();
#endif
    }
    else
    {
        if (_TIM_bGetInitConfig())
        {
            TIM_vInitTimer16();
#ifdef SYSTEM_TIMER_ENABLED
            (void)STIM_InitModule();
#endif

#ifdef MCAL_MPCM_INCLUDED
            MCAL_initCallback(&TimerMpcmCb, TIM_MpcmEventCallback, NULL);
            (void)MPCM_enableEvent(&TimerMpcmCb, MCAL_EVENT_CLOCK_CHANGE_ANY);
#endif
            TIM_tTaskState = TIM_WORK;
        }
        else
        {
            return TASK_NOT_INITIALISED;
        }
    }
    return TASK_INITIALISED;//allways true
}


/******************************************************************************/
/* PRIVATE FUNCTION DEFINITIONS                                                */
/******************************************************************************/
/* Axivion Disable Style MisraC2012-13.3 : Axivion consider decrement of volatile object to have side effect, not critical here */
//lint -save -e661 -e662   "access to out-of-bounds pointer" warning deactivated, tested to work correctly
//lint -save -e946         "Relational or subtract operator applied to pointers", tested to work correctly
//lint -save -e923 -e926   "cast from pointer to unsigned int", tested to work correctly

/* Handles all 8-Bit timers */
static void TIM_vHandleTimer(void)
{
    Ttimer8 *pucTimerPtr;
    TtimerConfig const * ptTimerConfig;
    uint8_t ucPrescaler;

    ptTimerConfig = TIM_ptTimer8Config; //sets pointer to config of first 8 bit segment
    pucTimerPtr   = TIM_ptTimer8Config[0].pucPrescalerPtr; // point to first prescaler (TIM_uc8Start1)

    for (;;)
    {
        ptTimerConfig++;
        (pucTimerPtr)++;
        while ( pucTimerPtr < ptTimerConfig->pucPrescalerPtr )//do for all timers in segment
        {
            if ( *pucTimerPtr != 0u ) // if not timer down
            {
                (*pucTimerPtr)--;     // decrement timer
            }
            pucTimerPtr++; // next timer in segment
        }
        ucPrescaler = ptTimerConfig->ucPrescaler;

        if ( ucPrescaler != TIM_END_MARK )//terminate loop if prescaler is zero -->end mark
        {
            (*pucTimerPtr)++;     //increment prescaler
            if (*pucTimerPtr == ucPrescaler)//prescaler down?
            {
                //reset prescaler
                *pucTimerPtr = 0;
            }
            else
            {
                // done, current prescaler not down
                break;
            }
        }
        else
        {
            //end mark reached --> last timer segment done
            break;
        }
    }
}


/* Handles both 8 and 16-Bit timers */
static void TIM_vHandleTimer16(void)
{
    Ttimer8            *pucTimerPtr;  ///< timer pointer
    TtimerConfig const *ptTimerConfig;///< pointer to entry in config table
    uint8_t               ucPrescaler;  ///< current prescaler value
    uint8_t               ucTimerType = TIM_8BIT; ///< internal state variable

    ptTimerConfig = TIM_ptTimer8Config;//init config pointer

    while (ucTimerType <= TIM_16BIT)
    {
        if (ucTimerType == TIM_16BIT)
        {
            ptTimerConfig = TIM_ptTimer16Config;//init config pointer
        }

        pucTimerPtr = ptTimerConfig->pucPrescalerPtr; // point to first prescaler (TIM_uc8Start1/TIM_uc16Start1)

        for (;;)
        {
            ptTimerConfig++;
            pucTimerPtr++;
#if ALIGNMENT > 1
            if ((ucTimerType != TIM_8BIT) && (pucTimerPtr < (ptTimerConfig->pucPrescalerPtr)))
            {
                //following cast is ok, just lowest 2 bits important
                //code works for all width of addresses
  #if (ALIGNMENT == 4)
                while ( (((uint32_t)((uint32_t)pucTimerPtr)) & 3u) != 0u )
                {
                    pucTimerPtr++;
                }
  #else
                //Alignment is 2
                //cast is ok, just LSB important
                if ( (((uint8_t)((uint32_t)pucTimerPtr) & 1u)) != 0u )
                {
                    pucTimerPtr++;
                }
  #endif
            }
#endif
            while (pucTimerPtr < (ptTimerConfig->pucPrescalerPtr))//do for all timers in segment
            {
                if (ucTimerType == TIM_16BIT)
                {
                    if (*((Ttimer16*)(volatile void*)pucTimerPtr) != 0u)   // if not 16-Bit timer down
                    {
                        (*((Ttimer16*)(volatile void*)pucTimerPtr))--;   //decrement timer
                    }
                    pucTimerPtr+=sizeof(uint16_t);//lint !e9016 "pointer arithmetic [MISRA 2012 Rule 18.4 advisory]"
                }
                else
                {
                    if (*pucTimerPtr != 0u)   //if not 8-Bit timer down
                    {
                        (*pucTimerPtr)--;   //decrement timer
                    }
                    pucTimerPtr++;
                }
            }
            ucPrescaler = ptTimerConfig->ucPrescaler;

            if ( ucPrescaler != TIM_END_MARK )//terminate loop if prescaler is zero -->end mark
            {
                (*pucTimerPtr)++;     //decrement prescaler
                if (*pucTimerPtr == ucPrescaler)//prescaler down?
                {
                    //load prescaler
                    *pucTimerPtr = 0;
                }
                else
                {
                    ucTimerType++;
                    break;
                }
            }
            else
            {
                ucTimerType++;
                break;
            }
        }
    }
}


// Initializes 8 and 16-Bit timers
static void TIM_vInitTimer16(void)
{
    Ttimer8 *pucPtr = &TIM_uc16End;

    while (pucPtr >= TIM_ptTimer8Config[0].pucPrescalerPtr)   //do for all (8 and 16-Bit) timers
    {
        *pucPtr = 0;
        pucPtr--;
    }
}

// Initializes 8-Bit timers
static void TIM_vInitTimer(void)
{
    Ttimer8 *pucPtr = &TIM_uc8End;

    while (pucPtr >= TIM_ptTimer8Config[0].pucPrescalerPtr)   //do for all 8-Bit timers
    {
        *pucPtr = 0;
        pucPtr--;
    }
}

//lint -restore -e661 -e662
//lint -restore -e946
//lint -restore -e923 -e926
