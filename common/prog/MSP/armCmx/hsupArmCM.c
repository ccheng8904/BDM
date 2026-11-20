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
*   COMP_ABBREV      HSUP    
*******************************************************************************/


/******************************************************************************/
/* DOCUMENTATION                                                              */
/******************************************************************************/
/** \file
*
*   \brief    Implementation of CortexMx specific HAL Support functions.
*
*/


/******************************************************************************/
/* INCLUDES                                                                   */
/******************************************************************************/
#include "bsh_stdinc.h" 
#include "hsup.h"
#include <stdint.h>
#include "processor.h" // find out cortex core 
#include "std_symbol.h"

#ifdef RTOS
#include "rtos_api.h"
#endif

/******************************************************************************/
/* PUBLIC FUNCTION DEFINITIONS                                                */
/******************************************************************************/
SYMBOL_WEAK void HSUP_vGenerateReset(void)
{
    NVIC_SystemReset();  //Reset the system provided by CMSIS
}


#ifndef NO_ISR_SUPPORT
#ifndef RTOS
static uint32_t HSUP_IntCnt = 0u; ///< interrupt disable counter, used for nested interrupt enable/disable
#endif

void HSUP_vDisableInt(void)
{
#ifdef RTOS
    RTOS_enterCriticalSection_HARD();
#else
    if(HSUP_IntCnt == 0u)
    {
        (void)__disable_irq();
    }
    HSUP_IntCnt++;
#endif
}


void HSUP_vEnableInt(void)
{
#ifdef RTOS
    RTOS_exitCriticalSection_HARD();
#else
    if ( HSUP_IntCnt > 0u )
    {
        HSUP_IntCnt--;
    }
    if(HSUP_IntCnt == 0u)
    {
        __enable_irq();
    }
#endif
}
#endif


/* ===============================================================================================*/
/*                                      Delay                                                     */
/* ===============================================================================================*/

#ifdef __NOT_TARGET_CC__
//hide assembler stuff for lint & co 
#else
#if defined(__GNUC__)
/**************************************************************************************************/
/* GCC and armclang: HSUP_DelayClk(uint16 delay, uint32 clock)                                                 */
/**************************************************************************************************/
__attribute__((naked))
void HSUP_DelayClk(uint16 delay, uint32 clock)
{
    __asm__ __volatile__
    (
  #if !defined(__ARMCC_VERSION)
    ".syntax unified       \n"
  #endif

  #if defined(CORTEX_M0) || defined(CORTEX_M0_PLUS) || defined(CORTEX_M23)
    #if defined(CLOCK_IN_KHZ)
    // following calculation will be done: r0 = delay * clock / 32000
    // to prevent overflow and division:
    // r0 = (((clock * 8389) / 65536) * delay) / 4096  (error < 50ppm)
        "mov  r2,#8389     \n"   // r2  = 8389
        "muls r1,r2        \n"   // r1 *= 8389
        "lsrs r1,#16       \n"   // r1 /= 65536
        "muls r0,r1        \n"   // r0 = r0 * r1
        "lsrs r0,#12       \n"   // r0 /= 4096
    #else
        "muls r0,r1        \n"
        "lsrs r0,#5        \n"   // loop needs 32 cycles ( 1 +4*7 + 3 )
    #endif  // CLOCK_IN_KHZ
  #else
    #if defined(CLOCK_IN_KHZ)
    // following calculation will be done: r0 = delay * clock / 32000
    // to prevent overflow (up to 524MHz clock):
    // r0 = (((clock / 8)) * delay) / 4000
        "lsrs r1,#3        \n"   // clock = clock / 8
        "mul r0,r1         \n"   // r0 = (clock / 8) * delay
        "mov r1,#4000      \n"   // r1 = 4000
        "udiv r0,r0,r1     \n"   // r0 = r0 / 4000
    #else
        "mul r0,r1         \n"   // r0 = clock * delay
        "lsrs r0,#5        \n"   // r0 = r0 / 32
    #endif  // CLOCK_IN_KHZ
  #endif    // CORTEX_M0 || CORTEX_M0_PLUS
  
    //loop needs about 32 cycles ( 1 +4*7 + 3 )
    "loop:                 \n"   // loop will be handled at least one time
    #if defined(HSUP_DELAY_ADDITIONAL_DICE_CYCLE)
        "nop               \n"   // waste  aditional cycle (if branch is faster than 3 cycles)
    #endif
        "subs r0,#1        \n"   // decrement r0 (1 cycle)        Do so many cycles to relativise
        "push {r0-r5}      \n"   // waste time   (7 cycle)        the fuzzy time caused by the branch
        "pop  {r0-r5}      \n"   // waste time   (7 cycle)        and possible flash wait states.
        "push {r0-r5}      \n"   // waste time   (7 cycle)        Accuracy is better than 10%.
        "pop  {r0-r5}      \n"   // waste time   (7 cycle)        Very low delays should be avoided.
        "bcs loop          \n"   // branch if not negative ( about 3 cycles if branch taken)
        "bx lr             \n"

  #if !defined(__ARMCC_VERSION)
    ".syntax divided       \n"
  #endif
    );
}


