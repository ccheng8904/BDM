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
 *  COMP_ABBREV      JOG
 ******************************************************************************/


/******************************************************************************/
/* DOCUMENTATION                                                              */
/******************************************************************************/
/** \file joglibXS.c
 *  \brief Application specific implementation for joglib.
 *
 */


/******************************************************************************/
/* INCLUDES                                                                   */
/******************************************************************************/
#include "bsh_stdinc.h"
#include <stdint.h>
#include "LibDefines.h"
#include "joglib.h"
#include "joglibpriv.h"
#include "SegmentDef.h"

/******************************************************************************/
/* PRIVATE DEFINITIONS                                                        */
/******************************************************************************/

#define JOG_NUMBER_OF_ENCODERS 1u ///< Number of used encoders

/******************************************************************************/
/* PRIVATE FUNCTION DECLARATIONS                                              */
/******************************************************************************/
static void JOG_vIsrHandleJog(void);

/******************************************************************************/
/* PRIVATE DATA DEFINITIONS                                                   */
/******************************************************************************/

SDEF_NO_INIT uint8_t JOG_aucIsrState[JOG_NUMBER_OF_ENCODERS];


// steps and direction of each jog.
int8_t JOG_ascSteps[JOG_NUMBER_OF_ENCODERS];


SDEF_SetSegmentRW(TIMER8_10MS)
// Timer for speed measurement.
SDEF_NO_INIT uint8_t JOG_aTim8SpeedTimer[JOG_NUMBER_OF_ENCODERS];
SDEF_SetSegmentRW_Default()


// Time stamp.
uint16_t JOG_auiLastStepTime[JOG_NUMBER_OF_ENCODERS];



// Threshold time for detection of fast rotation in micro seconds.
const uint16_t JOG_uiSpeedOnThresholdTime = 15000u;


/* Threshold time for detection of slow rotation in 10ms.
 * should not be higher then 60ms
 */
const uint8_t JOG_ucSpeedOffThreholdTime10ms = 5u;


// Multiplier for counted ticks in fast mode.
const uint8_t JOG_ucSpeedMultiplier = 5u;



// number of supported jogs.
const uint8_t JOG_ucEncoderCount = JOG_NUMBER_OF_ENCODERS;



/** \brief number of steps for each cycle
 *
 *   Be careful using encoders which delivers 2 steps for a complety cycle
 *   Debouncing will be bad
 */
#define JOG_STEPS_PER_CYCLE  1u

/* State transition table
 * This table contains all transitions depend on current state (Line)
 * and input signal (column).
 */
#if (JOG_STEPS_PER_CYCLE == 1u) 
const uint8_t JOG_aucStateTransitionTable[7u][4u] =
{
    //      AB                AB                AB                AB
    //      00                01                10                11
    {JOG_STATE_IDLE,   JOG_STATE_LEFT1,  JOG_STATE_RIGHT1, JOG_STATE_IDLE},  ///< transitions for state JOG_STATE_IDLE
    {JOG_STATE_RIGHT2, JOG_STATE_RIGHT1, JOG_STATE_RIGHT1, JOG_STATE_IDLE},  ///< transitions for state JOG_STATE_RIGHT1
    {JOG_STATE_RIGHT2, JOG_STATE_RIGHT3, JOG_STATE_RIGHT2, JOG_STATE_IDLE},  ///< transitions for state JOG_STATE_RIGHT2
    {JOG_STATE_RIGHT3, JOG_STATE_RIGHT3, JOG_STATE_RIGHT3, JOG_STATE_ADD },  ///< transitions for state JOG_STATE_RIGHT3
    {JOG_STATE_LEFT2,  JOG_STATE_LEFT1,  JOG_STATE_LEFT1,  JOG_STATE_IDLE},  ///< transitions for state JOG_STATE_LEFT1
    {JOG_STATE_LEFT2,  JOG_STATE_LEFT2,  JOG_STATE_LEFT3,  JOG_STATE_IDLE},  ///< transitions for state JOG_STATE_LEFT2
    {JOG_STATE_LEFT3,  JOG_STATE_LEFT3,  JOG_STATE_LEFT3,  JOG_STATE_SUB }   ///< transitions for state JOG_STATE_LEFT3
};

#endif
#if (JOG_STEPS_PER_CYCLE == 2u) 

#define JOG_STYLE1 ///< Style of JOG, just requiered for JOG_STEPS_PER_CYCLE  2

