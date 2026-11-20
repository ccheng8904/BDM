/*******************************************************************************
 *  Copyright (c) 2017 BSH Hausgeraete GmbH,
 *  Carl-Wery-Str. 34, 81739 Munich, Germany, www.bsh-group.de
 *
 *  All rights reserved. This program and the accompanying materials
 *  are protected by international copyright laws.
 *  Please contact copyright holder for licensing information.
 *
 *******************************************************************************
 *  PROJECT          MCAL
 *  COMP_ABBREV      MPCM
 ******************************************************************************/


/******************************************************************************/
/* DOCUMENTATION                                                              */
/******************************************************************************/
/** \file
 *
 *  \ingroup  MCAL
 *
 *  \brief    Implementation of MPCM.
 *
 */


/******************************************************************************/
/* INCLUDES                                                                   */
/******************************************************************************/

#include <stdbool.h>
#include "mpcm.h"
// #include "mpcm_priv.h"

#ifdef MCAL_MPCM_NDEBUG
    #define NDEBUG
#endif 
#include "mcal_assert.h"


/******************************************************************************/
/* PRIVATE DATA DEFINITIONS                                                   */
/******************************************************************************/

/** Variable for storing first instance in callback linked list */
static struct STDCB_Callback *MCPCM_CallbackList = NULL;

/** Current power mode */
static enum MPCM_PowerMode MPCM_CurrentPowerMode = MPCM_PMODE_RESET;

/** Current clock tree configuration */
static const struct MPCM_Config *MPCM_CurrentConfig = NULL;

/** Clock tree configurations for all power modes */
static const struct MPCM_PowerModesConfig *MPCM_PmodeConfig = NULL;

/** Variable for storing request to wakeup from sleep / stop mode */
static volatile bool MPCM_WakeupRequest = false;

/** Variable for storing request abort power mode change */
static volatile bool MPCM_AbortRequest = false;


/******************************************************************************/
/* PRIVATE FUNCTION DECLARATIONS AND PRIVATE MACRO FUNCTION DEFINITIONS       */
/******************************************************************************/


static enum MCAL_Error MPCM_doPowerModeChange(enum MPCM_PowerMode powerMode);


static void MPCM_executeCallbacks(enum MCAL_Event event, enum MPCM_PowerMode powerMode)
{
    (void)powerMode;
    MCAL_invokeCallbacks(MCPCM_CallbackList, MCAL_MODULE_MPCM, event, NULL);
}


static const struct MPCM_Config *MPCM_getConfigForPowerMode(enum MPCM_PowerMode powerMode)
{
    if(MPCM_PmodeConfig == NULL)
    {
        return NULL;
    }
    
    if(powerMode == MPCM_PMODE_RUN)
    {
        return MPCM_PmodeConfig->run;
    }
    else if(powerMode == MPCM_PMODE_LPRUN)
    {
        return MPCM_PmodeConfig->lpRun;
    }
    else if(powerMode == MPCM_PMODE_SLEEP)
    {
        return MPCM_PmodeConfig->sleep;
    }
    else if(powerMode == MPCM_PMODE_STOP)
    {
        return MPCM_PmodeConfig->stop;
    }
    else
    {
        return NULL;
    }
}


static bool MPCM_isClockChange(const struct MPCM_Config *newConfig)
{
    if((newConfig == NULL) || (newConfig == MPCM_CurrentConfig))
    {
        return false;
    }
    else
    {
        return true;
    }
}


/******************************************************************************/
/* PUBLIC FUNCTION DEFINITIONS                                                */
/******************************************************************************/

enum MCAL_Error MPCM_init(const struct MPCM_PowerModesConfig *pmodeCfg)
{
    MCAL_assert(pmodeCfg);
    
    MPCM_PmodeConfig = pmodeCfg;
    
    MCAL_RETURN_ON_ERROR(MPCM_doPowerModeChange(MPCM_PMODE_RUN));
    
    MPCM_CurrentPowerMode = MPCM_PMODE_RUN;
    
