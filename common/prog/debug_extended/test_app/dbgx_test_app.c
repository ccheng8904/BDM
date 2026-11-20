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
 *  COMP_ABBREV      DBGXTA
 ******************************************************************************/


/******************************************************************************/
/* DOCUMENTATION                                                              */
/******************************************************************************/
/** \file
 *
 *  \brief    Test implementation of DBGXTA.
 *
 *  \details  Test of the api layer of the debug extended module.
 */


/******************************************************************************/
/* INCLUDES                                                                   */
/******************************************************************************/
#include "dbgx_test_app.h"

#include "BSH_stdinc.h"

#include "api.h"
#include "protocol.h"
#include "interface.h"
#include "debug/RTT/SEGGER_RTT.h"
#include "debug/debug_api.h"
#include <timerlib.h>

#include "debug_extended/api_cfg.h"


/******************************************************************************/
/* PRIVATE DATA DEFINITIONS                                                   */
/******************************************************************************/
static volatile uint32_t DBGXTA_timeMeasurementNonFiltered[13];
static volatile uint32_t DBGXTA_timeMeasurementFiltered[13];


/******************************************************************************/
/* PRIVATE FUNCTION DECLARATIONS                                              */
/******************************************************************************/

static void DBGXTA_testLayerFunctions( void );
static void DBGXTA_testApiLayerNonFiltered( void );
static void DBGXTA_testFilteredFunctions( void );
static void DBGXTA_testNonFilteredComposedFunctions( void );
static void DBGXTA_testFilteredComposedFunctions( void );
static void DBGXTA_testIntegerOutputFormat( void );
static void DBGXTA_testIntegerArrOutputFormat( void );
static void DBGXTA_testStringInput( void );
static void DBGXTA_testStringInput_CB(uint8_t *Data, uint16_t DataLen);
static void DBGXTA_measureApisNonFiltered( void );
static void DBGXTA_measureApisFiltered( void );
static void DBGXTA_testDefaultFormatParameterNonFiltered( void );
static void DBGXTA_testDefaultFormatParameterFiltered( void );


/******************************************************************************/
/* PRIVATE FUNCTION DEFINITIONS                                                */
/******************************************************************************/

/*Fine to switch lint off in this file. Just for debug purposes. NOT for release. For Debug it works fine.
  e534 results in using SEGGER_RTT_WriteString() without using the return value.
       No user should call this function directly. Instead you should use the spi-layer functions.
  */
/*lint -save -e534 -e835 -e845 */

static void DBGXTA_testLayerFunctions( void )
{
    SEGGER_RTT_WriteString(0, "*********************************** DBGXTA_testLayerFunctions ***********************************  \r\n");

    // for test purposes
     char testChar = 'a';

     // Call Segger RTT directly
     SEGGER_RTT_WriteString(0, "Hello World from SEGGER !!!");

     // Call function of the "Debug"-component
     DBG_writeString("Hello world from debug component !!!");

     // Call function of the "interface"-layer
     const uint8_t textArray[40] = {"Hello world from interface layer !!!   "};
     DBGI_SendInterfaceBytes(4, &textArray[0], (uint16_t)39);

     // Call function of the "protocol"-layer
     DBGP_writeProtCharArray(&testChar, "Hello world from protocol layer !!!", (uint16_t)37);

     // Empty lines
     SEGGER_RTT_WriteString(0, "\r\n");
}


static void DBGXTA_testApiLayerNonFiltered( void )
{
    // The following test commands are copied from the documentation: /doc/FNG_DebugDataUsersManual.md

    SEGGER_RTT_WriteString(0, "*********************************** DBGXTA_testApiLayerNonFiltered ***********************************  \r\n");

    DBGX_logChar_NonFiltered('a');

    DBGX_logStr_NonFiltered("Here we go.");

    uint16_t MyVariable = 15;
    DBGX_logInt_NonFiltered(MyVariable, DBGX_UNSIGNED_HEXADECIMAL);

    int8_t MyArray[] = {-1, 0, 1, 2, 3};
    DBGX_logIntArr_NonFiltered(MyArray, 5, DBGX_SIGNED_DECIMAL);

    DBGX_logTime_NonFiltered();

    DBGX_logLocat_NonFiltered();

    // Empty lines
    SEGGER_RTT_WriteString(0, "\r\n");
}


