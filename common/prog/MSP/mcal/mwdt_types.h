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
 *  COMP_ABBREV      MWDT
 ******************************************************************************/


#ifndef MWDT_TYPES_H
#define MWDT_TYPES_H

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
 *  \brief    Public definitions and type definitions for unit MWDT.
 *
 */


/******************************************************************************/
/* INCLUDES                                                                   */
/******************************************************************************/
#include "BSH_stdinc.h"


/******************************************************************************/
/* PUBLIC TYPE DEFINITIONS                                                    */
/******************************************************************************/

enum MWDT_DebugConfig
{
    MWDT_DEBUG_RUNNING,     // watchdog runs during debug session
    MWDT_DEBUG_STOPPED      // watchdog is stopped during debug session
};

enum MWDT_Mode
{
    MWDT_NORMAL_MODE,       // window watchdog not active
    MWDT_WINDOW_MODE        // window watchdog active
};

struct MWDT_Config
{
    enum MWDT_Mode           mode;              // normal / window watchdog mode
    uint16_t                 upperTimeLimit;    // timeout in ms, range 1-65535
    uint16_t                 lowerTimeLimit;    // window in ms, ignored in normal mode
    enum MWDT_DebugConfig    debugConfig;       // watchdog state during debug session
};

#ifdef __cplusplus
}
#endif

#endif // MWDT_TYPES_H
