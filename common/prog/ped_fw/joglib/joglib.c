/*******************************************************************************
 *  Copyright (c) 2017 BSH Hausgeraete GmbH,
 *  Carl-Wery-Str. 34, 81739 Munich, Germany, www.bsh-group.de
 *
 *  All rights reserved. This program and the accompanying materials
 *  are protected by international copyright laws.
 *  Please contact copyright holder for licensing information.
 *
 *******************************************************************************
 *  PROJECT          SW_LIB
 *  COMP_ABBREV      JOG
 ******************************************************************************/


/******************************************************************************/
/* DOCUMENTATION                                                              */
/******************************************************************************/
/** \file
 *  \ingroup  joglib
 *  \brief    Implementation of joglib
 *
 *  \details  Evaluate ticks of incremental encoder.
 *            De-bouncing and speed detection included.
 */


/******************************************************************************/
/* INCLUDES                                                                   */
/******************************************************************************/
#include "bsh_stdinc.h"
#include <stdint.h>
#include <stdbool.h>
#include "joglib.h"
#include "joglibpriv.h"
#include "timer/timerlib.h"
#include "hsup.h"

/******************************************************************************/
/* PRIVATE DEFINITIONS                                                        */
/******************************************************************************/

/******************************************************************************/
/* PRIVATE TYPE DEFINITIONS                                                   */
/******************************************************************************/

/******************************************************************************/
/* PRIVATE FUNCTION DECLARATIONS                                              */
/******************************************************************************/
void JOG_vEvaluateSpeed(uint8_t ucEncoder);

/******************************************************************************/
/* PRIVATE DATA DEFINITIONS                                                   */
/******************************************************************************/
/**  \brief Speed status of up to 8 jog dials.
 *   JOG_ucSpeed save the speed status where LSB represents jog with ID 0
 */
static uint8_t JOG_ucSpeed = 0u;


/******************************************************************************/
/* PUBLIC FUNCTION DEFINITIONS                                                */
/******************************************************************************/

void JOG_vHandleEncoders(void)

{
    for (uint8_t i = 0u; i < JOG_ucEncoderCount; i++)
    {
        JOG_aucIsrState[i] = JOG_aucStateTransitionTable[JOG_aucIsrState[i]][_JOG_ucGetLineState(i)]; 

        if (JOG_aucIsrState[i] == (uint8_t)JOG_STATE_ADD)
        {
            JOG_ascSteps[i]++;
            JOG_vEvaluateSpeed(i);
            JOG_aucIsrState[i] = (uint8_t)JOG_STATE_IDLE;
        }
        if (JOG_aucIsrState[i] == (uint8_t)JOG_STATE_SUB)
        {
            JOG_ascSteps[i]--;
            JOG_vEvaluateSpeed(i);
            JOG_aucIsrState[i] = (uint8_t)JOG_STATE_IDLE;
        }
    }
}


int8_t JOG_scGetSteps(uint8_t ucEncoder)
{
    int8_t steps;

#ifndef UNIT_TEST
    HSUP_vDisableInt();
#endif
    steps = JOG_ascSteps[ucEncoder];
    JOG_ascSteps[ucEncoder] = 0;
#ifndef UNIT_TEST
    HSUP_vEnableInt();
#endif
    return steps;
}


int8_t JOG_scGetSpeedSteps(uint8_t ucEncoder)
{
    int8_t  steps = JOG_scGetSteps(ucEncoder);

    if (JOG_bIsFastDetected(ucEncoder))
    {
        steps = (int8_t)(steps * (int8_t)JOG_ucSpeedMultiplier);
    }
    return steps;
}


bool JOG_bIsFastDetected(uint8_t ucEncoder)
{
    uint8_t ucMask = (uint8_t)(1u<<ucEncoder);
    
    return ( ((JOG_ucSpeed & ucMask) == 0u) ? false : true); 
}


/******************************************************************************/
/* PRIVATE FUNCTION DEFINITIONS                                               */
/******************************************************************************/


/** \brief Evaluate current speed of rotation for multi jog dial.
 *
 * This function evaluates the speed of the rotary encoder
 *
 * \param ucEncoder: ID of encoder
 */
void JOG_vEvaluateSpeed(uint8_t ucEncoder)
{
   uint16_t uiTimeNow;
   uint16_t uiTimeDelta;

   uint8_t ucMask = (uint8_t)(1u<<ucEncoder);


   uiTimeNow = TIM_uiGetCircleMicroSeconds();                        // load current time
   uiTimeDelta = (uint16_t)(uiTimeNow - JOG_auiLastStepTime[ucEncoder]); // calculate elapsed time


   if ( JOG_aTim8SpeedTimer[ucEncoder] > 0u ) //timer down to switch off speed bit (do not check circulating counter after more then 65ms)
   {
      if (uiTimeDelta <= JOG_uiSpeedOnThresholdTime)//is rotation fast?
      {
         JOG_ucSpeed |=ucMask;//fast!
      }
      else
      {
         //take old speed bit
      }
   }
   else
   {
      JOG_ucSpeed &=~ucMask;//slow!
   }
   JOG_auiLastStepTime[ucEncoder] = uiTimeNow;                        //save current time;
   JOG_aTim8SpeedTimer[ucEncoder] = JOG_ucSpeedOffThreholdTime10ms;   //load speed off timer

}
