### The example of the file ModuleHeaderProcessData.c

```c
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
 *  \brief    Implementation of Process Data SW Module Header 
 *
 */


/******************************************************************************/
/* INCLUDES                                                                   */
/******************************************************************************/
#include "bsh_stdinc.h"
#include "std_lib/std_symbol.h"
#include "firmware_update/mal/moduleAdministration.h"
#include "SegmentDef.h"

/******************************************************************************/
/* PRIVATE DATA DEFINITIONS                                                   */
/******************************************************************************/
/*lint -esym(528,MDHD_*) "Symbol '...' not referenced"
 *     referenced by boot manager!
 */

SDEF_SetSegmentConst(FWU_PROCESS_DATA_SW_MODULE_HEADER)
static SYMBOL_USED const struct MAL_ModuleHeader_s MDHD_ModuleHeaderProcessData =
{
    .crc_expected = 0u,   // will be replaced by patchbin
    .ISRtbl_ptr   = NULL,   // unused
    .size         = FWU_PROCESS_DATA_SIZE,
    .executionAddress = 0u, // unused
    .sw_version =
    {
     .version =
        { /** \todo please specify id-string for your process data module */
            .ID       = {0,1,2,3,4,5,6,7},
            .major    = {0x12, 0x34},
            .minor    = {0x98, 0x76},
            .revision = {0xfe, 0xdc},
            .build    = {0xab, 0xcd, 0xef, 0x01}
        }
    },
    .reserved     = 0u,
    .hw_id_array  = NULL    // unused
};
SDEF_SetSegmentConst_Default()
```
[Back to page of firmware_update help](../README.md)