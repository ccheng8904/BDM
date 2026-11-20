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
 *  COMP_ABBREV      STK
 ******************************************************************************/


#ifndef STACK_MONITOR_H
#define STACK_MONITOR_H

#ifdef __cplusplus
extern "C" {
#endif


/******************************************************************************/
/* DOCUMENTATION                                                              */
/******************************************************************************/
/** \file  stack_monitor.h
 *
 *  \ingroup  stack_monitor
 *
 *  \brief    Public definitions and declarations for unit stack_monitor.
 *
 *  \details  User can read out percentage of stack usage via 
 *            variable uint32_t STK_StackConsumption by e.g. Edith
 */


/******************************************************************************/
/* INCLUDES                                                                   */
/******************************************************************************/
#include "bsh_stdinc.h"
#include <stdbool.h>


/******************************************************************************/
/* PUBLIC DEFINITIONS                                                         */
/******************************************************************************/
#define DEP_STK  ///< dependencies


  
/******************************************************************************/
/* PUBLIC TYPE DEFINITIONS                                                    */
/******************************************************************************/



/******************************************************************************/
/* PUBLIC FUNCTION DEFINITIONS                                                */
/******************************************************************************/

/** Task handler of stack_monitor component.
 *  \note This function is called in task scheduler.
 *
 *  \return TASK_INITIALISED
 */
extern uint8_t STK_HandleTask(void);


/** Get remaining bytes of STACK.
 *  \note This function returns number of unused bytes in the STACK.
 *
 *  \param isUserStack: only used for RX platform, true for user stack,
 *                      false for interrupt stack. Ignored for other platforms.
 *  \return number of bytes
 */
extern uint32_t STK_getStackRemainingBytes(const bool isUserStack);

#ifdef __cplusplus
}
#endif

#endif // STACK_MONITOR_H
