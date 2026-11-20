/*******************************************************************************
 *  Copyright (c) 2017 BSH Hausgeraete GmbH,
 *  Carl-Wery-Str. 34, 81739 Munich, Germany, www.bsh-group.de
 *
 *  All rights reserved. This program and the accompanying materials
 *  are protected by international copyright laws.
 *  Please contact copyright holder for licensing information.
 *
 *******************************************************************************
 * PROJECT          SW_LIB
 * MODULE-PREFIX    PHYP
 ******************************************************************************/


/******************************************************************************/
/* DOCUMENTATION                                                              */
/******************************************************************************/
/** \file
 *
 *  \ingroup  Physpan
 *
 *  \brief    Implementation of PhyCap32XS.
 *
 *  \details  A detailed file description that may span several lines and
 *            that has to be terminated by an empty line.
 */


/******************************************************************************/
/* include MkDefines is absolutely necessary for diverse non-compiler tools   */
/* esp. for static (PC-lint, Axivion) and dynamic (Axivion) SW unit test      */
/******************************************************************************/
#include "bsh_stdinc.h"

/******************************************************************************/
/* INCLUDES                                                                   */
/******************************************************************************/
#include <stdint.h>
#include "LibTypes.h"
#include "SegmentDef.h"
#include "timer/timerlib.h"
#include "physpan.h"
#include "utility/utility.h"

#include "hsup.h"
//#include "hadc.h"
//#include "mapping.h"
#include "processor.h"
#include "phyppriv.h"

/******************************************************************************/
/* PRIVATE DEFINITIONS                                                        */
/******************************************************************************/
#define PHYP_SAMPLING_TIME          7500  ///< Sampling time in µs 
#define PHYP_CAP_SAMPLING_COEFF        8  ///< Tau = 64ms,  (PHYP_SAMPLING_TIME x PHYP_CAP_SAMPLING_COEFF)
#define PHYP_CAP_MIN_COEFF_FALL       16  ///< -> settling time (95%) =  (PHYP_CAP_MIN_COEFF_FALL x 9 x PHYP_SAMPLING_TIME)
#define PHYP_CAP_MIN_COEFF_RISE       32  ///< faster if potence of 2
#define PHYP_CAP_PLAUSI_COEFF          6  ///< filter coefficient for plausibility peak filter (Falling time < PHYP_KEY_REACTION_TIME_ON)
#define PHYP_MAX_PERCENT_SUM         150  ///< Keyboard will locked if percent sum of all keys reach this border [%]

#define PHYP_MIN_LOCK_THRESHOLD       50  ///< [%] if any key is above this value minimum tracking stops for some time
#define PHYP_MIN_THRESHOLD            35  ///< [%] minumum for keys with relative values above this value stop tracking
#define PHYP_ON_THRESHOLD             60  ///< [%] threshold to set a key
#define PHYP_OFF_THRESHOLD            45  ///< [%] threshold to reset a key
#define PHYP_KEY_REACTION_TIME_ON    160  ///< reaction time for a key to be pressed
#define PHYP_KEY_REACTION_TIME_OFF   100  ///< reaction time for a key to be released

#define PHYP_IMPLAUSIBLE_LOCK_TIME   150
#define PHYP_MINIMUM_LOCK_TIME       200

#define PHYP_MIN_START       0xF8000000   ///< start value of minimum filter


/******************************************************************************/
/* PRIVATE TYPE DEFINITIONS                                                   */
/******************************************************************************/


/******************************************************************************/
/* PRIVATE FUNCTION DECLARATIONS AND PRIVATE MACRO FUNCTION DEFINITIONS       */
/******************************************************************************/




/******************************************************************************/
/* PRIVATE DATA DEFINITIONS                                                   */
/******************************************************************************/
static ulong  PHYP_aulCapFilteredValues[PHYP_C_KEY_COUNT];   ///< Filtered value for each key
static ulong  PHYP_aulCapMinimum[PHYP_C_KEY_COUNT] = {
                                                      PHYP_MIN_START,PHYP_MIN_START,PHYP_MIN_START,PHYP_MIN_START,
                                                      PHYP_MIN_START
                                                    };// Minimum/Base-Level for each key (initilised with high value)
static uint  PHYP_auiADbuffer[PHYP_C_KEY_COUNT];            ///< ADC-Values for each key
static uint  PHYP_auiCapKeyValues[PHYP_C_KEY_COUNT];        ///< copy of ADC-Values for each key
static uint  PHYP_auiPeakKeyValues[PHYP_C_KEY_COUNT];       ///< peak key values for each key (used for plausibility check)
static uchar PHYP_aucCapRelativeKeyValues[PHYP_C_KEY_COUNT];///< Current relative key values [%]
static Tpanel PHYP_tUndebouncedKeyState;         ///< current undebounced key state
static Tpanel PHYP_tOldUndebouncedKeyState;      ///< old undebounced key state
static uchar  PHYP_ucReleaseCounter;             ///< debounce counter to release the keys
static uchar  PHYP_ucPressCounter;               ///< debounce counter to press the keys
static BOOL   PHYP_bDataSetReady;
static uint   PHYP_uiPercentSum;
static T_CONST uint PHYP_auiCapRange[PHYP_C_KEY_COUNT] =   {5000, 5000, 5000,5000, 5000}; ///< Sensetivitiy for each key (16Bit digits)
const uchar PHYP_ucCapOnThreshold = PHYP_ON_THRESHOLD;    // definition for library internal use
const uchar PHYP_ucCapOffThreshold= PHYP_OFF_THRESHOLD;   // definition for library internal use
const uchar PHYP_ucCapNumberOfKeys= PHYP_C_KEY_COUNT;     // definition for library internal use

