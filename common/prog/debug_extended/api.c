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

/******************************************************************************/
/* DOCUMENTATION                                                              */
/******************************************************************************/
/** \file
 *
 *  \ingroup   debug_extended
 *
 *  \brief     Definitions and declarations of generic part of the debug_extended
 *             component: API layer
 *
 *  \details  
 */

/******************************************************************************/
/* INCLUDES                                                                   */
/******************************************************************************/

#include <stdint.h>
#include <string.h>
#include <timerlib.h>
#include "hsup.h"

#include "debug_extended/api.h"
#include "debug_extended/error.h"
#include "debug_extended/protocol.h"
#include "sts_sim_test_uty.h"


/******************************************************************************/
/* PRIVATE PREPROCESSOR DEFINITIONS                                           */
/******************************************************************************/

#if (! defined STS_SIM_TEST_CI)
    #define STATIC_DBGX    static
#else
    #define STATIC_DBGX
#endif

#define DBGX_NOT_DEFINED_MODULE_IDX              ((uint32_t)0xFFFFFFFFU)

/* Masks for the parameter "Format" in the functions DBGX_logInt...: */
#define DBGX_FORMAT_INTLEN_MASK                  ((uint32_t)0x00FF0000U)
#define DBGX_FORMAT_SIGNED_MASK                  ((uint32_t)0x0000FF00U)
#define DBGX_FORMAT_BASE_MASK                    ((uint32_t)0x000000FFU)

/* Integer string length, inclusive sign and '\0': */
#define DBGX_INTEGER_BINARY_STRING_LEN_32BIT     ((uint8_t)34)
#define DBGX_INTEGER_DECIMAL_STRING_LEN_32BIT    ((uint8_t)12)

/* Array size for DbgAttributes data: */
#define DBGX_ATTRIBUTE_MODULE_STRING_LEN         ((uint8_t)2)
#define DBGX_ATTRIBUTE_LEVEL_STRING_LEN          ((uint8_t)1)
#define DBGX_ATTRIBUTE_SCENARIO_STRING_LEN       ((uint8_t)4)
#define DBGX_ATTRIBUTE_TIME_STRING_LEN           ((uint8_t)8)

/* Position of the DbgAttributes data: */
#define DBGX_ATTRIBUTE_MODULE_STRING_POSITION    ((uint8_t)0)
#define DBGX_ATTRIBUTE_LEVEL_STRING_POSITION     ((uint8_t)2)
#define DBGX_ATTRIBUTE_SCENARIO_STRING_POSITION  ((uint8_t)3)
#define DBGX_ATTRIBUTE_TIME_STRING_POSITION      ((uint8_t)7)

#define DBGX_COMPARE_CHARACTERS_CONCATENATION    ((uint8_t)5)

/******************************************************************************/
/* PRIVATE DATA DECLARATIONS                                                  */
/******************************************************************************/
static struct DBGX_ApiCallbacksGen_s DBGX_ApiCallbacks;         /* Callback functions for debug_extended Rx-data using application protocol (e.g. touch) */
static struct DBGX_FilterGen_s       DBGX_CentralConfigFilter;  /* Filter for debug_extended Tx-data */

static volatile uint32_t DBGX_ErrorCodeForDebugger      = 0U;   /* Exceptional error code for inspection on debugger */
static volatile uint32_t DBGX_ErrorParameterForDebugger = 0U;   /* Exceptional error parameter for inspection on debugger */

static const char *DBGX_ModuleAbbreviations      = NULL;
static uint8_t DBGX_NumberOfModuleAbbreviations  = 0U;
static uint8_t DBGX_LenOfModuleAbbreviation;

static const DBGX_InputCallbackFunctions_t *DBGX_InputCallbackFunctions_Gen = NULL;  /* Callback functions for cooperative interface input */
static const uint8_t *DBGX_NumberOfInputCallbackFunctions = NULL;
static uint8_t DBGX_MaxNumberOfInputCallbackFunctions;

// Maximum payload of the debug_extended output string. Gets overwritten, if DbgAttributes are activated.
static uint16_t DBGX_AssemblerOutStringMaxLen = (uint16_t)DBGP_LEN_OF_PROT_FRAME_ARRAY;
static bool DBGX_DbgAttributesActivated = false;

/******************************************************************************/
/* PRIVATE FUNCTION DECLARATIONS                                              */
/******************************************************************************/
static void DBGX_receiveProtData_CB(uint16_t Module_ID, uint16_t Function, uint8_t *Data, uint16_t DataLen);
static uint32_t DBGX_getModuleIndex(uint16_t Module_ID);
static void DBGX_prepareDbgAttributesString(uint8_t Module_ID, char *DbgAttributes);
static int32_t DBGX_sendDataUnconditionally(uint32_t CallingFilter, uint16_t Module_ID, uint16_t Function, const uint8_t *Data, uint16_t DataLen);
static void DBGX_writeProtCharArrayFiltered(uint32_t CallingFilter, const char *CharArray, uint16_t NumberOfArrayElements);
static void DBGX_writeProtCharArrayNonFiltered(const char *CharArray, uint16_t NumberOfArrayElements);
static bool DBGX_isFlagSet(uint32_t FlagWord, uint8_t FlagIdx);
static bool DBGX_isDebugExtendedOutputActivated(uint32_t CallingFilter);
static uint8_t DBGX_getIntegerLenFromFormat(uint32_t Format);
static void DBGX_handleErrAfterErrOutput(uint32_t ErrorCode, uint32_t ErrorParameter);
static void DBGX_convModuleToString(uint8_t Module, char *String);
static void DBGX_completeDbgAttributesString(bool FilteredFunction, uint32_t CallingFilter, char *DbgAttributes);
static uint16_t DBGX_genIntArrayString(const void *IntArray, uint16_t NumberOfArrayElements, uint32_t Format, char *ArrayString);
static uint16_t DBGX_genIntSourceLocationString(const char *FileString, uint32_t LineNumber, char *SourceLocationString);
static void DBGX_concatenateStrings(const char *InCharArrayA, const char *InCharArrayB, char *OutBytesArray);
static const char *DBGX_GetFileNameFromFilePath(const char *path);
static void DBGX_writeStringRightAligned(const char *StringInputAttribute, uint8_t MaxLenOfStringInputAttribute, uint8_t StartPositionInStringOutputAttributes, char *DbgAttributesOutput);
static uint8_t DBGX_getLevelFromFilter(uint32_t CallingFilter);
static uint32_t DBGX_getScenarioFromFilter(uint32_t CallingFilter);
static char DBGX_convLevelToLevelChar(uint8_t Level);

#if (! defined STS_SIM_TEST_CI)
    static void DBGX_calc_Formatted_PositiveIntegerStringPart(uint32_t PositiveIntegerValueForCalc, uint8_t Base, uint8_t IntegerStringIndex, char *IntegerString);
    static void DBGX_calc_Formatted_IntegerString(uint32_t IntegerValue, uint8_t IntegerLen, uint8_t Signed, uint8_t Base, char *IntegerString);
    static void DBGX_getIntegerCharacteristicsFromFormat(uint32_t Format, uint8_t *IntLen, uint8_t *Signed, uint8_t *Base);
    static char DBGX_convertCipherToCharacter(uint8_t Cipher);
    static uint32_t DBGX_get_IntegerValue_From_ValuePointerContents(const void *Pointer, uint8_t IntegerLen);
#endif

//Axivion Disable Style MisraC2012-18.4: Pointer = Pointer + Integer is tested as OK
/******************************************************************************/
/* PUBLIC FUNCTION DEFINITIONS                                                */
/******************************************************************************/

void DBGX_activateDbgAttributes( void )
{
    DBGX_DbgAttributesActivated = true;

    // Maximum length for the output string via the debug interface with DbgAttributes:
    DBGX_AssemblerOutStringMaxLen = ((uint16_t)((DBGP_LEN_OF_PROT_FRAME_ARRAY - DBGP_LEN_ATTRIBUTES_STRING) - DBGP_LEN_ATTRIBUTES_STRING_POSTFIX));
}

void DBGX_initDebugExtendedAPI_ForCallbacks(const struct DBGX_ApiCallbacksGen_s *ApiCallbacks)
{
    int32_t ErrorReturnCode;

    DBGX_ApiCallbacks = *ApiCallbacks;
    STS_logStringAndU16Hex("DBGX_ApiCallbacks.MaxNumberModules_Rx:      ", DBGX_ApiCallbacks.MaxNumberModules_Rx);
    ///STS_logStringAndU32ArrayHex("DBGX_ApiCallbacks.ReceiveDataFunctionsOfApi_CB[]:  ", (uint32_t *)DBGX_ApiCallbacks.ReceiveDataFunctionsOfApi_CB, DBGX_ApiCallbacks.MaxNumberModules_Rx);
                     /* The physical addresses can move; for that testing output only for special cases */

    STS_logStringAndU16ArrayHex("DBGX_ApiCallbacks.ModuleIDs[]:               ", DBGX_ApiCallbacks.ModuleIDs, DBGX_ApiCallbacks.MaxNumberModules_Rx);

    ErrorReturnCode = DBGP_notifyProtReceiveData_CB(DBGX_receiveProtData_CB);
    if (ErrorReturnCode != DBGX_STAT_NO_ERROR)
    {
        DBGE_saveError(DBGX_ERR_NOTIFY_PROBLEM_PROT_LAYER);
    }
}

