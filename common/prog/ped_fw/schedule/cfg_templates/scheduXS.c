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
 *  COMP_ABBREV      SCH
 ******************************************************************************/

/******************************************************************************/
/* DOCUMENTATION                                                              */
/******************************************************************************/
/** \file
 *
 *  \brief    Configuration of schedule library, debugging modes
 *
 */

/******************************************************************************/
/* INCLUDES                                                                   */
/******************************************************************************/
#include "bsh_stdinc.h"
#include <stdint.h>
#include <stdbool.h>
#include "LibDefines.h"
#include "schedule.h"
#include "schpriv.h"
#include "utility/utility.h"
#include "timer/timerlib.h"
#include "hwdt.h"
#include "hsup.h"

/**************************************************************************************************/
/* LOCAL DEFINITIONS                                                                              */
/**************************************************************************************************/

#define SCH_NO_DEBUG     0U      ///< Used for debug compiler switches: No time measurement
#define SCH_BETWEEN_TIME 1U      ///< Used for debug compiler switches: Measurement of maximum time between two calls of a task
#define SCH_RUNTIME      2U      ///< Used for debug compiler switches: Measurement of maximum runtime of a task


#ifndef TIME_DEBUG_MODE
    #define TIME_DEBUG_MODE 0U
#endif

// select one of following switches:
#if TIME_DEBUG_MODE == 1U
    #define SCH_TIME_DEBUG_MODE SCH_BETWEEN_TIME   ///< Measurement of maximum time between calls active
#elif TIME_DEBUG_MODE == 2U
    #define SCH_TIME_DEBUG_MODE SCH_RUNTIME        ///< Measurement of maximum runtime active
#else
    #define SCH_TIME_DEBUG_MODE SCH_NO_DEBUG       ///< Time debug mode inactive
#endif

#if (SCH_TIME_DEBUG_MODE != SCH_NO_DEBUG)
    #define SCH_TIME_DEBUG_TABLE_SIZE   1U ///< Size of debug table, specify how many task can watched at the same time \b --> #SCH_ptTimeMeasurement
#endif

/**************************************************************************************************/
/* LOCAL TYPE DEFINITIONS                                                                         */
/**************************************************************************************************/

#if (SCH_TIME_DEBUG_MODE == SCH_RUNTIME)
/** Type used for runtime measurement */
typedef struct _TTimeMeasurement
{
  uint16_t  uiMaxTime;      ///< Maximum runtime of task
  uint16_t  uiLastCallTime; ///< last call time of module (value of circulating counter)
  uint32_t ulCallCounter;  ///< counts the calls of task
  uint32_t ulSummaryTime;  ///< summary runtime of module
}TTimeMeasurement; 
#endif

/** Type used for measurement of time between task calls*/
#if (SCH_TIME_DEBUG_MODE == SCH_BETWEEN_TIME)
typedef struct _TTimeMeasurement/// structure used for "between" time measurement
{
  uint16_t  uiMaxTime;      ///< Maximum time between calls of task
  uint16_t  uiLastCallTime; ///< last call time of module (value of circulating counter)
}TTimeMeasurement;
#endif


/**************************************************************************************************/
/* LOCAL DECLARATIONS                                                                             */
/**************************************************************************************************/


/** This function is called before each task (in debug mode)
    \param  tTaskId: \n
            Id of task which will be called after.\n
         \b Type: TtaskId
*/
#if (SCH_TIME_DEBUG_MODE != SCH_NO_DEBUG)
static void SCH_vPreTaskCall (TtaskId tTaskId);
#endif

/** This function is called after each task (in debug mode)
    \param tTaskId: \n
           Id of task which was called before.\n
        \b Type: TtaskId
*/
#if (SCH_TIME_DEBUG_MODE == SCH_RUNTIME)
static void SCH_vPostTaskCall (TtaskId tTaskId);
#endif



/**************************************************************************************************/
/* LOCAL DATA                                                                                     */
/**************************************************************************************************/


#if (SCH_TIME_DEBUG_MODE != SCH_NO_DEBUG)

/** Structure for time debugging

This structure saves data for time measurement.
First task saved in this array is the task with ID equal to SCH_ucStartScanModuleId.
A number of SCH_TIME_DEBUG_TABLE_SIZE task can be watched at the same time.

\note Changing the variable SCH_ucStartScanModuleId (e.g. with Gvwin or Emulator) will delete all prior results!

*/
static TTimeMeasurement SCH_ptTimeMeasurement[SCH_TIME_DEBUG_TABLE_SIZE];//lint -e9003 "may be defined in block scope" */


/** First task to be watched

This is the ID of first task to be watched. The task with this ID is the first entry in table SCH_ptTimeMeasurement.
If SCH_TIME_DEBUG_TABLE_SIZE is samller than number of task in system this variable can be changed via emulator or GVWIN/EDITH.
Changing this variable externally will effect an erease of SCH_ptTimeMeasurement before starting measurement.
*/
volatile uint8_t SCH_ucStartScanModuleId;///< First module to measure, to change with GV-WIN

static uint8_t SCH_ucStartScanCopy;///< Copy of first task to watch, helps sensing changes in SCH_ucStartScanModuleId
#endif



