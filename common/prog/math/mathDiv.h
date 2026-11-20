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
 ******************************************************************************/


#ifndef MATHDIV_H
#define MATHDIV_H

#ifdef __cplusplus
extern "C" {
#endif


/******************************************************************************/
/* DOCUMENTATION                                                              */
/******************************************************************************/
/** \file
 *
 *  \ingroup  mathematics
 *
 *  \brief    Public definitions and declarations for unit math_div.
 *
 *  \details  soft divisions, if not provided by core.
 */



/******************************************************************************/
/* INCLUDES                                                                   */
/******************************************************************************/
#include "bsh_stdinc.h"
#include <stdint.h>
#include <stdbool.h>


/******************************************************************************/
/* PUBLIC DEFINITIONS                                                         */
/******************************************************************************/

/** \brief    Selection of MTDIV max divisor and result width.
 *
 *  \note     By default the MTDIV width is 32-bit, but can be reduced to 
 *            16-bit to save memory by defining MTDIV_WIDTH_16 if 32-bit
 *            division is not used.
 */
#ifndef MTDIV_WIDTH_16
#define MTDIV_WIDTH_32
#endif // MTDIV_WIDTH_16

/******************************************************************************/
/* PUBLIC TYPE DEFINITIONS                                                    */
/******************************************************************************/

/** \brief    Divisor and result type.
 *
 *  \note     Divisor and result type is dependent on MTDIV_WIDTH_xx setting.
 */
#ifdef MTDIV_WIDTH_16
typedef uint16_t    MTDIV_width_t;
#else
typedef uint32_t    MTDIV_width_t;
#endif


/** \brief    Return structure from generic division function.
 */
struct MTDIV_Result
{
    MTDIV_width_t quotient;
    MTDIV_width_t remainder;
};


/******************************************************************************/
/* PUBLIC FUNCTION DECLARATIONS                                               */
/******************************************************************************/

/** \brief    Main division and modulo function.
 *
 *  \note     This function performs division and modulo calculation and is
 *            intended mainly for internal use. Be careful when using this 
 *            function, as divisor and result width are dependent
 *            on MTDIV_WIDTH_xx setting. By calling this function you get
 *            either quotient or modulo, depending on div parameter.
 *
 *  \param    dividend:  the dividend
 *  \param    divisor:   the divider
 *
 *  \return   struct MTDIV_Result containing both quotient and remainder.
 */
extern struct MTDIV_Result MTDIV_div_mod(uint32_t dividend, MTDIV_width_t divisor);


/** \brief    Division 32bit by 16bit, result fits into 16bit
 *
 *  \note     Be careful, the maximum result is 0xFFFF. You should be sure, that
 *            your result fits into uint16_t.
 *
 *  \param    dividend:  the dividend
 *  \param    divisor:   the divider
 *
 *  \return   result of division
 */
static inline uint16_t MTDIV_div_32_16(uint32_t dividend, uint16_t divisor)
{
  #ifdef NO_CORE_DEVIDER
    return (uint16_t)MTDIV_div_mod(dividend, (MTDIV_width_t)divisor).quotient;
  #else
    return (uint16_t)(dividend / (uint32_t)divisor);
  #endif
}


/** \brief    Modulo 32bit by 16bit, result fits into 16bit
 *
 *  \note     Be careful, the maximum result is 0xFFFF. You should be sure, that
 *            your result fits into uint16_t.
 *
 *  \param    dividend:  the dividend
 *  \param    divisor:   the divider
 *
 *  \return   modulo of division
 */
static inline uint16_t MTDIV_mod_32_16(uint32_t dividend, uint16_t divisor)
{
  #ifdef NO_CORE_DEVIDER
    return (uint16_t)MTDIV_div_mod(dividend, divisor).remainder;
  #else
    return (uint16_t)(dividend % (uint32_t)divisor);
  #endif
}


#ifdef MTDIV_WIDTH_32
/** \brief    Division 32bit by 32bit, result is 32bit
 *
 *  \note     This function is optimized for 32bit microcontrollers.
 *            Division by 0 returns 0xFFFFFFFF.
 *
 *  \param    dividend:  the dividend
 *  \param    divisor:   the divider
 *
 *  \return   result of division
 */
static inline uint32_t MTDIV_div_32_32(uint32_t dividend, uint32_t divisor)
{
  #ifdef NO_CORE_DEVIDER
    return MTDIV_div_mod(dividend, divisor).quotient;
  #else
    return (uint32_t)(dividend / divisor);
  #endif
}


/** \brief    Modulo 32bit by 32bit, result is 32bit
 *
 *  \note     This function is optimized for 32bit microcontrollers.
 *
 *  \param    dividend:  the dividend
 *  \param    divisor:   the divider
 *
 *  \return   modulo of division
 */
static inline uint32_t MTDIV_mod_32_32(uint32_t dividend, uint32_t divisor)
{
  #ifdef NO_CORE_DEVIDER
    return MTDIV_div_mod(dividend, divisor).remainder;
  #else
    return (uint32_t)(dividend % divisor);
  #endif
    
}
#endif // MTDIV_WIDTH_32

#ifdef __cplusplus
}
#endif

#endif // MATHDIV_H