void DBGX_initDebugExtendedAPI_ForFilter(const struct DBGX_FilterGen_s *CentralConfigFilter)
{
    DBGX_CentralConfigFilter = *CentralConfigFilter;
}

void DBGX_initModuleAbbreviations(const char *ModuleAbbreviations, uint8_t NumberOfModuleAbbreviations,
                                  uint8_t LenOfModuleAbbreviation)
{
    DBGX_ModuleAbbreviations         = ModuleAbbreviations;
    DBGX_NumberOfModuleAbbreviations = NumberOfModuleAbbreviations;
    DBGX_LenOfModuleAbbreviation     = LenOfModuleAbbreviation;
}

int32_t DBGX_notifyReceiveData_CB(DBGX_ReceiveDataFunctionOfApi_t ReceiveDataFunctionOfApi_CB, uint16_t ExpectedModuleID)
{
    int32_t  ErrorReturnCode;
    uint16_t Idx;

    ErrorReturnCode = DBGX_STAT_NO_ERROR;

    STS_logStringAndU16Hex("DBGX_ApiCallbacks.MaxNumberModules_Rx = ", DBGX_ApiCallbacks.MaxNumberModules_Rx);
    STS_logStringAndU16ArrayHex("DBGX_ApiCallbacks.ModuleIDs[] = ", DBGX_ApiCallbacks.ModuleIDs, DBGX_ApiCallbacks.MaxNumberModules_Rx);

    for(Idx = 0U; Idx < DBGX_ApiCallbacks.MaxNumberModules_Rx ; Idx++)
    {
        if (DBGX_ApiCallbacks.ModuleIDs[Idx] == ExpectedModuleID)
        {
            STS_logString("Module found"); 
            if (DBGX_ApiCallbacks.ReceiveDataFunctionsOfApi_CB[Idx] == NULL)
            {
                STS_logString("Callback function notified, because not yet notified"); 
                DBGX_ApiCallbacks.ReceiveDataFunctionsOfApi_CB[Idx] = ReceiveDataFunctionOfApi_CB;
            }

            break;
        }
    }

    if (Idx >= DBGX_ApiCallbacks.MaxNumberModules_Rx)
    {
        STS_logString("Module not found or too many modules notifying Rx-callback"); 
        ErrorReturnCode = DBGX_STAT_NOT_NOTIFIED;
        DBGE_saveError(DBGX_ERR_MODULE_NOT_NOTIFIED);
    }

    ///STS_logStringAndU32ArrayHex("DBGX_ApiCallbacks.ReceiveDataFunctionsOfApi_CB[]:  ", (uint32_t *)DBGX_ApiCallbacks.ReceiveDataFunctionsOfApi_CB, DBGX_ApiCallbacks.MaxNumberModules_Rx);
                     /* The physical addresses can move; for that testing output only for special cases */

    return ErrorReturnCode;
}

int32_t DBGX_sendData_Gen(uint16_t Module_ID, uint32_t CallingFilter, uint16_t Function, const uint8_t *Data, uint16_t DataLen)
{
    int32_t ErrorStatus;

    if (DBGX_isDebugExtendedOutputActivated(CallingFilter))
    {
        ErrorStatus = DBGX_sendDataUnconditionally(CallingFilter, Module_ID, Function, Data, DataLen);
    }
    else
    {
        ErrorStatus = DBGX_STAT_NO_ERROR;
    }

    return ErrorStatus;
}

void DBGX_logChar_Gen(uint32_t CallingFilter, char Char)
{
    char CharArray[sizeof(char)];

    CharArray[0] = Char;
    STS_logString("DBGX_logChar_Gen");
    DBGX_writeProtCharArrayFiltered(CallingFilter, CharArray, (uint16_t)sizeof(char));
}

void DBGX_logStr_Gen(uint32_t CallingFilter, const char *String)
{
    DBGX_writeProtCharArrayFiltered(CallingFilter, String, (uint16_t)strlen(String));
}

void DBGX_logInt_Gen(uint32_t CallingFilter, const void *IntegerAddress, uint32_t Format)
{
    uint8_t  IntegerLen;
    uint8_t  Signed;
    uint8_t  Base;
    uint32_t IntegerValue;
    char     IntegerString[DBGX_INTEGER_BINARY_STRING_LEN_32BIT];

    DBGX_getIntegerCharacteristicsFromFormat(Format, &IntegerLen, &Signed, &Base);
    IntegerValue = DBGX_get_IntegerValue_From_ValuePointerContents(IntegerAddress, IntegerLen);
    DBGX_calc_Formatted_IntegerString(IntegerValue, IntegerLen, Signed, Base, IntegerString);

    DBGX_writeProtCharArrayFiltered(CallingFilter, IntegerString, (uint16_t)strlen(IntegerString));
}

void DBGX_logIntArr_Gen(uint32_t CallingFilter, const void *IntArray, uint16_t NumberOfArrayElements, uint32_t Format)
{
    char     ArrayString[DBGP_LEN_OF_PROT_FRAME_ARRAY];
    uint16_t ArrayStringLen;

    ArrayStringLen = DBGX_genIntArrayString(IntArray, NumberOfArrayElements, Format, ArrayString);
    DBGX_writeProtCharArrayFiltered(CallingFilter, ArrayString, ArrayStringLen);
}

void DBGX_logTime_Gen(uint32_t CallingFilter)
{
    uint32_t TimeInMicroseconds;

    TimeInMicroseconds = (uint32_t)TIM_u32GetCircleMicroSeconds();
    DBGX_logInt_Gen(CallingFilter, &TimeInMicroseconds, DBGX_FORMAT_INTLEN_32 | DBGX_FORMAT_UNSIGNED | DBGX_FORMAT_BASE_10);
}

void DBGX_logLocat_Gen(uint32_t CallingFilter, const char *File, uint32_t LineNumber)
{
    char     SourceLocationString[DBGP_LEN_OF_PROT_FRAME_ARRAY];
    uint16_t SourceLocationStringLen;

    SourceLocationStringLen = DBGX_genIntSourceLocationString(File, LineNumber, SourceLocationString);
    DBGX_writeProtCharArrayFiltered(CallingFilter, SourceLocationString, SourceLocationStringLen);
}

void DBGX_logStrStr_Gen(uint32_t CallingFilter, const char *StringA, const char *StringB)
{
    char ConcatenatedString[DBGP_LEN_OF_PROT_FRAME_ARRAY];

    DBGX_concatenateStrings(StringA, StringB, ConcatenatedString);
    DBGX_writeProtCharArrayFiltered(CallingFilter, ConcatenatedString, (uint16_t)strlen(ConcatenatedString));
}

void DBGX_logStrInt_Gen(uint32_t CallingFilter, const char *String, const void *IntegerAddress, uint32_t Format)
{
    uint8_t  IntegerLen;
    uint8_t  Signed;
    uint8_t  Base;
    uint32_t IntegerValue;
    char     IntegerString[DBGX_INTEGER_BINARY_STRING_LEN_32BIT];
    char     ConcatenatedString[DBGP_LEN_OF_PROT_FRAME_ARRAY];

    DBGX_getIntegerCharacteristicsFromFormat(Format, &IntegerLen, &Signed, &Base);
    IntegerValue = DBGX_get_IntegerValue_From_ValuePointerContents(IntegerAddress, IntegerLen);
    DBGX_calc_Formatted_IntegerString(IntegerValue, IntegerLen, Signed, Base, IntegerString);

    DBGX_concatenateStrings(String, IntegerString, ConcatenatedString);

    DBGX_writeProtCharArrayFiltered(CallingFilter, ConcatenatedString, (uint16_t)strlen(ConcatenatedString));
}

void DBGX_logStrIntArr_Gen(uint32_t CallingFilter, const char *String, const void *IntArray, uint16_t NumberOfArrayElements, uint32_t Format)
{
    char ArrayString[DBGP_LEN_OF_PROT_FRAME_ARRAY];
    char ConcatenatedString[DBGP_LEN_OF_PROT_FRAME_ARRAY];

    (void)DBGX_genIntArrayString(IntArray, NumberOfArrayElements, Format, ArrayString);

    DBGX_concatenateStrings(String, ArrayString, ConcatenatedString);

    DBGX_writeProtCharArrayFiltered(CallingFilter, ConcatenatedString, (uint16_t)strlen(ConcatenatedString));
}

void DBGX_logStrStrLocat_Gen(uint32_t CallingFilter, const char *StringA, const char *StringB, const char *Function)
{
    char ConcatenatedString[DBGP_LEN_OF_PROT_FRAME_ARRAY];

    DBGX_concatenateStrings(StringA, StringB, ConcatenatedString);
    DBGX_concatenateStrings(ConcatenatedString, Function, ConcatenatedString);

    DBGX_writeProtCharArrayFiltered(CallingFilter, ConcatenatedString, (uint16_t)strlen(ConcatenatedString));
}

