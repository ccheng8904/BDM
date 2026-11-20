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
 *  \ingroup  firmware_update
 *
 *  \brief    Implementation SW and HW version handling.
 *
 */


/******************************************************************************/
/* INCLUDES                                                                   */
/******************************************************************************/

#include "bsh_stdinc.h"
#include "firmware_update/BootManager/BootManagerModule.h"
#include "firmware_update/otp_mem/fwu_otp_memory.h"


/******************************************************************************/
/* PRIVATE DEFINITIONS                                                        */
/******************************************************************************/

#if defined(DBUS2_PROD_MSG_IN_APP)
extern const struct FWU_sw_version_s SW_version;
#endif

/******************************************************************************/
/* PRIVATE TYPE DEFINITION                                                    */
/******************************************************************************/


/******************************************************************************/
/* PUBLIC FUNCTION DEFINITIONS                                                */
/******************************************************************************/

const struct FWU_hw_version_s* FWU_getHardwareVersion(void)
{
    return OTP_getHardwareVersion();
}

const struct FWU_sw_version_s* FWU_getSoftwareVersion(void)
{
  #if defined(REMOTE_FIRMWARE_UPDATE)
    const struct MAL_ModuleHeader_s *module_header;
    module_header = BMMOD_GetMemModuleHeader(MAL_PRODUCT_APP1);
    return &module_header->sw_version;
    
  #elif defined(DBUS2_PROD_MSG_IN_APP)
    return &SW_version;
    
  #else
    return NULL;
  #endif
}