static void DBGXTA_testFilteredFunctions( void )
{
    // The following test commands are defined within the filters_aut.h from following directory: app/MyVariant/prog/debug_extended
    // The output results depend on the configurations in the filters.spc file from following directory: app/MyVariant/prog/debug_extended

    // The expecting results for the current test configuration are commented on the rigth side

    SEGGER_RTT_WriteString(0, "*********************************** DBGXTA_testFilteredFunctions ***********************************  \r\n");

    // Expected result with the current configuration:                                        Getting this output?
    DBGX_logStr_ERR("------------ Test basic filters ------------  ");                        // YES
    DBGX_logStr_ERR("Hello World from following function: Filtered ERR");                     // YES
    DBGX_logStr_WARN("Hello World from following function: Filtered WARN");                   // YES
    DBGX_logStr_INFO("Hello World from following function: Filtered INFO");                   // NO    - INFO undef, off
    SEGGER_RTT_WriteString(0, "\r\n");

    // Test whole ERR section
    DBGX_logStr_ERR("------------ Test whole ERR section ------------ ");                     // YES
    DBGX_logChar_ERR('E');                                                                    // YES
    DBGX_logStr_ERR("String");                                                                // YES
    uint16_t MyVariable = 15;
    DBGX_logInt_ERR(MyVariable, DBGX_UNSIGNED_HEXADECIMAL);                                   // YES
    int8_t MyIntArray[] = {-1, 0, 1, 2, 3};
    DBGX_logIntArr_ERR(MyIntArray, 5, DBGX_SIGNED_DECIMAL);                                   // YES
    DBGX_logTime_ERR();                                                                       // YES
    DBGX_logLocat_ERR();                                                                      // YES
 
    // Test scenario DBGX_TOUCH
    DBGX_logStr_ERR("------------ Test output DBGX_TOUCH section ------------ ");             // YES
    DBGX_logStr_SCN_DBGX_TOUCH("Test module activated: DBGX_TOUCH");                          // YES
    DBGX_logStr_ERR_SCN_DBGX_TOUCH("Test module: DBGX_TOUCH    -    ERR");                    // YES
    DBGX_logStr_WARN_SCN_DBGX_TOUCH("Test module: DBGX_TOUCH    -    WARN");                  // YES
    DBGX_logStr_INFO_SCN_DBGX_TOUCH("Test module: DBGX_TOUCH    -    INFO");                  // YES

    // Test scenario DBGX_POWER
    DBGX_logStr_ERR("------------ Test output ERR section ------------ ");                    // YES
    DBGX_logStr_SCN_DBGX_POWER("Test module activated: DBGX_POWER");                          // NO
    DBGX_logStr_ERR_SCN_DBGX_POWER("Test module: DBGX_POWER    -    ERR");                    // YES
    DBGX_logStr_WARN_SCN_DBGX_POWER("Test module: DBGX_POWER    -    WARN");                  // YES
    DBGX_logStr_INFO_SCN_DBGX_POWER("Test module: DBGX_POWER    -    INFO");                  // NO

    // Empty lines
    SEGGER_RTT_WriteString(0, "\r\n");
}

static void DBGXTA_testNonFilteredComposedFunctions( void )  
{
    SEGGER_RTT_WriteString(0, "*********************************** DBGXTA_testNonFilteredComposedFunctions ***********************************  \r\n");

    DBGX_logStrStr_NonFiltered("Hello ", "world !!!");

    // String that is too long for the output
    DBGX_logStrStr_NonFiltered("Helloooooooooooooooooooooooooooooooooooooooooooooooooooooooooo myyyyyyyyyyyyyyyyyyyyyyyyy ", "world !!!");

    uint16_t MyInteger = 24;
    DBGX_logStrInt_NonFiltered("My integer in HEX =  ", MyInteger, DBGX_UNSIGNED_HEXADECIMAL);

    int8_t MyIntArray[] = {-1, 0, 1, 2, 3};
    DBGX_logStrIntArr_NonFiltered("My integer array in DEC: ", MyIntArray, 5, DBGX_FORMAT_SIGNED | DBGX_FORMAT_INTLEN_8 | DBGX_FORMAT_BASE_10);

    DBGX_logStrStrLocat_NonFiltered("Source ", "function: ");

    DBGX_logStrStrLocatFull_NonFiltered("Example ", "path: ");

    uint16_t MyBasicInteger = 24;
    DBGX_logStrIntLocat_NonFiltered("Integer in hex ", MyBasicInteger, DBGX_UNSIGNED_HEXADECIMAL);

    uint16_t MyDetailedInteger = 24;
    DBGX_logStrIntLocatFull_NonFiltered("Int [hex] ", MyDetailedInteger, DBGX_UNSIGNED_HEXADECIMAL);

    // Empty lines
    SEGGER_RTT_WriteString(0, "\r\n");
}

