/*******************************************************************************
*   Copyright (c) 2023 BSH Hausgeraete GmbH,
*   Carl-Wery-Str. 34, 81739 Munich, Germany, www.bsh-group.de
*
*   All rights reserved. This program and the accompanying materials
*   are protected by international copyright laws.
*   Please contact copyright holder for licensing information.
*
********************************************************************************
*   PROJECT          Generic SW
*   PROCESSOR        CMS32M67xx
*******************************************************************************/

/******************************************************************************/
/* DOCUMENTATION                                                              */
/******************************************************************************/
/** \file    IntTbl.h
*
*   \brief    external declarations for interrupt service routines.
*
*   \details  Definitions of features for HAL. Include header files here to define compiler defines that are used
*             in the IntTbl.c file in framework to override default ISR.
*
*/

#ifndef INTTBL_H
#define INTTBL_H

#ifdef __cplusplus
extern "C" {
#endif

/******************************************************************************/
/* INCLUDES                                                                   */
/******************************************************************************/
#include "bsh_stdinc.h"
#include "IntTblArmCM.h"
#include "int_prio.h"


/* uncomment if necessary */
// #define PRIORITY_HIGH    INT_PRIO_0
// #define PRIORITY_MEDIUM  INT_PRIO_1
// #define PRIORITY_LOW     INT_PRIO_2
#if defined(RTOS)
#define PRIORITY_RTOS_SERVICES_LOW     INT_PRIO_2
#define PRIORITY_RTOS_SERVICES_LOWEST  INT_PRIO_3    /* Reserved for RTOS only */
#define RTOS_SVCALL_IRQN_NAME          SVCall_IRQn
#endif

/* Platform specific IRQs - cms32m67 */
/* Axivion Disable MisraC2012-8.6 : Type, object or function without definition */
/* Axivion Disable MisraC2012-8.5 : Function declared in more than one file. */

#ifdef CMS32M67XX
extern void LVI_IRQHandler(void);
extern void INTP0_IRQHandler(void);
extern void INTP1_IRQHandler(void);
extern void INTP2_IRQHandler(void);
extern void INTP3_IRQHandler(void);
extern void TM01H_IRQHandler(void);
extern void CCP_IRQHandler(void);
extern void ACMP_IRQHandler(void);
extern void ADC_IRQHandler(void);
extern void SSP0_IRQHandler(void);
extern void I2C0_IRQHandler(void);
extern void UART0_IRQHandler(void);
extern void UART1_IRQHandler(void);
extern void EPWM_IRQHandler(void);
extern void TIMER0_IRQHandler(void);
extern void TIMER1_IRQHandler(void);
extern void HALL_IRQHandler(void);
extern void LSITIMER_IRQHandler(void);
extern void TM00_IRQHandler(void);
extern void TM01_IRQHandler(void);
extern void TM02_IRQHandler(void);
extern void TM03_IRQHandler(void);
extern void FMC_IRQHandler(void);
#endif

extern void ITBL_setCustomInterruptPriority (void);
/* Axivion Enable Style MisraC2012-8.6 */ 
/* Axivion Enable Style MisraC2012-8.5 */ 

#ifdef __cplusplus
}
#endif

#endif //INTTBL_H
