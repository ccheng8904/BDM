/*******************************************************************************
 *  Copyright (c) 2024 BSH Hausgeraete GmbH,
 *  Carl-Wery-Str. 34, 81739 Munich, Germany, www.bsh-group.de
 *
 *  All rights reserved. This program and the accompanying materials
 *  are protected by international copyright laws.
 *  Please contact copyright holder for licensing information.
 *
 *******************************************************************************
 *  PROJECT          Generic SW
 ******************************************************************************/


#ifndef SYSTICK_STOPWATCH_H
#define SYSTICK_STOPWATCH_H

#ifdef __cplusplus
extern "C" {
#endif


/******************************************************************************/
/* DOCUMENTATION                                                              */
/******************************************************************************/
/** \file
 *
 *  \brief    Declarations of system tick stopwatch STSW for Tau.
 *
 */



/******************************************************************************/
/* INCLUDES                                                                   */
/******************************************************************************/
#include <stdint.h>


/******************************************************************************/
/* PUBLIC DEFINITIONS                                                         */
/******************************************************************************/

/******************************************************************************/
/* PUBLIC TYPE DEFINITIONS                                                    */
/******************************************************************************/
/** Time structure */
struct STSW_timeStruct
{
    uint16_t ns; ///< nano seconds
    uint16_t us; ///< micro seconds
    uint16_t ms; ///< milli seconds
    uint32_t s;  ///< seconds
};


/******************************************************************************/
/* PUBLIC FUNCTION DECLARATIONS                                               */
/******************************************************************************/
void STSW_ClockInit(void);
uint64_t STSW_getTime(void);
struct STSW_timeStruct STSW_getStopTime(uint64_t StartTime);
void STSW_InterruptHandler(void);



#ifdef __cplusplus
}
#endif

#endif // SYSTICK_STOPWATCH_H
