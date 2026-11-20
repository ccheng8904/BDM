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
 *  COMP_ABBREV      DBGX
 ******************************************************************************/

#ifndef DEBUG_EXTENDED_API_H
#define DEBUG_EXTENDED_API_H

/******************************************************************************/
/* DOCUMENTATION                                                              */
/******************************************************************************/
/** \file
 *
 *  \ingroup  debug_extended
 *
 *  \brief    Public definitions and declarations of generic part of the
 *            debug_extended component: API layer
 *
 *  \details  
 */

/******************************************************************************/
/* INCLUDES                                                                   */
/******************************************************************************/
#include <stdint.h>
#include <stdbool.h>
#include "debug_extended/protocol.h"
#include "dbgx_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/******************************************************************************/
/* PUBLIC TYPE DEFINITIONS                                                    */
/******************************************************************************/


/******************************************************************************/
/* PUBLIC PREPROCESSOR DEFINITIONS                                            */
/******************************************************************************/

/* For the return codes of API functions: */
#define DBGX_STAT_NO_ERROR             DBGP_STAT_NO_ERROR
#define DBGX_STAT_NOTIFY_PROBLEM       DGBP_STAT_NOTIFY_PROBLEM
#define DBGX_STAT_NOT_NOTIFIED         ((int32_t)-1)

/* For the bitwise encoding / decoding of filter flag words: */
#define DBGX_LEV_BIT_IDX_SHIFT         26
#define DBGX_SCN_BIT_IDX_SHIFT         20
#define DBGX_SCN_BIT_IDX_MASK          ((uint32_t)0x03F00000)
#define DBGX_SCN_WORD_IDX_MASK         ((uint32_t)0x0000FFFF)
#define DBGX_NO_FILTER                 ((uint32_t)0x00000000)

/* For generating of the level characters: */
#define DBGX_LEV_NONE                  ((uint8_t)0x00)
#define DBGX_LEV_ERR_BITIDX            ((uint8_t)0x01)
#define DBGX_LEV_WARN_BITIDX           ((uint8_t)0x02)
#define DBGX_LEV_INFO_BITIDX           ((uint8_t)0x03)

/* For the integer formats of debug_extended data:
 * Format definitions for DBGX_logInt...: 0x00llssbb
 *   ll: Integer length: 1, 2, 4
 *   ss: Signed / unsigned: 1 / 0
 *   bb: Base: 2 for binary, 10 for decimal, 8 for octal, 16 for hexadecimal  
*/
#define DBGX_BITS_PER_BYTE                     8
#define DBGX_FORMAT_INTLEN_8_NON_SHIFT         ((uint32_t)1U)
#define DBGX_FORMAT_INTLEN_16_NON_SHIFT        ((uint32_t)2U)
#define DBGX_FORMAT_INTLEN_32_NON_SHIFT        ((uint32_t)4U)
#define DBGX_FORMAT_UNSIGNED_NON_SHIFT         ((uint32_t)0U)
#define DBGX_FORMAT_SIGNED_NON_SHIFT           ((uint32_t)1U)
#define DBGX_FORMAT_BASE_2_NON_SHIFT           ((uint32_t)2U)
#define DBGX_FORMAT_BASE_8_NON_SHIFT           ((uint32_t)8U)
#define DBGX_FORMAT_BASE_10_NON_SHIFT          ((uint32_t)10U)
#define DBGX_FORMAT_BASE_16_NON_SHIFT          ((uint32_t)16U)

#define DBGX_FORMAT_INTLEN_8                   ((uint32_t)0)  // was introduced with LTS 1.17 and needs to remain due to compatibility
#define DBGX_FORMAT_INTLEN_16                  ((uint32_t)0)  // was introduced with LTS 1.17 and needs to remain due to compatibility
#define DBGX_FORMAT_INTLEN_32                  ((uint32_t)0)  // was introduced with LTS 1.17 and needs to remain due to compatibility
#define DBGX_FORMAT_UNSIGNED                   (DBGX_FORMAT_UNSIGNED_NON_SHIFT  << (1 * DBGX_BITS_PER_BYTE))
#define DBGX_FORMAT_SIGNED                     (DBGX_FORMAT_SIGNED_NON_SHIFT    << (1 * DBGX_BITS_PER_BYTE))
#define DBGX_FORMAT_BASE_2                     (DBGX_FORMAT_BASE_2_NON_SHIFT    << (0 * DBGX_BITS_PER_BYTE))
#define DBGX_FORMAT_BASE_8                     (DBGX_FORMAT_BASE_8_NON_SHIFT    << (0 * DBGX_BITS_PER_BYTE))
#define DBGX_FORMAT_BASE_10                    (DBGX_FORMAT_BASE_10_NON_SHIFT   << (0 * DBGX_BITS_PER_BYTE))
#define DBGX_FORMAT_BASE_16                    (DBGX_FORMAT_BASE_16_NON_SHIFT   << (0 * DBGX_BITS_PER_BYTE))

