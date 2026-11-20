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

#ifndef JOGLIBPRIV_H
#define JOGLIBPRIV_H

#ifdef __cplusplus
extern "C" {
#endif

/******************************************************************************/
/* DOCUMENTATION                                                              */
/******************************************************************************/
/** \file
 *  \ingroup  joglib
 *
 *  \brief    Public definitions and declarations for internal use in joglib.
 *
 */

/******************************************************************************/
/* INCLUDES                                                                   */
/******************************************************************************/
#include "bsh_stdinc.h"
#include <stdint.h>
#include <stdbool.h>
#include "SegmentDef.h"
/******************************************************************************/
/* PUBLIC DEFINITIONS                                                         */
/******************************************************************************/


/**
*  InputLineDef Definitions for possible state of jog lines
*/
#define JOG_LINE_A ((uint8_t)(1u<<0u)) ///< mask for input line A, used in extern defined function _JOG_ucGetLineState.
#define JOG_LINE_B ((uint8_t)(1u<<1u)) ///< mask for input line B, used in extern defined function _JOG_ucGetLineState.


/**
 *  \brief StateDefinition enumeration for possible states of jog state machine
 */
enum{
 JOG_STATE_IDLE,   ///< idle state

 JOG_STATE_RIGHT1, ///< 1st state for right rotation
 JOG_STATE_RIGHT2, ///< 2nd state for right rotation
 JOG_STATE_RIGHT3, ///< 3rd state for right rotation
 JOG_STATE_LEFT1,  ///< 1st state for left rotation
 JOG_STATE_LEFT2,  ///< 2nd state for left rotation
 JOG_STATE_LEFT3,  ///< 3rd state for left rotation
 JOG_STATE_ADD,    ///< last state of right rotation, step counter will be incremented, return to idele state
 JOG_STATE_SUB     ///< last state of left rotation, step counter will be decremented, return to idle state
};

#define  JOG_STATE_DETENT1   JOG_STATE_RIGHT3  ///< detent position 1 (used for 2 steps per cycle), state RIGHT3 unused
#define  JOG_STATE_DETENT2   JOG_STATE_LEFT3   ///< detent position 2 (used for 2 steps per cycle), state LEFT3 unused

/******************************************************************************/
/* PUPLIC DATA DECLARATIONS                                                   */
/******************************************************************************/

/** \brief Holds state of each jog.
 *
 *  \details JOG_aucIsrState saves the state of each jog dial.
 *           This array must be defined in the library template.
 *           For allowed states \sa StateDefinitions
 */
extern uint8_t JOG_aucIsrState[];



/** \brief Holds steps and direction of each jog.
 *
 *  \details JOG_ascSteps saves the state of each jog dial.
 *  The sign of JOG_ascSteps[] indicates the direction (positive = right rotation)
 *  Each call of \sa JOG_scGetSteps resets the steps of selected jog
 */
extern int8_t JOG_ascSteps[];



SDEF_SetSegmentRW(TIMER8_10MS)
/**  \brief SW-Timer for speed measurement.

 *   Provides a 10ms SW-timer for each jog dial.
 *   This field has to be defined in library template.
 */
extern uint8_t JOG_aTim8SpeedTimer[];
SDEF_SetSegmentRW_Default()


/**  \brief Time stamp for each jogdial
 *
 *   JOG_aucLastStepTime[] provides a time stamp for each jog dial.
 *   This time stamp is set each time when a tick was detected
 *   using the 16 bit circulating timer.
 *   This field has to be defined in library template.
 */
extern uint16_t JOG_auiLastStepTime[];



/**  \brief Threshold time for detection of fast rotation in micro seconds.
 *
 *   When time between two ticks is shorter than this threshold,
 *   fast rotation will be detected.
 *   Scale: 1 micro second
 *   This constant has to be defined in library template.
 *   For 15ms define:
 *   \code const uint16_t JOG_uiSpeedOnThresholdTime = 15000; \endcode
 */
extern const uint16_t JOG_uiSpeedOnThresholdTime;



/**  \brief Threshold time for detection of slow rotation in 10ms.
 *
 *   When time between two ticks is longer than this time slow rotation will be detected.
 *   Range: [0..255]
 *   Scale: 10ms
 *   This constant has to be defined in library template.
 *   For 50ms define:
 *   \code const uint8_t JOG_ucSpeedOffThreholdTime10ms  = 5; \endcode
 */
extern const uint8_t JOG_ucSpeedOffThreholdTime10ms;



/**  \brief Multiplier for counted ticks in fast mode.
 *
 *   When fast rotation is detected function \sa JOG_scGetSpeedSteps counted ticks multiplied with this factor.
 */
extern const uint8_t JOG_ucSpeedMultiplier;



/**  \brief number of supported jogs.
 *
 *   JOG_ucEncoderCount represents the number of supported jog encoders.
 *   Must be defined in library template!
 *   Range: [1..8]
 */
extern const uint8_t JOG_ucEncoderCount;

/**  \brief State transition table
 *
 *   This table cantains all transitions depend on current state (Line)
 *   and input signal (column).
 *   For different decoders variants of transition table is required.
 */
extern const uint8_t JOG_aucStateTransitionTable[7][4];


/******************************************************************************/
/* PUBLIC FUNCTION DECLARATIONS                                               */
/******************************************************************************/


/**  \brief Returns masked input line state.
 *
 *   This function must be defined in library template
 *   Do not call this function JOG_vHandleSingleEncoder/JOG_vHandleMultiEncoder will do this for you.
 *   \sa InputLineDef
 *
 *   \param ucEncoder ID of encoder to be read
 *   \return masked input line state
 */
extern uint8_t _JOG_ucGetLineState(uint8_t ucEncoder);


#ifdef __cplusplus
}
#endif

#endif
