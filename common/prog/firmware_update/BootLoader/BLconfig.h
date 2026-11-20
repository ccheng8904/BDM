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


#ifndef BLCONFIG_H
#define BLCONFIG_H

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
 *  \brief    project specific settings for boot loader
 *
 */



/******************************************************************************/
/* INCLUDES                                                                   */
/******************************************************************************/
#include <stdint.h>
#include "LibDefines.h"

/******************************************************************************/
/* PUBLIC DEFINITIONS                                                         */
/******************************************************************************/

#define BLCFG_company     "BSH"
#define BLCFG_procVendor  LD_STRINGIZE(UC_VENDOR)
#define BLCFG_procFamily  LD_STRINGIZE(PROC_FAMILY)
#define BLCFG_projectName "Generic SW"

#ifdef __cplusplus
}
#endif

#endif // BLCONFIG_H