#elif __CC_ARM
/**************************************************************************************************/
/* Keil: HSUP_DelayClk(uint16 delay, uint32 clock)                                                */
/**************************************************************************************************/
__asm void HSUP_DelayClk(uint16 delay, uint32 clock)
{
  #if defined(CORTEX_M0) || defined(CORTEX_M0_PLUS)
    #if defined(CLOCK_IN_KHZ)
        // following calculation will be done: r0 = delay * clock / 32000
        // to prevent overflow and division:
        // r0 = (((clock * 8389) / 65536) * delay) / 4096  (error < 50ppm)
        mov  r2,#8389     ; r2  = 8389
        muls r1,r2,r1     ; r1 *= 8389
        lsrs r1,#16       ; r1 /= 65536
        muls r0,r1,r0     ; r0 = r1 * r0
        lsrs r0,#12       ; r0 /= 4096
    #else
        muls r0,r1,r0     ;
        lsrs r0,#5        ; loop needs 32 cycles ( 1 +4*7 + 3 )
    #endif   // CLOCK_IN_KHZ
  #else
    #if defined(CLOCK_IN_KHZ)
        // following calculation will be done: r0 = delay * clock / 32000
        // to prevent overflow (up to 524MHz clock):
        // r0 = (((clock / 8)) * delay) / 4000
        lsrs r1,#3        ; clock = clock / 8
        mul r0,r1         ; r0 = (clock / 8) * delay
        mov r1,#4000      ; r1 = 4000
        udiv r0,r0,r1     ; r0 = r0 / 4000
    #else
        mul r0,r1         ; r0 = clock * delay
        lsrs r0,#5        ; r0 = r0 / 32
    #endif // CLOCK_IN_KHZ
  #endif   // CORTEX_M0 || CORTEX_M0_PLUS
  
   //loop needs about 32 cycles ( 1 +4*7 + 3 )
loop                  ; loop will be handled at least one time        
    subs r0,#1        ; decrement r0 (1 cycle)        Do so many cycles to relativise
  #if defined(HSUP_DELAY_ADDITIONAL_DICE_CYCLE)
    nop               ; waste  aditional cycle (if branch is faster than 3 cycles)
  #endif
    push {r0-r5}      ; waste time   (7 cycle)        the fuzzy time caused by the branch
    pop  {r0-r5}      ; waste time   (7 cycle)        and possible flash wait states.
    push {r0-r5}      ; waste time   (7 cycle)        Accuracy is better than 10%.
    pop  {r0-r5}      ; waste time   (7 cycle)        Very low delays should be avoided.
    bcs loop          ; branch if not negative ( about 3 cycles if branch taken)
    bx lr             ;
}

#else
    #error "Unsupported compiler (not GCC, clang or Keil/ARM)"
#endif


/* ===============================================================================================*/
/*                                  Bit Reverse                                                   */
/* ===============================================================================================*/

#if defined(CORTEX_M0) || defined(CORTEX_M0_PLUS) || defined(CORTEX_M23)
static uint32_t HSUP_xMirror(uint32_t value, uint32_t width)
{
    value = __REV(value);
    value >>= ((sizeof(uint32_t) - width) * 8);
    value = ((value & 0xaaaaaaaau) >> 1u) | ((value & 0x55555555u) << 1u);
    value = ((value & 0xccccccccu) >> 2u) | ((value & 0x33333333u) << 2u);
    value = ((value & 0xf0f0f0f0u) >> 4u) | ((value & 0x0f0f0f0fu) << 4u);
    return value;
}


uint32_t HSUP_u32Mirror(uint32_t value)
{
    return  HSUP_xMirror( value, sizeof(value));
}