void DBGX_logStrStrLocatFull_Gen(uint32_t CallingFilter, const char *StringA, const char *StringB, const char *File, uint32_t LineNumber, const char *Function)
{
    char ConcatenatedString[DBGP_LEN_OF_PROT_FRAME_ARRAY];
    char SourceLocationString[DBGP_LEN_OF_PROT_FRAME_ARRAY];
    const char *Filename;

    Filename = DBGX_GetFileNameFromFilePath(File);
    (void)DBGX_genIntSourceLocationString(Filename, LineNumber, SourceLocationString);

    DBGX_concatenateStrings(StringA, StringB, ConcatenatedString);
    DBGX_concatenateStrings(ConcatenatedString, SourceLocationString, ConcatenatedString);
    DBGX_concatenateStrings(ConcatenatedString, Function, ConcatenatedString);

    DBGX_writeProtCharArrayFiltered(CallingFilter, ConcatenatedString, (uint16_t)strlen(ConcatenatedString));
}

void DBGX_logStrIntLocat_Gen(uint32_t CallingFilter, const char *String, const void *IntegerAddress, uint32_t Format, const char *Function)
{
    uint8_t  IntegerLen;
    uint8_t  Signed;
    uint8_t  Base;
    uint32_t IntegerValue;
    char     IntegerString[DBGX_INTEGER_BINARY_STRING_LEN_32BIT];
    char     ConcatenatedString[DBGP_LEN_OF_PROT_FRAME_ARRAY];
    const char *space = " ";

    DBGX_getIntegerCharacteristicsFromFormat(Format, &IntegerLen, &Signed, &Base);
    IntegerValue = DBGX_get_IntegerValue_From_ValuePointerContents(IntegerAddress, IntegerLen);
    DBGX_calc_Formatted_IntegerString(IntegerValue, IntegerLen, Signed, Base, IntegerString);

    DBGX_concatenateStrings(String, IntegerString, ConcatenatedString);
    DBGX_concatenateStrings(ConcatenatedString, space, ConcatenatedString);
    DBGX_concatenateStrings(ConcatenatedString, Function, ConcatenatedString);

    DBGX_writeProtCharArrayFiltered(CallingFilter, ConcatenatedString, (uint16_t)strlen(ConcatenatedString));
}

void DBGX_logStrIntLocatFull_Gen(uint32_t CallingFilter, const char *String, const void *IntegerAddress, uint32_t Format, const char *File, uint32_t LineNumber, const char *Function)
{
    uint8_t  IntegerLen;
    uint8_t  Signed;
    uint8_t  Base;
    uint32_t IntegerValue;
    char     IntegerString[DBGX_INTEGER_BINARY_STRING_LEN_32BIT];
    char     SourceLocationString[DBGP_LEN_OF_PROT_FRAME_ARRAY];
    char     ConcatenatedString[DBGP_LEN_OF_PROT_FRAME_ARRAY];
    const char *space = " ";
    const char *Filename;

    DBGX_getIntegerCharacteristicsFromFormat(Format, &IntegerLen, &Signed, &Base);
    IntegerValue = DBGX_get_IntegerValue_From_ValuePointerContents(IntegerAddress, IntegerLen);
    DBGX_calc_Formatted_IntegerString(IntegerValue, IntegerLen, Signed, Base, IntegerString);

    Filename = DBGX_GetFileNameFromFilePath(File);
    (void)DBGX_genIntSourceLocationString(Filename, LineNumber, SourceLocationString);

    DBGX_concatenateStrings(String, IntegerString, ConcatenatedString);
    DBGX_concatenateStrings(ConcatenatedString, space, ConcatenatedString);
    DBGX_concatenateStrings(ConcatenatedString, SourceLocationString, ConcatenatedString);
    DBGX_concatenateStrings(ConcatenatedString, Function, ConcatenatedString);

    DBGX_writeProtCharArrayFiltered(CallingFilter, ConcatenatedString, (uint16_t)strlen(ConcatenatedString));
}

void DBGX_logChar_NonFiltered_Gen(char Char)
{
    char CharArray[sizeof(char)];

    CharArray[0] = Char;
    STS_logString("DBGX_logChar_Gen");
    DBGX_writeProtCharArrayNonFiltered(CharArray, (uint16_t)sizeof(char));
}

void DBGX_logStr_NonFiltered_Gen(const char *String)
{
    DBGX_writeProtCharArrayNonFiltered(String, (uint16_t)strlen(String));
}

void DBGX_logInt_NonFiltered_Gen(const void *IntegerAddress, uint32_t Format)
{
    uint8_t  IntegerLen;
    uint8_t  Signed;
    uint8_t  Base;
    uint32_t IntegerValue;
    char     IntegerString[DBGX_INTEGER_BINARY_STRING_LEN_32BIT];

    DBGX_getIntegerCharacteristicsFromFormat(Format, &IntegerLen, &Signed, &Base);
    IntegerValue = DBGX_get_IntegerValue_From_ValuePointerContents(IntegerAddress, IntegerLen);
    DBGX_calc_Formatted_IntegerString(IntegerValue, IntegerLen, Signed, Base, IntegerString);

    DBGX_writeProtCharArrayNonFiltered(IntegerString, (uint16_t)strlen(IntegerString));
}

void DBGX_logIntArr_NonFiltered_Gen(const void *IntArray, uint16_t NumberOfArrayElements, uint32_t Format)
{
    char     ArrayString[DBGP_LEN_OF_PROT_FRAME_ARRAY];
    uint16_t ArrayStringLen;

    ArrayStringLen = DBGX_genIntArrayString(IntArray, NumberOfArrayElements, Format, ArrayString);
    DBGX_writeProtCharArrayNonFiltered(ArrayString, ArrayStringLen);
}

void DBGX_logTime_NonFiltered_Gen(void)
{
    uint32_t TimeInMicroseconds;

    TimeInMicroseconds = (uint32_t)TIM_u32GetCircleMicroSeconds();
    DBGX_logInt_NonFiltered_Gen(&TimeInMicroseconds, DBGX_FORMAT_INTLEN_32 | DBGX_FORMAT_UNSIGNED | DBGX_FORMAT_BASE_10);
}

void DBGX_logLocat_NonFiltered_Gen(const char *File, uint32_t LineNumber)
{
    char     SourceLocationString[DBGP_LEN_OF_PROT_FRAME_ARRAY];
    uint16_t SourceLocationStringLen;

    SourceLocationStringLen = DBGX_genIntSourceLocationString(File, LineNumber, SourceLocationString);
    DBGX_writeProtCharArrayNonFiltered(SourceLocationString, SourceLocationStringLen);
}

void DBGX_logStrStr_NonFiltered_Gen(const char *StringA, const char *StringB)
{
    char ConcatenatedString[DBGP_LEN_OF_PROT_FRAME_ARRAY];
    DBGX_concatenateStrings(StringA, StringB, ConcatenatedString);

    DBGX_writeProtCharArrayNonFiltered(ConcatenatedString, (uint16_t)strlen(ConcatenatedString));
}

void DBGX_logStrInt_NonFiltered_Gen(const char *String, const void *IntegerAddress, uint32_t Format)
{
    uint8_t  IntegerLen;
    uint8_t  Signed;
    uint8_t  Base;
    uint32_t IntegerValue;
    char     IntegerString[DBGX_INTEGER_BINARY_STRING_LEN_32BIT];
    char     ConcatenatedString[DBGP_LEN_OF_PROT_FRAME_ARRAY];

    DBGX_getIntegerCharacteristicsFromFormat(Format, &IntegerLen, &Signed, &Base);
    IntegerValue = DBGX_get_IntegerValue_From_ValuePointerContents(IntegerAddress, IntegerLen);
    DBGX_calc_Formatted_IntegerString(IntegerValue, IntegerLen, Signed, Base, IntegerString);

    DBGX_concatenateStrings(String, IntegerString, ConcatenatedString);

    DBGX_writeProtCharArrayNonFiltered(ConcatenatedString, (uint16_t)strlen(ConcatenatedString));
}

void DBGX_logStrIntArr_NonFiltered_Gen(const char *String, const void *IntArray, uint16_t NumberOfArrayElements, uint32_t Format)
{
    char ArrayString[DBGP_LEN_OF_PROT_FRAME_ARRAY];
    char ConcatenatedString[DBGP_LEN_OF_PROT_FRAME_ARRAY];

    (void)DBGX_genIntArrayString(IntArray, NumberOfArrayElements, Format, ArrayString);

    DBGX_concatenateStrings(String, ArrayString, ConcatenatedString);

    DBGX_writeProtCharArrayNonFiltered(ConcatenatedString, (uint16_t)strlen(ConcatenatedString));
}