static void DBGXTA_testFilteredComposedFunctions( void )
{
    // The following test commands are defined within the filters_aut.h from following directory: app/MyVariant/prog/debug_extended
    // The output results depend on the configurations in the filters.spc file from following directory: app/MyVariant/prog/debug_extended

    // The expecting results for the current test configuration are commented on the rigth side

    SEGGER_RTT_WriteString(0, "*********************************** DBGXTA_testFilteredComposedFunctions ***********************************  \r\n");

    DBGX_logStrStr_SCN_DBGX_TOUCH("Hello ", "world !!!");
  
    // String that is too long for the output
    DBGX_logStrStr_SCN_DBGX_TOUCH("Helloooooooooooooooooooooooooooooooooooooooooooooooooooooooooo myyyyyyyyyyyyyyyyyyyyyyyyy ", "world !!!");
  
    uint16_t MyInteger = 24;
    DBGX_logStrInt_SCN_DBGX_TOUCH("My integer in HEX =  ", MyInteger, DBGX_UNSIGNED_HEXADECIMAL);

    int8_t MyIntArray[] = {-1, 0, 1, 2, 3};
    DBGX_logStrIntArr_SCN_DBGX_TOUCH("My integer array in DEC: ", MyIntArray, 5, DBGX_FORMAT_SIGNED | DBGX_FORMAT_INTLEN_8 | DBGX_FORMAT_BASE_10);

    DBGX_logStrStrLocat_SCN_DBGX_TOUCH("Source ", "function: ");

    DBGX_logStrStrLocatFull_SCN_DBGX_TOUCH("Example ", "path: ");

    uint16_t MyBasicInteger = 24;
    DBGX_logStrIntLocat_SCN_DBGX_TOUCH("Integer in hex ", MyBasicInteger, DBGX_UNSIGNED_HEXADECIMAL);

    uint16_t MyDetailedInteger = 24;
    DBGX_logStrIntLocatFull_SCN_DBGX_TOUCH("Int [hex] ", MyDetailedInteger, DBGX_UNSIGNED_HEXADECIMAL);
    // Empty lines
    SEGGER_RTT_WriteString(0, "\r\n");
}

static void DBGXTA_testIntegerOutputFormat( void )
{
    // The following commands test the possible integer output formats, that are defined in the api_cfg.h

    SEGGER_RTT_WriteString(0, "*********************************** DBGXTA_testIntegerOutputFormat ***********************************  \r\n");

    uint8_t MyU8Integer = 125;
    DBGX_logInt_NonFiltered(MyU8Integer, DBGX_UNSIGNED_DUAL);
    DBGX_logInt_NonFiltered(MyU8Integer, DBGX_UNSIGNED_OCTAL);
    DBGX_logInt_NonFiltered(MyU8Integer, DBGX_UNSIGNED_DECIMAL);
    DBGX_logInt_NonFiltered(MyU8Integer, DBGX_UNSIGNED_HEXADECIMAL);

    uint16_t MyU16Integer = 32125;
    DBGX_logInt_NonFiltered(MyU16Integer, DBGX_UNSIGNED_DUAL);
    DBGX_logInt_NonFiltered(MyU16Integer, DBGX_UNSIGNED_OCTAL);
    DBGX_logInt_NonFiltered(MyU16Integer, DBGX_UNSIGNED_DECIMAL);
    DBGX_logInt_NonFiltered(MyU16Integer, DBGX_UNSIGNED_HEXADECIMAL);

    uint32_t MyU32Integer = 2105376125;
    DBGX_logInt_NonFiltered(MyU32Integer, DBGX_UNSIGNED_DUAL);
    DBGX_logInt_NonFiltered(MyU32Integer, DBGX_UNSIGNED_OCTAL);
    DBGX_logInt_NonFiltered(MyU32Integer, DBGX_UNSIGNED_DECIMAL);
    DBGX_logInt_NonFiltered(MyU32Integer, DBGX_UNSIGNED_HEXADECIMAL);

    int8_t MyS8Integer = -125;
    DBGX_logInt_NonFiltered(MyS8Integer, DBGX_SIGNED_DUAL);
    DBGX_logInt_NonFiltered(MyS8Integer, DBGX_SIGNED_OCTAL);
    DBGX_logInt_NonFiltered(MyS8Integer, DBGX_SIGNED_DECIMAL);
    DBGX_logInt_NonFiltered(MyS8Integer, DBGX_SIGNED_HEXADECIMAL);

    int16_t MyS16Integer = -15000;
    DBGX_logInt_NonFiltered(MyS16Integer, DBGX_SIGNED_DUAL);
    DBGX_logInt_NonFiltered(MyS16Integer, DBGX_SIGNED_OCTAL);
    DBGX_logInt_NonFiltered(MyS16Integer, DBGX_SIGNED_DECIMAL);
    DBGX_logInt_NonFiltered(MyS16Integer, DBGX_SIGNED_HEXADECIMAL);

    int32_t MyS32Integer = -5000000;
    DBGX_logInt_NonFiltered(MyS32Integer, DBGX_SIGNED_DUAL);
    DBGX_logInt_NonFiltered(MyS32Integer, DBGX_SIGNED_OCTAL);
    DBGX_logInt_NonFiltered(MyS32Integer, DBGX_SIGNED_DECIMAL);
    DBGX_logInt_NonFiltered(MyS32Integer, DBGX_SIGNED_HEXADECIMAL);

    // Empty lines
    SEGGER_RTT_WriteString(0, "\r\n");
}

