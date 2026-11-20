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
*   PROCESSOR        SH30F9X71

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


SYMBOL_IRQ void UART2_IRQHandler(void)
{
  #ifdef MCAL_MUART_INCLUDED
    MUART_IRQ_Handler(MUART2_IRQ_INDEX);
  #endif
}


SYMBOL_IRQ void UART3_IRQHandler(void)
{
  #ifdef MCAL_MUART_INCLUDED
    MUART_IRQ_Handler(MUART3_IRQ_INDEX);
  #endif
}


/************************ I2C ************************/
void TWI0_IRQHandler(void)
{
  #ifdef MCAL_MI2C_INCLUDED
    MI2C_IRQ_Handler(MI2C0_IRQ_INDEX);
  #endif
}


/************************ TIMER ************************/
SYMBOL_IRQ void PCA0_IRQHandler(void)
{
  #ifdef MCAL_MTIM_INCLUDED
    MTIM_IRQ_Handler(MTIM_PCA0_IRQ_INDEX);
  #endif
}


SYMBOL_IRQ void PCA1_IRQHandler(void)
{
  #ifdef MCAL_MTIM_INCLUDED
    MTIM_IRQ_Handler(MTIM_PCA1_IRQ_INDEX);
  #endif
}


SYMBOL_IRQ void PCA2_IRQHandler(void)
{
  #ifdef MCAL_MTIM_INCLUDED
    MTIM_IRQ_Handler(MTIM_PCA2_IRQ_INDEX);
  #endif
}


SYMBOL_IRQ void PCA3_IRQHandler(void)
{
  #ifdef MCAL_MTIM_INCLUDED
    MTIM_IRQ_Handler(MTIM_PCA3_IRQ_INDEX);
  #endif
}


/************************ SPI ************************/
void SPI0_IRQHandler(void)
{
  #ifdef MCAL_MSPI_INCLUDED
    MSPI_IRQ_Handler(MSPI0_IRQ_INDEX);
  #endif
}


void SPI1_LED_IRQHandler(void)
{
  #ifdef MCAL_MSPI_INCLUDED
    MSPI_IRQ_Handler(MSPI1_IRQ_INDEX);
  #endif
}


/************************ ADC ************************/
void ADC_IRQHandler(void)
{
  #ifdef MCAL_MADC_INCLUDED
    MADC_IRQ_Handler(MADC0_IRQ_INDEX);
  #endif
}


/************************ EXTI ************************/
void EXTI0_IRQHandler(void)
{
  #ifdef MCAL_MEXTI_INCLUDED
    MEXTI_IRQ_Handler(MEXTI0_IRQ_INDEX);
  #endif
}


void EXTI1_IRQHandler(void)
{
  #ifdef MCAL_MEXTI_INCLUDED
    MEXTI_IRQ_Handler(MEXTI1_IRQ_INDEX);
  #endif
}


void EXTI2_IRQHandler(void)
{
  #ifdef MCAL_MEXTI_INCLUDED
    MEXTI_IRQ_Handler(MEXTI2_IRQ_INDEX);
  #endif
}


void EXTI3_IRQHandler(void)
{
  #ifdef MCAL_MEXTI_INCLUDED
    MEXTI_IRQ_Handler(MEXTI3_IRQ_INDEX);
  #endif
}


void EXTI9_4_IRQHandler(void)
{
  #ifdef MCAL_MEXTI_INCLUDED
    MEXTI_IRQ_Handler(MEXTI9_4_IRQ_INDEX);
  #endif
}


void EXTI15_10_IRQHandler(void)
{
  #ifdef MCAL_MEXTI_INCLUDED
    MEXTI_IRQ_Handler(MEXTI15_10_IRQ_INDEX);
  #endif
}


/************************ DMA ************************/
void DMA_CH0_1_IRQHandler(void)
{
  #ifdef MCAL_MDMA_INCLUDED
    MDMA_IRQ_Handler(MDMA1_IRQ_INDEX);
  #endif
}


void DMA_CH2_3_IRQHandler(void)
{
  #ifdef MCAL_MDMA_INCLUDED
    MDMA_IRQ_Handler(MDMA1_IRQ_INDEX);
  #endif
}
