/*******************************************************************************
*   Copyright (c) 2024 BSH Hausgeraete GmbH,
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
/** \file     int_handle_cfg.c
*
*   \brief    This file defines the interrupt functions.
*
*
*/

/******************************************************************************/
/* INCLUDES                                                                   */
/******************************************************************************/
#include "bsh_stdinc.h"
#include <stdint.h>
#include "processor.h"
#include "IntTbl.h"
#include "SegmentDef.h"

#ifdef MCAL_INCLUDED
#include "mcal/mcal_includes.h"
#endif

/******************************************************************************/
/* IRQ FUNCTIONS DEFINITIONS                                                  */
/******************************************************************************/

/************************ UART ************************/
SYMBOL_IRQ void UART0_IRQHandler(void)
{
  #ifdef MCAL_MUART_INCLUDED
    MUART_IRQ_Handler(MUART0_IRQ_INDEX);
  #endif
}


SYMBOL_IRQ void UART1_IRQHandler(void)
{
  #ifdef MCAL_MUART_INCLUDED
    MUART_IRQ_Handler(MUART1_IRQ_INDEX);
  #endif
}


/************************ I2C ************************/
SYMBOL_IRQ void I2C0_IRQHandler(void)
{
  #ifdef MCAL_MI2C_INCLUDED
    MI2C_IRQ_Handler(MI2C0_IRQ_INDEX);
  #endif
}


/************************ TIMER ************************/
SYMBOL_IRQ void TIMER0_IRQHandler(void)
{
  #ifdef MCAL_MTIM_INCLUDED
    MTIM_IRQ_Handler(MTIM_TMR0_IRQ_INDEX);
  #endif
}


SYMBOL_IRQ void TIMER1_IRQHandler(void)
{
  #ifdef MCAL_MTIM_INCLUDED
    MTIM_IRQ_Handler(MTIM_TMR1_IRQ_INDEX);
  #endif
}


SYMBOL_IRQ void TM00_IRQHandler(void)
{
  #ifdef MCAL_MTIM_INCLUDED
    MTIM_IRQ_Handler(MTIM_TM00_IRQ_INDEX);
  #endif
}


SYMBOL_IRQ void TM01_IRQHandler(void)
{
  #ifdef MCAL_MTIM_INCLUDED
    MTIM_IRQ_Handler(MTIM_TM01_IRQ_INDEX);
  #endif
}


SYMBOL_IRQ void TM02_IRQHandler(void)
{
  #ifdef MCAL_MTIM_INCLUDED
    MTIM_IRQ_Handler(MTIM_TM02_IRQ_INDEX);
  #endif
}


SYMBOL_IRQ void TM03_IRQHandler(void)
{
  #ifdef MCAL_MTIM_INCLUDED
    MTIM_IRQ_Handler(MTIM_TM03_IRQ_INDEX);
  #endif
}


SYMBOL_IRQ void TM01H_IRQHandler(void)
{
  #ifdef MCAL_MTIM_INCLUDED
    MTIM_IRQ_Handler(MTIM_TM01H_IRQ_INDEX);
  #endif
}


/************************ SPI ************************/
SYMBOL_IRQ void SSP0_IRQHandler(void)
{
  #ifdef MCAL_MSPI_INCLUDED
    MSPI_IRQ_Handler(MSPI0_IRQ_INDEX);
  #endif
}


/************************ ADC ************************/
SYMBOL_IRQ void ADC_IRQHandler(void)
{
  #ifdef MCAL_MADC_INCLUDED
    MADC_IRQ_Handler(MADC0_IRQ_INDEX);
  #endif
}


/************************ EXTI ************************/
SYMBOL_IRQ void INTP0_IRQHandler(void)
{
  #ifdef MCAL_MEXTI_INCLUDED
    MEXTI_IRQ_Handler(MEXTI0_IRQ_INDEX);
  #endif
}


SYMBOL_IRQ void INTP1_IRQHandler(void)
{
  #ifdef MCAL_MEXTI_INCLUDED
    MEXTI_IRQ_Handler(MEXTI1_IRQ_INDEX);
  #endif
}


SYMBOL_IRQ void INTP2_IRQHandler(void)
{
  #ifdef MCAL_MEXTI_INCLUDED
    MEXTI_IRQ_Handler(MEXTI2_IRQ_INDEX);
  #endif
}


SYMBOL_IRQ void INTP3_IRQHandler(void)
{
  #ifdef MCAL_MEXTI_INCLUDED
    MEXTI_IRQ_Handler(MEXTI3_IRQ_INDEX);
  #endif
}


/************************ Analog ************************/
SYMBOL_IRQ void ACMP_IRQHandler(void)
{
}


SYMBOL_IRQ void LVI_IRQHandler(void)
{
}


/************************ Timer / Motor control ***********/
SYMBOL_IRQ void CCP_IRQHandler(void)
{
}


SYMBOL_IRQ void EPWM_IRQHandler(void)
{
}


SYMBOL_IRQ void HALL_IRQHandler(void)
{
}


SYMBOL_IRQ void LSITIMER_IRQHandler(void)
{
}


/************************ Flash ************************/
SYMBOL_IRQ void FMC_IRQHandler(void)
{
}