static void DBGXTA_testIntegerArrayOutputFormat( void )
{
    // The following commands test the possible integer output formats, that are defined in the api_cfg.h

    SEGGER_RTT_WriteString(0, "*********************************** DBGXTA_testIntegerArrOutputFormat ***********************************  \r\n");

    uint8_t MyU8IntegerArray[6] = {0, 1, 42, 64, 128, 255};
    DBGX_logIntArr_NonFiltered(MyU8IntegerArray, 6, DBGX_UNSIGNED_DUAL);
    DBGX_logIntArr_NonFiltered(MyU8IntegerArray, 6, DBGX_UNSIGNED_OCTAL);
    DBGX_logIntArr_NonFiltered(MyU8IntegerArray, 6, DBGX_UNSIGNED_DECIMAL);
    DBGX_logIntArr_NonFiltered(MyU8IntegerArray, 6, DBGX_UNSIGNED_HEXADECIMAL);

    uint16_t MyU16IntegerArray[6] = {0, 1, 64, 130, 256, 65535};
    DBGX_logIntArr_NonFiltered(MyU16IntegerArray, 6, DBGX_UNSIGNED_DUAL);
    DBGX_logIntArr_NonFiltered(MyU16IntegerArray, 6, DBGX_UNSIGNED_OCTAL);
    DBGX_logIntArr_NonFiltered(MyU16IntegerArray, 6, DBGX_UNSIGNED_DECIMAL);
    DBGX_logIntArr_NonFiltered(MyU16IntegerArray, 6, DBGX_UNSIGNED_HEXADECIMAL);

    uint32_t MyU32IntegerArray[6] = {0, 1, 64, 130, 65535, 4294967295};
    DBGX_logIntArr_NonFiltered(MyU32IntegerArray, 6, DBGX_UNSIGNED_DUAL);
    DBGX_logIntArr_NonFiltered(MyU32IntegerArray, 6, DBGX_UNSIGNED_OCTAL);
    DBGX_logIntArr_NonFiltered(MyU32IntegerArray, 6, DBGX_UNSIGNED_DECIMAL);
    DBGX_logIntArr_NonFiltered(MyU32IntegerArray, 6, DBGX_UNSIGNED_HEXADECIMAL);

    int8_t MyS8IntegerArray[6] = {-128, -1, 0, 1, 64, 127};
    DBGX_logIntArr_NonFiltered(MyS8IntegerArray, 6, DBGX_SIGNED_DUAL);
    DBGX_logIntArr_NonFiltered(MyS8IntegerArray, 6, DBGX_SIGNED_OCTAL);
    DBGX_logIntArr_NonFiltered(MyS8IntegerArray, 6, DBGX_SIGNED_DECIMAL);
    DBGX_logIntArr_NonFiltered(MyS8IntegerArray, 6, DBGX_SIGNED_HEXADECIMAL);

    int16_t MyS16IntegerArray[6] = {-32768, -128, 0, 256, 200, 32767};
    DBGX_logIntArr_NonFiltered(MyS16IntegerArray, 6, DBGX_SIGNED_DUAL);
    DBGX_logIntArr_NonFiltered(MyS16IntegerArray, 6, DBGX_SIGNED_OCTAL);
    DBGX_logIntArr_NonFiltered(MyS16IntegerArray, 6, DBGX_SIGNED_DECIMAL);
    DBGX_logIntArr_NonFiltered(MyS16IntegerArray, 6, DBGX_SIGNED_HEXADECIMAL);

    int32_t MyS32IntegerArray[6] = {-2147483648, -128, 0, 256, 200, 2147483647};
    DBGX_logIntArr_NonFiltered(MyS32IntegerArray, 6, DBGX_SIGNED_DUAL);
    DBGX_logIntArr_NonFiltered(MyS32IntegerArray, 6, DBGX_SIGNED_OCTAL);
    DBGX_logIntArr_NonFiltered(MyS32IntegerArray, 6, DBGX_SIGNED_DECIMAL);
    DBGX_logIntArr_NonFiltered(MyS32IntegerArray, 6, DBGX_SIGNED_HEXADECIMAL);

    // Empty lines
    SEGGER_RTT_WriteString(0, "\r\n");
}

static void DBGXTA_testStringInput( void )
{
    SEGGER_RTT_WriteString(0, "*********************************** DBGXTA_testStringInput ***********************************  \r\n");

    static char array[50];

    int32_t status = DBGX_getString(array, (sizeof(array) / sizeof(array[0])));
    if(status >= 0)
    {
        DBGX_logIntArr_NonFiltered(array, (uint16_t)status, DBGX_SIGNED_DECIMAL);
    }
}

