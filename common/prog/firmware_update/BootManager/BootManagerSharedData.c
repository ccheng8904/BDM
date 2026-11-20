/*******************************************************************************
 *  Copyright (c) 2017 BSH Hausgeraete GmbH,
 *  Carl-Wery-Str. 34, 81739 Munich, Germany, www.bsh-group.de
 *
 *  All rights reserved. This program and the accompanying materials
 *  are protected by international copyright laws.
 *  Please contact copyright holder for licensing information.
 *
 *******************************************************************************
 *  PROJECT          Firmware Update Boot-Manager
 ******************************************************************************/


/******************************************************************************/
/* DOCUMENTATION                                                              */
/******************************************************************************/
/** \file
 *
 *  \ingroup  BootManager
 *
 *  \brief    Implementation of for Boot Manager shared data.
 *
 */


/******************************************************************************/
/* INCLUDES                                                                   */
/******************************************************************************/
#include "bsh_stdinc.h"
#include <stddef.h>
#include <stdbool.h>
#include <stdint.h>
#include "std_lib/std_symbol.h"
#include "BootManagerSharedData.h"
#include "BootManagerSharedDataUc.h"
#include "firmware_update/mal/module_administration_cfg.h"
#include "firmware_update/fwu_config.h"
#include "SegmentDef.h"
#if defined(VARIANT_BOOTMANAGER) && defined(FEATURE_FUNCTIONAL_SAFETY_ENABLED)
#include "FuncSafe/fSafeXS.h"
#include "FuncSafe/fs_psm.h"
#endif


/******************************************************************************/
/* PRIVATE DEFINITIONS                                                        */
/******************************************************************************/
#ifdef __IAR_SYSTEMS_ICC__
    #define NO_INIT __no_init
#else
    #define NO_INIT   ///< data not initialized in startup
#endif


#ifndef FWU_DEFAULT_START_MODULE
#define FWU_DEFAULT_START_MODULE    MAL_PRODUCT_APP1
#endif 

#ifndef FWU_DEFAULT_BAUDRATE
#define FWU_DEFAULT_BAUDRATE        DBUS_DEFAULT_BAUDRATE
#endif

/******************************************************************************/
/* PRIVATE TYPE DEFINITIONS                                                   */
/******************************************************************************/

/** \brief struct for volatile data memory map
 *
 *  \note  The data hold in this structure shall not be initialized during start-up
 */
struct BMDAT_sharedData_s
{
    size_t   IsrTableAddress;  ///< physical address of interrupt vector table
    uint32_t errorFlags;       ///< error flags
    enum MAL_ModuleEnum errorModule; ///< module-ID caused an error during booting
    enum MAL_ModuleEnum bootModule;  ///< identifier of module to boot
    uint16_t baudRate;         ///< start baud rate for boot loader
    uint16_t startAttempts;    ///< number of start attempts for boot manager
    uint16_t fatalErrorCounter;///< fatal error counter (used in case of functional safety error)
    enum BMDAT_magicPatterns magicPattern;     ///< holds a magic pattern to recognize uninitialized data
    enum BMDAT_RamCheckState ramCheckState;          ///< flag for SW/HW reset
};


/******************************************************************************/
/* PRIVATE DATA DEFINITIONS                                                   */
/******************************************************************************/

/** \brief    data which is shared between different applications
 *
 *  \details  The following data is placed in a special segment and is used
 *            to share data between the boot manager and other applications.
 */
SDEF_SetSegmentRW(FWU_SHARED_DATA) // put it into segment FWU_SHARED_DATA
SYMBOL_USED NO_INIT struct BMDAT_sharedData_s BMDAT_sharedData;
SDEF_SetSegmentRW_Default()



/******************************************************************************/
/* PUBLIC FUNCTION DEFINITIONS                                                */
/******************************************************************************/

void BMDAT_initSharedData(void)
{
    if (BMDAT_isPowerOnReset() || (BMDAT_isDataInitialized() == false))
    {
        BMDAT_sharedData = (struct BMDAT_sharedData_s)
        {
            .IsrTableAddress   = 0U,
            .errorFlags        = 0U,
            .errorModule       = MAL_NO_MODULE,
            .bootModule        = FWU_DEFAULT_START_MODULE,
            .baudRate          = FWU_DEFAULT_BAUDRATE,
            .startAttempts     = 0U,
            .fatalErrorCounter = 0U,
            .magicPattern      = BMDAT_magicPattern,
            .ramCheckState     = BMDAT_UNINIT_DATA_CHECKED
        };
        BMDAT_setDataInitializedFlag();
    }
    if (BMDAT_sharedData.bootModule != FWU_DEFAULT_START_MODULE)
    {
        // load default values after next reset
        BMDAT_resetDataInitializedFlag();
    }
#if defined(VARIANT_BOOTMANAGER) && defined(FEATURE_FUNCTIONAL_SAFETY_ENABLED)
    FSF_DoProgramCounterMonitoring(FSF_BM_PLATFORM_INITIALIZED, FSF_BM_SHARED_DATA_INITIALIZED);
#endif
}

void BMDAT_setErrorFlag(enum BMDAT_errorFlags error)
{
    BMDAT_sharedData.errorFlags |= (uint32_t)error;
}


uint32_t BMDAT_getErrorFlags(void)
{
    return BMDAT_sharedData.errorFlags;
}


void BMDAT_setErrorModule(enum MAL_ModuleEnum errorModule)
{
    BMDAT_sharedData.errorModule = errorModule;
}


void BMDAT_incrementStartAttempts(void)
{
    BMDAT_sharedData.startAttempts++; // may overflow, feature just for debugging
}


uint16_t BMDAT_getStartAttempts(void)
{
    return BMDAT_sharedData.startAttempts;
}


void BMDAT_incrementFatalErrorCounter(void)
{
    BMDAT_sharedData.fatalErrorCounter++; // may overflow, feature just for debugging
}


uint16_t BMDAT_getFatalErrorCounter(void)
{
    return BMDAT_sharedData.fatalErrorCounter;
}


void BMDAT_setIsrTableAddr(size_t tableAddr)
{
    BMDAT_sharedData.IsrTableAddress = tableAddr;
}


size_t BMDAT_getIsrTableAddr(void)
{
    return BMDAT_sharedData.IsrTableAddress;
}


void BMDAT_setBootModule(enum MAL_ModuleEnum bootModule)
{
    BMDAT_sharedData.bootModule = bootModule;
    BMDAT_setDataInitializedFlag(); 
}


enum MAL_ModuleEnum BMDAT_getBootModule(void)
{
    return BMDAT_sharedData.bootModule;
}


void BMDAT_setBaudRate(uint16_t baudRate)
{
    BMDAT_sharedData.baudRate = baudRate;
}


uint16_t BMDAT_getBaudRate(void)
{
    return BMDAT_sharedData.baudRate;
}

void BMDAT_setMagicPattern(enum BMDAT_magicPatterns magicPattern)
{
    BMDAT_sharedData.magicPattern = magicPattern;
}


enum BMDAT_magicPatterns BMDAT_getMagicPattern(void)
{
    return BMDAT_sharedData.magicPattern;
}


void BMDAT_setUninitDataChecked(void)
{
    BMDAT_sharedData.ramCheckState = BMDAT_UNINIT_DATA_CHECKED;
}

bool BMDAT_isUninitDataChecked(void)
{
    return ((BMDAT_sharedData.ramCheckState == BMDAT_UNINIT_DATA_CHECKED) ? true : false);
}
