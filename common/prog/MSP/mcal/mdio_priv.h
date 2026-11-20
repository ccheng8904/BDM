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
 *  COMP_ABBREV      MDIO
 ******************************************************************************/


#ifndef MDIO_PRIV_H
#define MDIO_PRIV_H

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
 *  \brief    Private definitions and declarations for unit MDIO.
 *
 */


/******************************************************************************/
/* INCLUDES                                                                   */
/******************************************************************************/

#include "BSH_stdinc.h"

#include <stdint.h>
#include <stdbool.h>

#include "mcal/mdio_types.h"
#include "mdio_mc.h"


/******************************************************************************/
/* PUBLIC FUNCTION DECLARATIONS                                               */
/******************************************************************************/


/** Initialization function for MDIO.
 *  \note This function initializes all properties on peripheral HW for certain I/O pin.
 *  \param channel: pointer to MDIO_Channel instance
 *  \param cfg: pointer to configuration structure
 *  \return void
 */
extern enum MCAL_Error MDIO_configure(const struct MDIO_Channel *channel, const struct MDIO_Config *cfg);

extern void MDIO_enablePort(const struct MDIO_Port *port);

extern const struct MDIO_Port *MDIO_getPort(const struct MDIO_Channel *channel);

extern enum MDIO_Pin MDIO_getPin(const struct MDIO_Channel *channel);


#ifdef __cplusplus
}
#endif

#endif // MDIO_PRIV_H