static void DBGXTA_testStringInput_CB(uint8_t *Data, uint16_t DataLen)
{
    DBGX_logStr_NonFiltered("*********** DBGXTA_testStringInput_CB************");

    switch (Data[0])
    {
        case '0':
            DBGX_logStr_NonFiltered("Input callback function: DBGXTA_testStringInput_CB");
            DBGX_logIntArr_NonFiltered(Data, DataLen, DBGX_SIGNED_DECIMAL);
            break;

        default:
            break;
    }
}


static void DBGXTA_measureApisNonFiltered( void )
{
    uint32_t actualSysTime;
    
    actualSysTime = (uint32_t)TIM_u32GetCircleMicroSeconds(); 
    DBGX_logChar_NonFiltered('a');
    DBGXTA_timeMeasurementNonFiltered[0] = (uint32_t)TIM_u32GetCircleMicroSeconds() - actualSysTime;

    actualSysTime = (uint32_t)TIM_u32GetCircleMicroSeconds(); 
    DBGX_logStr_NonFiltered("Here we go.");
    DBGXTA_timeMeasurementNonFiltered[1] = (uint32_t)TIM_u32GetCircleMicroSeconds() - actualSysTime;

    uint16_t MyVariable = 15;
    actualSysTime = (uint32_t)TIM_u32GetCircleMicroSeconds(); 
    DBGX_logInt_NonFiltered(MyVariable, DBGX_UNSIGNED_HEXADECIMAL);
    DBGXTA_timeMeasurementNonFiltered[2] = (uint32_t)TIM_u32GetCircleMicroSeconds() - actualSysTime;

    int8_t MyArray[] = {-1, 0, 1, 2, 3};
    actualSysTime = (uint32_t)TIM_u32GetCircleMicroSeconds(); 
    DBGX_logIntArr_NonFiltered(MyArray, 5, DBGX_SIGNED_DECIMAL);
    DBGXTA_timeMeasurementNonFiltered[3] = (uint32_t)TIM_u32GetCircleMicroSeconds() - actualSysTime;

    actualSysTime = (uint32_t)TIM_u32GetCircleMicroSeconds(); 
    DBGX_logTime_NonFiltered();
    DBGXTA_timeMeasurementNonFiltered[4] = (uint32_t)TIM_u32GetCircleMicroSeconds() - actualSysTime;

    actualSysTime = (uint32_t)TIM_u32GetCircleMicroSeconds(); 
    DBGX_logLocat_NonFiltered();
    DBGXTA_timeMeasurementNonFiltered[5] = (uint32_t)TIM_u32GetCircleMicroSeconds() - actualSysTime;

    actualSysTime = (uint32_t)TIM_u32GetCircleMicroSeconds(); 
    DBGX_logStrStr_NonFiltered("Hello ", "world !!!");
    DBGXTA_timeMeasurementNonFiltered[6] = (uint32_t)TIM_u32GetCircleMicroSeconds() - actualSysTime;

    uint16_t MyInteger = 24;
    actualSysTime = (uint32_t)TIM_u32GetCircleMicroSeconds(); 
    DBGX_logStrInt_NonFiltered("My integer in HEX = ", MyInteger, DBGX_UNSIGNED_HEXADECIMAL);
    DBGXTA_timeMeasurementNonFiltered[7] = (uint32_t)TIM_u32GetCircleMicroSeconds() - actualSysTime;

    int8_t MyIntArray[] = {-1, 0, 1, 2, 3};
    actualSysTime = (uint32_t)TIM_u32GetCircleMicroSeconds(); 
    DBGX_logStrIntArr_NonFiltered("My integer array in HEX: ", MyIntArray, 5, DBGX_SIGNED_DECIMAL);
    DBGXTA_timeMeasurementNonFiltered[8] = (uint32_t)TIM_u32GetCircleMicroSeconds() - actualSysTime;

    actualSysTime = (uint32_t)TIM_u32GetCircleMicroSeconds(); 
    DBGX_logStrStrLocat_NonFiltered("Source ", "function: ");
    DBGXTA_timeMeasurementNonFiltered[9] = (uint32_t)TIM_u32GetCircleMicroSeconds() - actualSysTime;

    actualSysTime = (uint32_t)TIM_u32GetCircleMicroSeconds(); 
    DBGX_logStrStrLocatFull_NonFiltered("Example ", "path: ");
    DBGXTA_timeMeasurementNonFiltered[10] = (uint32_t)TIM_u32GetCircleMicroSeconds() - actualSysTime;

    uint16_t MyBasicInteger = 24;
    actualSysTime = (uint32_t)TIM_u32GetCircleMicroSeconds(); 
    DBGX_logStrIntLocat_NonFiltered("Integer in hex ", MyBasicInteger, DBGX_UNSIGNED_HEXADECIMAL);
    DBGXTA_timeMeasurementNonFiltered[11] = (uint32_t)TIM_u32GetCircleMicroSeconds() - actualSysTime;

    uint16_t MyDetailedInteger = 24;
    actualSysTime = (uint32_t)TIM_u32GetCircleMicroSeconds(); 
    DBGX_logStrIntLocatFull_NonFiltered("Int [hex] ", MyDetailedInteger, DBGX_UNSIGNED_HEXADECIMAL);
    DBGXTA_timeMeasurementNonFiltered[12] = (uint32_t)TIM_u32GetCircleMicroSeconds() - actualSysTime;
}

