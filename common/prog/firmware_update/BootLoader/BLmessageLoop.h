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


#ifndef BLMESSAGELOOP_H
#define BLMESSAGELOOP_H

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
 *  \brief    Public definitions and declarations for Boot-Loader Message Loop.
 *
 *  \details  Handles the message loop for Boot-Loader and RAM-Loader
 *            and calls the according service functions.
 *
 */



/******************************************************************************/
/* INCLUDES                                                                   */
/******************************************************************************/
#include <stdint.h>


/******************************************************************************/
/* PUBLIC FUNCTION DECLARATIONS                                               */
/******************************************************************************/



/** \brief    handles the ROM-Loader message loop
 *
 *  \param    bootTimeOut:  initial time out until communication established
 */
void BLMLP_handleMessageLoop(uint16_t bootTimeOut);


#ifdef __cplusplus
}
#endif

#endif // BLMESSAGELOOP_H
