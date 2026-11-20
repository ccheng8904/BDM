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


#ifndef BOOTMANAGERSHAREDDATA_H
#define BOOTMANAGERSHAREDDATA_H

#ifdef __cplusplus
extern "C" {
#endif


/******************************************************************************/
/* DOCUMENTATION                                                              */
/******************************************************************************/
/** \file
 *
 *  \ingroup  BootManager
 *
 *  \brief    Public definitions and declarations for Boot Manager shared data.
 *
 */


/******************************************************************************/
/* INCLUDES                                                                   */
/******************************************************************************/
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include "firmware_update/mal/module_administration_cfg.h"
#include "firmware_update/BootManager/BootManagerSharedDataUc.h"



/******************************************************************************/
/* PUBLIC TYPE DEFINITIONS                                                    */
/******************************************************************************/



/** Enumeration for error flags */
enum BMDAT_errorFlags
{
    BMDAT_ER_NO_ERROR         = 0x00U,   ///< There is no error
    BMDAT_ER_BOOT_MODULE      = 0x01U,   ///< unsupported boot module
    BMDAT_ER_COMPRESSION_TYPE = 0x02U,   ///< unsupported compression type 
    BMDAT_ER_CRC              = 0x04U,   ///< CRC check failed
    BMDAT_ER_CONSISTENCY      = 0x10U,   ///< fragmented module not consistent
    BMDAT_ER_INTEGRITY        = 0x20U,   ///< integrity check failed
    BMDAT_ER_UNSTARTABLE      = 0x40U,   ///< boot module not loadable
    BMDAT_ER_FW_ID            = 0x80U    ///< firmware id check failed
};

enum BMDAT_magicPatterns
//lint --e{575} "Warning 575: enumeration constant exceeds range for integers", no problem
{
    BMDAT_startBlAfterReset     = 0x00U,
    BMDAT_longBootTimeOut       = 0x5A5A7979U,
    BMDAT_startAppAfterReset    = 0x55AA7799U,
    BMDAT_magicPattern          = 0x5A714B78U
};

/* Enumeration for exclusion uninitialized data(SharedData + UninitData) from StartUp Ram Check
 * BMDAT_UNINIT_DATA_UNCHECKED - User can force checking uninitialized data during reset
 * BMDAT_UNINIT_DATA_CHECKED - Data already checked, data will be excluded from check
 * "RANDOM_STATE_AFTER_HW_RESET" - Data will be checked by StartUp RAM check*/
enum BMDAT_RamCheckState
//lint --e{575} "Warning 575: enumeration constant exceeds range for integers", no problem
{
    BMDAT_UNINIT_DATA_UNCHECKED = 0x2A19B85DU,
    BMDAT_UNINIT_DATA_CHECKED   = 0x4AB60E39U
};


extern struct BMDAT_sharedData_s BMDAT_sharedData;

/******************************************************************************/
/* PUBLIC FUNCTION DECLARATIONS                                               */
/******************************************************************************/


/** \brief    initializes the shared data structures.
 *
 *  \details  This function shall be called by the boot manager only.
 *            The interfaces shall be called before any other function.
 *
 */
extern void BMDAT_initSharedData(void);


/** \brief    sets an error during booting.
 *
 *  \param    error:  error type happened
 */
extern void BMDAT_setErrorFlag(enum BMDAT_errorFlags error);


/** \brief    sets the module which caused an error during booting.
 *
 *  \param    errorModule:  id of module causing an error
 */
extern void BMDAT_setErrorModule(enum MAL_ModuleEnum errorModule);


/** \brief    delivers the error flags
 *
 *  \return   all happened errors during booting
 */
extern uint32_t BMDAT_getErrorFlags(void);


/** \brief    increments the number of start attempts.
 */
extern void BMDAT_incrementStartAttempts(void);


/** \brief    returns the number of start attempts.
 *
 *  \return   number of start attempts
 */
extern uint16_t BMDAT_getStartAttempts(void);


/** \brief    increments the number of fatal errors.
 */
extern void BMDAT_incrementFatalErrorCounter(void);


/** \brief    returns the number of fatal errors.
 *
 *  \return   number of start attempts
 */
extern uint16_t BMDAT_getFatalErrorCounter(void);



/** \brief    sets the start address of the interrupt vector table.
 *
 *  \param    tableOffset:  physical address of vector table
 *  \note     This interface is needed when the boot manager provides an isr dispatcher (e.g. for cortex M0)
 */
extern void BMDAT_setIsrTableAddr(size_t tableAddr);


/** \brief    returns the address of the interrupt vector table.
 *
 *  \return   physical address of vector table
 */
extern size_t BMDAT_getIsrTableAddr(void);


/** \brief    sets the enumerator of application to be started.
 *
 *  \param    bootModule:  enumerator of app to be booted next after reset
 */
extern void BMDAT_setBootModule(enum MAL_ModuleEnum bootModule);


/** \brief    returns the enumerator of application to be started.
 *
 *  \return   enumerator of application to be booted next after reset
 */
extern enum MAL_ModuleEnum BMDAT_getBootModule(void);



/** \brief    sets the baud rate for programmer.
 *
 *  \param    baudRate:  baud rate divided by 100
 */
extern void BMDAT_setBaudRate(uint16_t baudRate);


/** \brief    returns the start baud rate for programmer
 *
 *  \return   baud rate divided by 100
 */
extern uint16_t BMDAT_getBaudRate(void);

/** \brief    sets the magic pattern
 *
 *  After all data are initialized, this interface may be used
 *  to indicate initialization has been finished
 *
 *  \param magicPattern one of several magic patterns
 */
extern void BMDAT_setMagicPattern(enum BMDAT_magicPatterns magicPattern);


/** \brief  returns the magic pattern
 *
 *  the value has been set by \sa BMDAT_setMagicPattern
 *  before reset.
 *
 *  \return   magic pattern
 */
extern enum BMDAT_magicPatterns BMDAT_getMagicPattern(void);


/** \brief  sets the ram check state
 *
 *  Ram check state is set to BMDAT_UNINIT_DATA_CHECKED
 */
extern void BMDAT_setUninitDataChecked(void);


/** \brief  returns true if uninitialized data (Shared Data + Unit Data)
 *          was already checked by FS one-time Ram check
 *
 *  \return true  - uninitialized data is already checked
 *          false - uninitialized data is not checked
 */
extern bool BMDAT_isUninitDataChecked(void);

#ifdef __cplusplus
}
#endif

#endif // BOOTMANAGERSHAREDDATA_H
