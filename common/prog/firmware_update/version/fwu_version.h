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


#ifndef FIRMWAREUPDATEVERSION_H
#define FIRMWAREUPDATEVERSION_H

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
 *  \brief    Public struct definitions for SW and HW version.
 *
 */



/******************************************************************************/
/* INCLUDES                                                                   */
/******************************************************************************/
#include <stdint.h>
#include "std_lib/std_version.h"
#include "hsup.h"
#include "LibDefinesUc.h"


/******************************************************************************/
/* PUBLIC TYPE DEFINITIONS                                                    */
/******************************************************************************/

/** \brief    structure for SW-Version
 *
 *  \details  The SW-Version is located inside SW-module header
 */
struct FWU_sw_version_s
{
    struct STDV_version version;    ///< standard BSH versioning structure
};


/** \brief    structure for HW-Version
 *
 *  \details  The HW-Version shall be located in a protected area in memory.
 *            It cannot be changed due to remote update
 */
PACKED struct POSTPACKED FWU_hw_version_s
{
    /** denotes the CRC check over unique HW ID data structure
     *  \note: the CRC is saved in big endian format!!
     */
    uint32_t CRC;                  
    struct STDV_version version;    ///< standard BSH versioning structure
};


/******************************************************************************/
/* PUBLIC FUNCTION DECLARATIONS                                               */
/******************************************************************************/

const struct FWU_hw_version_s* FWU_getHardwareVersion(void);

const struct FWU_sw_version_s* FWU_getSoftwareVersion(void);


#ifdef __cplusplus
}
#endif

#endif
