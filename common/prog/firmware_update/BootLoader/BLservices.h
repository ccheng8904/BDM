/*******************************************************************************
 *  Copyright (c) 2017 BSH Hausgeraete GmbH,
 *  Carl-Wery-Str. 34, 81739 Munich, Germany, www.bsh-group.de
 *
 *  All rights reserved. This program and the accompanying materials
 *  are protected by international copyright laws.
 *  Please contact copyright holder for licensing information.
 *
 *******************************************************************************
 *  PROJECT          Generic SW / Boot-Loader
 ******************************************************************************/


#ifndef BLSERVICES_H
#define BLSERVICES_H

#ifdef __cplusplus
extern "C" {
#endif


/******************************************************************************/
/* DOCUMENTATION                                                              */
/******************************************************************************/
/** \file
 *
 *  \ingroup  firmware_update
 *
 *  \brief    Public definitions and declarations Boot-Loader service functions.
 */



/******************************************************************************/
/* INCLUDES                                                                   */
/******************************************************************************/
#include <stdint.h>
#include "BLcom.h"


/******************************************************************************/
/* PUBLIC FUNCTION DECLARATIONS                                               */
/******************************************************************************/
/** Initializes the unit */
void BLSRV_init(void);

/** do the service for a received command and returns the result
 *
 * \details The 4 byte message frame is input for this function
 *          If required for the specific command additional data frame will be
 *          received and proceeded.
 *
 * \param buffer: pointer the the receive message buffer
 * \return message result
 */
enum BLCOM_rxResult BLSRV_doCommandService(const uint8_t *buffer);


#ifdef __cplusplus
}
#endif

#endif // BLSERVICES_H
