/*******************************************************************************
 *  Copyright (c) 2017 BSH Hausgeraete GmbH,
 *  Carl-Wery-Str. 34, 81739 Munich, Germany, www.bsh-group.de
 *
 *  All rights reserved. This program and the accompanying materials
 *  are protected by international copyright laws.
 *  Please contact copyright holder for licensing information.
 *
 *******************************************************************************
 *   PROJECT          Scheduler NEW (Generic SW)
 *   AUTHOR           Raundal
 *   CREATED          12.05.2018
 ******************************************************************************/

/**************************************************************************************************/
/* DOCUMENTATION                                                                                  */
/**************************************************************************************************/
/// \file Implementation of module scheduler_time_monitor


/**************************************************************************************************/
/* INCLUDES                                                                                       */
/**************************************************************************************************/
#include "bsh_stdinc.h"
#include "timerlib.h"
#if defined (PLATFORM_SUPPORTS_32BIT_HWTIMER)
#include "timerMcXs.h" //32bit HW timer
#endif //PLATFORM_SUPPORTS_32BIT_HWTIMER
#include "scheduler_time_monitor.h"
#include "scheduler/cfg_templates/scheduler_cfg.h"

/**************************************************************************************************/
/* LOCAL DEFINITIONS                                                                              */
/**************************************************************************************************/
#define NUMBER_OF_JITTER_SAMPLE    10U

/**************************************************************************************************/
/* LOCAL TYPE DEFINITIONS                                                                         */
/**************************************************************************************************/
struct SCH_TM_LastRefValues
{
    uint32_t last_time_period;
    uint32_t last_call_time;
    uint8_t  jitter_sample_counter;
    int32_t  task_jitter_sum;
};

/**************************************************************************************************/
/* LOCAL FUNCTION DECLARATIONS                                                                    */
/**************************************************************************************************/

/**************************************************************************************************/
/* EXTERN VARIABLE DEFINITIONS                                                                    */
/**************************************************************************************************/

/**************************************************************************************************/
/* LOCAL VAR DEFINITIONS                                                                          */
/**************************************************************************************************/
#if(DEBUG_MODE == DEBUG_SINGLE_MODULE)
struct SCH_TM_TimeProperties SCH_TimeMonitor[DEBUG_SINGLE_MODULE];
static struct SCH_TM_LastRefValues ReferenceTable[DEBUG_SINGLE_MODULE];

#else
struct SCH_TM_TimeProperties SCH_TimeMonitor[NO_OF_MODULES];
static struct SCH_TM_LastRefValues ReferenceTable[NO_OF_MODULES];
#endif


/**************************************************************************************************/
/* LOCAL FUNCTION DECLARATIONS                                                                    */
/**************************************************************************************************/
static void SCH_TM_saveGlobalTimeValues(const uint8_t task_index, const uint32_t *task_period, const int32_t *jitter_average);
static int32_t SCH_TM_doSubstraction_s32(const uint32_t minuend, const uint32_t subtrahend);
static uint32_t SCH_TM_doSubstraction_u32(const uint32_t minuend, const uint32_t subtrahend);
static int32_t SCH_TM_calculateJitterAverage(uint32_t task_period, uint8_t  task_index);
/**************************************************************************************************/
/* LOCAL FUNCTION DEFINITIONS                                                                     */
/**************************************************************************************************/
/**!
 *  \fn         static void SCH_TM_saveGlobalTimeValues(const uint8_t task_index, const uint32_t *task_period, const int32_t *jitter_average)
 *  \brief      This function saves the global time properties which can be viewed by user in the SCH_TasksTimeMonitor
 *
 *  \param      TaskID task_index
 *              uint32_t *task_period
 *              int32_t *jitter
 *
 *  \return     none
 *
 *  \details
 */
static void SCH_TM_saveGlobalTimeValues(const uint8_t task_index, const uint32_t *task_period, const int32_t *jitter_average)
{
    SCH_TimeMonitor[task_index].time_periods[SCH_TimeMonitor[task_index].sample_counter] = *task_period;
    SCH_TimeMonitor[task_index].task_jitter_average = *jitter_average;
}

/*!
 *
 *  \fn         static int32_t SCH_TM_doSubstraction_s32(const int32_t minuend, const int32_t subtrahend)
 *  \brief      Calculates the difference between the two parameters minuend and subtrahend with overflow prevention.
 *
 *  \param      int32_t minuend
 *  \param      int32_t subtrahend
 *
 *  \return     difference of minuend minus subtrahend
 *
 */
static int32_t SCH_TM_doSubstraction_s32(const uint32_t minuend, const uint32_t subtrahend)
{
    int64_t difference;
    
    difference = (int64_t)minuend - (int64_t)subtrahend;

    if (difference > INT32_MAX)
    {
        difference = INT32_MAX;
    }

    if (difference < INT32_MIN)
    {
        difference = INT32_MIN;
    }

    return ((int32_t)difference);
}