static void DBGXTA_measureApisFiltered( void )
{
    uint32_t actualSysTime;
    
    actualSysTime = (uint32_t)TIM_u32GetCircleMicroSeconds(); 
    DBGX_logChar_ERR_SCN_DBGX_TOUCH('a');
    DBGXTA_timeMeasurementFiltered[0] = (uint32_t)TIM_u32GetCircleMicroSeconds() - actualSysTime;

    actualSysTime = (uint32_t)TIM_u32GetCircleMicroSeconds(); 
    DBGX_logStr_ERR_SCN_DBGX_TOUCH("Here we go.");
    DBGXTA_timeMeasurementFiltered[1] = (uint32_t)TIM_u32GetCircleMicroSeconds() - actualSysTime;

    uint16_t MyVariable = 15;
    actualSysTime = (uint32_t)TIM_u32GetCircleMicroSeconds(); 
    DBGX_logInt_ERR_SCN_DBGX_TOUCH(MyVariable, DBGX_UNSIGNED_HEXADECIMAL);
    DBGXTA_timeMeasurementFiltered[2] = (uint32_t)TIM_u32GetCircleMicroSeconds() - actualSysTime;

    int8_t MyArray[] = {-1, 0, 1, 2, 3};
    actualSysTime = (uint32_t)TIM_u32GetCircleMicroSeconds(); 
    DBGX_logIntArr_ERR_SCN_DBGX_TOUCH(MyArray, 5, DBGX_SIGNED_DECIMAL);
    DBGXTA_timeMeasurementFiltered[3] = (uint32_t)TIM_u32GetCircleMicroSeconds() - actualSysTime;

    actualSysTime = (uint32_t)TIM_u32GetCircleMicroSeconds(); 
    DBGX_logTime_ERR_SCN_DBGX_TOUCH();
    DBGXTA_timeMeasurementFiltered[4] = (uint32_t)TIM_u32GetCircleMicroSeconds() - actualSysTime;

    actualSysTime = (uint32_t)TIM_u32GetCircleMicroSeconds(); 
    DBGX_logLocat_ERR_SCN_DBGX_TOUCH();
    DBGXTA_timeMeasurementFiltered[5] = (uint32_t)TIM_u32GetCircleMicroSeconds() - actualSysTime;

    actualSysTime = (uint32_t)TIM_u32GetCircleMicroSeconds(); 
    DBGX_logStrStr_ERR_SCN_DBGX_TOUCH("Hello ", "world !!!");
    DBGXTA_timeMeasurementFiltered[6] = (uint32_t)TIM_u32GetCircleMicroSeconds() - actualSysTime;

    uint16_t MyInteger = 24;
    actualSysTime = (uint32_t)TIM_u32GetCircleMicroSeconds(); 
    DBGX_logStrInt_ERR_SCN_DBGX_TOUCH("My integer in HEX = ", MyInteger, DBGX_UNSIGNED_HEXADECIMAL);
    DBGXTA_timeMeasurementFiltered[7] = (uint32_t)TIM_u32GetCircleMicroSeconds() - actualSysTime;

    int8_t MyIntArray[] = {-1, 0, 1, 2, 3};
    actualSysTime = (uint32_t)TIM_u32GetCircleMicroSeconds(); 
    DBGX_logStrIntArr_ERR_SCN_DBGX_TOUCH("My integer array in HEX: ", MyIntArray, 5, DBGX_SIGNED_DECIMAL);
    DBGXTA_timeMeasurementFiltered[8] = (uint32_t)TIM_u32GetCircleMicroSeconds() - actualSysTime;

    actualSysTime = (uint32_t)TIM_u32GetCircleMicroSeconds(); 
    DBGX_logStrStrLocat_ERR_SCN_DBGX_TOUCH("Source ", "function: ");
    DBGXTA_timeMeasurementFiltered[9] = (uint32_t)TIM_u32GetCircleMicroSeconds() - actualSysTime;

    actualSysTime = (uint32_t)TIM_u32GetCircleMicroSeconds(); 
    DBGX_logStrStrLocatFull_ERR_SCN_DBGX_TOUCH("Example ", "path: ");
    DBGXTA_timeMeasurementFiltered[10] = (uint32_t)TIM_u32GetCircleMicroSeconds() - actualSysTime;

    uint16_t MyBasicInteger = 24;
    actualSysTime = (uint32_t)TIM_u32GetCircleMicroSeconds(); 
    DBGX_logStrIntLocat_ERR_SCN_DBGX_TOUCH("Integer in hex ", MyBasicInteger, DBGX_UNSIGNED_HEXADECIMAL);
    DBGXTA_timeMeasurementFiltered[11] = (uint32_t)TIM_u32GetCircleMicroSeconds() - actualSysTime;

    uint16_t MyDetailedInteger = 24;
    actualSysTime = (uint32_t)TIM_u32GetCircleMicroSeconds(); 
    DBGX_logStrIntLocatFull_ERR_SCN_DBGX_TOUCH("Int [hex] ", MyDetailedInteger, DBGX_UNSIGNED_HEXADECIMAL);
    DBGXTA_timeMeasurementFiltered[12] = (uint32_t)TIM_u32GetCircleMicroSeconds() - actualSysTime;
}


