/*******************************************************************************
 *  Copyright (c) 2017 BSH Hausgeraete GmbH,
 *  Carl-Wery-Str. 34, 81739 Munich, Germany, www.bsh-group.de
 *
 *  All rights reserved. This program and the accompanying materials
 *  are protected by international copyright laws.
 *  Please contact copyright holder for licensing information.
 *
 *******************************************************************************
 * PROJECT          Generic SW
 * COMP_ABBREV      PHYP
 ******************************************************************************/


/******************************************************************************/
/* DOCUMENTATION                                                              */
/******************************************************************************/
/** \file
 *
 *  \ingroup  Physpan
 *
 *  \brief    Implementation of PhyCapXS.
 *
 *  \details  This is the configuration for capacitive panel library
 */

/******************************************************************************/
/* INCLUDES                                                                   */
/******************************************************************************/
#include "bsh_stdinc.h"
#include <stdint.h>
#include <stdbool.h>
#include "LibDefines.h"
#include "timer/timerlib.h"
#include "physpan.h"
#include "utility/utility.h"
#include "hsup.h"
#include "phyppriv.h"
#include "SegmentDef.h"
/******************************************************************************/
/* PRIVATE DEFINITIONS                                                        */
/******************************************************************************/

#define PHYP_SAMPLING_TIME          7500u  ///< Sampling time in µs 
#define PHYP_CAP_SAMPLING_COEFF        8u  ///< Tau = 64ms,  (PHYP_SAMPLING_TIME x PHYP_CAP_SAMPLING_COEFF)
#define PHYP_CAP_MIN_COEFF_FALL       16u  ///< -> settling time (95%) =  (PHYP_CAP_MIN_COEFF_FALL x 9 x PHYP_SAMPLING_TIME)
#define PHYP_CAP_MIN_COEFF_RISE       32u  ///< faster if potence of 2
#define PHYP_CAP_PLAUSI_COEFF          6u  ///< filter coefficient for plausibility peak filter (Falling time < PHYP_KEY_REACTION_TIME_ON)
#define PHYP_MAX_PERCENT_SUM         150u  ///< Keyboard will locked if percent sum of all keys reach this border [%]

#define PHYP_MIN_LOCK_THRESHOLD       50u  ///< [%] if any key is above this value minimum tracking stops for some time
#define PHYP_MIN_THRESHOLD            35u  ///< [%] minumum for keys with relative values above this value stop tracking
#define PHYP_ON_THRESHOLD             60u  ///< [%] threshold to set a key
#define PHYP_OFF_THRESHOLD            45u  ///< [%] threshold to reset a key
#define PHYP_KEY_REACTION_TIME_ON    160u  ///< reaction time for a key to be pressed
#define PHYP_KEY_REACTION_TIME_OFF   100u  ///< reaction time for a key to be released

#define PHYP_IMPLAUSIBLE_LOCK_TIME   150u
#define PHYP_MINIMUM_LOCK_TIME       200u

#define PHYP_MIN_START       0xF800u       ///< start value of minimum filter


/******************************************************************************/
/* PRIVATE TYPE DEFINITIONS                                                   */
/******************************************************************************/



/******************************************************************************/
/* PRIVATE FUNCTION DECLARATIONS AND PRIVATE MACRO FUNCTION DEFINITIONS       */
/******************************************************************************/




/******************************************************************************/
/* PRIVATE DATA DEFINITIONS                                                   */
/******************************************************************************/

  
/*lint -e9003 -e728
 *  The arrays are not explicitly initialized by intention, as array length is flexible (e728)
 *  Also some variables maybe defined in block scope, but the maintainability will be worse (e9003)
 *  Unfortunately it is not possible to switch off e9003 and e728 for selected lines
 */
static const  uint8_t PHYP_aucCapRange[PHYP_C_KEY_COUNT] =   {80,80,80,80,80};
static uint16_t  PHYP_auiCapFilteredValues[PHYP_C_KEY_COUNT];    ///< Filtered value for each key
static uint16_t  PHYP_auiCapKeyValues[PHYP_C_KEY_COUNT];         ///< copy of ADC-Values for each key
static uint16_t  PHYP_auiPeakKeyValues[PHYP_C_KEY_COUNT];        ///< peak key values for each key (used for plausibility check)
static uint8_t   PHYP_aucCapRelativeKeyValues[PHYP_C_KEY_COUNT]; ///< Current relative key values [%]


