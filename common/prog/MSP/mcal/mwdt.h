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
 *  COMP_ABBREV      MWDT
 ******************************************************************************/


#ifndef MWDT_H
#define MWDT_H

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
 *  \brief    Public definitions and declarations for unit MWDT.
 *
 */
/******************************************************************************/
/* INCLUDES                                                                   */
/******************************************************************************/
#include "BSH_stdinc.h"
#include <stdint.h>
#include <string.h>
#include <stdbool.h>
#include "std_lib/std_callback.h"

#include "mcal/mcal.h"
#include "mcal/mwdt_types.h"


/******************************************************************************/
/* PUBLIC DATA DEFINITIONS                                                    */
/******************************************************************************/

/* Predefined Watchdog timeouts.
You are free to create your own timeout in your project files */
#define MWDT_1MS_UPPER_TIMELIMIT        1U
#define MWDT_10MS_UPPER_TIMELIMIT       10U
#define MWDT_100MS_UPPER_TIMELIMIT      100U
#define MWDT_1S_UPPER_TIMELIMIT         1000U
#define MWDT_8S_UPPER_TIMELIMIT         8000U
#define MWDT_32S_UPPER_TIMELIMIT        32000U

/* Predefined Watchdog window times.
You are free to create your own time definitions in your project files */
#define MWDT_10MS_LOWER_TIMELIMIT       10U
#define MWDT_100MS_LOWER_TIMELIMIT      100U
#define MWDT_1S_LOWER_TIMELIMIT         1000U
#define MWDT_8S_LOWER_TIMELIMIT         8000U
#define MWDT_16S_LOWER_TIMELIMIT        16000U

/******************************************************************************/
/* PRIVATE DATA DEFINITIONS                                                    */
/******************************************************************************/

/* Defined Watchdog precision limit time in %.
If is your selected Watchdog timeouts achievable in hardware then Watchdog is set and MWDT_init return MCAL_OK.
In other case Watchdog is not set (is in previous state) and MWDT_init return MCAL_ERROR_CONFIG */
#define MWDT_PRECISION_LIMIT_CHECK      5u

/******************************************************************************/
/* PUBLIC FUNCTION DECLARATIONS                                               */
/******************************************************************************/

/** Initialization function for MWDT.
 *  \note This function prepares, configures and enables watchdog peripheral unit
 *        according to defined MWDT configuration structure.
 *  \param config: pointer to configuration structure
 *  \return standard MCAL return value
 */
extern enum MCAL_Error MWDT_init(const struct MWDT_Config *config);


/** Function for Watchdog triggering.
 *  \param none
 *  \return none
 */

/* Axivion Next Line MisraC2012-8.5 : Function declared in more than one file. */
extern void MWDT_trigger(void);


/** Initializes local configuration structure for MWDT.
 *  \note Necessary for avoiding HardFault because of uninitialized local
 *        MWDT configuration structure in case MWDT_init() is not called.
 *  \param none
 *  \return none
 */
extern void MWDT_initConfigStruct(void);


#ifdef __cplusplus
}
#endif

#endif // MWDT_H
