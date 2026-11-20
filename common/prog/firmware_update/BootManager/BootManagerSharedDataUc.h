/*******************************************************************************
 *  Copyright (c) 2017 BSH Hausgeraete GmbH,
 *  Carl-Wery-Str. 34, 81739 Munich, Germany, www.bsh-group.de
 *
 *  All rights reserved. This program and the accompanying materials
 *  are protected by international copyright laws.
 *  Please contact copyright holder for licensing information.
 *
 *******************************************************************************
 *  PROJECT          PP Boot-Manager
 ******************************************************************************/


#ifndef BOOTMANAGERSHAREDDATAUC_H
#define BOOTMANAGERSHAREDDATAUC_H

#ifdef __cplusplus
extern "C" {
#endif


/******************************************************************************/
/* DOCUMENTATION                                                              */
/******************************************************************************/
/** \file
 *
 *  \ingroup  BootManager
 *
 *  \brief    Private definitions and declarations for Boot Manager shared data.
 *
 *  \details  These interfaces are not public and for internal use only.
 *            It shall be implemented platform specific
 */


/******************************************************************************/
/* INCLUDES                                                                   */
/******************************************************************************/
#include <stdbool.h>




/******************************************************************************/
/* PUBLIC FUNCTION DECLARATIONS                                               */
/******************************************************************************/



/** \brief    returns true in case of usual power in reset
 *            that has to be terminated by an empty line.
 *
 *  \details  This function should be implemented for each platform
 *            If the platform does not support this feature this function shall
 *            return false.
 *            In this case a generic algorithm is used to check whether shared
 *            data is initialized or not.
 *
 *  \return   true: power on reset, false: no power on or feature not supported
 */
extern bool BMDAT_isPowerOnReset(void);


/** \brief    Sets the shared data as initialized.
 *
 *  \details  This function shall be implemented for each platform
 *            It may be implemented by setting a magic pattern inside no-init RAM.
 *            Interface \sa BMSD_initSharedData calls this after init is done.
 *            
 *  \note     The magic pattern will be read before writing so a possible
 *            parity check shall be deactivated or special RAM shall be used.
 *            Care must be taken, that the used RAM is not initialized by start-up
 *
 */
extern void BMDAT_setDataInitializedFlag(void);


/** \brief    Sets the shared data as uninitialized.
 *
 *  \details  This function shall be implemented for each platform
 */
extern void BMDAT_resetDataInitializedFlag(void);


/** \brief    returns true if initialized flag was already set after power-on.
 *
 *  \details  This function shall be implemented for each platform.
 *            
 *  \note     The magic pattern will be read before writing so a possible
 *            parity check shall be deactivated or special RAM shall be used.
 *            Care must be taken, that the used RAM is not initialized by start-up
 *
 */
extern bool BMDAT_isDataInitialized(void);



#ifdef __cplusplus
}
#endif

#endif // BOOTMANAGERSHAREDDATAUC_H
