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
 *  COMP_ABBREV      MPCM
 *  PROCESSOR        CMS32M67xx
 ******************************************************************************/


/******************************************************************************/
/* DOCUMENTATION                                                              */
/******************************************************************************/
/** \file     mpcm_priv.h
 *
 *  \ingroup  MCAL
 *
 *  \brief    Private definitions and declarations for unit MPCM.
 *
 */

#ifndef MPCM_PRIV_H
#define MPCM_PRIV_H

#ifdef __cplusplus
extern "C" {
#endif

/******************************************************************************/
/* INCLUDES                                                                   */
/******************************************************************************/

#include "bsh_stdinc.h"
#include "mcal/mcal.h"
#include "mcal/mpcm_types.h"
#include "mpcm_mc.h"


/******************************************************************************/
/* PRIVATE FUNCTION DECLARATIONS                                              */
/******************************************************************************/

/** Low level clock configuration of clock tree and power mode.
 *  \note This function initializes clock tree based on clock config structure
 *  \param clockConfig: pointer to configuration structure
 */
extern enum MCAL_Error MPCM_configure(const struct MPCM_Config *config);


/** Low level configuration of clock output pin.
 *  \note This function initializes pin that outputs clock bus to physical pin
 *  \param clockOutputChannel: pointer to configuration structure
 */
extern enum MCAL_Error MPCM_configureClockOutput(const struct MPCM_ClockOutputChannel *clockOutputChannel);


/** Low level power mode configuration of power mode.
 *  \note This function sets power mode register according power mode parameter
 *  \param powerMode: new power mode
 */
extern void MPCM_setPowerModeReg(enum MPCM_PowerMode powerMode);


#ifdef __cplusplus
}
#endif

#endif // MPCM_PRIV_H