SDEF_SetSegmentRW(TIMER8_10MS)
static Ttimer8 PHYP_tim8ImplausibleLockTimer;///< Timer locks keyboard after implausible result
static Ttimer8 PHYP_tim8MinimumLockTimer;    ///< Timer locks keyboard after implausible result

SDEF_SetSegmentRW_Default() // switch back to default RW segment

/******************************************************************************/
/* PUBLIC FUNCTION DEFINITIONS                                                */
/******************************************************************************/

void _PHYP_vNotifyKeyStateChanged(void)
{
    
}

/******************************** _PHYP_bIsCapConfigurationReady(void) ********************************/
BOOL  _PHYP_bIsCapConfigurationReady(void)
{
   /** \todo Initialize the peripherals for pannel (e.g. Interrupts, ADC) */
   return TRUE;
}


/*********************************** _PHYP_bIsCapDataSetValid(void) ***********************************/
BOOL  _PHYP_bIsCapDataSetValid(void)
{
   uint* puiAdBuffer;
   uchar ucLoop = PHYP_C_KEY_COUNT;

   puiAdBuffer = MPX_puiGetScannedAdc();//get pointer to last scanned values from ADC

   if (puiAdBuffer)//if not NULL copy to key buffer
   {
      while (ucLoop--)
      {
         PHYP_auiCapKeyValues[ucLoop]=puiAdBuffer[ucLoop];
      }
      return TRUE;
   }
   else
   {
      //scan not finished
      return FALSE;
   }
}


/********************************* _PHYP_uiCapGetRange(uchar ucKeyId) *********************************/
uint _PHYP_uiCapGetRange(uchar ucKeyId)
{
   return PHYP_auiCapRange[ucKeyId];
}


/******************************** _PHYP_vCapReleaseKey(uchar ucKeyId) *********************************/
void _PHYP_vCapReleaseKey(uchar ucKeyId)
{
   if (_PHYP_bCapIsPlausible(ucKeyId))
   {
      PHYP_tUndebouncedKeyState &= ~((Tpanel)(1<<ucKeyId));//clear the key bit
   }
}


/********************************** _PHYP_vCapSetKey(uchar ucKeyId) ***********************************/
void _PHYP_vCapSetKey(uchar ucKeyId)

{
   if (_PHYP_bCapIsPlausible(ucKeyId))
   {
      PHYP_tUndebouncedKeyState |= ((Tpanel)(1<<ucKeyId));//set the key bit
   }
}

/*********************************** _PHYP_vCapDoPostKeyScan(void) ************************************/
void  _PHYP_vCapDoPostKeyScan(void)
{
   Tpanel tKeyState = PHYP_tKeyState;
   if (~PHYP_tUndebouncedKeyState & PHYP_tOldUndebouncedKeyState)//any key released in last cycle?
   {
      //Yey: So restart release sample sequence
      PHYP_ucReleaseCounter = 0;
   }
   if (PHYP_tUndebouncedKeyState & ~PHYP_tOldUndebouncedKeyState)//any key pressed in last cycle?
   {
      //Yey: So restart press sample sequence
      PHYP_ucPressCounter =   0;
   }

   //debounced to release?
   if (++PHYP_ucReleaseCounter >= PHYP_CAP_CALC_POST_SAMPLES(PHYP_SAMPLING_TIME,PHYP_KEY_REACTION_TIME_OFF) )
   {
      tKeyState &= PHYP_tUndebouncedKeyState;//all released keys are debounced now
      PHYP_ucReleaseCounter = 0;
   }

   //debounced to press?
   if (++PHYP_ucPressCounter >= PHYP_CAP_CALC_POST_SAMPLES(PHYP_SAMPLING_TIME,PHYP_KEY_REACTION_TIME_ON) )
   {
      tKeyState  |= PHYP_tUndebouncedKeyState;//all pressed keys are debounced now
      PHYP_ucPressCounter   = 0;
   }

   //anything changed?
   if (tKeyState != PHYP_tKeyState)
   {
      PHYP_tKeyState = tKeyState; //save the new key state
      _PHYP_vNotifyKeyStateChanged();
   }

   PHYP_tOldUndebouncedKeyState = PHYP_tUndebouncedKeyState;
}