/* For the format building brick compositions for putting out integer values using DBGX_logInt... - Parameter "Format" */
#define DBGX_UNSIGNED_DUAL                     (DBGX_FORMAT_UNSIGNED | DBGX_FORMAT_BASE_2)
#define DBGX_UNSIGNED_OCTAL                    (DBGX_FORMAT_UNSIGNED | DBGX_FORMAT_BASE_8)
#define DBGX_UNSIGNED_DECIMAL                  (DBGX_FORMAT_UNSIGNED | DBGX_FORMAT_BASE_10)
#define DBGX_UNSIGNED_HEXADECIMAL              (DBGX_FORMAT_UNSIGNED | DBGX_FORMAT_BASE_16)

#define DBGX_SIGNED_DUAL                       (DBGX_FORMAT_SIGNED   | DBGX_FORMAT_BASE_2)
#define DBGX_SIGNED_OCTAL                      (DBGX_FORMAT_SIGNED   | DBGX_FORMAT_BASE_8)
#define DBGX_SIGNED_DECIMAL                    (DBGX_FORMAT_SIGNED   | DBGX_FORMAT_BASE_10)
#define DBGX_SIGNED_HEXADECIMAL                (DBGX_FORMAT_SIGNED   | DBGX_FORMAT_BASE_16)

/* Do not use the following format definitions any more. They were introduced with LTS 1.17 and need to remain due to compatibility only */
#define DBGX_UINT8_DUAL                DBGX_UNSIGNED_DUAL
#define DBGX_UINT8_OCTAL               DBGX_UNSIGNED_OCTAL
#define DBGX_UINT8_DECIMAL             DBGX_UNSIGNED_DECIMAL
#define DBGX_UINT8_HEXADECIMAL         DBGX_UNSIGNED_HEXADECIMAL

#define DBGX_UINT16_DUAL               DBGX_UNSIGNED_DUAL
#define DBGX_UINT16_OCTAL              DBGX_UNSIGNED_OCTAL
#define DBGX_UINT16_DECIMAL            DBGX_UNSIGNED_DECIMAL
#define DBGX_UINT16_HEXADECIMAL        DBGX_UNSIGNED_HEXADECIMAL

#define DBGX_UINT32_DUAL               DBGX_UNSIGNED_DUAL
#define DBGX_UINT32_OCTAL              DBGX_UNSIGNED_OCTAL
#define DBGX_UINT32_DECIMAL            DBGX_UNSIGNED_DECIMAL
#define DBGX_UINT32_HEXADECIMAL        DBGX_UNSIGNED_HEXADECIMAL

#define DBGX_INT8_DECIMAL              DBGX_SIGNED_DECIMAL
#define DBGX_INT16_DECIMAL             DBGX_SIGNED_DECIMAL
#define DBGX_INT32_DECIMAL             DBGX_SIGNED_DECIMAL

/* For the solving of detected exceptional errors: */
#define DBGX_ERR_SOLVING_ID_MASK       ((uint32_t)0x70000000)
#define DBGX_ERR_SOLVING_NONE          ((uint32_t)0x00000000)
#define DBGX_ERR_SOLVING_RESET         ((uint32_t)0x10000000)
#define DBGX_ERR_SOLVING_CALLBACK      ((uint32_t)0x20000000)   /* Callback function DBGX_solveExceptionalError will be called*/

#define DBGX_SHIFT_INT_LEN_TO_FORMAT(size)   ((size) << (2 * (DBGX_BITS_PER_BYTE)))
#define DBGX_ADD_INT_LEN_TO_FORMAT(param)    DBGX_SHIFT_INT_LEN_TO_FORMAT(sizeof(param))

/************************************************************
* Unique exceptional error codes for DBGX: 0x*0000000 - 0x*000FFFF */

