/*******************************************************************************
 *  Copyright (c) 2017 BSH Hausgeraete GmbH,
 *  Carl-Wery-Str. 34, 81739 Munich, Germany, www.bsh-group.de
 *
 *  All rights reserved. This program and the accompanying materials
 *  are protected by international copyright laws.
 *  Please contact copyright holder for licensing information.
 *
 *******************************************************************************
 *  PROJECT          Firmware Update MAL Driver definitions
 ******************************************************************************/


#ifndef MAL_DRIVER_H
#define MAL_DRIVER_H

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
 *  \brief    Public definitions and declarations for unit MAL Driver.
 *
 */


/******************************************************************************/
/* INCLUDES                                                                   */
/******************************************************************************/
#include <stdint.h>
#include "firmware_update/mal/module_administration_cfg.h"


/** \name Bit masks for moduleFlags
@{**/
#define MAL_CTX_MF_READ_BLOCK               0x01U ///< read attribute
#define MAL_CTX_MF_WRITE                    0x02U ///< write attribute
#define MAL_CTX_MF_ERASE                    0x04U ///< erase attribute
#define MAL_CTX_MF_EXECUTABLE_EXT           0x08U ///< extension/fragment of an executable
#define MAL_CTX_MF_EXECUTABLE               0x10U ///< executable context
#define MAL_CTX_MF_NO_INTEGRITY_CHECK       0x20U ///< disable implicit integrity checking for modules in internal flash
#define MAL_CTX_MF_NO_COMPATIBILITY_CHECK   0x40U ///< disable compatibility checks between HW & SW
#define MAL_CTX_MF_WE           (uint8_t)(MAL_CTX_MF_WRITE | MAL_CTX_MF_ERASE)
#define MAL_CTX_MF_RWEX         (uint8_t)(MAL_CTX_MF_READ_BLOCK | MAL_CTX_MF_WRITE | MAL_CTX_MF_ERASE | MAL_CTX_MF_EXECUTABLE)
#define MAL_CTX_MF_RWEXX        (uint8_t)(MAL_CTX_MF_READ_BLOCK | MAL_CTX_MF_WRITE | MAL_CTX_MF_ERASE | MAL_CTX_MF_EXECUTABLE_EXT)
#define MAL_CTX_MF_RWE          (uint8_t)(MAL_CTX_MF_READ_BLOCK | MAL_CTX_MF_WRITE | MAL_CTX_MF_ERASE)
/**@}*/



#ifdef __cplusplus
}
#endif

#endif // MAL_DRIVER_H
