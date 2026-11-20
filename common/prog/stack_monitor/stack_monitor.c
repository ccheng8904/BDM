/*******************************************************************************
 *  Copyright (c) 2017 BSH Hausgeraete GmbH,
 *  Carl-Wery-Str. 34, 81739 Munich, Germany, www.bsh-group.de
 *
 *  All rights reserved. This program and the accompanying materials
 *  are protected by international copyright laws.
 *  Please contact copyright holder for licensing information.
 *
 *******************************************************************************
 *  PROJECT          Generic SW
 *  COMP_ABBREV      STK
 ******************************************************************************/


/******************************************************************************/
/* DOCUMENTATION                                                              */
/******************************************************************************/
/** \file  stack_monitor.c
 *
 *  \ingroup  stack_monitor
 *
 *  \brief    Public definitions and declarations for unit stack_monitor.
 *
 *  \details  
 */


/******************************************************************************/
/* INCLUDES                                                                   */
/******************************************************************************/
#include "bsh_stdinc.h"
#include "stack_monitor.h"
#include "processor.h"
#include <stdint.h>
#include <stdbool.h>
#include "math/mathDiv.h"
#include "LibDefines.h"
#ifdef __ICCRx__
#include <intrinsics.h>
#endif

/* Switch off: Note 923: cast from unsigned int to pointer [MISRA 2012 Rule 11.1, required], [MISRA 2012 Rule 11.4, required], [MISRA 2012 Rule 11.6, required]
 * It was checked and it works well */
/*lint -save -e923 */
/******************************************************************************/
/* PRIVATE DEFINITIONS                                                        */
/******************************************************************************/
#ifdef __NOT_CC__
    #define STACK_BASE  ((uint32_t)0u)
    #define STACK_LIMIT ((uint32_t)0u)

 #ifdef __ICCRx__
    #define ISTACK_BASE  ((uint32_t)0u)
    #define ISTACK_LIMIT ((uint32_t)0u)
 #endif
#elif defined(__ICCRx__)
    #pragma section="USTACK"
    #pragma section="ISTACK"

    #define STACK_BASE (__section_begin("USTACK"))
    #define STACK_LIMIT (__section_end("USTACK"))

    #define ISTACK_BASE (__section_begin("ISTACK"))
    #define ISTACK_LIMIT (__section_end("ISTACK"))

#elif defined(__GNUC__) && !defined(__ARMCC_VERSION)
    /* Stack definition for GCC compiler*/
    extern uint32_t __StackTop;

    #define STACK_LIMIT (&__StackTop)
    #define STACK_BASE ((uint32_t)(STACK_LIMIT) - (uint32_t)(STACKSIZE) + (uint32_t)(STACK_MAGIC_SIZE))
#elif defined(__CC_ARM) || defined(__ARMCC_VERSION)
    /* Stack definition for Keil compiler*/
    extern uint32_t Image$$ARM_LIB_STACK$$ZI$$Base;
    extern uint32_t Image$$ARM_LIB_STACK$$ZI$$Limit;

    #define STACK_BASE (&Image$$ARM_LIB_STACK$$ZI$$Base)
    #define STACK_LIMIT (&Image$$ARM_LIB_STACK$$ZI$$Limit)
#endif /* #elif __CC_ARM */

#define STACK_MAGIC_PATTERN         0xFAFEFAFEU
#define FILL_PATTERN                0xCDCDCDCDU
#ifdef __ICCRx__
#define SAFE_INDENT  80U
#endif


/******************************************************************************/
/* PRIVATE DATA DEFINITIONS                                                   */
/******************************************************************************/
struct STK_StackBoundaries
{
    uint32_t    start;
    uint32_t    end;
};

#ifdef __ICCRx__
static const struct STK_StackBoundaries STK_IStack = {(uint32_t)ISTACK_BASE, (uint32_t)ISTACK_LIMIT};
static uint32_t STK_IStackConsumption = 0;
#endif

static const struct STK_StackBoundaries STK_Stack = {(uint32_t)STACK_BASE, (uint32_t)STACK_LIMIT};

/******************************************************************************/
/* PRIVATE FUNCTION DECLARATIONS                                              */
/******************************************************************************/
static void STK_fillStack(const uint32_t startStack, const uint32_t endStack, const bool isUserStack);
static uint32_t STK_GetStackConsumption(const uint32_t startStack, const uint32_t endStack);
static bool STK_InitializeStack(void);

/******************************************************************************/
/* PUBLIC FUNCTION DEFINITIONS                                                */
/******************************************************************************/

/** Task handler of stack_monitor component.
 *  \note This function is called in task scheduler.
 *
 *  \return TASK_INITIALISED
 */
uint8_t STK_HandleTask(void)
{
    static volatile uint32_t STK_StackConsumption = 0;
#if defined(__ICCRx__) || defined(__NOT_CC__)
    static bool STK_StackInitialized = false;

    if (!STK_StackInitialized)
    {
        STK_StackInitialized = STK_InitializeStack();
    }
#endif

    STK_StackConsumption = STK_GetStackConsumption(STK_Stack.start, STK_Stack.end);
#ifdef __ICCRx__
    STK_IStackConsumption = STK_GetStackConsumption(STK_IStack.start, STK_IStack.end);
    (void)STK_IStackConsumption; // prevent optimization
#endif
    (void)STK_StackConsumption; // prevent optimization
#ifdef __ICCRx__
    return STK_StackInitialized ? TASK_INITIALISED : TASK_NOT_INITIALISED;
#else
    return TASK_INITIALISED;
#endif
}