/* The module ID of debug_extended data received from DBGP (from PC) is not known
 *  > Notify the debug_extended data callback function of the module by DBGX_notifyReceiveData_CB. */
#define DBGX_ERR_NDEF_MODULE_IDX                  0x00000000

/* Too many modules of the current module or module is already notified
 *  > Increase DBGX_MAX_NUMBER_MODULES_RX or do not notify more than once. */
#define DBGX_ERR_MODULE_NOT_NOTIFIED              0x00000010

/* Text output in protocol layer failed for non-filtered data. */
#define DBGX_ERR_PRT_ARRAY_NON_FILTERED           0x00000020

/* Text output in protocol layer failed for filtered data. */
#define DBGX_ERR_PRT_ARRAY_FILTERED               0x00000030

/* Notify if Rx-data function of the protocol layer has failed */
#define DBGX_ERR_NOTIFY_PROBLEM_PROT_LAYER        0x00000040

/******************************************************************************/
/* PUBLIC FUNCTION DECLARATIONS                                               */
/******************************************************************************/

/**********************************
* Application related interface (functions can be encapsulated for the applications in the config part) */

/*========
* Debug_extended interface controller output data transfer for application protocol: */

/** \brief  Sends the data of the application protocol (e.g. touch data) via the debug_extended interface, filtered
 *  \note   Use the encapsulating DBGX_sendData in your application
 *
 *  \param  Module_ID      ID of the module (e.g touch), needed for the PC GUI tool
 *  \param  CallingFilter  Debug_extended output data filter settings at the calling location
 *  \param  Function       Function of the debug_extended output data, needed for the PC GUI tool
 *  \param  Data           Pointer to payload (user data) of debug_extended output data for the PC GUI tool
 *  \param  DataLen        Number of bytes of payload (user data) of debug_extended output data for the PC GUI tool
 *  \return Error status (==0: no error; < 0: error)
 */
extern int32_t DBGX_sendData_Gen(uint16_t Module_ID, uint32_t CallingFilter, uint16_t Function, const uint8_t *Data, uint16_t DataLen);

/*========
* Debug_extended interface controller input transfer for application protocol: */

/** \brief  Notifies the receive data function for application protocol (e.g. touch data)
 *  \note   You can use this function non-encapsulated in your application
 *
 *  \param  ReceiveDataFunctionOfAPI_CB  Function pointer to the API callback function for Rx-data
 *  \param  ExpectedModuleID             Module ID of the callback function for Rx-data (ID expected by the
 *                                       debug_extended protocol layer for the module, e.g. for touch)
 *  \return Error status (==0: no error; < 0: error)
 */
extern int32_t DBGX_notifyReceiveData_CB(DBGX_ReceiveDataFunctionOfApi_t ReceiveDataFunctionOfApi_CB, uint16_t ExpectedModuleID);

/*========
* Debug_extended interface controller output data transfer for text: */

/** \brief  Sends a character via the debug_extended interface, filtered
 *  \note   Use the encapsulating DBGX_logChar in your application
 *
 *  \param  CallingFilter  Debug_extended output data filter settings at the calling location
 *  \param  Char           Character to be sent
 */
extern void DBGX_logChar_Gen(uint32_t CallingFilter, char Char);

/** \brief  Sends a string via the debug_extended interface, NULL-termination
 *          character not included, filtered
 *  \note   Use the encapsulating DBGX_logStr in your application
 *
 *  \param  CallingFilter  Debug_extended output data filter settings at the calling location
 *  \param  String         Pointer to the string
 */
extern void DBGX_logStr_Gen(uint32_t CallingFilter, const char *String);

/** \brief  Sends an integer value as characters via the debug_extended interface, filtered
 *  \note   Use the encapsulating DBGX_logInt in your application
 *
 *  \param  CallingFilter   Debug_extended output data filter settings at the calling location
 *  \param  IntegerAddress  Pointer to the integer value to be sent
 *  \param  Format          Format of the integer value, see the definitions of DBGX_FORMAT_...
 */
extern void DBGX_logInt_Gen(uint32_t CallingFilter, const void *IntegerAddress, uint32_t Format);

/** \brief  Sends an array of integer values as characters via the debug_extended
 *          interface, array elements separated by space, filtered
 *  \note   Use the encapsulating DBGX_logIntArr in your application
 *
 *  \param  CallingFilter          Debug_extended output data filter settings at the calling location
 *  \param  IntegerArrayAddress    Pointer to the integer array values to be sent
 *  \param  NumberOfArrayElements  Number of elements in the array
 *  \param  Format                 Format of the integer value, see the definitions of DBGX_FORMAT_...
 */
