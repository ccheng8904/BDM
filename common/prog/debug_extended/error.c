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
 *  COMP_ABBREV      DBGE
 ******************************************************************************/

/******************************************************************************/
/* DOCUMENTATION                                                              */
/******************************************************************************/
/** \file
 *
 *  \ingroup  debug_extended
 *
 *  \brief    Definitions and declarations of generic part of the debug_extended
 *            component: Exceptional error memory
 *
 *  \details  
 */

/******************************************************************************/
/* INCLUDES                                                                   */
/******************************************************************************/

#include "debug_extended/error.h"


/******************************************************************************/
/* PRIVATE TYPE DEFINITIONS                                                   */
/******************************************************************************/


/******************************************************************************/
/* PUBLIC DATA DECLARATIONS                                                  */
/******************************************************************************/

/******************************************************************************/
/* PRIVATE DATA DECLARATIONS                                                  */
/******************************************************************************/
static struct DBGE_SavedErrors_s DBGE_SavedErrors = {DBGE_NO_ERROR, DBGE_NO_ERROR, 0U, 0U}; /* Exceptional error memory */

static const DBGE_ErrorSolvingFunction_t   *DBGE_ErrorSolvingFunctions_Gen = NULL;  /* Callback function for solving of exceptional errors */
static const uint8_t *DBGE_NumberOfErrorSolvingFunctions = NULL;
static uint8_t DBGE_MaxNumberOfErrorSolvingFunctions;

/******************************************************************************/
/* PRIVATE FUNCTION DECLARATIONS                                              */
/******************************************************************************/
static uint8_t DBGE_getNumberOfSavedErrors(void);

/******************************************************************************/
/* PUBLIC FUNCTION DEFINITIONS                                                */
/******************************************************************************/

void DBGE_saveError(uint32_t ErrorCode)
{
    DBGE_saveErrorWithParam(ErrorCode, 0U);
}

void DBGE_saveErrorWithParam(uint32_t ErrorCode, uint32_t ErrorParameter)
{
    HSUP_vDisableInt();

    if (DBGE_SavedErrors.FirstError == DBGE_NO_ERROR)
    {
        DBGE_SavedErrors.FirstError = ErrorCode;
        DBGE_SavedErrors.FirstErrorParameter = ErrorParameter;
    }
    else
    {
        DBGE_SavedErrors.LastError = ErrorCode;
        DBGE_SavedErrors.LastErrorParameter = ErrorParameter;
    }

    HSUP_vEnableInt();
}

uint8_t DBGE_readFirstError(uint32_t *ErrorCode, uint32_t *ErrorParameter)
{
    *ErrorCode      = DBGE_SavedErrors.FirstError;
    *ErrorParameter = DBGE_SavedErrors.FirstErrorParameter;

    return DBGE_getNumberOfSavedErrors();
}

uint8_t DBGE_readLastError(uint32_t *ErrorCode, uint32_t *ErrorParameter)
{
    *ErrorCode      = DBGE_SavedErrors.LastError;
    *ErrorParameter = DBGE_SavedErrors.LastErrorParameter;

    return DBGE_getNumberOfSavedErrors();
}

void DBGX_setErrorSolvingFunctions(const DBGE_ErrorSolvingFunction_t *ErrorSolvingFunctionArray, const uint8_t *NumberOfErrorSolvingFunctions, uint8_t MaxNumberOfErrorSolvingFunctions)
{
    DBGE_ErrorSolvingFunctions_Gen = ErrorSolvingFunctionArray;
    DBGE_NumberOfErrorSolvingFunctions = NumberOfErrorSolvingFunctions;
    DBGE_MaxNumberOfErrorSolvingFunctions = MaxNumberOfErrorSolvingFunctions;
}

void DBGE_solveExceptionalError(uint32_t ErrorCode, uint32_t ErrorParameter)
{
    STS_logStringAndU32Hex("ErrorCode = ", ErrorCode);
    STS_logStringAndU32Hex("ErrorParameter = ", ErrorParameter);

    for(uint8_t index = 0; index < *DBGE_NumberOfErrorSolvingFunctions; index++)
    {
        if(index < DBGE_MaxNumberOfErrorSolvingFunctions)
        {
            DBGE_ErrorSolvingFunctions_Gen[index](ErrorCode, ErrorParameter);
        }
        else
        {
            break;
        }
    }
}


/******************************************************************************/
/* PRIVATE FUNCTION DEFINITIONS                                               */
/******************************************************************************/

/** \brief  Gets the number of saved errors
 *
 *  \return  Number of saved errors (0, 1 or 2)
 */
static uint8_t DBGE_getNumberOfSavedErrors(void)
{
    uint8_t NumberOfErrors;

    if (DBGE_SavedErrors.LastError != DBGE_NO_ERROR)
    {
        NumberOfErrors = 2U;
    }
    else if (DBGE_SavedErrors.FirstError != DBGE_NO_ERROR)
    {
        NumberOfErrors = 1U;
    }
    else
    {
        NumberOfErrors = 0U;
    }

    return NumberOfErrors;
}