/** Get remaining bytes of STACK.
 *
 *  \note This function return number of unused bytes in the STACK
 *
 *  \param isUserStack: only used in RX platform, true for user stack,
 *                      false for interrupt stack. Ignored in other platforms.
 *
 *  \return number of bytes
 */
uint32_t STK_getStackRemainingBytes(const bool isUserStack)
{
    uint32_t startStack = STK_Stack.start;
    uint32_t endStack   = STK_Stack.end;
    uint32_t freeWords  = 0;

#ifdef __ICCRx__
    if (!isUserStack)
    {
    	startStack = STK_IStack.start;
    	endStack = STK_IStack.end;
    }
#else
    (void)isUserStack;
#endif

    for (uint32_t addr = (startStack + WORD_SIZE); addr < endStack; addr += WORD_SIZE)
    {
        if ((*(uint32_t*)addr) == FILL_PATTERN)
        {
            freeWords++;
        }
        else
        {
            break;
        }
    }

    return (freeWords * WORD_SIZE);
}

/******************************************************************************/
/* PRIVATE FUNCTION DEFINITIONS                                               */
/******************************************************************************/
/** Initialization of Stack.
 *  \note This function initializes content of stack.
 *
 *  \return true: success false: SP is out of range
 */
#ifdef __ICCRx__
static bool STK_InitializeStack(void)
#else
/* Hint: for CortexM platforms this interface is called as a constructor during startup */
 __attribute__((constructor)) static bool STK_InitializeStack(void)
#endif
{
    uint32_t startStack = STK_Stack.start;
    uint32_t endStack   = STK_Stack.end;
#ifdef __ICCRx__
    uint32_t actualSP   = __get_USP_register();
#else
    uint32_t actualSP   = __get_MSP();
#endif

    /* First of all, make sure the stack pointer is pointing into the stack,
       otherwise it does not make sense to continue. */
    if ((actualSP < startStack) || (actualSP > endStack))
    {
        return false;
    }

    /* Fill STACK (or USTACK in Rx platforms) by pattern */
    STK_fillStack(startStack, endStack, true);

#ifdef __ICCRx__
    startStack = STK_IStack.start;
    endStack   = STK_IStack.end;
    actualSP   = __get_ISP_register();

    if ((actualSP < startStack) || (actualSP > endStack))
    {
        return false;
    }

    /* Fill ISTACK by pattern */
    STK_fillStack(startStack, endStack, false);
#endif

#if (!defined(FEATURE_FUNCTIONAL_SAFETY_ENABLED) && !defined(__ICCRx__))
    /* Place the magic numbers at the beginning of STACK */
    *(uint32_t*)startStack = (uint32_t)STACK_MAGIC_PATTERN;
    /* and at the end of STACK */
    *(uint32_t*)endStack = (uint32_t)STACK_MAGIC_PATTERN;
#endif
    return true;
}

/** Fill STACK with pattern.
 *
 *  \param startStack: start address of STACK
 *  \param endStack: end address of STACK
 *  \param isUserStack: only used in RX platform, true for user stack,
 *                      false for interrupt stack. Ignored in other platforms.
 *
 *  \return void
 */
static void STK_fillStack(const uint32_t startStack, const uint32_t endStack, const bool isUserStack)
{
    uint32_t actualSP;
#ifdef __ICCRx__
    if (isUserStack)
    {
        actualSP = __get_USP_register() - (WORD_SIZE + SAFE_INDENT);
    }
    else
    {
        actualSP = __get_ISP_register() - (WORD_SIZE + SAFE_INDENT);
    }
#else
    (void)isUserStack;
    actualSP   = __get_MSP() - WORD_SIZE;
#endif

    if ((startStack < actualSP) && (actualSP < endStack))
    {
        for ( ; actualSP > startStack; actualSP -= WORD_SIZE)
        {
            *(uint32_t*)actualSP = FILL_PATTERN;
        }
    }
}

/** Get consumption of STACK.
 *
 *  \note This function return percentual consumption/usage of STACK (1% - 100%)
 * 
 *  \param startStack: start address of STACK
 *  \param endStack: end address of STACK
 *
 *  \return consumption of STACK in %
 */
static uint32_t STK_GetStackConsumption(const uint32_t startStack, const uint32_t endStack)
{
    uint32_t freeWords = 0;
    uint32_t usedBytes;
    uint32_t consumption;

    for (uint32_t addr = (startStack + WORD_SIZE); addr < endStack; addr += WORD_SIZE)
    {
        if ((*(uint32_t*)addr) == FILL_PATTERN)
        {
            freeWords++;
        }
        else
        {
            break;
        }
    }
    usedBytes = (endStack - startStack) - (freeWords * WORD_SIZE);
    consumption = MTDIV_div_32_32((usedBytes * 100U), (endStack - startStack));

    return consumption;
}