const uint8_t JOG_aucStateTransitionTable[7u][4u] =
{

#ifdef JOG_STYLE1 // e.g. used in LC F24 project
    //      AB                AB                  AB                 AB
    //      00                01                  10                 11
    {JOG_STATE_IDLE,   JOG_STATE_DETENT1,  JOG_STATE_DETENT2, JOG_STATE_IDLE  },  ///< transitions for state JOG_STATE_IDLE    (wait for detent)
    {JOG_STATE_IDLE,   JOG_STATE_IDLE,     JOG_STATE_ADD,     JOG_STATE_RIGHT1},  ///< transitions for state JOG_STATE_RIGHT1  (wait for last step left)
    {JOG_STATE_RIGHT2, JOG_STATE_ADD,      JOG_STATE_IDLE,    JOG_STATE_IDLE  },  ///< transitions for state JOG_STATE_RIGHT2  (wait for last step left)

    {JOG_STATE_LEFT1,  JOG_STATE_DETENT1,  JOG_STATE_IDLE,    JOG_STATE_RIGHT1},  ///< transitions for state JOG_STATE_DETENT1 (look for direction)
    {JOG_STATE_LEFT1,  JOG_STATE_IDLE,     JOG_STATE_SUB,     JOG_STATE_IDLE  },  ///< transitions for state JOG_STATE_LEFT1   (wait for last step left)
    {JOG_STATE_IDLE,   JOG_STATE_SUB,      JOG_STATE_IDLE,    JOG_STATE_LEFT2 },  ///< transitions for state JOG_STATE_LEFT2   (wait for last step left)
    {JOG_STATE_RIGHT2, JOG_STATE_IDLE,     JOG_STATE_DETENT2, JOG_STATE_LEFT2 }   ///< transitions for state JOG_STATE_DETENT2 (look for direction)
 
#elif defined(JOG_STYLE2) // e.g. used in CP TE90 project
    {JOG_STATE_DETENT2,  JOG_STATE_IDLE,    JOG_STATE_IDLE,    JOG_STATE_DETENT1}, ///< transitions for state JOG_STATE_IDLE    (wait for detent)
    {JOG_STATE_ADD,      JOG_STATE_IDLE,    JOG_STATE_RIGHT1,  JOG_STATE_IDLE   }, ///< transitions for state JOG_STATE_RIGHT1  (wait for last step left)
    {JOG_STATE_IDLE,     JOG_STATE_RIGHT2,  JOG_STATE_IDLE,    JOG_STATE_ADD    }, ///< transitions for state JOG_STATE_RIGHT2  (wait for last step left)

    {JOG_STATE_IDLE,     JOG_STATE_LEFT1,   JOG_STATE_RIGHT1,  JOG_STATE_DETENT1}, ///< transitions for state JOG_STATE_DETENT1 (look for direction)
    {JOG_STATE_SUB,      JOG_STATE_LEFT1,   JOG_STATE_IDLE,    JOG_STATE_IDLE   }, ///< transitions for state JOG_STATE_LEFT1   (wait for last step left)
    {JOG_STATE_IDLE,     JOG_STATE_IDLE,    JOG_STATE_LEFT2,   JOG_STATE_SUB    }, ///< transitions for state JOG_STATE_LEFT2   (wait for last step left)
    {JOG_STATE_DETENT2,  JOG_STATE_RIGHT2,  JOG_STATE_LEFT2,   JOG_STATE_IDLE   }  ///< transitions for state JOG_STATE_DETENT2 (look for direction)
#endif
};
#endif

/******************************************************************************/
/* PUBLIC FUNCTION DEFINITIONS                                                */
/******************************************************************************/

uint8_t _JOG_ucGetLineState(uint8_t ucEncoder)
{  
    (void)ucEncoder; // unused for single encoder implementations
   /* todo
    * return pin status of encoder e.g.:
    * return (PORTG & 0x03);
    */
    return 0u; // must be implemented by application
}


uint8_t JOG_ucHandleInit(void)
{
   /* todo
    * register and start the interrupt service
    * JOG_vIsrHandleJog should be called in a timer interrupt service
    * e.g. with a frequency of 1khz.
    * hint: lower frequencies may cause lost of ticks when user rotate fast
    */
    JOG_vIsrHandleJog();// just to reference it, remove after registered as timer interupt
    return TASK_TERMINATE;
}


static void JOG_vIsrHandleJog(void)
{
    JOG_vHandleEncoders();
}

/******************************************************************************/
/* PRIVATE FUNCTION DEFINITIONS                                               */
/******************************************************************************/




