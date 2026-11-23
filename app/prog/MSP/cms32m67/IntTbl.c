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
    |        0x0000000 - 0x00000BF       |
    | ---------------------------------- |
    |        User Option Bytes           |
    |        0x0000000 - 0x00000C3       |
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

/* Interrupt Vector table for CMS32M67xx */

/* Axivion Disable Style MisraC2012-8.4 : Missing declaration for function definition */
SDEF_SetSegmentConst(vectors)
const ITBL_fptr SYMBOL_USED ITBL_peripheralVectors[]   =
{
/* Axivion Enable Style MisraC2012-8.4 */ 

// ||Handler function name||           ||POSITION||     ||Priority||     ||Address offset||
   /*lint -e{923}  cast tested to work correctly for ARM CortexMx */
#if defined(CMS32M67XX)
   LVI_IRQHandler               ,     //  0               7                 0x0040
   INTP0_IRQHandler             ,     //  1               8                 0x0044
   INTP1_IRQHandler             ,     //  2               9                 0x0048
   INTP2_IRQHandler             ,     //  3              10                 0x004C
   INTP3_IRQHandler             ,     //  4              11                 0x0050
   TM01H_IRQHandler             ,     //  5              12                 0x0054
   CCP_IRQHandler               ,     //  6              13                 0x0058
   ACMP_IRQHandler              ,     //  7              14                 0x005C
   ADC_IRQHandler               ,     //  8              15                 0x0060
   SSP0_IRQHandler              ,     //  9              16                 0x0064
   I2C0_IRQHandler              ,     // 10              17                 0x0068
   UART0_IRQHandler             ,     // 11              18                 0x006C
   UART1_IRQHandler             ,     // 12              19                 0x0070
   EPWM_IRQHandler              ,     // 13              20                 0x0074
   TIMER0_IRQHandler            ,     // 14              21                 0x0078
   TIMER1_IRQHandler            ,     // 15              22                 0x007C
   HALL_IRQHandler              ,     // 16              23                 0x0080
   LSITIMER_IRQHandler          ,     // 17              24                 0x0084
   ITBL_vSpuriousIsr            ,     // 18              25                 0x0088 (reserved)
   TM00_IRQHandler              ,     // 19              26                 0x008C
   TM01_IRQHandler              ,     // 20              27                 0x0090
   TM02_IRQHandler              ,     // 21              28                 0x0094
   TM03_IRQHandler              ,     // 22              29                 0x0098
   ITBL_vSpuriousIsr            ,     // 23              30                 0x009C (reserved)
   ITBL_vSpuriousIsr            ,     // 24              31                 0x00A0 (reserved)
   ITBL_vSpuriousIsr            ,     // 25              32                 0x00A4 (reserved)
   ITBL_vSpuriousIsr            ,     // 26              33                 0x00A8 (reserved)
   ITBL_vSpuriousIsr            ,     // 27              34                 0x00AC (reserved)
   ITBL_vSpuriousIsr            ,     // 28              35                 0x00B0 (reserved)
   ITBL_vSpuriousIsr            ,     // 29              36                 0x00B4 (reserved)
   ITBL_vSpuriousIsr            ,     // 30              37                 0x00B8 (reserved)
   FMC_IRQHandler               ,     // 31              38                 0x00BC
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
    // NVIC_SetPriority(LVI_IRQn, PRIORITY_MEDIUM);
    // NVIC_SetPriority(INTP0_IRQn, PRIORITY_MEDIUM);
    // NVIC_SetPriority(INTP1_IRQn, PRIORITY_MEDIUM);
    // NVIC_SetPriority(INTP2_IRQn, PRIORITY_MEDIUM);
    // NVIC_SetPriority(INTP3_IRQn, PRIORITY_MEDIUM);
    // NVIC_SetPriority(TM01H_IRQn, PRIORITY_MEDIUM);
    // NVIC_SetPriority(CCP_IRQn, PRIORITY_MEDIUM);
    // NVIC_SetPriority(ACMP_IRQn, PRIORITY_MEDIUM);
    // NVIC_SetPriority(ADC_IRQn, PRIORITY_MEDIUM);
    // NVIC_SetPriority(SSP0_IRQn, PRIORITY_MEDIUM);
    // NVIC_SetPriority(I2C0_IRQn, PRIORITY_MEDIUM);
    // NVIC_SetPriority(UART0_IRQn, PRIORITY_MEDIUM);
    // NVIC_SetPriority(UART1_IRQn, PRIORITY_MEDIUM);
    // NVIC_SetPriority(EPWM_IRQn, PRIORITY_MEDIUM);
    // NVIC_SetPriority(TIMER0_IRQn, PRIORITY_MEDIUM);
    // NVIC_SetPriority(TIMER1_IRQn, PRIORITY_LOW);
    // NVIC_SetPriority(HALL_IRQn, PRIORITY_LOW);
    // NVIC_SetPriority(LSITIMER_IRQn, PRIORITY_LOW);
    // NVIC_SetPriority(TM00_IRQn, PRIORITY_LOW);
    // NVIC_SetPriority(TM01_IRQn, PRIORITY_LOW);
    // NVIC_SetPriority(TM02_IRQn, PRIORITY_LOW);
    // NVIC_SetPriority(TM03_IRQn, PRIORITY_LOW);
    // NVIC_SetPriority(FMC_IRQn, PRIORITY_LOW);
}
