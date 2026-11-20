/*******************************************************************************
 *  Copyright (c) 2017 BSH Hausgeraete GmbH,
 *  Carl-Wery-Str. 34, 81739 Munich, Germany, www.bsh-group.de
 *
 *  All rights reserved. This program and the accompanying materials
 *  are protected by international copyright laws.
 *  Please contact copyright holder for licensing information.
 *
 *******************************************************************************
 *  PROJECT          Firmware Update
 ******************************************************************************/


/******************************************************************************/
/* DOCUMENTATION                                                              */
/******************************************************************************/
/** \file
 *
 *  \brief    Implementation of Module Header for programmer
 *
 */


/******************************************************************************/
/* INCLUDES                                                                   */
/******************************************************************************/
#include "bsh_stdinc.h"
#include "firmware_update/BootManager/BootManagerModule.h"
#include "SegmentDef.h"
#include "IntTbl.h"
#include "std_symbol.h"

#ifndef MCU_FW_GIT_REV_HASH
    #define MCU_FW_GIT_REV_HASH  0u
#endif
/******************************************************************************/
/* PRIVATE DATA DEFINITIONS                                                   */
/******************************************************************************/
//lint -esym(528,MDHD_ModuleHeader*) "Symbol 'MDHD_ModuleHeader' not referenced" referenced by boot manager
SDEF_SetSegmentConst(FWU_SW_MODULE_HEADER)
static SYMBOL_USED const struct MAL_ModuleHeader_s MDHD_ModuleHeader =
{
    .crc_expected = 0u, // will be replaced by patchbin!!
  #if !defined(__ICCRx__)
    .ISRtbl_ptr   = ITBL_coreVectors,
  #else
    .ISRtbl_ptr   = (const MAL_isrPtr_t *)(sizeof(struct MAL_ModuleHeader_s)+ROM1_START), //lint !e923 cast tested to work correctly on Rx
  #endif
    .size         = (uint32_t)((FWU_PROGRAMMER_END_ADDRESS-FWU_PROGRAMMER_START_ADDRESS)+1u),
  #ifdef PROGRAMMER_ROM
    .executionAddress = PROGRAMMER_ROM, // maybe inside RAM
  #else
    .executionAddress = ROM1_START, // maybe inside RAM
  #endif
    .sw_version =
    {
     .version =
        {
            .ID = {0},
            .major = {0},
            .minor = {0},
            .revision = {0},
            .build = STDV_ParseVersionBuild(MCU_FW_GIT_REV_HASH)
        }
    },
    .reserved = 0u,
    .hw_id_array = NULL
};
SDEF_SetSegmentConst_Default()
