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


/******************************************************************************/
/* DOCUMENTATION                                                              */
/******************************************************************************/
/** \file
 *
 *  \brief    Implementation of division (if not provided by core).
 *
 */


/******************************************************************************/
/* INCLUDES                                                                   */
/******************************************************************************/
#include "mathDiv.h"




/******************************************************************************/
/* PUBLIC FUNCTION DEFINITIONS                                                */
/******************************************************************************/

struct MTDIV_Result MTDIV_div_mod(uint32_t dividend, MTDIV_width_t divisor)
{
    struct MTDIV_Result result = {0u, 0u};
    
#ifndef NO_CORE_DEVIDER
    result.quotient = (MTDIV_width_t)(dividend / divisor);
    result.remainder =  (MTDIV_width_t)(dividend % divisor);
#else

  #ifdef MTDIV_WIDTH_16
    uint32_t tmp = (uint32_t)divisor << 15u;
    uint16_t bitmask = (uint16_t)1u << 15u;
  #else
    uint64_t tmp = (uint64_t)divisor << 31u;
    uint32_t bitmask = (uint32_t)1u << 31u;
  #endif
    
    for(; bitmask > 0u; bitmask >>= 1u)
    {
        if(dividend >= tmp)
        {
            result.quotient += bitmask;
            dividend -= (MTDIV_width_t)tmp;
        }
        tmp >>= 1u;
    }
    
    // remainder is leftover from dividend
    result.remainder = dividend;
    
#endif // NO_CORE_DEVIDER

    // Return quotient/remainder
    return result;
}

