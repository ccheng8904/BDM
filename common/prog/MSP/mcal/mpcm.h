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
 *  COMP_ABBREV      MPCM
 ******************************************************************************/


#ifndef MPCM_H
#define MPCM_H

#ifdef __cplusplus
extern "C" {
#endif


/******************************************************************************/
/* DOCUMENTATION                                                              */
/******************************************************************************/
/** \file
 *
 *  \ingroup  MCAL
 *
 *  \brief    Public definitions and declarations for unit MPCM.
 *
 */


/******************************************************************************/
/* INCLUDES                                                                   */
/******************************************************************************/

#include "bsh_stdinc.h"
#include "std_lib/std_callback.h"

#include "mcal/mcal.h"
#include "mcal/mpcm_priv.h"
#include "mcal/mpcm_types.h"
#include "mpcm_mc.h"


/******************************************************************************/
/* PUBLIC DEFINITIONS                                                         */
/******************************************************************************/


/******************************************************************************/
/* PUBLIC TYPE DEFINITIONS                                                    */
/******************************************************************************/


struct MPCM_PowerModesConfig
{
    const struct MPCM_Config *run;
    const struct MPCM_Config *lpRun;
    const struct MPCM_Config *sleep;
    const struct MPCM_Config *stop;
};


/******************************************************************************/
/* PUBLIC FUNCTION DECLARATIONS                                               */
/******************************************************************************/

/** Function for initial clock and power mode configuration.
 *  \note This function should be called before any MCAL module is initialized,
 *        to ensure, that correct clock frequencies are set.
 *  \param pmodeCfg: pointer to clock configurations for power modes
 */
extern enum MCAL_Error MPCM_init(const struct MPCM_PowerModesConfig *pmodeCfg);


/** Function for signalizing wake up request from sleep/stop mode.
 *  \note When the MCU enters sleep/stop mode, it remains in this mode until wake-up
 *        request is received from any interrupt that is capable of running such
 *        mode.
 */
extern void MPCM_wakeUp(void);


/** Function for signalizing abort of power mode change.
 *  \note This function is intended to be used in MCAL_EVENT_PMODE_CHANGE_REQ or
 *        MCAL_EVENT_CLOCK_CHANGE_REQ callback to signalize that some module is
 *        not ready for power mode or clock change transition.
 */
extern void MPCM_abortPowerModeChange(void);


/** Function for changing power mode.
 *  \note There are multiple stages during change of power mode. Any module can
 *        interrupt change of power mode by calling MPCM_abortPowerModeChange in
 *        callback function attached to any request to change event MCAL_EVENT_XXX_CHANGE_REQ.
 *        It is up to user to make sure that all modules are disabled/prepared
 *        for power mode change before calling this function, otherwise the
 *        success of power mode change depends on state of the modules.
 *  \param newPowerMode: new requested power mode
 *  \return standard MCAL rerun value
 */
extern enum MCAL_Error MPCM_changePowerMode(enum MPCM_PowerMode newPowerMode);


/** Function for getting clock frequency from specific clock source.
 *  \note If certain clock source is disabled, function returns 0
 *  \param clockSrc: clock source (bus)
 *  \return frequency of clock in Hz. 0 if bus is disabled or incorrectly set.
 */
extern uint32_t MPCM_getClockFreq(enum MPCM_Clock clockSrc);


/** Function for enabling of event.
 *  \brief This function enables specific event for callback functionality.
 *  \param callback: callback instance for which the event will be enabled
 *  \param event: event to be enabled;
 */
extern enum MCAL_Error MPCM_enableEvent(MCAL_Callback_t *callback, enum MCAL_Event event);


/** Function for disabling of event.
 *  \brief This function disables specific event for callback functionality.
 *  \param callback: callback instance for which the event will be disabled
 *  \param event: event to be disabled;
 */
extern enum MCAL_Error MPCM_disableEvent(MCAL_Callback_t *callback, enum MCAL_Event event);


#ifdef __cplusplus
}
#endif

#endif // MPCM_H
