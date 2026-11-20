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
 *  COMP_ABBREV      MCAL
 ******************************************************************************/


#ifndef MCAL_ASSERT_H
#define MCAL_ASSERT_H


/******************************************************************************/
/* DOCUMENTATION                                                              */
/******************************************************************************/
/** \file
 *
 *  \ingroup  MCAL
 *
 *  \brief    Public definitions and declarations for unit MCAL.
 *
 */


/******************************************************************************/
/* INCLUDES                                                                   */
/******************************************************************************/
#include "BSH_stdinc.h"

// Test if debug is switched off for MCAL or its modules
#if defined(MCAL_NDEBUG)
    #define NDEBUG
#endif

// Include assert library after initial debug checks
#include <assert.h>
/*Note 9026: boolean condition always evaluates to 'true/false' [MISRA 2012 Directive 4.9, advisory] */
/*lint -emacro(774,MCAL_RETURN_ON_ERROR) */


/* Note 9026: Function-like macro, '...', defined [MISRA 2012 Directive 4.9, advisory] */
/* Note 9024: '#/##' operator used in macro '...' [MISRA 2012 Rule 20.10, advisory] */

/*lint -save -e9026 -e9024 */

/** \brief    Macro for checking return value of function.
 *
 *  \details  If function does not return MCAL_OK, parent function is exited
 *            with the same error value
 */
#define MCAL_RETURN_ON_ERROR(function) do {enum MCAL_Error ret_val = (function); if(ret_val != MCAL_OK) {return ret_val;}} while(false);//lint !e823 "definition of macro ends in semi-colon"


#define MCAL_ASSERT_ON_ERROR(function) do {if((function) != MCAL_OK) {MCAL_error(#function);}} while(false);//lint !e823 "definition of macro ends in semi-colon"


#if (defined(NDEBUG) || defined(__NOT_CC__))

    #define MCAL_error(ignore)  ((void)0)

    #define MCAL_assert(ignore) ((void)0)

#elif defined (__ARMCC_VERSION)

    #define MCAL_error(__e)     __CLIBNS __aeabi_assert(__e, __FILE__, __LINE__)

    #define MCAL_assert(__e)    ((__e) ? (void)0 : __CLIBNS __aeabi_assert(#__e, __FILE__, __LINE__))

#elif defined (__GNUC__)

    #define MCAL_error(__e)     __assert_func(__FILE__, __LINE__, __ASSERT_FUNC, __e)

    #define MCAL_assert(__e)    ((__e) ? ((void)0) : __assert_func(__FILE__, __LINE__, __ASSERT_FUNC, #__e))

#endif

/*lint -restore -e9026 -e9024 */

#endif // MCAL_ASSERT_H