static Tpanel   PHYP_tUndebouncedKeyState;         ///< current undebounced key state
static Tpanel   PHYP_tOldUndebouncedKeyState;      ///< old undebounced key state
static uint8_t  PHYP_ucReleaseCounter;             ///< debounce counter to release the keys
static uint8_t  PHYP_ucPressCounter;               ///< debounce counter to press the keys
static uint16_t PHYP_uiPercentSum;                 ///< percent sum for plausibility check

const uint8_t   PHYP_ucCapOnThreshold = PHYP_ON_THRESHOLD;    // definition for library internal use
const uint8_t   PHYP_ucCapOffThreshold= PHYP_OFF_THRESHOLD;   // definition for library internal use
const uint8_t   PHYP_ucCapNumberOfKeys= PHYP_C_KEY_COUNT;     // definition for library internal use

uint16_t  PHYP_auiCapMinimum[PHYP_C_KEY_COUNT] = 
{
    PHYP_MIN_START,PHYP_MIN_START,PHYP_MIN_START,PHYP_MIN_START,
    PHYP_MIN_START
};// Minimum/Base-Level for each key (initialised with high value)


SDEF_SetSegmentRW(TIMER8_10MS)
static Ttimer8 PHYP_tim8ImplausibleLockTimer;///< Timer locks keyboard after implausible result
static Ttimer8 PHYP_tim8MinimumLockTimer;    ///< Timer locks keyboard after implausible result
SDEF_SetSegmentRW_Default()


/******************************************************************************/
/* PUBLIC FUNCTION DEFINITIONS                                                */
/******************************************************************************/

bool  _PHYP_bIsCapConfigurationReady(void)
{
   return true;

}

bool  _PHYP_bIsCapDataSetValid(void)
{
   /** \todo implement the key scan */
#if 0 // example implementation
   uint16_t* puiAdBuffer;
   uint8_t  ucLoop = PHYP_C_KEY_COUNT;

   puiAdBuffer = MPX_puiGetScannedAdc();//get pointer to last scanned values from ADC
   if (puiAdBuffer)//if not NULL copy to key buffer
   {
      while (ucLoop--)
      {
         PHYP_auiCapKeyValues[ucLoop]=puiAdBuffer[ucLoop];
      }
      return true;

   }
   else
   {
      //scan not finished
      return false;

   }
#else
    return true;
#endif
}

uint8_t _PHYP_ucCapGetRange(uint8_t keyId)
{
   return PHYP_aucCapRange[keyId];
}


void _PHYP_vCapReleaseKey(uint8_t keyId)
{
   if (_PHYP_bCapIsPlausible(keyId))
   {
      PHYP_tUndebouncedKeyState &= ~((Tpanel)((Tpanel)1u << keyId));//clear the key bit
   }
   else
   {
      //Not plausible: do not change anything
   }
}


void _PHYP_vCapSetKey(uint8_t keyId)
{
   if (_PHYP_bCapIsPlausible(keyId))
   {
      PHYP_tUndebouncedKeyState |= ((Tpanel)((Tpanel)1u << keyId));//clear the key bit
   }
   else
   {
      //Not plausible: do not change anything
   }
}


void  _PHYP_vCapDoPostKeyScan(void)
{
   Tpanel tKeyState = PHYP_tKeyState;
   if ((Tpanel)(~PHYP_tUndebouncedKeyState) & PHYP_tOldUndebouncedKeyState)//any key released in last cycle?
   {
      //Yes: So restart release sample sequence
      PHYP_ucReleaseCounter = 0u;
   }
   if (PHYP_tUndebouncedKeyState & (Tpanel)(~PHYP_tOldUndebouncedKeyState))//any key pressed in last cycle?
   {
      //Yey: So restart press sample sequence
      PHYP_ucPressCounter =   0u;
   }

   //debounced to release?
   if (++PHYP_ucReleaseCounter >= PHYP_CAP_CALC_POST_SAMPLES(PHYP_SAMPLING_TIME,PHYP_KEY_REACTION_TIME_OFF) )
   {
      tKeyState &= PHYP_tUndebouncedKeyState;//all released keys are debounced now
      PHYP_ucReleaseCounter = 0u;
   }

   //debounced to press?
   if (++PHYP_ucPressCounter >= PHYP_CAP_CALC_POST_SAMPLES(PHYP_SAMPLING_TIME,PHYP_KEY_REACTION_TIME_ON) )
   {
      tKeyState  |= PHYP_tUndebouncedKeyState;//all pressed keys are debounced now
      PHYP_ucPressCounter   = 0u;
   }

   //anything changed?
   if (tKeyState != PHYP_tKeyState)
   {
      PHYP_tKeyState = tKeyState; //save the new key state
      _PHYP_vNotifyKeyStateChanged();

   }

   PHYP_tOldUndebouncedKeyState = PHYP_tUndebouncedKeyState;
}