static void DBGXTA_testDefaultFormatParameterNonFiltered( void )
{
    DBGX_logStr_NonFiltered("*********** DBGXTA_testDefaultFormatParameterNonFiltered************");

    uint16_t MyVariable = 15;
    DBGX_logInt_NonFiltered(MyVariable, DBGX_UNSIGNED_HEXADECIMAL);
    DBGX_logStrInt_NonFiltered("My variable", MyVariable, DBGX_UNSIGNED_HEXADECIMAL);
    DBGX_logStrIntLocat_NonFiltered("My variable", MyVariable, DBGX_UNSIGNED_HEXADECIMAL);
    DBGX_logStrIntLocatFull_NonFiltered("My variable", MyVariable, DBGX_UNSIGNED_HEXADECIMAL);

    DBGX_logInt_NonFiltered(MyVariable);
    DBGX_logStrInt_NonFiltered("My variable", MyVariable);
    DBGX_logStrIntLocat_NonFiltered("My variable", MyVariable);
    DBGX_logStrIntLocatFull_NonFiltered("My variable", MyVariable);

    int8_t MyArray[] = {-1, 0, 1, 2, 3};
    DBGX_logIntArr_NonFiltered(MyArray, 5, DBGX_SIGNED_DECIMAL);
    DBGX_logStrIntArr_NonFiltered("Test array", MyArray, 5, DBGX_SIGNED_DECIMAL);

    DBGX_logIntArr_NonFiltered(MyArray, 5);
    DBGX_logStrIntArr_NonFiltered("Test array", MyArray, 5);
}