void DBGX_logStrStrLocat_NonFiltered_Gen(const char *StringA, const char *StringB, const char *Function)
{
    char ConcatenatedString[DBGP_LEN_OF_PROT_FRAME_ARRAY];

    DBGX_concatenateStrings(StringA, StringB, ConcatenatedString);
    DBGX_concatenateStrings(ConcatenatedString, Function, ConcatenatedString);

    DBGX_writeProtCharArrayNonFiltered(ConcatenatedString, (uint16_t)strlen(ConcatenatedString));
}

void DBGX_logStrStrLocatFull_NonFiltered_Gen(const char *StringA, const char *StringB, const char *File, uint32_t LineNumber, const char *Function)
{
    char ConcatenatedString[DBGP_LEN_OF_PROT_FRAME_ARRAY];
    char SourceLocationString[DBGP_LEN_OF_PROT_FRAME_ARRAY];
    const char *Filename;

    Filename = DBGX_GetFileNameFromFilePath(File);
    (void)DBGX_genIntSourceLocationString(Filename, LineNumber, SourceLocationString);

    DBGX_concatenateStrings(StringA, StringB, ConcatenatedString);
    DBGX_concatenateStrings(ConcatenatedString, SourceLocationString, ConcatenatedString);
    DBGX_concatenateStrings(ConcatenatedString, Function, ConcatenatedString);

    DBGX_writeProtCharArrayNonFiltered(ConcatenatedString, (uint16_t)strlen(ConcatenatedString));
}

void DBGX_logStrIntLocat_NonFiltered_Gen(const char *String, const void *IntegerAddress, uint32_t Format, const char *Function)
{
    uint8_t  IntegerLen;
    uint8_t  Signed;
    uint8_t  Base;
    uint32_t IntegerValue;
    char     IntegerString[DBGX_INTEGER_BINARY_STRING_LEN_32BIT];
    char     ConcatenatedString[DBGP_LEN_OF_PROT_FRAME_ARRAY];
    const char *space = " ";

    DBGX_getIntegerCharacteristicsFromFormat(Format, &IntegerLen, &Signed, &Base);
    IntegerValue = DBGX_get_IntegerValue_From_ValuePointerContents(IntegerAddress, IntegerLen);
    DBGX_calc_Formatted_IntegerString(IntegerValue, IntegerLen, Signed, Base, IntegerString);

    DBGX_concatenateStrings(String, IntegerString, ConcatenatedString);
    DBGX_concatenateStrings(ConcatenatedString, space, ConcatenatedString);
    DBGX_concatenateStrings(ConcatenatedString, Function, ConcatenatedString);

    DBGX_writeProtCharArrayNonFiltered(ConcatenatedString, (uint16_t)strlen(ConcatenatedString));
}

void DBGX_logStrIntLocatFull_NonFiltered_Gen(const char *String, const void *IntegerAddress, uint32_t Format, const char *File, uint32_t LineNumber, const char *Function)
{
    uint8_t  IntegerLen;
    uint8_t  Signed;
    uint8_t  Base;
    uint32_t IntegerValue;
    char     IntegerString[DBGX_INTEGER_BINARY_STRING_LEN_32BIT];
    char     SourceLocationString[DBGP_LEN_OF_PROT_FRAME_ARRAY];
    char     ConcatenatedString[DBGP_LEN_OF_PROT_FRAME_ARRAY];
    const char *space = " ";
    const char *Filename;

    DBGX_getIntegerCharacteristicsFromFormat(Format, &IntegerLen, &Signed, &Base);
    IntegerValue = DBGX_get_IntegerValue_From_ValuePointerContents(IntegerAddress, IntegerLen);
    DBGX_calc_Formatted_IntegerString(IntegerValue, IntegerLen, Signed, Base, IntegerString);

    Filename = DBGX_GetFileNameFromFilePath(File);
    (void)DBGX_genIntSourceLocationString(Filename, LineNumber, SourceLocationString);

    DBGX_concatenateStrings(String, IntegerString, ConcatenatedString);
    DBGX_concatenateStrings(ConcatenatedString, space, ConcatenatedString);
    DBGX_concatenateStrings(ConcatenatedString, SourceLocationString, ConcatenatedString);
    DBGX_concatenateStrings(ConcatenatedString, Function, ConcatenatedString);

    DBGX_writeProtCharArrayNonFiltered(ConcatenatedString, (uint16_t)strlen(ConcatenatedString));
}

int32_t DBGX_getString_Gen(char *String, uint16_t StringLen)
{
    return DBGP_getProtString(String, StringLen);
}

void DBGX_getString_CB(uint8_t *Data, uint16_t DataLen)
{
    for(uint8_t index = 0; index < *DBGX_NumberOfInputCallbackFunctions; index++)
    {
        if(index < DBGX_MaxNumberOfInputCallbackFunctions)
        {
            DBGX_InputCallbackFunctions_Gen[index](Data, DataLen);
        }
        else
        {
            break;
        }
    }
}

void DBGX_setInputCallbackFunctions(const DBGX_InputCallbackFunctions_t *InputCallbackFunctionArray, const uint8_t *NumberOfInputCallbackFunctions, uint8_t MaxNumberOfInputCallbackFunctionsFunctions)
{
    DBGX_InputCallbackFunctions_Gen = InputCallbackFunctionArray;
    DBGX_NumberOfInputCallbackFunctions = NumberOfInputCallbackFunctions;
    DBGX_MaxNumberOfInputCallbackFunctions = MaxNumberOfInputCallbackFunctionsFunctions;
}

void DBGX_handleErr_Gen(uint32_t ErrorCode)
{
    DBGX_logStrInt_NonFiltered_Gen("--> Exception code (hex): ", &ErrorCode, DBGX_FORMAT_INTLEN_32 | DBGX_FORMAT_UNSIGNED | DBGX_FORMAT_BASE_16);
    DBGX_handleErrAfterErrOutput(ErrorCode, 0U);
}

void DBGX_handleErrWithParam_Gen(uint32_t ErrorCode, uint32_t ErrorParameter)
{
    DBGX_logStrInt_NonFiltered_Gen("--> Exception code (hex): ", &ErrorCode, DBGX_FORMAT_INTLEN_32 | DBGX_FORMAT_UNSIGNED | DBGX_FORMAT_BASE_16);
    DBGX_logStrInt_NonFiltered_Gen("--> parameter (hex): ", &ErrorParameter, DBGX_FORMAT_INTLEN_32 | DBGX_FORMAT_UNSIGNED | DBGX_FORMAT_BASE_16);

    DBGX_handleErrAfterErrOutput(ErrorCode, ErrorParameter);
}

void DBGX_handleErrIf_Gen(bool Condition, uint32_t ErrorCode)
{
    if (Condition)
    {
        DBGX_handleErr_Gen(ErrorCode);
    }
}

void DBGX_handleErrWithParamIf_Gen(bool Condition, uint32_t ErrorCode, uint32_t ErrorParameter)
{
    if (Condition)
    {
        DBGX_handleErrWithParam_Gen(ErrorCode, ErrorParameter);
    }
}

uint8_t DBGX_readFirstError_Gen(uint32_t *ErrorCode, uint32_t *ErrorParameter)
{
    return DBGE_readFirstError(ErrorCode, ErrorParameter);
}

uint8_t DBGX_readLastError_Gen(uint32_t *ErrorCode, uint32_t *ErrorParameter)
{
    return DBGE_readLastError(ErrorCode, ErrorParameter);
}

void DBGX_writeErrorSourceLocation_Gen(const char *File, uint32_t LineNumber)
{
    DBGX_logLocat_NonFiltered_Gen(File, LineNumber);
    DBGX_logStr_NonFiltered_Gen(": ");
}

void DBGX_writeErrorSourceLocationCond_Gen(bool Condition, const char *File, uint32_t LineNumber)
{
    if (Condition)
    {
        DBGX_logLocat_NonFiltered_Gen(File, LineNumber);
        DBGX_logStr_NonFiltered_Gen(": ");
    }
}


/******************************************************************************/
/* PRIVATE FUNCTION DEFINITIONS                                               */
/******************************************************************************/

/** \brief  Checks, whether a flag in FlagWord, addressed by FlagIdx, is set
 *
 *  \param  FlagWord  Word containing flags
 *  \param  FlagIdx   Index of the flag in the word (1 .. 32)
 *
 *  \return Info, whether the flag is set (true: set; false: not set)
 */
static bool DBGX_isFlagSet(uint32_t FlagWord, uint8_t FlagIdx)
{
    bool     FlagIsSet;
    uint32_t FlagMask;
    static const uint16_t FlagMasks[] =
        {0x0001, 0x0002, 0x0004, 0x0008, 0x0010, 0x0020, 0x0040, 0x0080,
         0x0100, 0x0200, 0x0400, 0x0800, 0x1000, 0x2000, 0x4000, 0x8000};

    FlagIdx--;        /* FlagIdx:   1 .. 32   =>   0 .. 31 */

    if (FlagIdx < 16U)
    {
        FlagMask = (uint32_t)FlagMasks[FlagIdx];
    }
    else
    {
        FlagMask = ((uint32_t)FlagMasks[FlagIdx - (uint8_t)16]) << 16;
    }

    if ((FlagWord & FlagMask) != 0U)
    {
        FlagIsSet = true;
    }
    else
    {
        FlagIsSet = false;
    }

    return FlagIsSet;
}

