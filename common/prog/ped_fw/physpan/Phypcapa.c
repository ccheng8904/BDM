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
 *  \brief    Implementation of Physpan unit capacitive
 *
 *  \details  This unit calculates handles capacitive touch keys.
 *            disturbing variables and noise is suppressed by filtering 
 *            and tracking mechanism.
 */


/******************************************************************************/
/* INCLUDES                                                                   */
/******************************************************************************/
#include "bsh_stdinc.h"
#include <stdint.h>
#include "LibDefines.h"
#include "utility/utility.h"
#include "timer/timerlib.h"
#include "physpan.h"
#include "phyppriv.h"
#include "hsup.h"

/******************************************************************************/
/* PRIVATE DEFINITIONS                                                        */
/******************************************************************************/



/******************************************************************************/
/* PRIVATE TYPE DEFINITIONS                                                   */
/******************************************************************************/
enum taskEnum{
     PHYP_CAP_PLAUSIBILITY,    ///< State: Plausibility check, or minimum sampling
     PHYP_CAP_KEY_CALC,        ///< State: Debouncing of keys
     PHYP_CAP_MINIMUM,         ///< State: minimum tracking/filtering
     PHYP_CAP_CALIBRATION,     ///< State: Calibration used just for electronic assembly
     PHYP_CAP_INIT             ///< State: Initialisation of capacitive panel
};///< enumeration for task state


/******************************************************************************/
/* PRIVATE FUNCTION DECLARATIONS AND PRIVATE MACRO FUNCTION DEFINITIONS       */
/******************************************************************************/

static void PHYP_vCapHandleKeys(uint8_t keyId,  uint8_t percent);
static void PHYP_vNoKeyAction  (uint8_t keyId,  uint8_t percent);
static void PHYP_vNoAction(void);


/******************************************************************************/
/* PRIVATE DATA DEFINITIONS                                                   */
/******************************************************************************/

static uint8_t PHYP_ucCapTaskState = (uint8_t)PHYP_CAP_INIT;    ///< Task state of capacitive panel handler



/******************************************************************************/
/* PUBLIC FUNCTION DEFINITIONS                                                */
/******************************************************************************/

#if defined(PHYSPAN32)
uint8_t PHYP_ucCapCalcRelativeKeyValue(uint8_t ucKeyId, uint16_t uiValue)
{
    const uint16_t  range = _PHYP_uiCapGetRange(ucKeyId);
    const uint16_t  min   = _PHYP_uiCapGetMinimum(ucKeyId);
    const uint8_t   maxPercent = 0xffu;
    
    if (range == 0u)
    {
       return maxPercent;
    }

    if (uiValue <= min)
    {
        return 0u; // no negative values
    }
    else
    {
        const uint32_t  cent  = 100u;
        uint32_t percent;
        
        percent  = (uint32_t)(uiValue - min) * cent;
        percent += (uint32_t)(range/(2U)); //for rounding correctly
        percent /= (uint32_t)range;
        if ( percent != (uint32_t)((uint8_t)percent) ) // don't fit into lower 8 bit ?
        {
            return maxPercent;
        }
        else
        {
            return (uint8_t)(percent);
        }
   
    }
}
#else

#define OVERFLOW_VALUE (0xA300U)  // (((0xFFFFU/100) *64) & 0xFF00U)) (..& 0xFF00 allows the compiler just to take count the high byte of uiValue
#define SHIFT_DIV16 (4U)
#define CENT_DIV4 (uint16_t)(25U)
uint8_t PHYP_ucCapCalcRelativeKeyValue(uint8_t ucKeyId, uint16_t uiValue)
{
   const uint8_t  ucRange = _PHYP_ucCapGetRange(ucKeyId);
   const uint16_t uiMin   = PHYP_auiCapMinimum[ucKeyId];

   /* The following code solves the following calculation:
      relativeValue = (uiValue-uiMin)/256*100 /(ucRange/4) [%]
                    = (uiValue-uiMin)/64*100  /ucRange [%]

      The divisor 256 is required because of fixed point format of ucMin and uiValue [8.8] (LSB is 1/2^8=1/256)
      The divisor   4 is required because of fixed point format of ucRange [6.2] (LSB is 1/2^2=1/4)

   */
   if (uiValue<=uiMin)
   {
      return 0;
   }
   else
   {
      uiValue -= uiMin;//calculate current range for key
      /* Prevent overflow of the following calculation
         (..& 0xFF00 allows the compiler just to take count the high byte of uiValue
      */
      if ( uiValue >= OVERFLOW_VALUE)
      {
         return PHYP_MAX_VALUE;
      }
      else
      {
         uiValue >>=SHIFT_DIV16;           //:16  (64)
         uiValue  = (uint16_t)(uiValue*CENT_DIV4);     //*25  (100)
         uiValue /= ucRange;     //calculate values relative to range in %
         if (uiValue > (uint16_t)PHYP_MAX_VALUE)
         {
            return PHYP_MAX_VALUE;
         }
         else
         {
            return (uint8_t)(uiValue);
         }
      }
   }
}
#endif

