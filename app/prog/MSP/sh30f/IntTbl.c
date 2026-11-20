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

/**************************************************************************************************/
/* DOCUMENTATION                                                                                  */
/**************************************************************************************************/
/** \file    IntTbl.c
*
*   \brief    This file defines the interrupt table that contains the ISR addresses.
*
*   \details  This file was created automatically, no manual changes should be done in this file.
*             If required database or code generator must be adapted
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

/*
Memory mapping of the interrupt vector table that is partioned
    | ---------------------------------- |\
    |         __VectorsFixed[4]          | \
    |                                    |  >  Fixed Vectors (16 Bytes)
    |       0x0000000 - 0x000000F        | /   in file IntTblArmCM.c
    | ---------------------------------- |/
    |             Reserved               |
    |         for ROM Bootloader         |
    |        0x0000000 - 0x00007FF       |
    | ---------------------------------- |

    | ---------------------------------- |
    |        ITBL_coreVectors[16]        |
    |         moved Vector Table         |
    |  (0x0000000 - 0x000003F) + Offset  |
    | ---------------------------------- |

    | ---------------------------------- |
    |     ITBL_peripheralVectors[n]      |
    |         moved Vector Table         |
    |  (0x0000040 - 0x00000C0) + Offset  |
    | ---------------------------------- |
*/

/* Interrupt Vector table for SH30F9X71 */

/* Axivion Disable Style MisraC2012-8.4 : Missing declaration for function definition */
SDEF_SetSegmentConst(vectors)
const ITBL_fptr SYMBOL_USED ITBL_peripheralVectors[]   =
{
/* Axivion Enable Style MisraC2012-8.4 */ 

// ||Handler function name||           ||POSITION||     ||Priority||     ||Address offset||
   /*lint -e{923}  cast tested to work correctly for ARM CortexMx */
#if defined(SH30F9X71)
   WWDT_IRQHandler              ,     //  0               7                 0x0040
   BOD_IRQHandler               ,     //  1               8                 0x0044
   RCC_IRQHandler               ,     //  2               9                 0x0048
   EXTI0_IRQHandler             ,     //  3              10                 0x004C
   EXTI1_IRQHandler             ,     //  4              11                 0x0050
   EXTI2_IRQHandler             ,     //  5              12                 0x0054
   EXTI3_IRQHandler             ,     //  6              13                 0x0058
   DMA_CH0_1_IRQHandler         ,     //  7              14                 0x005C
   DMA_CH2_3_IRQHandler         ,     //  8              15                 0x0060
   ADC_IRQHandler               ,     //  9              16                 0x0064
   PWM0_IRQHandler              ,     // 10              17                 0x0068
   PWM1_IRQHandler              ,     // 11              18                 0x006C
   PWM2_IRQHandler              ,     // 12              19                 0x0070
   PWM3_IRQHandler              ,     // 13              20                 0x0074
   PCA0_IRQHandler              ,     // 14              21                 0x0078
   PCA1_IRQHandler              ,     // 15              22                 0x007C
   PCA2_IRQHandler              ,     // 16              23                 0x0080
   PCA3_IRQHandler              ,     // 17              24                 0x0084
   EXTI9_4_IRQHandler           ,     // 18              25                 0x0088
   UART0_IRQHandler             ,     // 19              26                 0x008C
   TWI0_IRQHandler              ,     // 20              27                 0x0090
   SPI0_IRQHandler              ,     // 21              28                 0x0094
   SPI1_LED_IRQHandler          ,     // 22              29                 0x0098
   UART1_IRQHandler             ,     // 23              30                 0x009C
   UART2_IRQHandler             ,     // 24              31                 0x00A0
   UART3_IRQHandler             ,     // 25              32                 0x00A4
   EXTI15_10_IRQHandler         ,     // 26              33                 0x00A8
   TIM0_IRQHandler              ,     // 27              34                 0x00AC
   TIM1_IRQHandler              ,     // 28              35                 0x00B0
   TIM2_IRQHandler              ,     // 29              36                 0x00B4
   TIM3_IRQHandler              ,     // 30              37                 0x00B8
   TOUCH_KEY_IRQHandler         ,     // 31              38                 0x00BC 
#endif
};
SDEF_SetSegmentConst_Default()

