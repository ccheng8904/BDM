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


#ifndef GECKO_BL_API_H
#define GECKO_BL_API_H

#ifdef __cplusplus
extern "C" {
#endif


/******************************************************************************/
/* DOCUMENTATION                                                              */
/******************************************************************************/
/** \file
 *
 *  \ingroup  gecko_bootloader
 *
 *  \brief    Public definitions BSH application interface to gecko boot loader
 *
 *  \details  Declaration to unify the access to the SiLabs gecko boot loader.
 *
 */



/******************************************************************************/
/* INCLUDES                                                                   */
/******************************************************************************/
#include <stdint.h>
#include <limits.h>
#include <stdbool.h>
#include "bsh_stdinc.h"
#include "std_lib/std_version.h"

/******************************************************************************/

/* PUBLIC DEFINITIONS                                                         */
/******************************************************************************/
/** identifier for invalid certificate */
#define GBTL_INVALID_CERTIFICATE (uint32_t)(-1)

/******************************************************************************/
/* PUBLIC FUNCTION DECLARATIONS                                               */
/******************************************************************************/

/** \brief selects the gecko ROM-Loader to boot after reset. */
extern void GBTL_bootGromLoaderAfterReset(void);


/** \brief selects the gecko boot loader to boot after reset and install image */
extern void GBTL_installAfterReset(void);


/** \brief reboots and installs a new image */
extern void GBTL_rebootAndInstall(void);


/** \brief reads the public key form OTP memory of secure element (SE)
 *
 *  \param key: pointer to buffer to hold the 64 bytes of public key
 *  \return true if key read out successfully, otherwise false (usually when key not set)
 */
extern bool GBTL_getPublicSignKey(uint8_t *key);


/** \brief reads the public boot loader key
 *
 *  \param key: pointer to buffer to hold the 64 bytes of public key
 *  \return true if key read out successfully, otherwise false (usually when certificate not set)
 */
extern bool GBTL_getPublicBootKey(uint8_t *key);


/** \brief reads the public application key
 *
 *  \param key: pointer to buffer to hold the 64 bytes of public key
 *  \return true if key read out successfully, otherwise false (usually when certificate not set)
 */
extern bool GBTL_getPublicAppKey(uint8_t *key);


/** \brief Delivers the version of bootloader certificate
 * 
 *  \return if successful the version will be returned otherwise GBTL_INVALID_CERTIFICATE
 */
extern uint32_t GBTL_getBootCertificateVersion(void);


/** \brief Delivers the version of application certificate
 * 
 *  \return if successful the version will be returned otherwise GBTL_INVALID_CERTIFICATE
 */

extern uint32_t GBTL_getAppCertificateVersion(void);


/** \brief Delivers a pointer to the version string of gecko bootloader
 * 
 *  \return pointer to the version sting of gecko bootloader
 */
extern const struct STDV_version * GBTL_getBootloaderVersion(void);


/** \brief Delivers the total remaining number of application upgrades
 * 
 *  \return remaining number of updates
 */
extern uint32_t GBTL_getRemainingUpdates(void);


/** \brief reads the version of Secure Element firmware
 *
 *  \param version[out]: pointer to the version
 *  \return true if version read out successfully, otherwise false
 */
extern bool GBTL_getSE_FirmwareVersion(uint32_t *version);


#ifdef __cplusplus
}
#endif

#endif // GECKO_BL_API_H