uint8_t PHYP_ucHandleCapacitive(void)
{
   /** type of function pointer to key action */
   typedef void (*TKeyActionFunc)(uint8_t keyId,uint8_t percent);
   typedef void (*TPostFunc)(void); ///< type of function pointer post function

   /*Function pointer arrays for PHYP_ucHandleCapacitive*/
   static const TKeyActionFunc  PHYP_atKeyAction[(uint8_t)PHYP_CAP_CALIBRATION+1u]=
   {
      _PHYP_vCapCheckPlausibility,  // action called for each key is task: PHYP_CAP_PLAUSIBILITY
    PHYP_vCapHandleKeys,        // action called for each key is task: PHYP_CAP_KEY_CALC
      _PHYP_vCapFilterMinimum,      // action called for each key is task: PHYP_CAP_MINIMUM
    PHYP_vNoKeyAction           // action called for each key is task: PHYP_CAP_CALIBRATION, nothing to do
   };// Action for each task to be called for each key

   static const TPostFunc  PHYP_atPostAction[(uint8_t)PHYP_CAP_CALIBRATION+1u]=
   {
      _PHYP_vCapDoPostPlausibility,// action called after task is done: PHYP_CAP_PLAUSIBILITY
      _PHYP_vCapDoPostKeyScan,     // action called after task is done: PHYP_CAP_KEY_CALC
    PHYP_vNoAction,            // action called after task is done: PHYP_CAP_MINIMUM
      _PHYP_vCapDoCalibration      // action called after task is done: PHYP_CAP_CALIBRATION
   };// Action for each task to be called after task is dine (post action)


   static const enum taskEnum PHYP_aucNextTask[(uint8_t)PHYP_CAP_CALIBRATION+1u] ={
       PHYP_CAP_KEY_CALC,          // this task has to be done after: PHYP_CAP_PLAUSIBILITY
       PHYP_CAP_MINIMUM,           // this task has to be done after: PHYP_CAP_KEY_CALC
       PHYP_CAP_PLAUSIBILITY,      // this task has to be done after: PHYP_CAP_MINIMUM
       PHYP_CAP_CALIBRATION        // this task has to be done after: PHYP_CAP_CALIBRATION (no task change, stay until reset)
   };// Next task state to be done




   if (PHYP_ucCapTaskState >=(uint8_t)PHYP_CAP_INIT)//PHYP_CAP_INIT or any undefined (default tree)
   {
      if ( _PHYP_bIsCapConfigurationReady() )
      {
         PHYP_ucCapTaskState = (uint8_t)PHYP_CAP_PLAUSIBILITY;//now task is initialised
         return TASK_INITIALISED;
      }
      else
      {
         return TASK_NOT_INITIALISED;
      }
   }

    // configuration is done...
    if ( _PHYP_bIsCapDataSetValid() )//new valid dataset present?
    {
        //do for each key
        for(uint8_t keyId = 0; keyId < PHYP_ucCapNumberOfKeys; keyId++)
        {
            //filter current key value (and get the filtered result)
            uint16_t value   = _PHYP_uiCapFilterData(keyId);
            uint8_t  percent =  PHYP_ucCapCalcRelativeKeyValue(keyId,value); //calc key value in [%]
            //do the action (depend on task state)
            PHYP_atKeyAction[PHYP_ucCapTaskState](keyId, percent);    
        }
        PHYP_atPostAction[PHYP_ucCapTaskState]();                        // do the post action
        PHYP_ucCapTaskState = (uint8_t)PHYP_aucNextTask[PHYP_ucCapTaskState];     // get the next task state
    }
    else
    {
        //no valid data
        // TP(PHYP_ucHandleCapacitive_NoValidData_TP);
    }
    return TASK_INITIALISED;
}


#if defined(PHYSPAN32)

void PHYP_vCapRestartKey(uint8_t ucKeyId)
{
    _PHYP_vCapRestartKey(ucKeyId);
    _PHYP_vCapReleaseKey(ucKeyId);//todo: done in _PHYP_vCapResetKey?
}


#else //PHYSPAN32
void PHYP_vCapBlockKey(uint8_t ucKeyId)
{

#ifndef UNIT_TEST
    PHYP_auiCapMinimum[ucKeyId] = PHYP_MAX_VALUE_U16 ;//This will block the clamped key until it is free
#endif
    _PHYP_vCapReleaseKey(ucKeyId);
}



#endif


void PHYP_vCapSetCalibrationMode(void)
{
    PHYP_ucCapTaskState = (uint8_t)PHYP_CAP_CALIBRATION;//sets the calibration mode
}



/******************************************************************************/
/* PRIVATE FUNCTION DEFINITIONS                                               */
/******************************************************************************/

/** Calculates the new key state for key \<keyId\>
 *
 *  This function checks if ON/OFF thresholds are reached.
 *  I preconditions to change the key state are reached the function bIsKeyDebounced() is called.
 *  If key is debounced the key state will be changed and
 *  application will be informed by calling _PHYP_vNotifyKeyStateChanged().
 *  
 *  
 *  \param keyId: The ID of Capacitive Key\n
 *     \b Range: 0..PHYP_C_KEY_COUNT-1
 *  
 *  \param percent: The relative value of the key
 *  
 */
static void PHYP_vCapHandleKeys(uint8_t keyId, uint8_t percent)
{
    if (percent<=PHYP_ucCapOffThreshold)
    {
        _PHYP_vCapReleaseKey(keyId);
    }
    else
    {
        if (percent >= PHYP_ucCapOnThreshold)
        {
            _PHYP_vCapSetKey(keyId);
        }
    }
}

/** dummy key action, just used as function pointer */
static void PHYP_vNoKeyAction( uint8_t keyId,  uint8_t percent)
{
    //do nothing, just used as function pointer
    (void)keyId;
    (void)percent;
}

/** dummy action, just used as function pointer */
static void PHYP_vNoAction(void)
{
   //do nothing, just used as function pointer
}