extern void DBGX_logIntArr_Gen(uint32_t CallingFilter, const void *IntegerArrayAddress, uint16_t NumberOfArrayElements, uint32_t Format);

/** \brief  Sends a decimal time stamp as characters via the debug_extended interface, in microseconds, filtered
 *  \note   Use the encapsulating DBGX_logTime in your application
 *
 *  \param  CallingFilter  Debug_extended output data filter settings at the calling location
 */
extern void DBGX_logTime_Gen(uint32_t CallingFilter);

/** \brief  Sends the source location as characters via the debug_extended
 *          interface, filtered
 *  \note   Use the encapsulating DBGX_logLocat in your application
 *
 *  \param  CallingFilter  Debug_extended output data filter settings at the calling location
 *  \param  File           String of the file of source location
 *  \param  LineNumber     Value of the line number of source location
 */
extern void DBGX_logLocat_Gen(uint32_t CallingFilter, const char *File, uint32_t LineNumber);

/** \brief  Sends a concatenation of two strings via the debug_extended interface, filtered
 *  \note   Use the encapsulating DBGX_logStrStr in your application
 *
 *  \param  CallingFilter  Debug_extended output data filter settings at the calling location
 *  \param  StringA        First string for the concatenation (Input string requires \0)
 *  \param  StringB        (Second) String to be concatenated behind InCharArrayA (Input string requires \0)
 */
extern void DBGX_logStrStr_Gen(uint32_t CallingFilter, const char *StringA, const char *StringB);

/** \brief  Sends a concatenation of a string and an integer via the debug_extended interface, filtered
 *  \note   Use the encapsulating DBGX_logStrInt in your application
 *
 *  \param  CallingFilter   Debug_extended output data filter settings at the calling location
 *  \param  String          Leading String before the integer value
 *  \param  IntegerAddress  Pointer to the integer value to be sent
 *  \param  Format          Format of the integer value, see the definitions of DBGX_FORMAT_...
 */
extern void DBGX_logStrInt_Gen(uint32_t CallingFilter, const char *String, const void *IntegerAddress, uint32_t Format);

/** \brief  Sends a concatenation of a string and an integer array via the debug_extended interface, filtered
 *  \note   Use the encapsulating DBGX_logStrIntArr in your application
 *
 *  \param  CallingFilter          Debug_extended output data filter settings at the calling location
 *  \param  String                 Leading String before the integer array
 *  \param  IntArray               Pointer to the integer array values to be sent
 *  \param  NumberOfArrayElements  Number of elements in the array
 *  \param  Format                 Format of the integer value, see the definitions of DBGX_FORMAT_...
 */
extern void DBGX_logStrIntArr_Gen(uint32_t CallingFilter, const char *String, const void *IntArray, uint16_t NumberOfArrayElements, uint32_t Format);

/** \brief  Sends a concatenation of two strings and the source location (just function name) as character
 *          via the debug_extended interface, filtered
 *  \note   Use the encapsulating DBGX_logStrStrLocat in your application
 *
 *  \param  CallingFilter  Debug_extended output data filter settings at the calling location
 *  \param  StringA        First string for the concatenation (Input string requires \0)
 *  \param  StringB        (Second) String to be concatenated behind InCharArrayA (Input string requires \0)
 *  \param  Function       String of the function name of the source location
 */
extern void DBGX_logStrStrLocat_Gen(uint32_t CallingFilter, const char *StringA, const char *StringB, const char *Function);

/** \brief  Sends a concatenation of two strings and the source location (filename, line number, function name) as character
 *          as character via the debug_extended interface, filtered
 *  \note   Use the encapsulating DBGX_logStrStrLocatFull in your application
 *
 *  \param  CallingFilter  Debug_extended output data filter settings at the calling location
 *  \param  StringA        First string for the concatenation (Input string requires \0)
 *  \param  StringB        (Second) String to be concatenated behind InCharArrayA (Input string requires \0)
 *  \param  File           String of the file of source location
 *  \param  LineNumber     Value of the line number of source location
 *  \param  Function       String of the function name of the source location
 */
extern void DBGX_logStrStrLocatFull_Gen(uint32_t CallingFilter, const char *StringA, const char *StringB, const char *File, uint32_t LineNumber, const char *Function);