/*!
 *
 *  \fn         static uint32_t SCH_TM_doSubstraction_u32(const uint32_t minuend, const uint32_t subtrahend)
 *  \brief      Calculate the difference between the two parameters minuend and subtrahend with overflow prevention.
 *
 *  \param      uint32_t minuend
 *  \param      uint32_t subtrahend
 *
 *  \return     difference of minuend minus subtrahend
 *
 */
static uint32_t SCH_TM_doSubstraction_u32(const uint32_t minuend, const uint32_t subtrahend)
{
    return ((subtrahend > minuend) ? 0UL : (minuend - subtrahend));
}

/*!
 *
 *  \fn         static int32_t SCH_TM_calculateJitterAverage(uint32_t task_period, uint8_t  task_index)
 *  \brief      Calculates the Jitter average.
 *
 *  \param      uint32_t task_period
 *  \param      uint8_t  task_index
 *
 *  \return     jitter_average
 *
 */
static int32_t SCH_TM_calculateJitterAverage(uint32_t task_period, uint8_t  task_index)
{
    int32_t jitter;
    
    int32_t jitter_average;

    if(task_period)
    {   
        jitter = SCH_TM_doSubstraction_s32(task_period , ReferenceTable[task_index].last_time_period );
             
        ReferenceTable[task_index].task_jitter_sum +=   jitter;
              
        ReferenceTable[task_index].jitter_sample_counter ++;
    }
    
    if((int32_t)ReferenceTable[task_index].jitter_sample_counter)
    {
        jitter_average = ReferenceTable[task_index].task_jitter_sum / (int32_t)ReferenceTable[task_index].jitter_sample_counter;
    }
    else
    {
        jitter_average = 0L;
    }
        
    if(ReferenceTable[task_index].jitter_sample_counter >= NUMBER_OF_JITTER_SAMPLE)
    {
        ReferenceTable[task_index].jitter_sample_counter = 0U;
               
        ReferenceTable[task_index].task_jitter_sum       = 0L;
    }
    return(jitter_average);
}


/**************************************************************************************************/
/* GLOBAL FUNCTION DEFINITIONS                                                                    */
/**************************************************************************************************/
/**!
 *  \brief      This function measures the Task Time Period and the Jitter
 *
 *  \param      TaskID task_id
 *
 *  \return     none
 *
 *  \details
 */
void SCH_TM_measureTaskTimeProperties(TaskID task_id)
{
    uint8_t  task_index;
    static int32_t  jitter_average = 0L;
    static uint32_t task_period = 0UL;
    uint32_t current_time;

#if(DEBUG_MODE == DEBUG_SINGLE_MODULE)
    task_index = 0U;
    SCH_TimeMonitor[task_index].Task_ID = task_id;
#else
    task_index = (uint8_t)task_id;
#endif

    current_time = (uint32_t)TIM_u32GetCircleMicroSeconds();
    
    if((ReferenceTable[task_index].last_call_time))
    {
        task_period = SCH_TM_doSubstraction_u32(current_time , ReferenceTable[task_index].last_call_time);
                      
        if(ReferenceTable[task_index].last_time_period)
        {
            jitter_average = SCH_TM_calculateJitterAverage(task_period, task_index);               
        }
       
        ReferenceTable[task_index].last_time_period = task_period;

        SCH_TM_saveGlobalTimeValues(task_index, &task_period, &jitter_average);
    }
    
    ReferenceTable[task_index].last_call_time = current_time;
}


/**!
 *  \brief      This function measures the Task Runtime and saves it in the Time Monitor Table
 *
 *  \param      TaskID task_id
 *
 *  \return     none
 *
 *  \details
 */
void SCH_TM_measureTaskRuntime(TaskID task_id)
{
    uint8_t task_index;
    uint32_t current_time;
    uint32_t task_runtime;

#if(DEBUG_MODE == DEBUG_SINGLE_MODULE)
    (void)task_id;
    task_index = 0U;
#else
    task_index = (uint8_t)task_id;
#endif

    current_time = (uint32_t)TIM_u32GetCircleMicroSeconds();

    task_runtime = SCH_TM_doSubstraction_u32(current_time , ReferenceTable[task_index].last_call_time);

    SCH_TimeMonitor[task_index].task_runtimes[SCH_TimeMonitor[task_index].sample_counter] = task_runtime;

    SCH_TimeMonitor[task_index].sample_counter++;

    if(SCH_TimeMonitor[task_index].sample_counter >= NUMBER_OF_SAMPLES)
    {
        SCH_TimeMonitor[task_index].sample_counter = 0U;
    }
}


/********************************************* End of File ******************************************************/
