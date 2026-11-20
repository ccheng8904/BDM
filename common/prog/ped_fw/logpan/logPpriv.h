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

#ifndef LOGPRIV_H
#define LOGPRIV_H

#ifdef __cplusplus
extern "C" {
#endif

/******************************************************************************/
/* DOCUMENTATION                                                              */
/******************************************************************************/
/** \file logPpriv.h
 *
 *  \brief Private definitions and declarations of logic panel library
 */


/******************************************************************************/
/* INCLUDES                                                                   */
/******************************************************************************/
#include "bsh_stdinc.h"
#include <stdint.h>
#include <stdbool.h>
#include "SegmentDef.h"

/******************************************************************************/
/* PUBLIC TYPE DEFINITIONS                                                    */
/******************************************************************************/


/** \brief Array of status for each defined grip
 *
 * This array saves the state for each key (or keyGrip).\n
 * This array must be defined in library template with required dimension.
 * 
 * \note This array must be defined in library template!
 * 
 */
extern uint8_t LPAN_aucGripState[];

extern const uint8_t LPAN_ucNumberOfGrips;///< Number of supported grips, must be defined by application

/******************************************************************************/
/* PUBLIC FUNCTION DECLARATIONS                                               */
/******************************************************************************/



/** \brief This function read out the current key state and returns true if keys of the grip \p ucGripId are pressed.
 *
 *  \param ucGripId: Id of the grip \n
 *          \b Range: (number of grips)-1
 *  
 *  \return true if key grip is pressed, else False
 *  
 *  \b Example: (Easy implementation of _LPAN_bIsKeyMask)
 *  \code
 *  bool _LPAN_bIsKeyMask(uint8_t ucGripId)
 *  {
 *      return LPAN_aptGripSettings[ucGripId]->tKeyMask & PHYP_tGetKeys();
 *  }
 *  \endcode
 *  
 *  \note This Function must be defined by application!\n
 */
bool _LPAN_bIsKeyMask(uint8_t ucGripId);


/** \brief This function returns true if the timer is down.
 *
 *  \param ucGripId: Id of the grip \n
 *
 *  \return true if timer is down, else false
 *
 *  \b Example:
 *  \code
 *  bool _LPAN_bIsTimerDown(uint8_t ucGripId)
 *  {
 *     return (LPAN_tim8KeyTimer[ucGripId] == 0u);
 *  }
 *  \endcode
 *
 *  \note This function must be defined by application!\n
 *        The timer is used as repeat and delay timer (not used at same time).\n
 */
bool _LPAN_bIsTimerDown(uint8_t ucGripId);


/** \brief This function loads the timer for this grip.
 *
 *  \param ucGripId: Id of the grip \n
 *  
 *  \param ucTimeId: Id of the time do be loaded
 *          \b Type: uint8_t\n
 *          \b Range: [0:Delaytime, 1: RepeatStartTime, 2:RepeatTime]

 *
 *  \b Example:
 *  \code
 *  _LPAN_vLoadTimer(uint8_t ucGripId, uint8_t ucTimeId)
 *  {
 *    *(LPAN_atEventConfigTable[ucGripId].pucTimer) = LPAN_atEventConfigTable[ucGripId].aucTiming[ucTimeId];
 *  }
 *  \endcode
 *
 *
 *
 *  \note This Function must be defined by application!\n
 *        The timer is used as repeat and delay timer (not used at same time).\n
 */
void _LPAN_vLoadTimer(uint8_t ucGripId, uint8_t ucTimeId);


/** \brief This function maps the physical event from logpan to a logical one.
 *  
 *  The mapping has two functions:\n
 *   - disabling unused events e.g LPAN_DELAY_RELEASE_EV by returning LPAN_NO_EVENT.
 *   - assigning logical events e.g. LPAN_SUPER_KEY_PRESSED_EV for physical #LPAN_PRESSED_EV
 *  
 *  
 *  \param ucGripId: Id of the grip
 *  
 *  \param ucPhysEvent: Id of the detected physical event for grip ucGripId\n
 *          \b Range: [#LPAN_RELEASED_EV, #LPAN_PRESSED_EV, #LPAN_DELAY_EV, #LPAN_REPEAT_EV, #LPAN_DELAY_RELEASE_EV, #LPAN_REPEAT_REL_EV]\n
 *  
 *  \b Example:
 *  \code
 *  uint8_t _LPAN_ucMapGripEvent(uint8_t ucGripId, uint8_t ucPhysEvent)
 *  {
 *    return LPAN_aaucGripMappingTable[ucGripId][ucPhysEvent-1];
 *  }
 *  \endcode
 *  
 *  
 *  
 *  \note This Function must be defined by application!\n
 *        The timer may be the same as for repeat time (not used at same time).\
 *        Use different timers if different timer segments are required.
 */
uint8_t _LPAN_ucMapGripEvent(uint8_t ucGripId, uint8_t ucPhysEvent);


#ifdef __cplusplus
}
#endif

#endif