/** \brief  Checks, whether the debug_extended output is activated by the filter flag words
 *
 *  \param  CallingFilter  Debug_extended output data filter settings at the calling location
 *  \return Info, whether activated (false: disabled; true: activated)
 */
static bool DBGX_isDebugExtendedOutputActivated(uint32_t CallingFilter)
{
    bool     DbgOutputActivated = false;

    /* Checking, whether the flag for level or the flag for scenario is set */
    /* in DBGX_CentralConfigFilter.FilterFlagWords[] */
    /* See the description at DBGX_CentralConfigFilter_Cfg.FilterFlagWords; */
    /* DBGX_CentralConfigFilter.FilterFlagWords is derived from that. */

    /* Calculating the indexes for DBGX_CentralConfigFilter.FilterFlagWords[]: */
    const uint8_t  ScenarioFlagIdx = (uint8_t)((CallingFilter & DBGX_SCN_BIT_IDX_MASK) >> DBGX_SCN_BIT_IDX_SHIFT);
    const uint8_t  LevelFlagIdx    = (uint8_t)(CallingFilter >> DBGX_LEV_BIT_IDX_SHIFT);
    const uint16_t ScenarioWordIdx = (uint16_t)(CallingFilter & DBGX_SCN_WORD_IDX_MASK);
                /* LevelWordIdx     is always      0              */

    /* Calculating DbgOutputActivated: */
    if (LevelFlagIdx != 0U)
    {
        /* Checking for level flag is centrally activated for the calling: */
        if (DBGX_isFlagSet(DBGX_CentralConfigFilter.FilterFlagWords[0], LevelFlagIdx))
        {
            DbgOutputActivated = true;
            STS_logString("Output activated from level");
        }
    }

    if ((!DbgOutputActivated) && (ScenarioFlagIdx != 0U) &&
        (ScenarioWordIdx < DBGX_CentralConfigFilter.NumberFilterFlagWords))
    {
        /* Checking for scenario flag is centrally activated for the calling: */
        if (DBGX_isFlagSet(DBGX_CentralConfigFilter.FilterFlagWords[ScenarioWordIdx], ScenarioFlagIdx))
        {
            DbgOutputActivated = true;
            STS_logString("Output activated from scenario");
        }
    }

    return DbgOutputActivated;
}


/** \brief  Prepares the string of debug_extended attributes; fills in the module ID
 *  \note   The string of debug_extended attributes contains the IDs of level,
 *          scenario and module and further information to be put out
 *          besides the essential debug_extended data
 *
 *  \param  Module_ID      ID of the module (e.g touch), needed for the PC GUI tool
 *  \param  DbgAttributes  Pointer to the string of indexes of level, scenario
 *                         module ID and other attributes needed for putting out
 *                         onto the debug_extended interface besides the debug_extended data
 */
static void DBGX_prepareDbgAttributesString(uint8_t Module_ID, char *DbgAttributes)
{
    DBGX_convModuleToString(Module_ID, DbgAttributes);
}

/** \brief  Sends the data of the application protocol (e.g. touch data) via
 *          the debug_extended interface, non-filtered
 *
 *  \param  CallingFilter  Debug_extended output data filter settings at the calling location
 *  \param  Module_ID      ID of the module (e.g touch), needed for the PC GUI tool
 *  \param  Function       Function of the debug_extended output data, needed for the PC GUI tool
 *  \param  Data           Pointer to payload (user data) of debug_extended output data
 *                         for the PC GUI tool
 *  \param  DataLen        Number of bytes of payload (user data) of debug_extended
 *                         output data for the PC GUI tool
 *  \return Error status (==0: no error; < 0: error)
 */
static int32_t DBGX_sendDataUnconditionally(uint32_t CallingFilter, uint16_t Module_ID,
                                            uint16_t Function, const uint8_t *Data, uint16_t DataLen)
{
    int32_t ErrorStatus;
    char DbgAttributes[DBGP_LEN_ATTRIBUTES_STRING];

    DBGX_prepareDbgAttributesString((uint8_t)Module_ID, DbgAttributes);
    DBGX_completeDbgAttributesString((bool)true, CallingFilter, DbgAttributes);
    ErrorStatus = DBGP_sendProtData(DbgAttributes, Module_ID, Function, Data, DataLen);
    return ErrorStatus;
}

/** \brief  Gets the index of a module ID
 *
 *  \param  Module_ID  ID of the module (e.g touch), needed for the PC GUI tool
 *  \return Index of the module ID
 */
static uint32_t DBGX_getModuleIndex(uint16_t Module_ID)
{
    uint32_t ModuleIndex;

    for(ModuleIndex = 0U ; ModuleIndex < DBGX_ApiCallbacks.MaxNumberModules_Rx ; ModuleIndex++)
    {
        if (DBGX_ApiCallbacks.ModuleIDs[ModuleIndex] == Module_ID)
        {
            break;
        }
    }

    if (ModuleIndex >= DBGX_ApiCallbacks.MaxNumberModules_Rx)
    {
        ModuleIndex = DBGX_NOT_DEFINED_MODULE_IDX;
        DBGE_saveError(DBGX_ERR_NDEF_MODULE_IDX);
    }

    return ModuleIndex;
}

/** \brief  Receives the data of the application protocol (e.g. touch data)
 *          via the debug_extended interface, callback function
 *  \note   Is called in DBGP via function pointer
 *  \note   Calls the correct function pointer in the array pointed by
 *          DBGX_ApiCallbacks.ReceiveDataFunctionsOfApi_CB[]
 *
 *  \param  Module_ID  ID of the module (e.g touch), needed for the PC GUI tool
 *  \param  Function   Function of the debug_extended output data, needed for the PC GUI tool
 *  \param  Data       Pointer to payload (user data) of debug_extended output data
 *                     for the PC GUI tool
 *  \param  DataLen    Number of bytes of payload (user data) of debug_extended
 *                     output data for the PC GUI tool
 */
STATIC_DBGX void DBGX_receiveProtData_CB(uint16_t Module_ID, uint16_t Function, uint8_t *Data, uint16_t DataLen)
{
    uint32_t ModuleIndex;

    STS_logStringAndU16Hex("Module_ID: ", Module_ID); 
    STS_logStringAndU16Hex("Function:  ", Function); 
    STS_logStringAndU8ArrayHex("Data[]:    ", Data, DataLen); 
    STS_logStringAndU16Hex("DataLen:   ", DataLen); 

    ModuleIndex = DBGX_getModuleIndex(Module_ID);

    STS_logStringAndU32Hex("ModuleIndex:   ", ModuleIndex); 

    if (ModuleIndex != DBGX_NOT_DEFINED_MODULE_IDX)
    {
        ///STS_logStringAndU32Hex("DBGX_ApiCallbacks.ReceiveDataFunctionsOfApi_CB[ModuleIndex]: ", (uint32_t)DBGX_ApiCallbacks.ReceiveDataFunctionsOfApi_CB[ModuleIndex]);
                     /* The physical addresses can move; for that testing output only for special cases */

        DBGX_ApiCallbacks.ReceiveDataFunctionsOfApi_CB[ModuleIndex](Function, Data, DataLen);
    }
}

/** \brief  Analyzes the format element: Gets integer value length,
 *          signed / unsigned and integer value base
 *
 *  \param  Format      Format of the integer value, see the definitions of DBGX_FORMAT_...
 *  \param  IntegerLen  Pointer to length of integer value (1, 2, 4 bytes)
 *  \param  Signed      Pointer to info, whether the integer value signed or unsigned
 *  \param  Base        Pointer to the base of integer value (2 for binary,
 *                      8 for octal, 10 for decimal, 16 for hexadecimal)
 */
STATIC_DBGX void DBGX_getIntegerCharacteristicsFromFormat(uint32_t Format, uint8_t *IntegerLen, uint8_t *Signed, uint8_t *Base)
{
    *IntegerLen = (uint8_t)((Format & DBGX_FORMAT_INTLEN_MASK) >> (2 * DBGX_BITS_PER_BYTE));
    *Signed     = (uint8_t)((Format & DBGX_FORMAT_SIGNED_MASK) >> (1 * DBGX_BITS_PER_BYTE));
    *Base       = (uint8_t)((Format & DBGX_FORMAT_BASE_MASK)                           );
}

/** \brief  Analyzes the format element: Gets the integer value length
 *
 *  \param  Format  Format of the integer value, see the definitions of DBGX_FORMAT_...
 *  \return Integer value length (1, 2, 4 bytes)
 */
static uint8_t DBGX_getIntegerLenFromFormat(uint32_t Format)
{
    uint8_t IntegerLen;

    IntegerLen = (uint8_t)((Format & DBGX_FORMAT_INTLEN_MASK) >> (2 * DBGX_BITS_PER_BYTE));
    return IntegerLen;
}