/********************* _PHYP_vCapCheckPlausibility(uchar ucKeyId,uchar ucPercent) *********************/
void _PHYP_vCapCheckPlausibility(uchar ucKeyId,uchar ucPercent)
{
   uint uiRelativePeakValue = PHYP_auiPeakKeyValues[ucKeyId];
   uint uiPercent;

   PHYP_aucCapRelativeKeyValues[ucKeyId] = ucPercent;//just for reporting/debugging
   if (ucPercent > 100)
   {
      ucPercent = 100;//limit relative value to 100%
   }
   uiPercent = (uint)(ucPercent<<8);
   if (uiPercent < uiRelativePeakValue)
   {
      uiRelativePeakValue-=(uint)(uiRelativePeakValue/PHYP_CAP_PLAUSI_COEFF);
      uiRelativePeakValue+=(uint)(uiPercent/PHYP_CAP_PLAUSI_COEFF);
   }
   else
   {
      uiRelativePeakValue = uiPercent;
   }
   PHYP_auiPeakKeyValues[ucKeyId] = uiRelativePeakValue;
   PHYP_uiPercentSum += (uchar)(uiRelativePeakValue>>8);
}

/******************************** _PHYP_bCapIsPlausible(uchar ucKeyId) ********************************/
/*lint -save -efunc(715,_PHYP_bCapIsPlausible)    disable lint args not used  */
BOOL _PHYP_bCapIsPlausible(uchar ucKeyId)
{
   return (BOOL)(!PHYP_tim8ImplausibleLockTimer);
}

/********************************* _PHYP_vCapDoPostPlausibility(void) *********************************/
void _PHYP_vCapDoPostPlausibility(void)
{
   if (PHYP_uiPercentSum > PHYP_MAX_PERCENT_SUM)
   {
      PHYP_tim8ImplausibleLockTimer = PHYP_IMPLAUSIBLE_LOCK_TIME;

   }
   PHYP_uiPercentSum = 0;
}

/******************************** _PHYP_vCapRestartKey(uchar ucKeyId) *********************************/
void  _PHYP_vCapRestartKey(uchar ucKeyId)
{
    PHYP_aulCapMinimum[ucKeyId] = PHYP_MIN_START;
    PHYP_tKeyState &= ~((Tpanel)(1<<ucKeyId));//clear the key bit
}


/******************************************************************************************************/
/* PLAUSIBILITY CHECKS                                                                                */
/******************************************************************************************************/

/******************************** _PHYP_uiCapFilterData(uchar ucKeyId) ********************************/
uint _PHYP_uiCapFilterData(uchar ucKeyId)
{
   if (PHYP_auiCapKeyValues[ucKeyId] != (uint)UI_INVALID)
   {
      PHYP_aulCapFilteredValues[ucKeyId]-= PHYP_aulCapFilteredValues[ucKeyId]/PHYP_CAP_SAMPLING_COEFF;
      PHYP_aulCapFilteredValues[ucKeyId]+= ((ulong)(((ulong)(PHYP_auiCapKeyValues[ucKeyId]))<<16))/PHYP_CAP_SAMPLING_COEFF;
   }
   return HSUP_uiGetHighWord(PHYP_aulCapFilteredValues[ucKeyId]);
}


/*********************** _PHYP_vCapFilterMinimum(uchar ucKeyId,uchar ucPercent) ***********************/
void _PHYP_vCapFilterMinimum(uchar ucKeyId,uchar ucPercent)
{
   ulong ulMinimum = PHYP_aulCapMinimum[ucKeyId];
   uint  uiValue   = HSUP_uiGetHighWord(PHYP_aulCapFilteredValues[ucKeyId]);

   if ( (ucPercent <= PHYP_MIN_THRESHOLD) && !PHYP_tim8MinimumLockTimer)
   {
      if (HSUP_uiGetHighWord(ulMinimum) <= uiValue)
      {
         ulMinimum-= ulMinimum/PHYP_CAP_MIN_COEFF_RISE;
         ulMinimum+= ((ulong)(((ulong)uiValue)<16))/PHYP_CAP_MIN_COEFF_RISE;
      }
      else
      {
         ulMinimum-= ulMinimum/PHYP_CAP_MIN_COEFF_FALL;
         ulMinimum+= ((ulong)(((ulong)uiValue)<16))/PHYP_CAP_MIN_COEFF_FALL;
      }
   }
   else
   {
      if (ucPercent > PHYP_MIN_LOCK_THRESHOLD)
      {
         PHYP_tim8MinimumLockTimer = PHYP_MINIMUM_LOCK_TIME;
      }
   }
   PHYP_aulCapMinimum[ucKeyId] = ulMinimum; //save new minimum
}


/*********************************** _PHYP_uiCapGetMinimum(ucKeyId) ***********************************/
uint _PHYP_uiCapGetMinimum(uchar ucKeyId)
{
    return HSUP_uiGetHighWord(PHYP_aulCapFilteredValues[ucKeyId]);
}


/*********************************** _PHYP_vCapDoCalibration(void) ************************************/
void _PHYP_vCapDoCalibration(void)
{
   /** \todo implement calibration */
}

/******************************************************************************/
/* PRIVATE FUNCTION DEFINITIONS                                               */
/******************************************************************************/


