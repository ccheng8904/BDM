/*******************************************************************************
 *  Copyright (c) 2017 BSH Hausgeraete GmbH,
 *  Carl-Wery-Str. 34, 81739 Munich, Germany, www.bsh-group.de
 *
 *  All rights reserved. This program and the accompanying materials
 *  are protected by international copyright laws.
 *  Please contact copyright holder for licensing information.
 *
 *******************************************************************************
 *  PROJECT          application SW ID module
 ******************************************************************************/


/******************************************************************************/
/* DOCUMENTATION                                                              */
/******************************************************************************/
/** \file sw_version.c
 *
 *  \brief    Implementation of SW ID module.
 *
 *  \details  Template file for SW ID structure.
 */


/******************************************************************************/
/* INCLUDES                                                                   */
/******************************************************************************/
#include "version/fwu_version.h"
#include "build_version.h"
#include "std_lib/std_symbol.h"

/******************************************************************************/
/* PRIVATE DATA DEFINITIONS                                                   */
/******************************************************************************/

/** \brief    SW ID structure
 *
 *  \details  SW version structure defined by project
 */

/* 
If fixed position of SW_version within application is needed, create
corresponding segment in linkerscript file and uncomment following lines.
*/
// SDEF_SetSegmentConst(APP_SWVERSION)
SYMBOL_USED const struct FWU_sw_version_s SW_version =
{
    .version =
    { /** \todo please specify id-string for your application */
        .ID       = STDV_ParseIdentifier(FW_UID),         // ID    defined by build process
        .major    = STDV_ParseVersion(VERSION_MAJOR),     // major defined by build process
        .minor    = STDV_ParseVersion(VERSION_MINOR),     // minor defined by build process
        .revision = STDV_ParseVersion(VERSION_REVISION),  // revision defined by build process
        .build    = STDV_ParseVersionBuild(VERSION_BUILD) // build defined by build process
    }
};
// SDEF_SetSegmentConst_Default()