uint16_t HSUP_u16Mirror(uint16_t value)
{
    return (uint16_t)(HSUP_xMirror( value,sizeof(value)));
}

uint8_t HSUP_ucMirror(uint8_t value)
{
    return (uint8_t)(HSUP_xMirror( value,sizeof(value)));
}


#else // CORTEX_M0

#if defined(__GNUC__)
// #pragma GCC diagnostic push
// #pragma GCC diagnostic ignored "-Wreturn-type"
/**************************************************************************************************/
/* uint32_t HSUP_u32Mirror(uint32_t value)                                                        */
/**************************************************************************************************/
__attribute__((naked))
uint32_t HSUP_u32Mirror(uint32_t value)
{
    __asm__ __volatile__(
        "rbit r0,r0              \n"  // reverse bits
        "bx lr                   \n"  // return
    );
}


/**************************************************************************************************/
/* uint16_t HSUP_u16Mirror(uint16_t value)                                                        */
/**************************************************************************************************/
__attribute__((naked))
uint16_t HSUP_u16Mirror(uint16_t value)
{
    __asm__ __volatile__(
        "rbit r0,r0              \n"  // reverse bits
        "lsr  r0,#16             \n"  // value >>= 16
        "bx lr                   \n"  // return
    );
}


/**************************************************************************************************/
/* uint8_t HSUP_ucMirror(uint8_t value)                                                           */
/**************************************************************************************************/
__attribute__((naked))
uint8_t HSUP_ucMirror(uint8_t value)
{
    __asm__ __volatile__(
        "rbit r0,r0              \n"  // reverse bits
        "lsr  r0,#24             \n"  // value >>= 24
        "bx lr                   \n"  // return
    );
}
// #pragma GCC diagnostic pop


#elif __CC_ARM
/**************************************************************************************************/
/* uint32_t HSUP_u32Mirror(uint32_t value)                                                        */
/**************************************************************************************************/
__asm uint32_t HSUP_u32Mirror(uint32_t value)
{
    rbit r0,r0              ;  // reverse bits
    bx lr                   ;  // return
}


/**************************************************************************************************/
/* uint16_t HSUP_u16Mirror(uint16_t value)                                                        */
/**************************************************************************************************/
__asm uint16_t HSUP_u16Mirror(uint16_t value)
{
    rbit r0,r0              ;  // reverse bits
    lsr  r0,#16             ;  // value >>= 16
    bx lr                   ;  // return
}


/**************************************************************************************************/
/* uint8_t HSUP_ucMirror(uint8_t value)                                                           */
/**************************************************************************************************/
__asm uint8_t HSUP_ucMirror(uint8_t value)
{
    rbit r0,r0              ;  // reverse bits
    lsr  r0,#24             ;  // value >>= 24
    bx lr                   ;  // return
}

#endif // __CC_ARM
#endif // CORTEX_M0


/* ===============================================================================================*/
/*                                  Endianness                                                    */
/* ===============================================================================================*/
#if defined(__GNUC__)// GCC, armclang
__attribute__((naked))
uint16_t HSUP_uiBigEndianToProc(uint16_t value)
{
    __asm("rev16 r0, r0");
    __asm("uxth r0, r0");
    __asm("bx lr");
}


__attribute__((naked))
uint16_t HSUP_uiProcToBigEndian(uint16_t value)
{
    __asm("rev16 r0, r0");
    __asm("uxth r0, r0");
    __asm("bx lr");
}


__attribute__((naked))
uint32_t HSUP_ulProcToBigEndian(uint32_t value)
{
    __asm("rev r0, r0");
    __asm("bx lr");
}

__attribute__((naked))
uint32_t HSUP_ulBigEndianToProc(uint32_t value)
{
    __asm("rev r0, r0");
    __asm("bx lr");
}

#elif defined ( __CC_ARM ) //Keil
__ASM uint16_t HSUP_uiBigEndianToProc(uint16_t value)
{
    rev16 r0, r0
    uxth  r0, r0    
    bx lr
}

__ASM uint16_t HSUP_uiProcToBigEndian(uint16_t value)
{
    rev16 r0, r0
    uxth  r0, r0
    bx lr
}

__ASM uint32_t HSUP_ulProcToBigEndian(uint32_t value)
{
    rev r0, r0
    bx lr
}

__ASM uint32_t HSUP_ulBigEndianToProc(uint32_t value)
{
    rev r0, r0
    bx lr
}
#endif


#endif //__NOT_TARGET_CC__