/** \brief  Sends a concatenation of a string, an integer and the source location (just function name)
 *          as character via the debug_extended interface, filtered
 *  \note   Use the encapsulating DBGX_logStrIntLocat in your application
 *
 *  \param  CallingFilter   Debug_extended output data filter settings at the calling location
 *  \param  String          Leading String before the integer value
 *  \param  IntegerAddress  Pointer to the integer value to be sent
 *  \param  Format          Format of the integer value, see the definitions of DBGX_FORMAT_...
 *  \param  Function        String of the function name of the source location
 */
extern void DBGX_logStrIntLocat_Gen(uint32_t CallingFilter, const char *String, const void *IntegerAddress, uint32_t Format, const char *Function);

/** \brief  Sends a concatenation of a string, an integer and the source location (filename, line number, function name)
 *          as character via the debug_extended interface, filtered
 *  \note   Use the encapsulating DBGX_logStrIntLocatFull in your application
 *
 *  \param  CallingFilter   Debug_extended output data filter settings at the calling location
 *  \param  String          Leading String before the integer value (Input string requires \0)
 *  \param  IntegerAddress  Pointer to the integer value to be sent
 *  \param  Format          Format of the integer value, see the definitions of DBGX_FORMAT_...
 *  \param  File            String of the file of source location
 *  \param  LineNumber      Value of the line number of source location
 *  \param  Function        String of the function name of the source location
 */
extern void DBGX_logStrIntLocatFull_Gen(uint32_t CallingFilter, const char *String, const void *IntegerAddress, uint32_t Format, const char *File, uint32_t LineNumber, const char *Function);

/** \brief  Non-filtered version of: \sa DBGX_logChar_Gen
 *  \note   Use the encapsulating DBGX_logChar_NonFiltered in your application
 */
extern void DBGX_logChar_NonFiltered_Gen(char Char);

/** \brief  Non-filtered version of: \sa DBGX_logStr_Gen
 *  \note   Use the encapsulating DBGX_logStr_NonFiltered in your application
 */
extern void DBGX_logStr_NonFiltered_Gen(const char *String);

/** \brief  Non-filtered version of: \sa DBGX_logInt_Gen
 *  \note   Use the encapsulating DBGX_logInt_NonFiltered in your application
 */
extern void DBGX_logInt_NonFiltered_Gen(const void *IntegerAddress, uint32_t Format);

/** \brief  Non-filtered version of: \sa DBGX_logIntArr_Gen
 *  \note   Use the encapsulating DBGX_logIntArr_NonFiltered in your application
 */
extern void DBGX_logIntArr_NonFiltered_Gen(const void *IntArray, uint16_t NumberOfArrayElements, uint32_t Format);

/** \brief  Non-filtered version of: \sa DBGX_logTime_Gen
 *  \note   Use the encapsulating DBGX_logTime_NonFiltered in your application
 */
extern void DBGX_logTime_NonFiltered_Gen(void);

/** \brief  Non-filtered version of: \sa DBGX_logLocat_Gen
 *  \note   Use the encapsulating DBGX_logLocat_NonFiltered in your application
 */
extern void DBGX_logLocat_NonFiltered_Gen(const char *File, uint32_t LineNumber);

/** \brief  Non-filtered version of: \sa DBGX_logStrStr_Gen
 *  \note   Use the encapsulating DBGX_logStrStr_NonFiltered in your application
 */
extern void DBGX_logStrStr_NonFiltered_Gen(const char *StringA, const char *StringB);

/** \brief  Non-filtered version of: \sa DBGX_logStrInt_Gen
 *  \note   Use the encapsulating DBGX_logStrInt_NonFiltered in your application
 */
extern void DBGX_logStrInt_NonFiltered_Gen(const char *String, const void *IntegerAddress, uint32_t Format);

/** \brief  Non-filtered version of: \sa DBGX_logStrIntArr_Gen
 *  \note   Use the encapsulating DBGX_logStrIntArr_NonFiltered in your application
 */
extern void DBGX_logStrIntArr_NonFiltered_Gen(const char *String, const void *IntArray, uint16_t NumberOfArrayElements, uint32_t Format);

/** \brief  Non-filtered version of: \sa DBGX_logStrStrLocat_Gen
 *  \note   Use the encapsulating DBGX_logStrStrLocat_NonFiltered in your application
 */
