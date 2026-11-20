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


#ifndef FIRMWAREUPDATE_H
#define FIRMWAREUPDATE_H

#ifdef __cplusplus
extern "C" {
#endif


/******************************************************************************/
/* DOCUMENTATION                                                              */
/******************************************************************************/
/** \file
 *
 *  \brief    Public definitions for FWU option bytes
 *
 *  \details  A detailed file description that may span several lines
 *            and that has to be terminated by an empty line.
 */



/******************************************************************************/
/* INCLUDES                                                                   */
/******************************************************************************/
#include "firmware_update/BootManager/BootManagerSharedData.h"
#ifdef REMOTE_FIRMWARE_UPDATE
    #include "firmware_update/mal/moduleAdministration.h"
#endif


/** \brief struct for integrity check configuration */
enum MAL_integrityConfig
{
    MAL_INTEGRITY_ENABLE       = MAL_ENABLE_DEF,
    MAL_INTEGRITY_IGNORE_ERROR = MAL_IGNORE_ERROR_DEF,
    MAL_INTEGRITY_DISABLE_CALC = MAL_DISABLE_CALC_DEF
};

/** \brief struct for compatibility check configuration */
enum MAL_compatibilityConfig
{
    MAL_COMPATIBILITY_ENABLE       = MAL_ENABLE_DEF,
    MAL_COMPATIBILITY_IGNORE_ERROR = MAL_IGNORE_ERROR_DEF
};

/** \brief struct for reserved opt. byte */
enum MAL_ReservedConfig
{
    MAL_RESERVED       = MAL_RESERVED_DEF
};

/** \brief    struct for FWU option bytes */
struct MAL_FwuOptBytes_s
{
    enum MAL_integrityConfig      integrityConfig;     ///< Opt. byte for integrity
    enum MAL_compatibilityConfig  compatibilityConfig; ///< Opt. byte for compatibility
    enum MAL_ReservedConfig       reserved[2];
};


extern const struct MAL_FwuOptBytes_s MAL_FwuOptBytes;


/** \brief    returns the Integrity Configuration variable
 *
 *  \details  Purpose of integrityConfig variable is to enable/disable/suppress integrity checks
 *            CRC calculation (FS ROM check)
 *
 *  \return   MAL_INTEGRITY_ENABLE: integrity check is enabled
 *            MAL_INTEGRITY_IGNORE_ERROR: integrity check errors is ignored but calculation is executed
 *            MAL_INTEGRITY_DISABLE_CALC: integrity check is disabled (errors + calculation)
 */
static inline enum MAL_integrityConfig MAL_getIntegrityOpt(void)
{
    return MAL_FwuOptBytes.integrityConfig;
}

/** \brief    returns the Compatibility Configuration variable
 *
 *  \details  Purpose of compatibilityConfig variable is to enable/disable/suppress compatibility checks
 *            firmware Id validation
 *
 *  \return   MAL_COMPATIBILITY_ENABLE: compatibility check is enabled
 *            MAL_COMPATIBILITY_IGNORE_ERROR: compatibility check errors is ignored
 */
static inline enum MAL_compatibilityConfig MAL_getCompatibilityOpt(void)
{
    return MAL_FwuOptBytes.compatibilityConfig;
}


/** \brief    returns true/false if Integrity check calculation is disabled or not
 *
 *  \details  For FWU1 (WIRED_FIRMWARE_UPDATE) there is available only integrity check (CRC)
 *            There is no BootManager binary where are stored default FWU opt bytes
 *            which are used in RELEASE case.
 *            From this reason we must handle it via NDEBUG(RELEASE) switch.
 *
 *  \return   true:  calculation is disabled
 *            false: calculation is enabled
 */
static inline bool FWU_isIntegCheckDisabledCalc(void)
{
#ifdef REMOTE_FIRMWARE_UPDATE
    return ((MAL_getIntegrityOpt() == MAL_INTEGRITY_DISABLE_CALC) ? true : false);
#else // WIRED_FIRMWARE_UPDATE
    #ifdef NDEBUG // RELEASE
        return false;
    #else
        return ((MAL_getIntegrityOpt() == MAL_INTEGRITY_DISABLE_CALC) ? true : false);
    #endif
#endif
}


/** \brief    returns true/false if Integrity checks errors are ignored or not
 *
 *  \details  For FWU1 (WIRED_FIRMWARE_UPDATE) there is available only integrity check (CRC)
 *            There is no BootManager binary where are stored default FWU opt bytes
 *            which are used in RELEASE case.
 *            From this reason we must handle it via NDEBUG(RELEASE) switch.
 *
 *  \return   true: errors are ignored
 *            false: errors are not ignored
 */
static inline bool FWU_isIntegCheckErrorIgnored(void)
{
#ifdef REMOTE_FIRMWARE_UPDATE
    return ((MAL_getIntegrityOpt() == MAL_INTEGRITY_IGNORE_ERROR) ? true : false);
#else // WIRED_FIRMWARE_UPDATE
    #ifdef NDEBUG // RELEASE
        return false;
    #else
        return ((MAL_getIntegrityOpt() == MAL_INTEGRITY_IGNORE_ERROR) ? true : false);
    #endif
#endif
}

/** \brief    returns true/false if Compatibility checks errors are ignored or not
 *
 *  \return   true: errors are ignored
 *            false: errors are not ignored
 */
static inline bool FWU_isCompatibilityCheckErrorIgnored(void)
{
    return ((MAL_getCompatibilityOpt() == MAL_COMPATIBILITY_IGNORE_ERROR) ? true : false);
}


#ifdef __cplusplus
}
#endif

#endif
