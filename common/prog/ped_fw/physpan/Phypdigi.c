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
 *  \brief    Implementation of Phypdigi.
 *
 *  \details  This unit scans an debounce the input of mechanical (digital)
 *            keys.
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
#include "phyppriv.h"


/******************************************************************************/
/* PRIVATE TYPE DEFINITIONS                                                   */
/******************************************************************************/
/** \brief  enumeration for the status of digital panel */
enum PHYP_digiStateEnum
{
    PHYP_digiNotInitState = 0u, ///< phypdigi in not initialized
    PHYP_digiInitState,         ///< phypdigi initialized
    PHYP_digiDebouncedState     ///< key status is debounced
};


/******************************************************************************/
/* PRIVATE FUNCTION DECLARATIONS                                              */
/******************************************************************************/
static bool PHYP_isDigiDebounceKeys(void);
static void PHYP_digiLoadTimer(uint16_t time);
static bool PHYP_isDigiTimerDown(void);

/******************************************************************************/
/* PRIVATE DATA DEFINITIONS                                                   */
/******************************************************************************/
static uint8_t  PHYP_ucDigiSampleCounter;  ///< counts the samples
static uint16_t PHYP_uiDigiTimer;          ///< Sw timer based on circular time



/******************************************************************************/
/* PUBLIC FUNCTION DEFINITIONS                                                */
/******************************************************************************/
uint8_t PHYP_ucHandleDigital(void)
{
    /** Status for digital panel */
    static enum PHYP_digiStateEnum PHYP_Status = PHYP_digiNotInitState;
    
    if ((PHYP_Status == PHYP_digiInitState) || (PHYP_Status == PHYP_digiDebouncedState))
    {
        if (PHYP_isDigiTimerDown())
        {
            PHYP_digiLoadTimer(PHYP_uiDigiSampleTime);
            if (PHYP_isDigiDebounceKeys())
            {   // key board is debounced
                PHYP_Status = PHYP_digiDebouncedState;
            }
        }
    }
    else
    {
        if (_PHYP_bIsDigiConfigurationReady()) // Do config before start sampling
        {
            PHYP_vRestartDigiSampleSequence();
            PHYP_Status = PHYP_digiInitState;
        }
    }
    // task initialized when panel is debounced
    return ((PHYP_Status == PHYP_digiDebouncedState) ? TASK_INITIALISED : TASK_NOT_INITIALISED);
     
}

void PHYP_vRestartDigiSampleSequence(void)
{
    _PHYP_vInitDigiSampleSequence();
    PHYP_ucDigiSampleCounter = 0u;
    PHYP_digiLoadTimer(PHYP_uiDigiSampleTime);
}

/******************************************************************************/
/* PRIVATE FUNCTION DEFINITIONS                                               */
/******************************************************************************/

/** \brief samples and multiplex the keyboard
 * 
 *  \return true if keyboard is debounced
 */ 
static bool PHYP_isDigiDebounceKeys(void)
{
    _PHYP_vGetDigiKeyState();
    if (_PHYP_bDigiSetNextChannel()) // do next channel/column of multiplexer
    {   // all columns done
        PHYP_ucDigiSampleCounter++;
        if ( PHYP_ucDigiSampleCounter >= PHYP_ucDigiSampleCount )
        { // all samples done for all columns
            PHYP_vRestartDigiSampleSequence();
            return true;
        }
    }
    return false;
}


/** \brief loads the timer with a specified time
 *
 *  \param time: time in micro seconds until timer elapse
 */
static void PHYP_digiLoadTimer(uint16_t time)
{
    PHYP_uiDigiTimer = (uint16_t)(TIM_uiGetCircleMicroSeconds() + time);
}

/** \brief checks if timer is down
 *
 *  \return true if timer is elapsed, false otherwise
 */
static bool PHYP_isDigiTimerDown(void)
{
    return ( (( (int16_t)((int16_t)TIM_uiGetCircleMicroSeconds() - (int16_t)PHYP_uiDigiTimer)) >= 0 ) ? true : false );
}
