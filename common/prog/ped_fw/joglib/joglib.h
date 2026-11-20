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

#ifndef JOGLIB_H
#define JOGLIB_H

#ifdef __cplusplus
extern "C" {
#endif

/******************************************************************************/
/* DOCUMENTATION                                                              */
/******************************************************************************/

/** \file     joglib.h
 *  \ingroup  joglib
 *  \brief   Public definitions and declarations of library joglib.
 *
 *  \details Evaluate ticks of incremental encoder.
 *           De-bouncing and speed detection is included.
 */

/******************************************************************************/
/* INCLUDES                                                                   */
/******************************************************************************/
#include "bsh_stdinc.h"
#include <stdint.h>
#include <stdbool.h>
#include "LibDefines.h"
#include "joglib/joglibXS.h"

/******************************************************************************/
/*   DEPENDENCIES                                                             */
/******************************************************************************/
#define DEPLIB_JOG MOD_TIM, ///< dependencies of library module


/******************************************************************************/
/* PUBLIC FUNCTION DECLARATIONS                                               */
/******************************************************************************/


/**  \brief Handler for multi encoder configuration
 *
 *   This function shall be called from timer interrupt.
 *   Up to 8 encoders are supported.
 *   The interrupt service shall be defined in library template
 */
void JOG_vHandleEncoders(void);


/**  \brief Returns the number of steps since last call.
 *
 *   For left rotation negative values will be returned.
 *   Each call of JOG_scGetSteps resets \sa JOG_ascSteps for selected channel.
 *
 *  \param  ucEncoder: ID of JOG to be read out [0..7].
 *  \return count of logical ticks (sign gives direction)
 */
int8_t JOG_scGetSteps(uint8_t ucEncoder);


/**  \brief Returns the number of steps since last call.
 *  
 *   For left rotation negative values will be returned.
 *   Each call resets \sa JOG_ascSteps for selected channel.
 *   When fast rotation is detected the count of steps will be multiplied with
 *   \sa JOG_ucSpeedMultiplier.
 *
 *  \param  ucEncoder: ID of JOG to be read out [0..7].
 *  \return count of logical ticks (sign gives direction)
 */
int8_t JOG_scGetSpeedSteps(uint8_t ucEncoder);


/**  \brief Returns the speed status of selected jog dial.
 *
 *   If fast rotation is detected for defined jog dial this function returns \c true.
 *
 *  \param  ucEncoder: ID of JOG to be read out [0..7].
 *  \return false for slow rotation, true for fast rotation
 */
bool JOG_bIsFastDetected(uint8_t ucEncoder);


/** \brief This is the task handler of JOG library.
 *
 *  The handler shall be called by scheduler until returns TASK_TERMINATE.
 *  This interface function has to be defined in library template e.g. to activate interrupts.
 *  Depend on implementation normally just TASK_TERMINATE will be returned.
 
 *  \return
 *  TASK_NOT_INITIALISED: module is not initialised\n
 *  TASK_TERMINATE:       module shall not be called again
 */
extern uint8_t JOG_ucHandleInit(void);

#ifdef __cplusplus
}
#endif

#endif
