/*******************************************************************************
 *  Copyright (c) 2017 BSH Hausgeraete GmbH,
 *  Carl-Wery-Str. 34, 81739 Munich, Germany, www.bsh-group.de
 *
 *  All rights reserved. This program and the accompanying materials
 *  are protected by international copyright laws.
 *  Please contact copyright holder for licensing information.
 *
 *******************************************************************************
 *  PROJECT          FLASH_LIB
 ******************************************************************************/

#ifndef FLASHLIB_H
#define FLASHLIB_H

#ifdef __cplusplus
extern "C" {
#endif

/******************************************************************************/
/* DOCUMENTATION                                                              */
/******************************************************************************/
#if defined(RX100)
/* RX100 */
/**
 *\if Flash
 * \file      flash.h
 *
 *  \brief    Public declarations and definitions of Flash Routines library
 *
 *\endif
 */
/**
 *\if Flash
 * \mainpage  Common used Functions and Definitions
 *\else
 * \file      flash.h
 *
 * \brief     Public declarations and definitions of Flash Routines library
 *
 *\endif
 *
 *\section introduction 1. Introduction
 *The <b>RX100 flash library</b> provides user application with routine to do flash operation like write or erase.
 *
 *\section Interface 2. Interface of Flash Library
 *The main interface comprises following functions:
 *
 *   - \ref FLASH_vSelectMemory()
 *   - \ref FLASH_bEnableAccess()
 *   - \ref FLASH_bDisableAccess()
 *   - \ref FLASH_bIsAccessEnabled()
 *   - \ref FLASH_ucGetCellSize()
 *   - \ref FLASH_bWrite()
 *   - \ref FLASH_bWriteByte()
 *   - \ref FLASH_bRead()
 *   - \ref FLASH_bErase()
 *   - \ref FLASH_tGetFunctionStatus()
 *   - \ref FLASH_tGetRX100FunctionStatus
 *   - \ref FLASH_bReadBlankStatus()
 *   - \ref FLASH_bReadLockStatus()
 *
 *To reduce ROM memory consumption in case of work with data flash part only
 *next functions can be used:
 *
 *   - \ref FLASH_bEnableAccessDataFlash()
 *   - \ref FLASH_bDisableAccessDataFlash()
 *   - \ref FLASH_bWriteDataFlash()
 *   - \ref FLASH_bReadDataFlash()
 *   - \ref FLASH_bEraseDataFlash()
 *   - \ref FLASH_bReadBlankStatusDataFlash()
 *
 *In order to give application code the possibility to control the flash operations, following
 *library interfaces are defined in the user configuration file FlashMcXS.c :
 *
 *   - \ref _FLASH_vResetWatchdogTimer() : Watchdog timer reset
 *   - \ref _FLASH_vOperationStart() : Called immediately before a flash operation starts
 *   - \ref _FLASH_vOperationFinished() : Called immediately after a flash operation was finished
 *
 *\remarks The user should adapt the implementations behind theses interfaces if not suitable to his needs.
 *
 *\section Usage 3. Usage of Flash Library
 *
 *For an effective use of the flash library, following must be done in the order:
 *
 *   -# Memory type must be selected at first, either \ref FLASH_CODE or \ref FLASH_DATA.
 *   -# Before accessing flash for "erase" or "write", the access must also be enabled using \ref FLASH_bEnableAccess().
 *   -# Once the steps above are completed, \ref FLASH_bWrite() or \ref FLASH_bWriteByte() can be used for writing,\n \ref FLASH_bRead() for reading and \ref FLASH_bErase() for erasing.\n <b>Caution!</b> The function FLASH_bWriteByte() is not usable for code memory, because it must be written allways n*4 bytes.
 *   -# When flash operation is finished, access must be disabled using \ref FLASH_bDisableAccess().
 *
 *\remarks
 *   - Flash Programming can be done sequentially according to CPU architecture (bytewise for data flash part and n*4 for code flash part).
 *   - Flash Erasing always concerns a whole flash page.
 *   - It is advisable to check return values of each function before calling the next function.
 *   - To set the flash page pointer to a valid address according to page alignment, \ref FLASH_ucGetCellSize()
 *     retrieves the correct page alignment (e.g. bytewise, wordwise etc.). This is important for overlying
 *     application handling with flash pages in a loop.
 *
 *\section Examples 4. Examples of Flash Library routines usage
 *
 *\subsection operation 4.1. Lock or unlock flash operation
 *
 *To lock flash routines and therefore preventing from random flash access (e.g: through \b EMC):
 *
 *\code
 *FLASH_bDisableAccess();
 *\endcode
 *
 *\remarks Flash operations are per default locked at system initialization.
 *
 *
 *To unlock flash routine in order to enable flash operations:
 *
 *\code
 *FLASH_bEnableAccess();
 *\endcode
 *
 *
 *\subsection write 4.2. Program Flash
 *
 *Following macro implementation can be used to program flash :
 *
 *\code
 *#define MAIN_vProgramDataFlash(puiAddr,pucBuf,uiLen) \
 *do {\
 *      FLASH_vSelectMemory(FLASH_DATA);
 *      FLASH_bEnableAccess();\
 *      FLASH_bWrite (((tAddress *)(puiAddr)),(tWriteData *)(pucBuf),(uiLen));\
 *      FLASH_bDisableAccess();\
 *   } while (0)
 *\endcode
 *
 *\subsection erase 4.3. Erase Flash
 *
 *Following macro implementation can be used to erase flash :
 *
 *\code
 *#define MAIN_vEraseCodeFlash(puiAddr) \
 *do {\
 *      FLASH_vSelectMemory(FLASH_CODE);
 *      FLASH_bEnableAccess();\
 *      FLASH_bErase((tAddress *)(puiAddr));\
 *      FLASH_bDisableAccess();\
 *   } while (0)
 *\endcode
 *
 *
 */

#else
/* Other */
/**
 *\if Flash
 *\file       flash.h
 *  \brief    Public declarations and definitions of Flash Routines library
\endif
*/
/**
 *\if Flash
 * \mainpage  Common used Functions and Definitions
 *\else
 * \file      flash.h
 *
 * \brief     Public declarations and definitions of Flash Routines library
 *\endif
 *
 *\section introduction 1. Introduction
 *The flash library provides user application with routine to do flash operation like write or erase.
 *
 *\section Interface 2. Interface of Flash Library
 *The main interface comprises following functions:
 *
 *   - \ref FLASH_bEnableAccess()
 *   - \ref FLASH_bDisableAccess()
 *   - \ref FLASH_bIsAccessEnabled()
 *   - \ref FLASH_ucGetCellSize()
 *   - \ref FLASH_bWrite()
 *   - \ref FLASH_bWriteByte()
 *   - \ref FLASH_bRead()
 *   - \ref FLASH_bErase()
 *   - \ref FLASH_tGetFunctionStatus()
 *
 *In order to give application code the possibility to control the flash operations, following
 *library interfaces are defined in the user configuration file FlashMcXS.c :
 *
 *   -      _FLASH_vResetWatchdogTimer() : Watchdog timer reset
 *   - \ref _FLASH_vOperationStart() : Called immediately before a flash operation starts
 *   - \ref _FLASH_vOperationFinished() : Called immediately after a flash operation was finished
 *   -      _FLASH_bSetupCPU() : Previous mandatory configuration before flash access (only used by R8C version)
 *   -      _FLASH_bIsInterruptible() : Actuall mode the lib is working in (only used by R8C version)
 *
 *\remarks The user should adapt the implementations behind theses interfaces if not suitable to his needs.
 *
 *\section Usage 3. Usage of Flash Library
 *
 *For an effective use of the flash library, following must be done in the order:
 *
 *   -# Before accessing flash for "erase" or "write", the access must also be enabled using \ref FLASH_bEnableAccess().
 *   -# Once the steps above are completed, \ref FLASH_bWrite() or \ref FLASH_bWriteByte() can be used for writing,\n \ref FLASH_bRead() for reading and \ref FLASH_bErase() for erasing.
 *   -# When flash operation is finished, access must be disabled using \ref FLASH_bDisableAccess().
 *
 *\remarks
 *   - Flash Programming can be done sequentially according to CPU architecture (for example bytewise).
 *   - Flash Erasing always concerns a whole flash page.
 *   - It is advisable to check return values of each function before calling the next function.
 *   - To set the flash page pointer to a valid address according to page alignment, \ref FLASH_ucGetCellSize()
 *     retrieves the correct page alignment (e.g. bytewise, wordwise etc.). This is important for overlying
 *     application handling with flash pages in a loop.  
 *
 *\section Examples 4. Examples of Flash Library routines usage
 *
 *\subsection operation 4.1. Lock or unlock flash operation
 *
 *To lock flash routines and therefore preventing from random flass access (e.g: through \b EMC):
 *
 *\code
 *FLASH_bDisableAccess();
 *\endcode
 *
 *\remarks Flash operations are per default locked at system initialization.
 *
 *
 *To unlock flash routine in order to enable flash operations:
 *
 *\code
 *FLASH_bEnableAccess();
 *\endcode
 *
 *
 *\subsection write 4.2. Program Flash
 *
 *Following macro implementation can be used to program flash :
 *
 *\code
 *#define MAIN_vProgramFlash(puiAddr,pucBuf,uiLen) \
 *do {\
 *      FLASH_bEnableAccess();\
 *      FLASH_bWrite (((tAddress *)(puiAddr)),(tWriteData *)(pucBuf),(uiLen));\
 *      FLASH_bDisableAccess();\
 *   } while (0)
 *\endcode
 *
 *\subsection erase 4.3. Erase Flash
 *
 *Following macro implementation can be used to erase flash :
 *
 *\code
 *#define MAIN_vEraseFlash(puiAddr) \
 *do {\
 *      FLASH_bEnableAccess();\
 *      FLASH_bErase((tAddress *)(puiAddr));\
 *      FLASH_bDisableAccess();\
 *   } while (0)
 *\endcode
 *
 *
 */
#endif //defined(RX100)

/******************************************************************************/
/* INCLUDES                                                                   */
/******************************************************************************/
#include "LibTypes.h"
#include "FlashMcXs.h"


/******************************************************************************/
/* PUBLIC DEFINITIONS                                                         */
/******************************************************************************/


/******************************************************************************/
/* PUBLIC TYPE DEFINITIONS                                                    */
/******************************************************************************/
/** \brief    possible cases returned by flash functions
 */
typedef enum
{
   NO_ERRORS                        =  0 , // All platforms
   USER_ACCESS_DISABLED             =  1 , // All platforms
   SETUP_CPU_PROBLEM                =  2 , // R8C
   MEMORY_PROCESS_TIMEOUT           =  3 , // R8C,M16C,RX6,MKE,STM32
   COMMAND_SEQUENCE_ERROR           =  4 , // R8C,M16C,Hcs08,RA4
   PROGRAM_HW_ERROR                 =  5 , // R8C,M16C,RX6
   ERASE_HW_ERROR                   =  6 , // R8C,M16C,NEC78k0r.RX6,RX100,MKE,RA4
   MEMORY_WRITE_TIMEOUT             =  7 , // STM8
   MEMORY_UNLOCK_TIMEOUT            =  8 , // STM8
   ADDRESS_OUT_OF_RANGE             =  9 , // All platforms
   FLMD0_PIN_AT_LOW_LEVEL           = 10 , // NEC78k0r
   PARAMETER_ERROR                  = 11 , // NEC78k0r,RX6,RX100, MKE, RA4
   PROTECTION_ERROR                 = 12 , // NEC78k0r,STM32,RX100,MKE,RA4
   WRITE_ERROR                      = 13 , // NEC78k0r,STM32,RX100,RA4
   IVERIFY_ERROR                    = 14 , // NEC78k0r
   BLANKCHECK_ERROR                 = 15 , // NEC78k0r,RX100,RX200
   DATA_LENGTH_OVERFLOW             = 16 , // R8C35A/36A
   PREVIOUS_PROCESS_INCOMPLETE      = 17 , // R8C35A/36A,STM32,RX6,RX100
   DATA_READ_PROTECTION             = 18 , // STM32,RX6
   MEMORY_WRITE_LOCKED              = 19 , // STM32,Hcs08
   ILLEGAL_COMMAND_OR_ACCESS        = 20 , // RX6,MKE,RA4
   FCU_PROCESSING_ERROR             = 21 , // RX6
   SOURCE_DATA_BAD_POSITION         = 22 , // RX6
   FLOW_ERROR                       = 23 , //
   NO_COMMAND_RUNNING               = 24 , //
   PARALLELISM_ERROR                = 25 , // STM32F4XX
   ALIGNMENT_ERROR                  = 26 , // STM32F4XX, MKE,Stm32g0,Stm32L4
   LIBRARY_INTERNAL_ERROR           = 27 , // RX100
   READ_COLLISION                   = 28 , // MKE
   MGSTAT0_RUNTIME_ERROR            = 29 , // MKE
   LIBRARY_NOT_INITIALIZED          = 30 , // MKE
   DATA_FLASH_NOT_AVAILABLE         = 31 , // MKE
   EEPROM_NOT_AVAILABLE             = 32 , // MKE
   PARTITIONS_SET_FAILED            = 33 , // MKE
   UNKNOWN_PARTITION_STATUS         = 34 , // MKE
   PARAMETER_OVERWRITE_DISABLED     = 35 , // MKE
   ACCESS_SIZE_ERROR                = 36 , // Stm32g0,Stm32L4
   PROGRAMMING_SEQUENCE_ERROR       = 37 , // Stm32g0,Stm32L4
   MISS_DATA_ERROR                  = 38 , // Stm32g0,Stm32L4
   FAST_PROGRAMMING_ERROR           = 39 , // Stm32g0,Stm32L4
   OPTIONS_BITS_VALIDITY_ERROR      = 40 , // Stm32g0,Stm32L4
   FENTRY_ERROR                     = 41 , // RA2,RA4
   SECURITY_ERROR                   = 42 , // RA4
   STROBE_ERROR                     = 43 , // STM32H7XX
   INCONSISTENCY_ERROR              = 44 , // STM32H7XX
   READ_SECURE_ERROR                = 45 , // STM32H7XX
   SINGLE_CORRECTION_ERROR          = 46 , // STM32H7XX
   ECC_DOUBLE_DETECTION_ERROR       = 47 , // STM32H7XX
   CRC_READ_ERROR                   = 48 , // STM32H7XX
   TIME_WINDOW_ERROR                = 49 , // SH30F MSPM0
   FLASH_STATUS_ERROR               = 50 , // SH30F
   FLASH_BANK_NOT_IN_READ_MODE      = 51 , // MSPM0
   FLASH_MISCELLANEOUS_ERROR        = 52   // MSPM0
}tStatusCode;

/**************************************************************************************************/
/* TEST INSTRUMENTATION: to be placed after (type) definitions and before function declarations   */
/**************************************************************************************************/
#if defined (MODULE_TEST)
#include "Flash_Stub.h"
#endif

/******************************************************************************/
/* PUBLIC FUNCTION DECLARATIONS                                               */
/******************************************************************************/
/* FLASH_bEnableAccess() */
/** \brief    application specific function to enable
 *            Flash access.
 *
 *  \return   true   -  operation sucessfull\n
 *            false  -  operation failed
 *  \remarks
 */
bool FLASH_bEnableAccess(void);

/* FLASH_bDisableAccess() */
/** \brief    application specific function to disable 
 *            flash access.
 *
 *  \return   true   -  operation sucessfull\n
 *            false  -  operation failed
 *  \remarks
 */
bool FLASH_bDisableAccess(void);

/* FLASH_bIsAccessEnabled() */
/** \brief    function to test flash access.
 *
 *  \return   true    - access enabled \n
 *            false   - access disabled
 *  \remarks
 */
bool FLASH_bIsAccessEnabled(void);

/* FLASH_ucGetCellSize() */
/** \brief    Retrieves the flash cell size in 8-bit units
 *
 *  \return
 *            size of flash cell in 8-bit units (1=8-Bit, 2=16-bits, 4=32-bits, 8=64-bits)
 *  \remarks
 *            - Flash cell size depends on platform and is not always the same
 *            - This is a helper routine to upper layers to compute the
 *              flash address before calling flash read/write/erase routines.
 */
uint8_t FLASH_ucGetCellSize (void);

/* FLASH_bWrite() */
#ifdef STM32_ARM
/** \brief    Accesses Flash for writing
 *
 *  \param    ptAddr  - flash start address for writing
 *  \param    ptData  - byte buffer containing data to write, read only
 *  \param    uiSize  - number of bytes to write
 *  \return
 *            true    - operation successful \n
 *            false   - operation failed
 *  \remarks
 *            - ptAddr != NULL : flash address must be valid
 *            - ptAddr should be even value. If is not the nearest even address is used too
 *            - allways at least 2 bytes are written at once
 *            - there should be even number of written bytes
 */
#else
/** \brief    Accesses Flash for writing
 *
 *  \param    ptAddr  - flash start address for writing
 *  \param    ptData  - byte buffer containing data to write, read only
 *  \param    uiSize  - number of bytes to write
 *  \return
 *            true    - operation successful \n
 *            false   - operation failed
 *\remarks 
 *            - ptAddr != NULL : flash address must be valid.
 *            - ptAddr must be \b even for a 16-bit controller (e.g M16C)
 */
#endif
bool FLASH_bWrite(tAddress *ptAddr, tWriteData *ptData, uint16_t uiSize);

/* FLASH_bWriteByte() */
#ifdef STM32_ARM
/** \brief    Accesses Flash for writing
 *
 *  \param    ptAddr  - flash start address for writing
 *  \param    ucData  - byte buffer containing data to write, read only
 *  \return
 *            true    - operation successful \n
 *            false   - operation failed
 *\remarks
 *            - ptAddr != NULL : flash address must be valid
 *            - ptAddr should be even value. If is not the nearest even address is used too
 *            - allways at least 2 bytes are written at once
 */
#else
/** \brief    Accesses Flash for writing
 *
 *  \param    ptAddr  - flash start address for writing
 *  \param    ucData  - byte buffer containing data to write, read only
 *  \return
 *            true    - operation successful \n
 *            false   - operation failed
 *  \remarks
 *            - ptAddr != NULL : flash address must be valid.
 *            - ptAddr must be \b even for a 16-bit controller (e.g M16C,STM32)
 */
#endif
bool FLASH_bWriteByte (tAddress *ptAddr, uint8_t ucData);

/* FLASH_bRead() */
/** \brief    Accesses Flash for read
 *
 *  \param    ptAddr  - flash start address for read
 *  \param    ptData  - byte buffer for read data
 *  \param    uiSize  - number of bytes to read
 *  \return
 *            true    - operation successful \n
 *            false   - operation failed
 *  \remarks 
 *            - ptAddr != NULL : Flash address must be valid.
 *            - ptAddr must be \b even for a 16-bit controller (e.g M16C,STM32)
 *            - ptData != NULL : Read buffer must be valid.
 *            - uiSize  != 0    : At least one data must be read.
 */
bool FLASH_bRead(tAddress *ptAddr, tReadData *ptData, uint16_t uiSize);

/* FLASH_bErase() */
/** \brief    Erases Flash 
 *
 *  \param    ptAddress - flash start address for erasing
 *  \return
 *            true    - operation successful \n
 *            false   - operation failed
 *  \remarks
 *            - Whole block including given flash address will be erased.
 */
bool FLASH_bErase(tAddress *ptAddress);

/* FLASH_tGetFunctionStatus() */
/** \brief    Returns error code of last called flash function
 *
 *  \return
 *            One of tStatusCode values
 *  \remarks
 *            - Functions FLASH_bIsAccessEnabled() and FLASH_ucGetCellSize() have no influence on this function.
 */
tStatusCode FLASH_tGetFunctionStatus(void);

#ifdef __cplusplus
}
#endif

#endif //FLASHLIB_H