void _PHYP_vCapCheckPlausibility(uint8_t keyId,uint8_t percent)
{
    uint16_t relativePeakValue = PHYP_auiPeakKeyValues[keyId];
    uint16_t uiPercent;
    
    PHYP_aucCapRelativeKeyValues[keyId] = percent;//just for reporting/debugging
    if (percent > 100u)
    {
        percent = 100u;//limit relative value to 100%
    }
    uiPercent = (uint16_t)((uint16_t)percent<<8u);
    if (uiPercent < relativePeakValue)
    {
        relativePeakValue-=(uint16_t)(relativePeakValue/PHYP_CAP_PLAUSI_COEFF);
        relativePeakValue+=(uint16_t)(uiPercent/PHYP_CAP_PLAUSI_COEFF);
    }
    else
    {
        relativePeakValue = uiPercent;
    }
    PHYP_auiPeakKeyValues[keyId] = relativePeakValue;
    PHYP_uiPercentSum += (uint16_t)(relativePeakValue >> (uint16_t)8u);
    
    (void)PHYP_aucCapRelativeKeyValues[0]; // just to reference it 
}


bool _PHYP_bCapIsPlausible(uint8_t keyId)
{
    (void)keyId;
    return ((PHYP_tim8ImplausibleLockTimer == 0u) ? true : false);
}


void _PHYP_vCapDoPostPlausibility(void)
{
    if (PHYP_uiPercentSum > PHYP_MAX_PERCENT_SUM)
    {
        PHYP_tim8ImplausibleLockTimer = PHYP_IMPLAUSIBLE_LOCK_TIME;
    }
    PHYP_uiPercentSum = 0u;
}


uint16_t _PHYP_uiCapFilterData(uint8_t keyId)
{
   if (PHYP_auiCapKeyValues[keyId] != (uint16_t)UI_INVALID)
   {
      PHYP_auiCapFilteredValues[keyId] -= (uint16_t)(PHYP_auiCapFilteredValues[keyId]/(uint16_t)PHYP_CAP_SAMPLING_COEFF);
      PHYP_auiCapFilteredValues[keyId] += (uint16_t)(PHYP_auiCapKeyValues[keyId]/(uint16_t)PHYP_CAP_SAMPLING_COEFF);
   }
   return PHYP_auiCapFilteredValues[keyId];
}


void _PHYP_vCapFilterMinimum(uint8_t keyId,uint8_t percent)
{
   uint16_t minimum = PHYP_auiCapMinimum[keyId];
   uint16_t value   = PHYP_auiCapFilteredValues[keyId];


   if ( (percent <= PHYP_MIN_THRESHOLD) && (PHYP_tim8MinimumLockTimer == 0u) )
   {
      if (minimum <= value)
      {
         minimum-= (uint16_t)(minimum/(uint16_t)PHYP_CAP_MIN_COEFF_RISE);
         minimum+= (uint16_t)(value/(uint16_t)PHYP_CAP_MIN_COEFF_RISE);
      }
      else
      {
         minimum-= (uint16_t)(minimum/(uint16_t)PHYP_CAP_MIN_COEFF_FALL);
         minimum+= (uint16_t)(value/(uint16_t)PHYP_CAP_MIN_COEFF_FALL);
      }
   }
   else
   {
      if (percent > PHYP_MIN_LOCK_THRESHOLD)
      {
         PHYP_tim8MinimumLockTimer = PHYP_MINIMUM_LOCK_TIME;
      }
   }
   PHYP_auiCapMinimum[keyId] = minimum; //save new minimum
}


void _PHYP_vCapDoCalibration(void)
{
    /** \todo implement calibration */
}

void _PHYP_vNotifyKeyStateChanged(void)
{
    /** \todo inform the application */
}


/******************************************************************************/
/* PRIVATE FUNCTION DEFINITIONS                                               */
/******************************************************************************/

