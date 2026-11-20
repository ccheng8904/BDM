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
 *  COMP_ABBREV      LPAN
 ******************************************************************************/


/******************************************************************************/
/* DOCUMENTATION                                                              */
/******************************************************************************/
/** \file
 *  \brief    Implementation of logpan lib.
 */

/******************************************************************************/
/* INCLUDES                                                                   */
/******************************************************************************/
#include "bsh_stdinc.h"
#include <stdint.h>
#include "logPpriv.h"
#include "logpan.h"


/******************************************************************************/
/* PUBLIC FUNCTION DEFINITIONS                                                */
/******************************************************************************/

uint8_t LPAN_ucGetKeyEvent(void)
{
   uint8_t ucState;
   uint8_t ucEvent = LPAN_NO_EVENT;
   uint8_t ucGripId;

    static const uint8_t LPAN_aucPressedState[3]={LPAN_PRESSED_STATE,LPAN_REPEAT_STATE,LPAN_REPEAT_STATE};///< Transition after Key was pressed
    static const uint8_t LPAN_aucPressedEvent[3]={LPAN_PRESSED_EV, LPAN_DELAY_EV, LPAN_REPEAT_EV};        ///< Event after key pressed
    static const uint8_t LPAN_aucReleasedEvent[3]={LPAN_NO_EVENT, LPAN_RELEASED_EV, LPAN_REPEAT_REL_EV};  ///< Event after key released

   for (ucGripId=0;ucGripId<LPAN_ucNumberOfGrips;ucGripId++)//loop fo all defined grips
   {
      ucState = LPAN_aucGripState[ucGripId];//load state for current grip
      if(_LPAN_bIsKeyMask(ucGripId))//Is current grip pressed?
      {
         //**** Key(s) are pressed! *******
         //Is timer for Current grip down? (always TRUE for RELEASED-State)
         if ( _LPAN_bIsTimerDown(ucGripId) || (ucState == (uint8_t)LPAN_RELEASED_STATE) )
         {
            //timer down (or state was RELEASED)
            //1. Load timer for this grip depending on current state
            _LPAN_vLoadTimer(ucGripId,ucState);
            //2. Get pressed event depending on state
            ucEvent = LPAN_aucPressedEvent[ucState];
            //3. Set next pressed state
            ucState = LPAN_aucPressedState[ucState];
         }
         else
         {
            //nothing changed
         }
      }
      else
      {
         //****** Key(s) are released! ********
         //1. Get release event depending on state
         ucEvent = LPAN_aucReleasedEvent[ucState];
         //2. Set state to released
         ucState = LPAN_RELEASED_STATE;
      }
      LPAN_aucGripState[ucGripId] = ucState;//save new state for current grip
      if (ucEvent != (uint8_t)LPAN_NO_EVENT)//any event detected?
      {
         return _LPAN_ucMapGripEvent(ucGripId, (uint8_t)(ucEvent-1u));//return logical event for calculated physical event
      }
   }
   //No Event detected!
   return LPAN_NO_EVENT; 
}

