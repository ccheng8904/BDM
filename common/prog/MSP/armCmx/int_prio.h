/*******************************************************************************
*   Copyright (c) 2016 BSH Hausgeraete GmbH,
*   Carl-Wery-Str. 34, 81739 Munich, Germany, www.bsh-group.de
*
*   All rights reserved. This program and the accompanying materials
*   are protected by international copyright laws.
*   Please contact copyright holder for licensing information.
*
********************************************************************************
*   PROJECT          PP PED-Framework
*   PROCESSOR        ARM CortexMx
*   COMP_ABBREV      INTP    
*******************************************************************************/


/******************************************************************************/
/* DOCUMENTATION                                                              */
/******************************************************************************/
/** \file
*
*   \brief    Default initialization of Interrupt Levels
*
*/



#ifndef INTPRIO_H
#define INTPRIO_H

#ifdef __cplusplus
extern "C" {
#endif


#if defined(CORTEX_M0) || defined(CORTEX_M0_PLUS) || defined(CORTEX_M23)
#define DFLT_INT_PRIO 0x70707070U   /*!< Programmable priority level of 0-3 for each interrupt    */
#define NVIC_PRIO_BITS         2U   /*!< STM32F0XX uses 2 Bits for the Priority Levels            */
#else
#define DFLT_INT_PRIO 0x70707070U   /*!< Programmable priority level of 0-15 for each interrupt   */
#define NVIC_PRIO_BITS         4U   /*!< STM32F4XX uses 4 Bits for the Priority Levels            */
#endif

#define INT_PRIO_0     0U            /*!< Interrupt Priority 0                                     */
#define INT_PRIO_1     1U            /*!< Interrupt Priority 1                                     */
#define INT_PRIO_2     2U            /*!< Interrupt Priority 2                                     */
#define INT_PRIO_3     3U            /*!< Interrupt Priority 3                                     */
#define INT_PRIO_4     4U            /*!< Interrupt Priority 4                                     */
#define INT_PRIO_5     5U            /*!< Interrupt Priority 5                                     */
#define INT_PRIO_6     6U            /*!< Interrupt Priority 6                                     */
#define INT_PRIO_7     7U            /*!< Interrupt Priority 7                                     */
#define INT_PRIO_8     8U            /*!< Interrupt Priority 8                                     */
#define INT_PRIO_9     9U            /*!< Interrupt Priority 9                                     */
#define INT_PRIO_10   10U            /*!< Interrupt Priority 10                                    */
#define INT_PRIO_11   11U            /*!< Interrupt Priority 11                                    */
#define INT_PRIO_12   12U            /*!< Interrupt Priority 12                                    */
#define INT_PRIO_13   13U            /*!< Interrupt Priority 13                                    */
#define INT_PRIO_14   14U            /*!< Interrupt Priority 14                                    */
#define INT_PRIO_15   15U            /*!< Interrupt Priority 15                                    */


/** Set Priority of configurable interrupts.

\b description: \n
 Default interrupt priority of Cortex-M is highest priority 0.
 Set all interrupts to medium priority (DFLT_INT_PRIO)
 Each priority field holds a priority value, 0-255.
 The lower the value, the greater the priority of the corresponding interrupt.
 The processor implements only bits[7:4] of each field, bits[3:0] read as zero and ignore writes
 FM4 uses 4 Bits for the Priority Levels

\note This interface is not reentrant.

\param none

\return none
*/

extern void INTP_setDefaultInterruptPriority(void);

/** Set Interrupt Priority

\b description: \n
 The function sets the priority of an interrupt by using CMSIS function
 NVIC_SetPriority with parameters Interrupt number and Priority to set.

\note This interface is not reentrant.

\param none

\return none
*/

extern void INTP_setApplicationPriority(void);

#ifdef __cplusplus
}
#endif

#endif