    return MCAL_OK;
}


void MPCM_wakeUp(void)
{
    MPCM_WakeupRequest = true;
}


void MPCM_abortPowerModeChange(void)
{
    MPCM_AbortRequest = true;
}


enum MCAL_Error MPCM_changePowerMode(enum MPCM_PowerMode newPowerMode)
{
    const struct MPCM_Config *newConfig;
    MPCM_AbortRequest = false;
    
    if(newPowerMode != MPCM_CurrentPowerMode)
    {
        newConfig = MPCM_getConfigForPowerMode(newPowerMode);
        
        /* Signalize to all attached modules, that there is request to change power mode (clocks) */
        MPCM_executeCallbacks(MCAL_EVENT_PMODE_CHANGE_REQ, newPowerMode);
        
        if(MPCM_AbortRequest)
        {
            return MCAL_ERROR_BUSY;
        }
        
        /* If clock tree is changing, signalize also request to change clocks */
        if(MPCM_isClockChange(newConfig))
        {
            MPCM_executeCallbacks(MCAL_EVENT_CLOCK_CHANGE_REQ, newPowerMode);
        }
        
        /* Check if all modules are ready to change power mode */
        if(MPCM_AbortRequest)
        {
            return MCAL_ERROR_BUSY;
        }
        else
        {
            MCAL_RETURN_ON_ERROR(MPCM_doPowerModeChange(newPowerMode));
            
            /* If sleep or stop mode, return to previous mode after wakeup */
            if((newPowerMode == MPCM_PMODE_SLEEP) || (newPowerMode == MPCM_PMODE_STOP))
            {
                // Ignore return value - current power mode is already verified to work
                (void)MPCM_doPowerModeChange(MPCM_CurrentPowerMode);
            }
            else
            {
                MPCM_CurrentPowerMode = newPowerMode;
            }
        }
    }
    
    return MCAL_OK;
}


enum MCAL_Error MPCM_enableEvent(MCAL_Callback_t *callback, enum MCAL_Event event)
{
    if(STDCB_AttachCallback(&MCPCM_CallbackList, callback) == 0)
    {
        STDCB_EnableEvents(callback, (uint32_t)event);
        return MCAL_OK;
    }
    else
    {
        return MCAL_ERROR;
    }
}


enum MCAL_Error MPCM_disableEvent(MCAL_Callback_t *callback, enum MCAL_Event event)
{
    STDCB_DisableEvents(callback, (uint32_t)event);
    return MCAL_OK;
}


/******************************************************************************/
/* PRIVATE FUNCTION DEFINITIONS                                               */
/******************************************************************************/

static enum MCAL_Error MPCM_doPowerModeChange(enum MPCM_PowerMode powerMode)
{
    const struct MPCM_Config *newConfig;
    
    newConfig = MPCM_getConfigForPowerMode(powerMode);
    
    if(MPCM_isClockChange(newConfig))
    {
        MPCM_executeCallbacks(MCAL_EVENT_CLOCK_CHANGE_PRE, powerMode);
        MCAL_RETURN_ON_ERROR(MPCM_configure(newConfig));
        MPCM_CurrentConfig = newConfig;
        MPCM_executeCallbacks(MCAL_EVENT_CLOCK_CHANGE_POST, powerMode);
    }
    
    MPCM_executeCallbacks(MCAL_EVENT_PMODE_CHANGE_PRE, powerMode);
    
    if((powerMode == MPCM_PMODE_SLEEP) || (powerMode == MPCM_PMODE_STOP))
    {
        /* Wait in loop until MPCM_wakeUp is called from any interrupt */
        MPCM_WakeupRequest = false;
        while(!MPCM_WakeupRequest)
        {
            MPCM_setPowerModeReg(powerMode);
        }
    }
    else
    {
        MPCM_setPowerModeReg(powerMode);
    }
    
    MPCM_executeCallbacks(MCAL_EVENT_PMODE_CHANGE_POST, powerMode);
    
    return MCAL_OK;
}
