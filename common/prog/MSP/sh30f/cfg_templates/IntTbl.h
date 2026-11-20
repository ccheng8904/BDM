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
*   PROCESSOR        SH30F9X71
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

/* Platfom specific IRQs - sh30f */
/* Axivion Disable MisraC2012-8.6 : Type, object or function without definition */
/* Axivion Disable MisraC2012-8.5 : Function declared in more than one file. */

#ifdef SH30F9X71
extern void WWDT_IRQHandler(void);
extern void BOD_IRQHandler(void);
extern void RCC_IRQHandler(void);
extern void EXTI0_IRQHandler(void);
extern void EXTI1_IRQHandler(void);
extern void EXTI2_IRQHandler(void);
extern void EXTI3_IRQHandler(void);
extern void DMA_CH0_1_IRQHandler(void);
extern void DMA_CH2_3_IRQHandler(void);
extern void ADC_IRQHandler(void);
extern void PWM0_IRQHandler(void);
extern void PWM1_IRQHandler(void);
extern void PWM2_IRQHandler(void);
extern void PWM3_IRQHandler(void);
extern void PCA0_IRQHandler(void);
extern void PCA1_IRQHandler(void);
extern void PCA2_IRQHandler(void);
extern void PCA3_IRQHandler(void);
extern void EXTI9_4_IRQHandler(void);
extern void UART0_IRQHandler(void);
extern void TWI0_IRQHandler(void);
extern void SPI0_IRQHandler(void);
extern void SPI1_LED_IRQHandler(void);
extern void UART1_IRQHandler(void);
extern void UART2_IRQHandler(void);
extern void UART3_IRQHandler(void);
extern void EXTI15_10_IRQHandler(void);
extern void TIM0_IRQHandler(void);
extern void TIM1_IRQHandler(void);
extern void TIM2_IRQHandler(void);
extern void TIM3_IRQHandler(void);
extern void TOUCH_KEY_IRQHandler(void);
#endif

extern void ITBL_setCustomInterruptPriority (void);
/* Axivion Enable Style MisraC2012-8.6 */ 
/* Axivion Enable Style MisraC2012-8.5 */ 

#ifdef __cplusplus
}
#endif

#endif //INTTBL_H