/** \brief  Gets the integer value from the integer value pointer (taking care of the endianess)
 *
 *  \param  ValuePointer  Pointer to the integer value
 *  \param  IntegerLen    Number of bytes of the integer value
 *  \return Integer value with the right length (1, 2, 4 bytes) and endianess
 */
STATIC_DBGX uint32_t DBGX_get_IntegerValue_From_ValuePointerContents(const void *ValuePointer, uint8_t IntegerLen)
{
    uint32_t IntegerValue;
    const uint8_t  *PointerTo8Bit;
    const uint16_t  *PointerTo16Bit;
    const uint32_t  *PointerTo32Bit;

    IntegerValue = 0U;
    switch (IntegerLen)
    {
        //Axivion Disable Style MisraC2012-11.5: Is tested as OK - pointer alignment fits to the type of variable. Careful usage for debug_extended sessions only. @@Could be handled in future by a central pointer value copy function of the whole framework, as soon as available.
        case (uint8_t)DBGX_FORMAT_INTLEN_8_NON_SHIFT:
            PointerTo8Bit = ValuePointer;
            IntegerValue  = (uint32_t)(*PointerTo8Bit);
            break;

        case (uint8_t)DBGX_FORMAT_INTLEN_16_NON_SHIFT:
            PointerTo16Bit = ValuePointer;
            IntegerValue   = (uint32_t)(*PointerTo16Bit);
            break;

        case (uint8_t)DBGX_FORMAT_INTLEN_32_NON_SHIFT:
        default:
            PointerTo32Bit = ValuePointer;
            IntegerValue   = (uint32_t)(*PointerTo32Bit);
            break;
        //Axivion Enable Style MisraC2012-11.5
    }

    return IntegerValue;
}

/** \brief  Calculates the formatted integer string from the integer value
 *
 *  \param  IntegerValue   Integer value
 *  \param  IntegerLen     Number of bytes of the integer value (1, 2, 4)
 *  \param  Signed         Info, whether the format is signed or unsigned
 *                         != DBGX_FORMAT_SIGNED_NON_SHIFT: unsigned
 *                         == DBGX_FORMAT_SIGNED_NON_SHIFT: signed
 *                         no boolean because taken from an integer with mask
 *  \param  Base           Base of the format (2, 8, 10, 16)
 *  \param  IntegerString  Pointer to the string of formatted integer value
 */
STATIC_DBGX void DBGX_calc_Formatted_IntegerString(uint32_t IntegerValue, uint8_t IntegerLen, uint8_t Signed, uint8_t Base, char *IntegerString)
{
    const uint32_t MaxPositiveInteger = ((uint32_t)0x00000001 << (((uint32_t)IntegerLen * (uint32_t)DBGX_BITS_PER_BYTE) - (uint32_t)1U)) - 1U;
    const uint32_t MaxUnsignedInteger = ((uint32_t)2U * MaxPositiveInteger) + (uint32_t)1U;

    uint32_t PositiveIntegerValueForCalc;
    uint8_t  IntegerStringIndex;

    if (Signed != (uint8_t)DBGX_FORMAT_SIGNED_NON_SHIFT)
    {
        PositiveIntegerValueForCalc = IntegerValue;
        IntegerStringIndex          = 0U;
    }
    else
    {
        if (IntegerValue > MaxPositiveInteger)
        {
            PositiveIntegerValueForCalc = (MaxUnsignedInteger - IntegerValue) + 1U;
            IntegerStringIndex          = 1U;
            IntegerString[0]            = '-';
        }
        else
        {
            PositiveIntegerValueForCalc = IntegerValue;
            IntegerStringIndex          = 0U;
        }
    }

    IntegerString[IntegerStringIndex] = '\0';
    if (Base >= (uint8_t)2)   /* Prevents endless loop in DBGX_calc_Formatted_PositiveIntegerStringPart */
    {
        DBGX_calc_Formatted_PositiveIntegerStringPart(PositiveIntegerValueForCalc, Base, IntegerStringIndex, IntegerString);
    }
}

/** \brief  Calculates the formatted positive integer string part from the integer value
 *
 *  \param  PositiveIntegerValueForCalc  Positive integer value for calculating the formatted
 *                                       positive integer string part
 *  \param  Base                         Base of the format (2, 8, 10, 16)
 *  \param  IntegerStringIndex           String index for the positive integer string part
 *                                       (0 or 1; negative sign can be at index 0)
 *  \param  IntegerString                Pointer to the string of formatted integer value
 */
STATIC_DBGX void DBGX_calc_Formatted_PositiveIntegerStringPart(uint32_t PositiveIntegerValueForCalc, uint8_t Base, uint8_t IntegerStringIndex, char *IntegerString)
{
    uint32_t IntegerValue = PositiveIntegerValueForCalc;
    uint32_t Divider = 1U;
    uint8_t  Cipher;

    while ((Divider * Base) <= IntegerValue)
    {
        STS_logStringAndU32Hex("1: Divider = ", Divider);

        if (((Divider * Base) / Base) != Divider)  /* If divider overflow (32 bit) */
        {
            /* Divider is fine, because IntegerValue has also 32 bit only */
            STS_logStringAndU32Hex("2: Divider = ", Divider);
            break;
        }

        Divider = Divider * Base;
    }

    STS_logStringAndU32Hex("IntegerValue = ", IntegerValue); 
    STS_logStringAndU32Hex("Divider      = ", Divider); 

    while (Divider > 0U)
    {
        Cipher = (uint8_t)(IntegerValue / Divider);
        IntegerValue = IntegerValue % Divider;
        STS_logStringAndU8Hex("Cipher = ", Cipher);
        STS_logStringAndU32Hex("IntegerValue = ", IntegerValue);
        STS_logStringAndU32Hex("Divider = ", Divider);

        IntegerString[IntegerStringIndex] = DBGX_convertCipherToCharacter(Cipher);
        IntegerStringIndex++;

        Divider = Divider / Base;
    }
    IntegerString[IntegerStringIndex] = '\0';
}

/** \brief  Converts a cipher to a character
 *
 *  \param  Cipher  Cipher:  0,   1,   2, ...,   9,  10, ...,  15
 *  \return Character; the cipher is converted to: '0', '1', '2', ..., '9', 'A', ..., 'F'
 */
STATIC_DBGX char DBGX_convertCipherToCharacter(uint8_t Cipher)
{
    char Character;
    uint8_t Result;

    if (Cipher <= (uint8_t)0x09U)
    {
        Result = Cipher + (uint8_t)0x30U;
        Character = (char)Result;
    }
    else if (Cipher <= (uint8_t)0x0FU)
    {
        Result = (Cipher - (uint8_t)0x0AU) + (uint8_t)0x41U;
        Character = (char)Result;
    }
    else
    {
        Character = '?';
    }

    return Character;
}

/** \brief  Sends a character array via the debug_extended interface, filtered; at the interface to the protocol layer
 *
 *  \param  CallingFilter          Debug_extended output data filter settings at the calling location
 *  \param  CharArray              Pointer to the character array
 *  \param  NumberOfArrayElements  Number of elements in the array
 */
static void DBGX_writeProtCharArrayFiltered(uint32_t CallingFilter, const char *CharArray, uint16_t NumberOfArrayElements)
{
    int32_t ErrorStatus;
    char DbgAttributes[DBGP_LEN_ATTRIBUTES_STRING];

    STS_logString("DBGX_writeProtCharArrayFiltered");
    if (DBGX_isDebugExtendedOutputActivated(CallingFilter))
    {
        STS_logString("DBGX_writeProtCharArrayFiltered: if");

        DBGX_completeDbgAttributesString((bool)true, CallingFilter, DbgAttributes);
        ErrorStatus = DBGP_writeProtCharArray(DbgAttributes, CharArray, NumberOfArrayElements);
        if (ErrorStatus != DBGX_STAT_NO_ERROR)
        {
            DBGE_saveErrorWithParam(DBGX_ERR_PRT_ARRAY_FILTERED, (uint32_t)ErrorStatus);
        }
    }
}

/** \brief  Sends a character array via the debug_extended interface, non-filtered;
 *          at the interface to the protocol layer
 *
 *  \param  CharArray              Pointer to the character array
 *  \param  NumberOfArrayElements  Number of elements in the array
 */
static void DBGX_writeProtCharArrayNonFiltered(const char *CharArray, uint16_t NumberOfArrayElements)
{
    int32_t ErrorStatus;
    char DbgAttributes[DBGP_LEN_ATTRIBUTES_STRING];

    STS_logString("DBGX_writeProtCharArrayNonFiltered");

    DBGX_completeDbgAttributesString((bool)false, (uint32_t)0, DbgAttributes);
    ErrorStatus = DBGP_writeProtCharArray(DbgAttributes, CharArray, NumberOfArrayElements);
    if (ErrorStatus != DBGX_STAT_NO_ERROR)
    {
        DBGE_saveErrorWithParam(DBGX_ERR_PRT_ARRAY_NON_FILTERED, (uint32_t)ErrorStatus);
    }
}

