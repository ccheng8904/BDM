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
*   COMP_ABBREV      HWVT
*   PROCESSOR        SH30F9X71
*******************************************************************************/


/******************************************************************************/
/* DOCUMENTATION                                                              */
/******************************************************************************/
/** \file     weakvectbl.h
*
*   \brief    Declaration of weak functions for Vector Table.
*
*
*/

#ifndef WEAKVECTBL_H
#define WEAKVECTBL_H

#ifdef __cplusplus
extern "C" {
#endif

/******************************************************************************/
/* INCLUDES                                                                   */
/******************************************************************************/
#include "bsh_stdinc.h"

extern void HWVT_vSpuriousIsr(void);

/* definitions of peripheral interrupts */
/*Axivion Disable MisraC2012-8.6 : Type, object or function without definition */
#ifdef SH30F9X71
extern void WWDT_IRQHandler                   (void)   __attribute__((weak)) __attribute__((alias("HWVT_vSpuriousIsr")));
extern void BOD_IRQHandler                    (void)   __attribute__((weak)) __attribute__((alias("HWVT_vSpuriousIsr")));
extern void RCC_IRQHandler                    (void)   __attribute__((weak)) __attribute__((alias("HWVT_vSpuriousIsr")));
extern void EXTI0_IRQHandler                  (void)   __attribute__((weak)) __attribute__((alias("HWVT_vSpuriousIsr")));
extern void EXTI1_IRQHandler                  (void)   __attribute__((weak)) __attribute__((alias("HWVT_vSpuriousIsr")));
extern void EXTI2_IRQHandler                  (void)   __attribute__((weak)) __attribute__((alias("HWVT_vSpuriousIsr")));
extern void EXTI3_IRQHandler                  (void)   __attribute__((weak)) __attribute__((alias("HWVT_vSpuriousIsr")));
extern void DMA_CH0_1_IRQHandler              (void)   __attribute__((weak)) __attribute__((alias("HWVT_vSpuriousIsr")));
extern void DMA_CH2_3_IRQHandler              (void)   __attribute__((weak)) __attribute__((alias("HWVT_vSpuriousIsr")));
extern void ADC_IRQHandler                    (void)   __attribute__((weak)) __attribute__((alias("HWVT_vSpuriousIsr")));
extern void PWM0_IRQHandler                   (void)   __attribute__((weak)) __attribute__((alias("HWVT_vSpuriousIsr")));
extern void PWM1_IRQHandler                   (void)   __attribute__((weak)) __attribute__((alias("HWVT_vSpuriousIsr")));
extern void PWM2_IRQHandler                   (void)   __attribute__((weak)) __attribute__((alias("HWVT_vSpuriousIsr")));
extern void PWM3_IRQHandler                   (void)   __attribute__((weak)) __attribute__((alias("HWVT_vSpuriousIsr")));
extern void PCA0_IRQHandler                   (void)   __attribute__((weak)) __attribute__((alias("HWVT_vSpuriousIsr")));
extern void PCA1_IRQHandler                   (void)   __attribute__((weak)) __attribute__((alias("HWVT_vSpuriousIsr")));
extern void PCA2_IRQHandler                   (void)   __attribute__((weak)) __attribute__((alias("HWVT_vSpuriousIsr")));
extern void PCA3_IRQHandler                   (void)   __attribute__((weak)) __attribute__((alias("HWVT_vSpuriousIsr")));
extern void EXTI9_4_IRQHandler                (void)   __attribute__((weak)) __attribute__((alias("HWVT_vSpuriousIsr")));
extern void UART0_IRQHandler                  (void)   __attribute__((weak)) __attribute__((alias("HWVT_vSpuriousIsr")));
extern void TWI0_IRQHandler                   (void)   __attribute__((weak)) __attribute__((alias("HWVT_vSpuriousIsr")));
extern void SPI0_IRQHandler                   (void)   __attribute__((weak)) __attribute__((alias("HWVT_vSpuriousIsr")));
extern void SPI1_LED_IRQHandler               (void)   __attribute__((weak)) __attribute__((alias("HWVT_vSpuriousIsr")));
extern void UART1_IRQHandler                  (void)   __attribute__((weak)) __attribute__((alias("HWVT_vSpuriousIsr")));
extern void UART2_IRQHandler                  (void)   __attribute__((weak)) __attribute__((alias("HWVT_vSpuriousIsr")));
extern void UART3_IRQHandler                  (void)   __attribute__((weak)) __attribute__((alias("HWVT_vSpuriousIsr")));
extern void EXTI15_10_IRQHandler              (void)   __attribute__((weak)) __attribute__((alias("HWVT_vSpuriousIsr")));
extern void TIM0_IRQHandler                   (void)   __attribute__((weak)) __attribute__((alias("HWVT_vSpuriousIsr")));
extern void TIM1_IRQHandler                   (void)   __attribute__((weak)) __attribute__((alias("HWVT_vSpuriousIsr")));
extern void TIM2_IRQHandler                   (void)   __attribute__((weak)) __attribute__((alias("HWVT_vSpuriousIsr")));
extern void TIM3_IRQHandler                   (void)   __attribute__((weak)) __attribute__((alias("HWVT_vSpuriousIsr")));
extern void TOUCH_KEY_IRQHandler              (void)   __attribute__((weak)) __attribute__((alias("HWVT_vSpuriousIsr")));
/* Axivion Enable Style MisraC2012-8.6 */ 

#endif


#ifdef __cplusplus
}
#endif

#endif
