/*******************************************************************************
 *  Copyright (c) 2017 BSH Hausgeraete GmbH,
 *  Carl-Wery-Str. 34, 81739 Munich, Germany, www.bsh-group.de
 *
 *  All rights reserved. This program and the accompanying materials
 *  are protected by international copyright laws.
 *  Please contact copyright holder for licensing information.
 *
 *******************************************************************************
 *  PROJECT          Firmware Update Memory Abstraction Layer
 ******************************************************************************/


/******************************************************************************/
/* DOCUMENTATION                                                              */
/******************************************************************************/
/** \file
 *
 *  \ingroup  firmware_update
 *
 *  \brief    Implementation of unit moduleAdministration.
 *
 */


/******************************************************************************/
/* INCLUDES                                                                   */
/******************************************************************************/
#include "bsh_stdinc.h"
#include <stdint.h>
#include "firmware_update/mal/moduleAdministration.h"
#include "firmware_update/mal/fwu_opt_bytes.h"
#include "SegmentDef.h"


#if defined (BM_BACKWARD_COMPATIBILITY)
    #define DUMMY_PT_HWVERSION NULL,
#else
    #define DUMMY_PT_HWVERSION
#endif //defined (BM_BACKWARD_COMPATIBILITY)

/** \brief    data map points to shared const data
 *
 *  \details  The following data is placed in a special segment and is used
 *            to share constant data between the boot manager and other applications.
 *  \note structure will be compiled for all application using mal interfaces,
 *        but binaries output of boot manager is used only for this structure
 */
SDEF_SetSegmentConst(FWU_DATA_ANCHOR)
SYMBOL_USED const struct MAL_DataAnchor_s MAL_DataAnchor = 
{
#ifdef FWU_FLEX_PARTITION_USED
    #ifdef VARIANT_BOOTMANAGER
        MAL_partitionFix,
        MAL_partitionFlex
    #else
        NULL, ///< data located in protected boot manager area
        MAL_partitionFlex
    #endif
#else
    #ifdef VARIANT_BOOTMANAGER
        DUMMY_PT_HWVERSION
        MAL_partitionFix,
        NULL
    #else
        DUMMY_PT_HWVERSION  ///< data located in protected boot manager area
        NULL,               ///< data located in protected boot manager area
        NULL
    #endif
#endif
};
SDEF_SetSegmentConst_Default()


/** \brief    Firmware option bytes
 *
 *  \details  Default opt. bytes BootManager and Programmer
 *
 *  \note This opt. bytes are used when build_type = RELEASE
 */
#if defined(VARIANT_BOOTMANAGER)||defined(VARIANT_PROGRAMMER)
SDEF_SetSegmentConst(FWU_OPT_BYTES)
SYMBOL_USED const struct MAL_FwuOptBytes_s MAL_FwuOptBytes =
{
    .integrityConfig     = MAL_INTEGRITY_ENABLE,
    .compatibilityConfig = MAL_COMPATIBILITY_ENABLE,
    .reserved            = {MAL_RESERVED,MAL_RESERVED}

};
SDEF_SetSegmentConst_Default()
#endif