/** \brief  Handles an exceptional error (error saving and error solving
 *          call) after its output on the debug_extended interface
 *
 *  \param  ErrorCode       Exceptional error code
 *  \param  ErrorParameter  Parameter of the error code (e.g. a return value of a library function)
 */
static void DBGX_handleErrAfterErrOutput(uint32_t ErrorCode, uint32_t ErrorParameter)
{
    uint32_t ErrorSolvingID;

    DBGX_ErrorCodeForDebugger      = ErrorCode;               /* For inspection on debugger */
    DBGX_ErrorParameterForDebugger = ErrorParameter;          /* ~                          */

    /* For inspection on debugger: set a breakpoint hereunder: */

    DBGE_saveErrorWithParam(ErrorCode, ErrorParameter);
    ErrorSolvingID = ErrorCode & DBGX_ERR_SOLVING_ID_MASK;

    switch (ErrorSolvingID)
    {
        case DBGX_ERR_SOLVING_CALLBACK:
            STS_logString("Error solving via callback"); 
            DBGE_solveExceptionalError(ErrorCode, ErrorParameter);
            break;

        case DBGX_ERR_SOLVING_RESET:
            STS_logString("Error solving via reset"); 
            HSUP_vGenerateReset();
            break;

        case DBGX_ERR_SOLVING_NONE:
        default:
            STS_logString("No error solving"); 
            break;
    }
}


/** \brief  Converts the module ID to a module string
 *
 *  \param  Module  ID of module of debug_extended data (e.g touch), needed for the PC GUI tool
 *  \param  String  Pointer to the string containing the ID of module
 */
static void DBGX_convModuleToString(uint8_t Module, char *String)
{
    const uint8_t Index = (uint8_t)(Module * DBGX_LenOfModuleAbbreviation);

    if (Index < DBGX_NumberOfModuleAbbreviations)
    {
        String[0] = DBGX_ModuleAbbreviations[Index];
        String[1] = DBGX_ModuleAbbreviations[Index + (uint8_t)1];
    }
    else
    {   
        String[0] = (char)'!';
        String[1] = (char)'!';
    }
}

/** \brief  Completes the string of debug_extended attributes; fills in the level ID,
 *          scenario ID and the system timestamp
 *  \note   The string of debug_extended attributes contains the IDs of level, scenario and module and
 *          further information to be put out besides the essential debug_extended data
 *
 *  \param  FilteredFunction  Information, if the function is filtered or not. Filtered functions have more
 *                            detailed DbgAttributes than non-filtered functions
 *  \param  CallingFilter     Debug_extended output data filter settings at the calling location
 *  \param  DbgAttributes     Pointer to the string of debug_extended attributes, needed
 *                            for putting out onto the debug_extended interface besides the debug_extended data
 */
static void DBGX_completeDbgAttributesString(bool FilteredFunction, uint32_t CallingFilter, char *DbgAttributes)
{
    if(DBGX_DbgAttributesActivated){
        // Module abbreviation
        char     ModuleString[DBGX_ATTRIBUTE_MODULE_STRING_LEN] = {'.', '.'};

        // Level
        uint8_t  Level;
        uint32_t Scenario;  // signals the scenario, e.g. Power, Touch, ... in hexadecimal
        char     LevelChar[DBGX_ATTRIBUTE_LEVEL_STRING_LEN + (uint8_t)1] = "."; // Increase array length for using it as string

        // Scenario
        char     IntegerString[DBGX_ATTRIBUTE_SCENARIO_STRING_LEN] = {'.', '.', '.', '.'};

        // System time
        uint32_t TimeInMicroseconds;
        uint32_t IntegerValue;
        uint32_t Format;
        uint8_t  IntegerLen;
        uint8_t  Signed;
        uint8_t  Base;
        char     TimeIntegerString[DBGX_ATTRIBUTE_TIME_STRING_LEN] = {'.', '.', '.', '.', '.', '.', '.', '.'};

        // Concatenate the module
        if(FilteredFunction)
        {
            ModuleString[0] = 'T';
            ModuleString[1] = 'T';
        }
        else
        {
            ModuleString[0] = 't';
            ModuleString[1] = 'T';
        }
        DBGX_writeStringRightAligned(ModuleString, (DBGX_ATTRIBUTE_MODULE_STRING_LEN), DBGX_ATTRIBUTE_MODULE_STRING_POSITION, DbgAttributes);

        // Concatenate the level
        if(FilteredFunction)
        {
            Level = DBGX_getLevelFromFilter(CallingFilter);
            LevelChar[0] = DBGX_convLevelToLevelChar(Level);
        }
        DBGX_writeStringRightAligned(LevelChar, (DBGX_ATTRIBUTE_LEVEL_STRING_LEN), DBGX_ATTRIBUTE_LEVEL_STRING_POSITION, DbgAttributes);

        // Concatenate the scenario
        if(FilteredFunction)
        {
            Scenario  = DBGX_getScenarioFromFilter(CallingFilter);
            DBGX_calc_Formatted_PositiveIntegerStringPart(Scenario, (uint8_t)DBGX_FORMAT_BASE_16_NON_SHIFT, 0U, IntegerString);
        }
        DBGX_writeStringRightAligned(IntegerString, DBGX_ATTRIBUTE_SCENARIO_STRING_LEN, DBGX_ATTRIBUTE_SCENARIO_STRING_POSITION, DbgAttributes);

        // Concatenate system time
        TimeInMicroseconds = (uint32_t)TIM_u32GetCircleMicroSeconds();
        Format = (DBGX_FORMAT_UNSIGNED | DBGX_FORMAT_INTLEN_32 | DBGX_FORMAT_BASE_16);
        DBGX_getIntegerCharacteristicsFromFormat(Format, &IntegerLen, &Signed, &Base);
        IntegerValue = DBGX_get_IntegerValue_From_ValuePointerContents(&TimeInMicroseconds, IntegerLen);
        DBGX_calc_Formatted_IntegerString(IntegerValue, IntegerLen, Signed, Base, TimeIntegerString);
        DBGX_writeStringRightAligned(TimeIntegerString, DBGX_ATTRIBUTE_TIME_STRING_LEN, DBGX_ATTRIBUTE_TIME_STRING_POSITION, DbgAttributes);
    }
}


/** \brief  Generates the integer array string for having an atomic (reentrant) integer string debug_extended output
 *
 *  \param  IntArray               Pointer to the integer array values to be sent
 *  \param  NumberOfArrayElements  Number of elements in the array
 *  \param  Format                 Format of the integer value, see the definitions of DBGX_FORMAT_...
 *  \param  ArrayString            Output string containing the integer array
 *  \return Number of characters in the integer array string
 */
static uint16_t DBGX_genIntArrayString(const void *IntArray, uint16_t NumberOfArrayElements, uint32_t Format, char *ArrayString)
{
    uint8_t  IntegerLen;
    uint8_t  Signed;
    uint8_t  Base;
    uint32_t IntegerValue;
    char     IntegerString[DBGX_INTEGER_BINARY_STRING_LEN_32BIT];
    uint8_t  IntegerStringLen;

    char     *ArrayStringPtr      = ArrayString;
    uint16_t ArrayStringIndex     = 0;
    uint16_t ArrayStringLen       = 0;
    uint16_t NextArrayStringIndex = 0;
    uint16_t ArrayElementIndex    = 0;
    uint16_t ArrayElementCounter;

    DBGX_getIntegerCharacteristicsFromFormat(Format, &IntegerLen, &Signed, &Base);

    for (ArrayElementCounter = 0U; ArrayElementCounter < NumberOfArrayElements; ArrayElementCounter++)
    {
        //Axivion Disable Style MisraC2012-11.5: Is tested as OK - pointer alignment fits to the type of variable. Careful usage for debug_extended sessions only. @@Could be handled in future by a central pointer value copy function of the whole framework, as soon as available.
        IntegerValue = DBGX_get_IntegerValue_From_ValuePointerContents
                                    (((const uint8_t *)IntArray) + ArrayElementIndex, IntegerLen);
        //Axivion Enable Style MisraC2012-11.5

        DBGX_calc_Formatted_IntegerString(IntegerValue, IntegerLen, Signed, Base, IntegerString);
        IntegerStringLen = (uint8_t)strlen(IntegerString);
        NextArrayStringIndex = (uint16_t)((uint32_t)ArrayStringIndex + (uint32_t)IntegerStringLen 
                               + (uint32_t)sizeof((char)' '));    //lint !e866 Unusual use of ' ' in size of - tested ok
        if (NextArrayStringIndex <= (uint16_t)DBGP_LEN_OF_PROT_FRAME_ARRAY)
        {
            (void)memcpy((void *)ArrayStringPtr, (const void *)IntegerString, (size_t)IntegerStringLen); //lint !e920 cast from pointer to void is OK: return value is not used
            ArrayStringPtr = ArrayStringPtr + IntegerStringLen;
            *ArrayStringPtr = ' ';
            ArrayStringPtr = ArrayStringPtr + sizeof((char)' ');   //lint !e866 Unusual use of ' ' in size of - tested ok
            ArrayStringLen = NextArrayStringIndex;
        }
        else
        {
            if (ArrayElementCounter != (uint16_t)0)
            {
                ArrayStringPtr = ArrayStringPtr - sizeof((char)' ');    //lint !e866 Unusual use of ' ' in size of - tested ok
            }
            else
            {
                ArrayStringLen = (uint16_t)sizeof((char)'!');    //lint !e866 Unusual use of ' ' in size of - tested ok
            }
            *ArrayStringPtr = '!';
            ArrayStringPtr = ArrayStringPtr + (uint16_t)sizeof((char)'\0');

            break;
        }
        ArrayStringIndex  = NextArrayStringIndex;
        ArrayElementIndex = ArrayElementIndex + DBGX_getIntegerLenFromFormat(Format);
    }

    *ArrayStringPtr = '\0';

    return ArrayStringLen;
}