/* Function's task is to set the interrupt priorities according to the user's needs, different from the default settings.
 * If necessary, uncomment the relevant lines and set the required interrupt priorities
 */
void ITBL_setCustomInterruptPriority(void)
{
    /* Cortex-M0+ settable interrupts */
    // NVIC_SetPriority(SVC_IRQn, PRIORITY_HIGH);
    // NVIC_SetPriority(PendSV_IRQn, PRIORITY_HIGH);
    // NVIC_SetPriority(SysTick_IRQn, PRIORITY_HIGH);
    /* Peripheral interrupts */
    // NVIC_SetPriority(WWDT_IRQn, PRIORITY_MEDIUM);
    // NVIC_SetPriority(BOD_IRQn, PRIORITY_MEDIUM);
    // NVIC_SetPriority(RCC_IRQn, PRIORITY_MEDIUM);
    // NVIC_SetPriority(EXTI0_IRQn, PRIORITY_MEDIUM);
    // NVIC_SetPriority(EXTI1_IRQn, PRIORITY_MEDIUM);
    // NVIC_SetPriority(EXTI2_IRQn, PRIORITY_MEDIUM);
    // NVIC_SetPriority(EXTI3_IRQn, PRIORITY_MEDIUM);
    // NVIC_SetPriority(DMA_CH0_1_IRQn, PRIORITY_MEDIUM);
    // NVIC_SetPriority(DMA_CH2_3_IRQn, PRIORITY_MEDIUM);
    // NVIC_SetPriority(ADC_IRQn, PRIORITY_MEDIUM);
    // NVIC_SetPriority(PWM0_IRQn, PRIORITY_MEDIUM);
    // NVIC_SetPriority(PWM1_IRQn, PRIORITY_MEDIUM);
    // NVIC_SetPriority(PWM2_IRQn, PRIORITY_MEDIUM);
    // NVIC_SetPriority(PWM3_IRQn, PRIORITY_MEDIUM);
    // NVIC_SetPriority(PCA0_IRQn, PRIORITY_MEDIUM);
    // NVIC_SetPriority(PCA1_IRQn, PRIORITY_MEDIUM);
    // NVIC_SetPriority(PCA2_IRQn, PRIORITY_MEDIUM);
    // NVIC_SetPriority(PCA3_IRQn, PRIORITY_MEDIUM);
    // NVIC_SetPriority(EXTI9_4_IRQn, PRIORITY_MEDIUM);
    // NVIC_SetPriority(UART0_IRQn, PRIORITY_MEDIUM);
    // NVIC_SetPriority(TWI0_IRQn, PRIORITY_MEDIUM);
    // NVIC_SetPriority(SPI0_IRQn, PRIORITY_MEDIUM);
    // NVIC_SetPriority(SPI1_LED_IRQn, PRIORITY_MEDIUM);
    // NVIC_SetPriority(UART1_IRQn, PRIORITY_MEDIUM);
    // NVIC_SetPriority(UART2_IRQn, PRIORITY_MEDIUM);
    // NVIC_SetPriority(UART3_IRQn, PRIORITY_MEDIUM);
    // NVIC_SetPriority(EXTI15_10_IRQn, PRIORITY_MEDIUM);
    // NVIC_SetPriority(TIM0_IRQn, PRIORITY_MEDIUM);
    // NVIC_SetPriority(TIM1_IRQn, PRIORITY_LOW);
    // NVIC_SetPriority(TIM2_IRQn, PRIORITY_LOW);
    // NVIC_SetPriority(TIM3_IRQn, PRIORITY_LOW);
    // NVIC_SetPriority(TouchKey_IRQn, PRIORITY_LOW);
}
