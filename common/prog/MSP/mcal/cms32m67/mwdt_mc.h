/*******************************************************************************
 *  Copyright (c) 2023 BSH Hausgeraete GmbH,
 *  Carl-Wery-Str. 34, 81739 Munich, Germany, www.bsh-group.de
 *
 *  All rights reserved. This program and the accompanying materials
 *  are protected by international copyright laws.
 *  Please contact copyright holder for licensing information.
 *
 *******************************************************************************
 *  PROJECT          MCAL
 *  COMP_ABBREV      MWDT
 *  PROCESSOR        CMS32M67xx
 ******************************************************************************/


/******************************************************************************/
/* DOCUMENTATION                                                              */
/******************************************************************************/
/** \file     mwdt_mc.h
 *
 *  \ingroup  MCAL
 *
 *  \brief    Public definitions and declarations for unit MWDT.
 *
 *  \details  The CMS32M67xx watchdog is enabled by the user option byte 0xC0
 *            (WDTON) and is clocked by the low speed internal oscillator
 *            (fIL = 15kHz).
 *
 */

#ifndef MWDT_MC_H
#define MWDT_MC_H

#ifdef __cplusplus
extern "C" {
#endif

/******************************************************************************/
/* INCLUDES                                                                   */
/******************************************************************************/
#include <assert.h>
#include "BSH_stdinc.h"
#include "processor.h"
#include "mcal/mwdt_types.h"
#include "mcal/mwdt.h"

/******************************************************************************/
/* PUBLIC FUNCTION DECLARATIONS                                               */
/******************************************************************************/

/** Initialization function for MWDT.
 *  \note This function prepares, configures and enables watchdog peripheral unit
 *        according to defined MWDT configuration structure.
 *  \param config: pointer to configuration structure
 *  \return standard MCAL return value
 */
extern enum MCAL_Error MWDT_initWDT(const struct MWDT_Config *config);


/** Function for Watchdog triggering.
 *  \param none
 *  \return none
 */
extern void MWDT_triggerWDT(void);


/** This function unlocks Watchdog peripheral.
 *  \note If peripheral is locked, any further changes are not possible,
 *        the only option to change Watchdog parameters is to reset MCU.
 *  \param none
 *  \return MCAL_Error: MCAL_OK = WDT unlocked, MCAL_ERROR_NOT_READY = WDT unlock failed
 */
extern enum MCAL_Error MWDT_unlock(void);

/** Initializes local configuration structure for MWDT.
 *  \note Necessary for avoiding HardFault because of uninitialized local MWDT
 *        configuration structure in case MWDT_init() is not called.
 *  \param none
 *  \return none
 */
extern void MWDT_initConfigStructMWDT(void);

#ifdef __cplusplus
}
#endif

#endif // MWDT_MC_H