/** \brief  Generates the source location string for having an atomic (reentrant) source location debug_extended output
 *
 *  \param  FileString            String of the file of source location
 *  \param  LineNumber            Value of the line number of source location
 *  \param  SourceLocationString  Pointer to the output string containing the source location (file and line number)
 *  \return Number of characters in the source location string
 */
static uint16_t DBGX_genIntSourceLocationString(const char *FileString, uint32_t LineNumber, char *SourceLocationString)
{
    uint16_t StringLenFile;
    uint16_t StringLenLine;
    uint16_t StringLenSourceLocation;
    char     LineString[DBGX_INTEGER_DECIMAL_STRING_LEN_32BIT];
    const char *Space = " ";

    DBGX_calc_Formatted_PositiveIntegerStringPart(LineNumber,
              (uint8_t)DBGX_FORMAT_BASE_10_NON_SHIFT, 0U, LineString);

    StringLenFile = (uint16_t)strlen(FileString);    /* FileString is zero-terminated */
    StringLenLine = (uint16_t)strlen(LineString);    /* LineString is zero-terminated */
    StringLenSourceLocation = (uint16_t)StringLenFile + (uint16_t)sizeof((char)' ') + (uint16_t)StringLenLine;    //lint !e866 Unusual use of ' ' in size of - tested ok

    if (StringLenSourceLocation <= (uint16_t)DBGP_LEN_OF_PROT_FRAME_ARRAY)
    {
        strcpy(SourceLocationString, FileString);
        strcat(SourceLocationString, Space);
        strncat(SourceLocationString, LineString, (size_t)StringLenLine);
        strcat(SourceLocationString, Space);
    }
    else
    {
        SourceLocationString[0] = (char)'!';
        StringLenSourceLocation = (uint16_t)sizeof((char)'!');    //lint !e866 Unusual use of ' ' in size of - tested ok
    }

    return StringLenSourceLocation;
}

/** \brief  Concatenates two strings to a single string for having atomic (reentrant) functions
 *
 *  \param  InCharArrayA   First string for the concatenation (Input string requires \0)
 *  \param  InCharArrayB   (Second) String to be concatenated behind InCharArrayA (Input string requires \0)
 *  \param  OutBytesArray  Pointer to the output string containing InCharArrayA followed by InCharArrayB
 *  \return Number of characters in the concatenated string
 */
static void DBGX_concatenateStrings(const char *InCharArrayA, const char *InCharArrayB, char *OutBytesArray)
{
    char       Array[DBGP_LEN_OF_PROT_FRAME_ARRAY];
    const char StringTooLong[] = {"!~Concatenated string too long"};

    const uint16_t LenArrayA = (uint16_t)strlen(InCharArrayA);
    const uint16_t LenArrayB = (uint16_t)strlen(InCharArrayB);
    const uint16_t StatusLen = (LenArrayA + LenArrayB + (uint16_t)1U); // Add the needed length for the null termination

    Array[0] = '\0';

    if (StatusLen <= DBGX_AssemblerOutStringMaxLen)
    {
        // The concatenation is not too long --> check if it had been too long yet.
        if (LenArrayA != (uint16_t)strlen(StringTooLong))
        {
            // Length of the input does not fit to StringToLong[] --> String was not too long so far
            strcat(Array, InCharArrayA);
            strcat(Array, InCharArrayB);
        }
        else if(strncmp (StringTooLong, InCharArrayA, DBGX_COMPARE_CHARACTERS_CONCATENATION) != 0)
        {
            // Length would fit to the StringToLong[], but the content is different
            strcat(Array, InCharArrayA);
            strcat(Array, InCharArrayB);
        }
        else
        {
            // String was already too long, so return the too long info
            strncpy(Array, StringTooLong, strlen(StringTooLong));
            Array[strlen(StringTooLong)] = '\0';
        }
    }
    else
    {
        // String is too long, so return the too long info
        strncpy(Array, StringTooLong, strlen(StringTooLong));
        Array[strlen(StringTooLong)] = '\0';
    }
    strcpy(OutBytesArray, Array);
}

/** \brief  Shortens the source path from the __FILE__ macro to the file name
 *
 *  \param  path  Source path from the __FILE__ macro
 *  \return The filename without the whole source path
 */
static const char *DBGX_GetFileNameFromFilePath(const char *path)
{
    const char *Filename = NULL;
    const char LastCharacter = '/';

    Filename = strrchr(path, LastCharacter);  //lint !e838 !e9034   name is used as a return value; different essential type tested and is ok

    if (Filename == NULL)
    {
        Filename = path;
    }
    return Filename;
}

/** \brief  Copies information to the corresponding position of the DgbAttributes string - right-aligned
 *  \note   Example: String gets converted from "1234...." to "....1234"
 *  \note   If the StringInputAttribute is shorter than the MaxLenOfStringInputAttribute, then it gets filled with '.'
 *  \note   Example: StringInputAttribute "AB" gets converted to "..AB"  (DbgAttributLen = 4)
 *
 *  \param  StringInputAttribute                   String that has to be inserted to the DbgAttributes
 *  \param  MaxLenOfStringInputAttribute           Intended length of the attribute inside the DbgAttribute string
 *  \param  StartPositionInStringOutputAttributes  Index of the position where the information should begin in the DbgAttributes
 *  \param  DbgAttributesOutput                    Pointer to the string of debug_extended attributes, needed
 *                                                 for putting out onto the debug_extended interface besides the
 *                                                 debug_extended data
 */
static void DBGX_writeStringRightAligned(const char *StringInputAttribute, uint8_t MaxLenOfStringInputAttribute, uint8_t StartPositionInStringOutputAttributes, char *DbgAttributesOutput)
{
    for(uint8_t IndexDownCounting = MaxLenOfStringInputAttribute - (uint8_t)1; IndexDownCounting != (uint8_t)255; IndexDownCounting--)
    {
        if((StringInputAttribute[IndexDownCounting] != '.') && (StringInputAttribute[IndexDownCounting] != '\0'))
        {
            DbgAttributesOutput[StartPositionInStringOutputAttributes + IndexDownCounting] = StringInputAttribute[IndexDownCounting];
        }
        else
        {
            DbgAttributesOutput[StartPositionInStringOutputAttributes] = '.';
            StartPositionInStringOutputAttributes++;
        }
    }
}

/** \brief  Gets the level ID from the filter flags of calling location
 *
 *  \param  CallingFilter  Debug_extended output data filter settings at the calling location
 *  \return Level ID
 */
static uint8_t DBGX_getLevelFromFilter(uint32_t CallingFilter)
{
    uint8_t Level;

    Level = (uint8_t)(CallingFilter >> DBGX_LEV_BIT_IDX_SHIFT);
    return Level;
}

/** \brief  Gets the scenario ID from the filter flags of calling location
 *
 *  \param  CallingFilter  Debug_extended output data filter settings at the calling location
 *  \return Scenario ID
 */
static uint32_t DBGX_getScenarioFromFilter(uint32_t CallingFilter)
{
    uint32_t Scenario;
    uint32_t Tmp;

    Tmp      = (CallingFilter & DBGX_SCN_BIT_IDX_MASK)  >> DBGX_SCN_BIT_IDX_SHIFT;
    Scenario = (CallingFilter & DBGX_SCN_WORD_IDX_MASK) << DBGX_BITS_PER_BYTE;
    Scenario = Scenario | Tmp;

    return Scenario;
}

/** \brief  Converts the level ID to a level character
 *
 *  \param  Level  ID of level of debug_extended data (ERR, WARN, INFO)
 *  \return Level character
 */
static char DBGX_convLevelToLevelChar(uint8_t Level)
{
    char LevelChar;

    switch (Level)
    {
        case DBGX_LEV_NONE:
            LevelChar = '0';
            break;
        case DBGX_LEV_ERR_BITIDX:
            LevelChar = 'E';
            break;
        case DBGX_LEV_WARN_BITIDX:
            LevelChar = 'W';
            break;
        case DBGX_LEV_INFO_BITIDX:
            LevelChar = 'I';
            break;
        default:
            LevelChar = '0';
            break;
    }

    return LevelChar;
}

//Axivion Enable Style MisraC2012-18.4

