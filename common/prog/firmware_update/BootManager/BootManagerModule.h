/*******************************************************************************
 *  Copyright (c) 2017 BSH Hausgeraete GmbH,
 *  Carl-Wery-Str. 34, 81739 Munich, Germany, www.bsh-group.de
 *
 *  All rights reserved. This program and the accompanying materials
 *  are protected by international copyright laws.
 *  Please contact copyright holder for licensing information.
 *
 *******************************************************************************
 *  PROJECT          Firmware Update Boot-Manager
 ******************************************************************************/


#ifndef BOOTMANAGERMEMORYMODULE_H
#define BOOTMANAGERMEMORYMODULE_H

#ifdef __cplusplus
extern "C" {
#endif


/******************************************************************************/
/* DOCUMENTATION                                                              */
/******************************************************************************/
/** \file
 *
 *  \ingroup BootManager
 *
 *  \brief   Public definitions and declarations for Boot Manager memory module
 *
 */

/******************************************************************************/
/* INCLUDES                                                                   */
/******************************************************************************/
#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include "firmware_update/fwu_config.h"
#include "firmware_update/version/fwu_version.h"
#include "firmware_update/mal/moduleAdministration.h"


/******************************************************************************/
/* PUBLIC FUNCTION DECLARATIONS                                               */
/******************************************************************************/
#ifdef VARIANT_BOOTMANAGER
/** \brief    initialises the module with specified module ID
 *
 *  \param    ModuleId:  id of module to initialise
 *  \return   returns true in case of success
 *  \note     Call this interface before any other
 */
extern bool BMMOD_bInitMemoryModule(enum MAL_ModuleEnum ModuleId);


/** \brief    starts the specified module.
 *
 *  \note     This function starts another application and never returns
 *
 */
extern void BMMOD_startModule(void);


/** \brief checks the integrity of the selected module
 *
 *  \return true if the integrity is OK
 *
 */
extern bool BMMOD_isModuleIntegrityOk(void);


/** \brief checks if module may be started
 *
 *  \note only modules with according attribute in partition table my be executed
 *  \return true if module may be started
 */
extern bool BMMOD_isModuleStartable(void); ///< checks if module may be started


/** \brief checks the validity of fw ids of modules
 *
 *  \note Fw ids from each header shall be identical with fw ids in protected area
 *  \return true if fw-ids are correct for present HW
 */
extern bool BMMOD_isModuleFirmwareIdValid(void);

#endif

/** \brief delivers pointer to memory module header
 *
 *  \param    module:  id of a memory module
 *
 *  \return pointer to memory module header or NULL when not found
 */
const struct MAL_ModuleHeader_s * BMMOD_GetMemModuleHeader(enum MAL_ModuleEnum module);

#ifdef __cplusplus
}
#endif

#endif // BOOTMANAGERMEMORYMODULE_H