/**************************************************************************************************/
/* LOCAL FUNCTIONS DEFINITION                                                                     */
/**************************************************************************************************/

#if (SCH_TIME_DEBUG_MODE == SCH_BETWEEN_TIME)
//******************************** BETWEEN-TIME MEASUREMENT ******************************************
static void SCH_vPreTaskCall (TtaskId tTaskId)
{
    uint16_t uiTime;
    uint16_t uiCurrentTime;
    TTimeMeasurement* ptTime;
    uint16_t Loop;


      tTaskId-=SCH_ucStartScanModuleId;
      if (tTaskId < SCH_TIME_DEBUG_TABLE_SIZE)//debug active for this module?
      {
            uiCurrentTime = TIM_uiGetCircleMicroSeconds();

            ptTime = &SCH_ptTimeMeasurement[tTaskId];

            //don't calculate time for first call (at time 0x0000 the between-time will not calculated too)
            if (ptTime->uiLastCallTime != 0U)
            {
                  uiTime = uiCurrentTime - ptTime->uiLastCallTime;

                  if (uiTime > ptTime->uiMaxTime)
                  {
                        ptTime->uiMaxTime = uiTime;//save max between time
                  }
            }
                  ptTime->uiLastCallTime = uiCurrentTime;

            if (SCH_ucStartScanModuleId  != SCH_ucStartScanCopy)
            {
                  SCH_ucStartScanCopy = SCH_ucStartScanModuleId;
                  for (Loop = 0; Loop < SCH_TIME_DEBUG_TABLE_SIZE; Loop++)
                  {
                      SCH_ptTimeMeasurement[Loop].uiMaxTime = 0U;
                      SCH_ptTimeMeasurement[Loop].uiLastCallTime = 0U;
                    #if (SCH_TIME_DEBUG_MODE == SCH_RUNTIME)
                      SCH_ptTimeMeasurement[Loop].ulCallCounter = 0U;
                      SCH_ptTimeMeasurement[Loop].ulSummaryTime = 0U;
                    #endif
                  }
            }
      }
}
#endif


#if (SCH_TIME_DEBUG_MODE == SCH_RUNTIME)
//******************************** RUNTIME MEASUREMENT ***********************************************
static void SCH_vPreTaskCall (TtaskId tTaskId)
{
    uint16_t* puiLastCallTime;//pointer to last call time
    uint16_t uiLoop;    //loop for array initialisation
    uint8_t* pucPtr;  //ptr for array initialisation

    tTaskId-=SCH_ucStartScanModuleId;

    if (tTaskId < SCH_TIME_DEBUG_TABLE_SIZE)//debug active for this module?
    {
        //initialization of SCH_ptTimeMeasurement, when SCH_ucStartScanModuleId changed
        if (SCH_ucStartScanCopy != SCH_ucStartScanModuleId)
        {
            SCH_ucStartScanCopy = SCH_ucStartScanModuleId;
            for (uiLoop=0U, pucPtr = (uint8_t*)SCH_ptTimeMeasurement;uiLoop<sizeof(SCH_ptTimeMeasurement);uiLoop++)
            {
                *pucPtr = 0U;
                pucPtr++;
            }
        }
        
        puiLastCallTime = &(SCH_ptTimeMeasurement[tTaskId].uiLastCallTime);
        *puiLastCallTime = TIM_uiGetCircleMicroSeconds();//last operation: load current time
    }
}




//******************************** RUNTIME MEASUREMENT ***********************************************
static void SCH_vPostTaskCall(TtaskId tTaskId)
{
    uint16_t uiCurrentTime;
    uint16_t uiRunningTime;
    TTimeMeasurement* ptTime;

    uiCurrentTime = TIM_uiGetCircleMicroSeconds();//first operation: load current time
    tTaskId-=SCH_ucStartScanModuleId;
    if (tTaskId < SCH_TIME_DEBUG_TABLE_SIZE)//debug active for this module?
    {
        ptTime = &SCH_ptTimeMeasurement[tTaskId];
        ptTime->ulCallCounter++;
        uiRunningTime = uiCurrentTime - ptTime->uiLastCallTime;
        ptTime->ulSummaryTime += uiRunningTime;
        if (uiRunningTime > ptTime->uiMaxTime)
        {
            ptTime->uiMaxTime = uiRunningTime;
        }
    }
    HWDT_vTrigger();
}
#endif



/**************************************************************************************************/
/* GLOBAL FUNCTIONS DEFINITION                                                                    */
/**************************************************************************************************/

uint8_t _SCH_ucDoTask(TtaskId tTaskId)
{
    uint8_t ucTaskState;

#if (SCH_TIME_DEBUG_MODE != SCH_NO_DEBUG)
    SCH_vPreTaskCall(tTaskId);
#endif
    ucTaskState = (SCH_ppfTaskHandlerTable[tTaskId])();
#if (SCH_TIME_DEBUG_MODE == SCH_RUNTIME)
   SCH_vPostTaskCall(tTaskId); //just for runtime measurement
#endif
    if (ucTaskState == TASK_TERMINATE)
    {
        SCH_vSwitchModule(tTaskId, (bool)false);//disable module 
    }
#ifdef TRIGGER_WDO
    HWDT_vTrigger();
#endif
    return ucTaskState;
}
