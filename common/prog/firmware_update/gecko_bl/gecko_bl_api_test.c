/*******************************************************************************
 *  Copyright (c) 2017 BSH Hausgeraete GmbH,
 *  Carl-Wery-Str. 34, 81739 Munich, Germany, www.bsh-group.de
 *
 *  All rights reserved. This program and the accompanying materials
 *  are protected by international copyright laws.
 *  Please contact copyright holder for licensing information.
 *
 *******************************************************************************
 *  PROJECT          Generic SW
 ******************************************************************************/


/******************************************************************************/
/* DOCUMENTATION                                                              */
/******************************************************************************/
/** \file
 *
 *  \ingroup  gecko_bootloader
 *
 *  \brief    Implementation BSH application interface to gecko boot loader.
 *
 *  \details  This Interface is implemented to unify the access to the SiLabs
 *            gecko boot loader.
 */


/******************************************************************************/
/* INCLUDES                                                                   */
/******************************************************************************/
#include "bsh_stdinc.h"
#include <stdint.h>
#include "gecko_bl_api.h"
#include "gecko_bl_api_test.h"

/******************************************************************************/
/* PRIVATE DATA DEFINITIONS                                                   */
/******************************************************************************/
volatile uint8_t GBTL_signKey[64];
volatile uint8_t GBTL_bootKey[64];
volatile uint8_t GBTL_appKey[64];
volatile uint32_t GBTL_bootCertVersion;
volatile uint32_t GBTL_appCertVersion;
volatile const struct STDV_version * GBTL_blVersion;
volatile uint32_t GBTL_remainingUpdates;
volatile bool GBTL_keyResult = false;
volatile uint32_t GBTL_SE_FirmwareVersion;

/******************************************************************************/
/* PUBLIC FUNCTION DEFINITIONS                                                */
/******************************************************************************/


bool GBTL_doIntegrationTest(void)
{
    if (GBTL_getPublicSignKey((uint8_t*)GBTL_signKey) &&
        GBTL_getPublicBootKey((uint8_t*)GBTL_bootKey) &&
        GBTL_getPublicAppKey((uint8_t*)GBTL_appKey) &&
        GBTL_getSE_FirmwareVersion((uint32_t*)(&GBTL_SE_FirmwareVersion)))
    {
        GBTL_keyResult = true; // all readout passed
    }
    GBTL_bootCertVersion = GBTL_getBootCertificateVersion();
    GBTL_appCertVersion = GBTL_getAppCertificateVersion();
    GBTL_blVersion = GBTL_getBootloaderVersion();
    GBTL_remainingUpdates = GBTL_getRemainingUpdates();
    return GBTL_keyResult;
}

