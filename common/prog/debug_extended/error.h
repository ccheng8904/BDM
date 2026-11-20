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

#ifndef DEBUG_EXTENDED_ERROR_H
#define DEBUG_EXTENDED_ERROR_H

/******************************************************************************/
/* DOCUMENTATION                                                              */
/******************************************************************************/
/** \file
 *
 *  \ingroup  debug_extended
 *
 *  \brief    Public definitions and declarations of generic part of the
 *            debug_extended component: Exceptional error memory
 *
 *  \details  
 */

/******************************************************************************/
/* INCLUDES                                                                   */
/******************************************************************************/
#include <stdint.h>
#include <stddef.h>
#include "hsup.h"
#include "sts_sim_test_uty.h"
#include "dbgx_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/******************************************************************************/
/* PUBLIC PREPROCESSOR DEFINITIONS                                            */
/******************************************************************************/

#define DBGE_NO_ERROR   ((uint32_t)0)

/******************************************************************************/
/* PUBLIC FUNCTION DECLARATIONS                                               */
/******************************************************************************/

/** \brief  Saves an error code in the exceptional error memory
 *  \note   Should not be called in an application outside of the DBGX
 *          component, use DBGX_handleErr... instead
 *  \note   (Error parameter value 0 appears nereby in the error memory)
 *
 *  \param  ErrorCode  Exceptional error code
 */
extern void DBGE_saveError(uint32_t ErrorCode);

/** \brief  Saves an error code with its parameter in the exceptional error memory
 *  \note   Should not be called in an application outside of the DBGX
 *          component, use DBGX_handleErr... instead
 *
 *  \param  ErrorCode       Exceptional error code
 *  \param  ErrorParameter  Parameter of the error code (e.g. a return value of a library function)
 */
extern void DBGE_saveErrorWithParam(uint32_t ErrorCode, uint32_t ErrorParameter);

/** \brief  Reads the first occurred error code with its parameter from the
 *          exceptional error memory
 *  \note   Should not be called directly in an application, use DBGX_readFirstError instead
 *
 *  \param  ErrorCode       Exceptional error code
 *  \param  ErrorParameter  Parameter of the error code (e.g. a return value of a library function)
 *  \return Number of saved errors (0, 1 or 2)
 */
extern uint8_t DBGE_readFirstError(uint32_t *ErrorCode, uint32_t *ErrorParameter);

/** \brief  Reads the last occurred error code with its parameter from the exceptional error memory
 *  \note   Should not be called directly in an application, use DBGX_readLastError instead
 *
 *  \param  ErrorCode       Exceptional error code
 *  \param  ErrorParameter  Parameter of the error code (e.g. a return value of a library function)
 *  \return Number of saved errors (0, 1 or 2)
 */
extern uint8_t DBGE_readLastError(uint32_t *ErrorCode, uint32_t *ErrorParameter);

/** \brief  Sets the exceptional error solving functions
 *  \note   Do not use this function in your application
 *
 *  \param  ErrorSolvingFunction  Pointer to the error solving functions
 *  \param  NumberOfErrorSolvingFunctions     Number of registered error callback functions
 *  \param  MaxNumberOfErrorSolvingFunctions  Maximum number of error callback functions
 */
extern void DBGX_setErrorSolvingFunctions(const DBGE_ErrorSolvingFunction_t *ErrorSolvingFunctionArray, const uint8_t *NumberOfErrorSolvingFunctions, uint8_t MaxNumberOfErrorSolvingFunctions);

/** \brief Callback function in case of exceptional errors
 *  \note  Will call user defined functions, according to DBGX_registerErrorSolvingFunction
 *
 *  \param ErrorCode       Error code of the exceptional error
 *  \param ErrorParameter  Error parameter of the exceptional error
 */
extern void DBGE_solveExceptionalError(uint32_t ErrorCode, uint32_t ErrorParameter);

#ifdef __cplusplus
}
#endif

#endif   /* From: ifndef DEBUG_EXTENDED_ERROR_H */

