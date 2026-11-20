/*******************************************************************************
 *  Copyright (c) 2017 BSH Hausgeraete GmbH,
 *  Carl-Wery-Str. 34, 81739 Munich, Germany, www.bsh-group.de
 *
 *  All rights reserved. This program and the accompanying materials
 *  are protected by international copyright laws.
 *  Please contact copyright holder for licensing information.
 *
 *******************************************************************************
 *   PROJECT          Generic SW
 *   PROCESSOR        CortexMx
 ******************************************************************************/


#ifndef INTTBLARMCM_H
#define INTTBLARMCM_H

#ifdef __cplusplus
extern "C" {
#endif


/******************************************************************************/
/* DOCUMENTATION                                                              */
/******************************************************************************/
/** \file
 *  \brief    declarations for Cortex M generic part of vector table.
 */


/******************************************************************************/
/* INCLUDES                                                                   */
/******************************************************************************/
#include "std_lib/std_symbol.h"


/** \brief    Stack pointer initialization address definition
 *
 *  \details  Init value for stack pointer.
 *            ANSI-C extensions encapsulated in macros 
 *            Hint: Linker decides about location
 */
#ifdef __NOT_CC__
    #define STACK_TOP       ((uint32_t)0u)
/* GCC macros */
#elif defined(__GNUC__) && !defined(__ARMCC_VERSION)
    extern uint32_t __StackTop;
    #define STACK_TOP       (&__StackTop)
/* Keil */
#else
    extern uint32_t Image$$ARM_LIB_STACK$$ZI$$Limit;
    #define STACK_TOP       &Image$$ARM_LIB_STACK$$ZI$$Limit
#endif

#ifndef ITBL_WEAK
    #define ITBL_WEAK
#endif
/******************************************************************************/
/* PUBLIC TYPE DEFINITIONS                                                    */
/******************************************************************************/
typedef void ( *ITBL_fptr)(void); ///< function pointer for vector table

enum INT_ExceptionHandlerType
{
    INT_HANDLER_TYPE_RESET = 0,
    INT_HANDLER_TYPE_NMI,
    INT_HANDLER_TYPE_HARDFAULT,
    INT_HANDLER_TYPE_MEMMANAGE,
    INT_HANDLER_TYPE_BUSFAULT,
    INT_HANDLER_TYPE_USAGEFAULT,
    INT_HANDLER_TYPE_COUNT
};
 
/******************************************************************************/
/* PUBLIC VARIABLE DECLARATIONS                                               */
/******************************************************************************/
/** If fault handlers are called this might be read out to evaluate stack frame */
extern volatile struct INT_StackFrame_s const *INT_StackFrame;

/******************************************************************************/
/* PUBLIC FUNCTION DECLARATIONS                                               */
/******************************************************************************/

extern void ITBL_vSpuriousIsr(void);
extern void ITBL_vResetHandler(void);
extern void HardFault_Handler(void);
extern void SpuriousIsr_Handler(uint32_t SpuriousInt);

/** \brief    Function declarations for common Cortex M interrupt handlers
 *
 *  \details  If the functions are not defined, ITBL_vSpuriousIsr is used.
 */

#if !defined(NO_ISR_SUPPORT) || defined(need_ModuleHeader)
extern void NMI_Handler       (void)   ITBL_WEAK;
#endif

#ifndef NO_ISR_SUPPORT
// ISR functions not implemented in HAL
extern void SVC_Handler       (void)   ITBL_WEAK;
extern void PendSV_Handler    (void)   ITBL_WEAK;
extern void SysTick_Handler   (void)   ITBL_WEAK;

#if !defined(CORTEX_M0) && !defined(CORTEX_M0_PLUS)
extern void UsageFault_Handler(void)   ITBL_WEAK;
extern void DebugMon_Handler  (void)   ITBL_WEAK;
extern void MemManage_Handler (void)   ITBL_WEAK;
extern void BusFault_Handler  (void)   ITBL_WEAK;
#endif // !defined(CORTEX_M0) && !defined(CORTEX_M0_PLUS)
#endif // NO_ISR_SUPPORT



/** starts another application
 *  
 *  \param startAddress: start address of application (start of vector table)
 *  \note take care that the vector table is at the specified address
 */
extern void ITBL_StartApplication(uint32_t startAddress);

#if defined (LOG_EXCEPTION_ENABLED)
/** \brief This Interface allow to log exception type
 * 
 *  \details User must define own strong function definition.
 */
    extern void INT_logException(uint32_t stackFrameRegister, enum INT_ExceptionHandlerType handlerType);
#endif
/******************************************************************************/
/* PUBLIC DATA DECLARATIONS                                                   */
/******************************************************************************/
//** core vector table possibly needed for module header */
extern const ITBL_fptr ITBL_coreVectors[];
//** fixed/root vector table possibly needed for module header */
extern const ITBL_fptr ITBL_fixedVectors[];


#ifdef __cplusplus
}
#endif

#endif
