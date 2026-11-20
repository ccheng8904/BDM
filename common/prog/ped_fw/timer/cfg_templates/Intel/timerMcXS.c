/*******************************************************************************
 *  Copyright (c) 2017 BSH Hausgeraete GmbH,
 *  Carl-Wery-Str. 34, 81739 Munich, Germany, www.bsh-group.de
 *
 *  All rights reserved. This program and the accompanying materials
 *  are protected by international copyright laws.
 *  Please contact copyright holder for licensing information.
 *
 *******************************************************************************
 *  PROJECT          PP PED Framework
 *  COMP_ABBREV      TIM
 ******************************************************************************/


/******************************************************************************/
/* DOCUMENTATION                                                              */
/******************************************************************************/
/** \file
    \brief Intel specific configuration template for timer library.

*/

/******************************************************************************/
/* INCLUDES                                                                   */
/******************************************************************************/
#include "bsh_stdinc.h"
#include "LibTypes.h"
#include "timerlib.h"
#include <sys/time.h>

BOOL _TIM_bGetInitConfig(void)
{
    return TRUE;
}

uint16_t TIM_uiGetCircleMicroSeconds(void)
{
    struct timeval timer_usec; 
    uint64_t timestamp_usec;
    
    if (!gettimeofday(&timer_usec, NULL))
    {
        timestamp_usec = ((uint64_t) timer_usec.tv_sec) * 1000000ll + 
            (uint64_t) timer_usec.tv_usec;
            
        return (uint16_t)timestamp_usec;
    }
    else
    {
        return 0u;
    }
}
