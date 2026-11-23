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
*   PROCESSOR        CMS32M67xx
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
#ifdef CMS32M67XX
extern void LVI_IRQHandler                    (void)   __attribute__((weak)) __attribute__((alias("HWVT_vSpuriousIsr")));
extern void INTP0_IRQHandler                  (void)   __attribute__((weak)) __attribute__((alias("HWVT_vSpuriousIsr")));
extern void INTP1_IRQHandler                  (void)   __attribute__((weak)) __attribute__((alias("HWVT_vSpuriousIsr")));
extern void INTP2_IRQHandler                  (void)   __attribute__((weak)) __attribute__((alias("HWVT_vSpuriousIsr")));
extern void INTP3_IRQHandler                  (void)   __attribute__((weak)) __attribute__((alias("HWVT_vSpuriousIsr")));
extern void TM01H_IRQHandler                  (void)   __attribute__((weak)) __attribute__((alias("HWVT_vSpuriousIsr")));
extern void CCP_IRQHandler                    (void)   __attribute__((weak)) __attribute__((alias("HWVT_vSpuriousIsr")));
extern void ACMP_IRQHandler                   (void)   __attribute__((weak)) __attribute__((alias("HWVT_vSpuriousIsr")));
extern void ADC_IRQHandler                    (void)   __attribute__((weak)) __attribute__((alias("HWVT_vSpuriousIsr")));
extern void SSP0_IRQHandler                   (void)   __attribute__((weak)) __attribute__((alias("HWVT_vSpuriousIsr")));
extern void I2C0_IRQHandler                   (void)   __attribute__((weak)) __attribute__((alias("HWVT_vSpuriousIsr")));
extern void UART0_IRQHandler                  (void)   __attribute__((weak)) __attribute__((alias("HWVT_vSpuriousIsr")));
extern void UART1_IRQHandler                  (void)   __attribute__((weak)) __attribute__((alias("HWVT_vSpuriousIsr")));
extern void EPWM_IRQHandler                   (void)   __attribute__((weak)) __attribute__((alias("HWVT_vSpuriousIsr")));
extern void TIMER0_IRQHandler                 (void)   __attribute__((weak)) __attribute__((alias("HWVT_vSpuriousIsr")));
extern void TIMER1_IRQHandler                 (void)   __attribute__((weak)) __attribute__((alias("HWVT_vSpuriousIsr")));
extern void HALL_IRQHandler                   (void)   __attribute__((weak)) __attribute__((alias("HWVT_vSpuriousIsr")));
extern void LSITIMER_IRQHandler               (void)   __attribute__((weak)) __attribute__((alias("HWVT_vSpuriousIsr")));
extern void TM00_IRQHandler                   (void)   __attribute__((weak)) __attribute__((alias("HWVT_vSpuriousIsr")));
extern void TM01_IRQHandler                   (void)   __attribute__((weak)) __attribute__((alias("HWVT_vSpuriousIsr")));
extern void TM02_IRQHandler                   (void)   __attribute__((weak)) __attribute__((alias("HWVT_vSpuriousIsr")));
extern void TM03_IRQHandler                   (void)   __attribute__((weak)) __attribute__((alias("HWVT_vSpuriousIsr")));
extern void FMC_IRQHandler                    (void)   __attribute__((weak)) __attribute__((alias("HWVT_vSpuriousIsr")));
/* Axivion Enable Style MisraC2012-8.6 */ 

#endif


#ifdef __cplusplus
}
#endif

#endif