extern void DBGX_logStrStrLocat_NonFiltered_Gen(const char *StringA, const char *StringB, const char *Function);

/** \brief  Non-filtered version of: \sa DBGX_logStrStrLocatFull_Gen
 *  \note   Use the encapsulating DBGX_logStrStrLocatFull_NonFiltered in your application
 */
extern void DBGX_logStrStrLocatFull_NonFiltered_Gen(const char *StringA, const char *StringB, const char *File, uint32_t LineNumber, const char *Function);

/** \brief  Non-filtered version of: \sa DBGX_logStrIntLocat_Gen
 *  \note   Use the encapsulating DBGX_logStrIntLocat_NonFiltered in your application
 */
extern void DBGX_logStrIntLocat_NonFiltered_Gen(const char *StringA, const void *IntegerAddress, uint32_t Format, const char *Function);

/** \brief  Non-filtered version of: \sa DBGX_logStrIntLocatFull_Gen
 *  \note   Use the encapsulating DBGX_logStrIntLocatFull_NonFiltered in your application
 */
extern void DBGX_logStrIntLocatFull_NonFiltered_Gen(const char *StringA, const void *IntegerAddress, uint32_t Format, const char *File, uint32_t LineNumber, const char *Function);

/*========
* Debug_extended interface controller input data transfer for text: */

/** \brief  Receives a string via the debug_extended interface
 *  \note   Use the encapsulating DBGX_getString in your application
 *
 *  \param  String     Address of the allocated memory, the input string will be saved into
 *  \param  StringLen  Length of the allocated array, the input string will be saved into
 */
extern int32_t DBGX_getString_Gen(char *String, uint16_t StringLen);

/** \brief  Notify the upper layer that the cooperative reading from the interface input is complete
 *  \note   This function is called from the lower layer
 *   *
 *  \param  Data     Address of the stored input data
 *  \param  DataLen  Length of the input data
 */
extern void DBGX_getString_CB(uint8_t *Data, uint16_t DataLen);

/** \brief  Sets the input callback functions
 *  \note   This function is called from the config-area. Do not call this function in your application
 *
 *  \param  InputCallbackFunctionArray      Address of the function to be called
 *  \param  NumberOfInputCallbackFunctions  Number of the registered callback functions
 *  \param  MaxNumberOfInputCallbackFunctionsFunctions  Maximum number of callback functions
 */
extern void DBGX_setInputCallbackFunctions(const DBGX_InputCallbackFunctions_t *InputCallbackFunctionArray, const uint8_t *NumberOfInputCallbackFunctions, uint8_t MaxNumberOfInputCallbackFunctionsFunctions);


/**********************************
* Configuration Interface: */

/** \brief  Activate the DbgAttributes for the debug_extended output data
 */
extern void DBGX_activateDbgAttributes( void );

/** \brief  Initializes the API of the debug_extended component for the configured callback functions (of rx data)
 *  \note   This function must be called before any access function may be used
 *
 *  \param  ApiCallbacks  Pointer to the list of the notified API callback functions for Rx-data
 */
extern void DBGX_initDebugExtendedAPI_ForCallbacks(const struct DBGX_ApiCallbacksGen_s *ApiCallbacks);

/** \brief  Initializes the API of the debug_extended component for the configured filter (of tx data)
 *  \note   This function must be called before any access function may be used
 *
 *  \param  DBGX_CentralConfigFilter_Cfg  Pointer to the central debug_extended output data filter settings
 */
extern void DBGX_initDebugExtendedAPI_ForFilter(const struct DBGX_FilterGen_s *CentralConfigFilter_Cfg);

/** \brief  Initializes the configured module abbreviations of the debug_extended
 *          component for putting into the debug_extended output
 *
 *  \param  ModuleAbbreviations          Pointer to array containing the module abbreviation strings
 *  \param  NumberOfModuleAbbreviations  Number of module abbreviations
 *  \param  LenOfModuleAbbreviations     Number of bytes of each module abbreviation
 */
extern void DBGX_initModuleAbbreviations(const char *ModuleAbbreviations, uint8_t NumberOfModuleAbbreviations,
                                        uint8_t LenOfModuleAbbreviation);

/**********************************
* Exceptional Error Interface: */

/** \brief  Puts out an exceptional error via the debug_extended interface and saves
 *          it in the exceptional error memory, without error parameter, without pre-condition
 *  \note   Use the encapsulating DBGX_handleErr in your application
 *
 *  \param  ErrorCode  Exceptional error code
 */
