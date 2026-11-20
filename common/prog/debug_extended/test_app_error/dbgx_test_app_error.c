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
 *  COMP_ABBREV      DBGXTE
 ******************************************************************************/


/******************************************************************************/
/* DOCUMENTATION                                                              */
/******************************************************************************/
/** \file
 *
 *  \brief    Test implementation of DBGXTE.
 *
 *  \details  Test of the api_error layer of the debug extended module.
 */


/******************************************************************************/
/* INCLUDES                                                                   */
/******************************************************************************/
#include "dbgx_test_app_error.h"

#include "BSH_stdinc.h"

#include "api_error_cfg.h"
#include "api_cfg.h"


/******************************************************************************/
/* PRIVATE DATA DEFINITIONS                                                   */
/******************************************************************************/

static void DBGXTE_testErrFunctions( void );
static void DBGXTE_testErrFunctionsCallback( void );
static void DBGXTE_solveCallbackTestOne( void );
static void DBGXTE_solveCallbackTestTwo( void );

uint8_t DBGXTE_ErrorSolvingReset = 0;


/******************************************************************************/
/* PRIVATE FUNCTION DECLARATIONS                                              */
/******************************************************************************/

static void DBGXTE_testErrFunctions( void ){
    // The following commands test the error solving functions of the debug_extended component

    DBGX_logStr_NonFiltered("******************* DBGXTE_testErrFunctions *******************");

    DBGX_logStr_NonFiltered("DBGX_handleErr: error code 0x2248");
    DBGX_handleErr(0x2248);

    DBGX_logStr_NonFiltered("DBGX_handleErrWithParam: error code 0x2248 ; parameter 123456789");
    DBGX_handleErrWithParam(0x2248, 123456789);

    DBGX_logStr_NonFiltered("DBGX_handleErrIf with condition and error code: 0x9876");
    for(uint8_t i = 0; i < 3; i++)
    {
        DBGX_handleErrIf(i%2, 0x9876);
    }

    DBGX_logStr_NonFiltered("DBGX_handleErrWithParamIf with condit: error code: 4444 ; parameter 444444");
    for(uint8_t i = 0; i < 3; i++)
    {
        bool condition = false;
        DBGX_handleErrWithParamIf(i%2, 444, 444444);
    }

    DBGX_logStr_NonFiltered("DBGX_readFirstError");
    uint32_t ErrorCodeFirstError = 0;
    uint32_t ErrorParameterFirstError = 0;
    (void)DBGX_readFirstError(&ErrorCodeFirstError, &ErrorParameterFirstError);
    DBGX_logStrInt_NonFiltered("ErrorCodeFirstError: ", &ErrorCodeFirstError, DBGX_FORMAT_UNSIGNED | DBGX_FORMAT_INTLEN_16 | DBGX_FORMAT_BASE_16);
    DBGX_logStrInt_NonFiltered("ErrorParameterFirstError: ", &ErrorParameterFirstError, DBGX_FORMAT_UNSIGNED | DBGX_FORMAT_INTLEN_16 | DBGX_FORMAT_BASE_16);

    DBGX_logStr_NonFiltered("DBGX_readLastError");
    uint32_t ErrorCodeLastError = 0;
    uint32_t ErrorParameterLastError = 0;
    (void)DBGX_readLastError(&ErrorCodeLastError, &ErrorParameterLastError);
    DBGX_logStrInt_NonFiltered("ErrorCodeLastError: ", &ErrorCodeLastError, DBGX_FORMAT_UNSIGNED | DBGX_FORMAT_INTLEN_32 | DBGX_FORMAT_BASE_16);
    DBGX_logStrInt_NonFiltered("ErrorParameterLastError: ", &ErrorParameterLastError, DBGX_FORMAT_UNSIGNED | DBGX_FORMAT_INTLEN_32 | DBGX_FORMAT_BASE_16);
}


static void DBGXTE_testErrFunctionsCallback( void ){
    // The following commands test the error solving functions of the debug_extended component

    DBGX_logStr_NonFiltered("**************** DBGXTE_testErrFunctionsCallback ****************");

    DBGX_handleErr(DBGXTE_ERR_CALLBACK_TEST);

    DBGX_handleErrWithParam(DBGXTE_ERR_CALLBACK_TEST, 0x00000001);

    DBGX_handleErrIf(DBGXTE_ErrorSolvingReset, DBGXTE_ERR_RESET_TEST);
}


static void DBGXTE_solveCallbackTestOne( void ){
    DBGX_logStr_NonFiltered("DBGE - Solve error 'callback test' with param '0'");
}


static void DBGXTE_solveCallbackTestTwo( void ){
    DBGX_logStr_NonFiltered("DBGE - Solve error 'callback test' with param '1'");
}


/******************************************************************************/
/* PRIVATE FUNCTION DEFINITIONS                                                */
/******************************************************************************/

void DBGXTE_solveExceptionalErrorFunctions( uint32_t errorCode, uint32_t errorParameter ){
    DBGX_logStr_NonFiltered("*********** DBGXTE_solveExceptionalErrorFunctions************");

    switch (errorCode)
    {
        case (0x00000000 | DBGX_ERR_SOLVING_CALLBACK):
            switch (errorParameter)
            {
                case 0x00000000:
                    DBGXTE_solveCallbackTestOne();
                    break;
                case 0x00000001:
                    DBGXTE_solveCallbackTestTwo();
                    break;
                default:
                    break;
            }
            break;

            default:
            break;
    }
}


uint8_t DBGXTE_handleTask( void )
{
    static uint8_t DBGXTE_TaskState = TASK_NOT_INITIALISED;

    if (DBGXTE_TaskState == TASK_INITIALISED)
    {
        DBGXTE_testErrFunctionsCallback();
    }
    else if (DBGXTE_TaskState == TASK_NOT_INITIALISED)
    {
        DBGX_init();
        (void)DBGX_registerErrorSolvingFunction(&DBGXTE_solveExceptionalErrorFunctions);
        DBGXTE_testErrFunctions();

        DBGXTE_TaskState = TASK_INITIALISED;
    }
    else if (DBGXTE_TaskState == TASK_TERMINATE)
    {
        // Do nothing
    }
    else
    {
        // Do nothing
    }

    return (DBGXTE_TaskState);
}