static void DBGXTA_testDefaultFormatParameterFiltered( void )
{
    DBGX_logStr_NonFiltered("*********** DBGXTA_testDefaultFormatParameterFiltered************");

    uint16_t MyVariable = 15;
    int8_t MyIntArray[] = {-1, 0, 1, 2, 3};

    DBGX_logStr_NonFiltered(" logInt with explicit format - level deactivated");
    DBGX_logInt_ERR(MyVariable, DBGX_UNSIGNED_HEXADECIMAL);
    DBGX_logStrInt_ERR("My variable ", MyVariable, DBGX_UNSIGNED_HEXADECIMAL);
    DBGX_logStrIntLocat_ERR("My variable ", MyVariable, DBGX_UNSIGNED_HEXADECIMAL);
    DBGX_logStrIntLocatFull_ERR("My variable ", MyVariable, DBGX_UNSIGNED_HEXADECIMAL);

    DBGX_logStr_NonFiltered(" logInt with explicit format - level deactivated");
    DBGX_logInt_ERR(MyVariable);
    DBGX_logStrInt_ERR("My variable ", MyVariable);
    DBGX_logStrIntLocat_ERR("My variable ", MyVariable);
    DBGX_logStrIntLocatFull_ERR("My variable ", MyVariable);

    DBGX_logStr_NonFiltered(" logInt with explicit format - level/sceario activated");
    DBGX_logInt_ERR_SCN_DBGX_TOUCH(MyVariable, DBGX_UNSIGNED_HEXADECIMAL);
    DBGX_logStrInt_ERR_SCN_DBGX_TOUCH("My variable ", MyVariable, DBGX_UNSIGNED_HEXADECIMAL);
    DBGX_logStrIntLocat_ERR_SCN_DBGX_TOUCH("My variable ", MyVariable, DBGX_UNSIGNED_HEXADECIMAL);
    DBGX_logStrIntLocatFull_ERR_SCN_DBGX_TOUCH("My variable ", MyVariable, DBGX_UNSIGNED_HEXADECIMAL);
    DBGX_logStr_NonFiltered(" - - - - - - - - - - - - - - - - - - ");

    DBGX_logStr_NonFiltered(" logInt without explicit format - level/sceario activated");
    DBGX_logInt_ERR_SCN_DBGX_TOUCH(MyVariable);
    DBGX_logStrInt_ERR_SCN_DBGX_TOUCH("My variable ", MyVariable);
    DBGX_logStrIntLocat_ERR_SCN_DBGX_TOUCH("My variable ", MyVariable);
    DBGX_logStrIntLocatFull_ERR_SCN_DBGX_TOUCH("My variable ", MyVariable);
    DBGX_logStr_NonFiltered(" - - - - - - - - - - - - - - - - - - ");

    DBGX_logStr_NonFiltered(" logInt with explicit format - level/sceario deactivated");
    DBGX_logInt_ERR_SCN_DBGX_POWER(MyVariable);
    DBGX_logStrInt_ERR_SCN_DBGX_POWER("My variable ", MyVariable);
    DBGX_logStrIntLocat_ERR_SCN_DBGX_POWER("My variable ", MyVariable);
    DBGX_logStrIntLocatFull_ERR_SCN_DBGX_POWER("My variable ", MyVariable);
    DBGX_logStr_NonFiltered(" - - - - - - - - - - - - - - - - - - ");


    DBGX_logStr_NonFiltered(" logIntArr with explicit format - level deactivated");
    DBGX_logIntArr_ERR(MyIntArray, 5, DBGX_SIGNED_DECIMAL);
    DBGX_logStrIntArr_ERR("My array ", MyIntArray, 5, DBGX_SIGNED_DECIMAL);
    DBGX_logStr_NonFiltered(" - - - - - - - - - - - - - - - - - - ");

    DBGX_logStr_NonFiltered(" logIntArr without explicit format - level deactivated");
    DBGX_logIntArr_ERR(MyIntArray, 5);
    DBGX_logStrIntArr_ERR("My array ", MyIntArray, 5);
    DBGX_logStr_NonFiltered(" - - - - - - - - - - - - - - - - - - ");
 
    DBGX_logStr_NonFiltered(" logIntArr with explicit format - level/scenario activated"); 
    DBGX_logIntArr_ERR_SCN_DBGX_TOUCH(MyIntArray, 5, DBGX_SIGNED_DECIMAL);
    DBGX_logStrIntArr_ERR_SCN_DBGX_TOUCH("My array ", MyIntArray, 5, DBGX_SIGNED_DECIMAL);
    DBGX_logStr_NonFiltered(" - - - - - - - - - - - - - - - - - - ");

    DBGX_logStr_NonFiltered(" logIntArr without explicit format - level/scenario activated");
    DBGX_logIntArr_ERR_SCN_DBGX_TOUCH(MyIntArray, 5);
    DBGX_logStrIntArr_ERR_SCN_DBGX_TOUCH("My array ", MyIntArray, 5);
    DBGX_logStr_NonFiltered(" - - - - - - - - - - - - - - - - - - ");
        
    DBGX_logStr_NonFiltered(" logIntArr with explicit format - level/scenario deactivated");
    DBGX_logIntArr_ERR_SCN_DBGX_POWER(MyIntArray, 5);
    DBGX_logStrIntArr_ERR_SCN_DBGX_POWER("My array ", MyIntArray, 5);
    DBGX_logStr_NonFiltered(" - - - - - - - - - - - - - - - - - - ");
}

/******************************************************************************/
/* PUBLIC FUNCTION DEFINITIONS                                                */
/******************************************************************************/

uint8_t DBGXTA_handleTask( void )
{
    static uint8_t DBGXTA_TaskState = TASK_NOT_INITIALISED;

    if (DBGXTA_TaskState == TASK_INITIALISED)
    {
        DBGXTA_measureApisNonFiltered();
        DBGXTA_measureApisFiltered();
        DBGXTA_testDefaultFormatParameterNonFiltered();
        DBGXTA_testDefaultFormatParameterFiltered();
    }
    else if (DBGXTA_TaskState == TASK_NOT_INITIALISED) // Initialize task
    {
        DBGX_init();
        (void)DBGX_registerInputCallbackFunction(&DBGXTA_testStringInput_CB);

        DBGXTA_testLayerFunctions();
        DBGXTA_testApiLayerNonFiltered();
        DBGXTA_testFilteredFunctions();
        DBGXTA_testNonFilteredComposedFunctions();
        DBGXTA_testFilteredComposedFunctions();
        DBGXTA_testIntegerOutputFormat();
        DBGXTA_testIntegerArrayOutputFormat();
        DBGXTA_testStringInput();

        DBGXTA_TaskState = TASK_INITIALISED;
    }
    else if (DBGXTA_TaskState == TASK_TERMINATE)
    {
        // Task can be terminated
    }
    else
    {
        // Task has run into undefined error
    }

    return (DBGXTA_TaskState);
}

/*lint -restore -e534  -e835 -e845*/