extern void DBGX_handleErr_Gen(uint32_t ErrorCode);

/** \brief  Puts out an exceptional error via the debug_extended interface and saves
 *          it in the exceptional error memory, with error parameter, without pre-condition
 *  \note   Use the encapsulating DBGX_handleErrWithParam in your application
 *
 *  \param  ErrorCode       Exceptional error code
 *  \param  ErrorParameter  Parameter of the error code (e.g. a return value of a library function)
 */
extern void DBGX_handleErrWithParam_Gen(uint32_t ErrorCode, uint32_t ErrorParameter);

/** \brief  Puts out an exceptional error via the debug_extended interface and saves
 *          it in the exceptional error memory, without error parameter, if pre-condition is fulfilled
 *  \note   Use the encapsulating DBGX_handleErrIf in your application
 *
 *  \param  Condition  Pre-condition
 *  \param  ErrorCode  Exceptional error code
 */
extern void DBGX_handleErrIf_Gen(bool Condition, uint32_t ErrorCode);

/** \brief  Puts out an exceptional error via the debug_extended interface and saves
 *          it in the exceptional error memory, with error parameter, if pre-condition is fulfilled
 *  \note   Use the encapsulating DBGX_handleErrWithParamIf in your application
 *
 *  \param  Condition       Pre-condition
 *  \param  ErrorCode       Exceptional error code
 *  \param  ErrorParameter  Parameter of the error code (e.g. a return value of a library function)
 */
extern void DBGX_handleErrWithParamIf_Gen(bool Condition, uint32_t ErrorCode, uint32_t ErrorParameter);

/** \brief  Reads the first occurred exceptional error after reset
 *  \note   Use the encapsulating DBGX_readFirstError in your application
 *
 *  \param  ErrorCode       Pointer to the exceptional error code
 *  \param  ErrorParameter  Pointer to the parameter of the error code (e.g. a return value of a library function)
 *  \return Number of saved errors (0, 1 or 2)
 */
extern uint8_t DBGX_readFirstError_Gen(uint32_t *ErrorCode, uint32_t *ErrorParameter);

/** \brief  Reads the last occurred exceptional error after reset
 *  \note   Use the encapsulating DBGX_readLastError in your application
 *
 *  \param  ErrorCode       Pointer to the exceptional error code
 *  \param  ErrorParameter  Pointer to the parameter of the error code (e.g. a return value of a library function)
 *  \return Number of saved errors (0, 1 or 2)
 */
extern uint8_t DBGX_readLastError_Gen(uint32_t *ErrorCode, uint32_t *ErrorParameter);

/**********************************
* Internal error interface between generic and configuration part: */

/** \brief  Writes the source location of an exceptional error
 *  \note   Do not use this function in your application
 *
 *  \param  File        String of the file of source location
 *  \param  LineNumber  Value of the line number of source location
 */
extern void DBGX_writeErrorSourceLocation_Gen(const char *File, uint32_t LineNumber);

/** \brief  Writes the source location of an exceptional error, if the pre-condition is fulfilled
 *  \note   Do not use this function in your application
 *
 *  \param  Condition   Pre-condition
 *  \param  File        String of the file of source location
 *  \param  LineNumber  Value of the line number of source location
 */
extern void DBGX_writeErrorSourceLocationCond_Gen(bool Condition, const char *File, uint32_t LineNumber);

#if defined STS_SIM_TEST_CI
extern void DBGX_calc_Formatted_PositiveIntegerStringPart(uint32_t PositiveIntegerValueForCalc, uint8_t Base, uint8_t IntegerStringIndex, char *IntegerString);
extern void DBGX_calc_Formatted_IntegerString(uint32_t IntegerValue, uint8_t IntegerLen, uint8_t Signed, uint8_t Base, char *IntegerString);
extern void DBGX_getIntegerCharacteristicsFromFormat(uint32_t Format, uint8_t *IntLen, uint8_t *Signed, uint8_t *Base);
extern char DBGX_convertCipherToCharacter(uint8_t Cipher);
extern uint32_t DBGX_get_IntegerValue_From_ValuePointerContents(const void *Pointer, uint8_t IntegerLen);
#endif


#ifdef __cplusplus
}
#endif

#endif   /* From: ifndef DEBUG_EXTENDED_API_H */

