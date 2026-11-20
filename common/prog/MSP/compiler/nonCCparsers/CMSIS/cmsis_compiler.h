/*******************************************************************************
*   Copyright (c) 2016 BSH Hausgeraete GmbH,
*   Carl-Wery-Str. 34, 81739 Munich, Germany, www.bsh-group.de
*
*   All rights reserved. This program and the accompanying materials
*   are protected by international copyright laws.
*   Please contact copyright holder for licensing information.
*
********************************************************************************
*   PROJECT          MCU Framework
*******************************************************************************/
#ifndef __CMSIS_COMPILER_H
#define __CMSIS_COMPILER_H

#include <stdint.h>

/** CMSIS_compiler.h implementation independent of target compiler **/

#ifndef   __ASM
  #define __ASM                                  __asm
#endif
#ifndef   __INLINE
  #define __INLINE                               inline
#endif
#ifndef   __STATIC_INLINE
  #define __STATIC_INLINE                        static inline
#endif
#ifndef   __STATIC_FORCEINLINE                 
  #define __STATIC_FORCEINLINE                   static inline
#endif                                           
#ifndef   __NO_RETURN
  #define __NO_RETURN                            _Noreturn
#endif
#ifndef   __USED
  #define __USED
#endif
#ifndef   __WEAK
  #define __WEAK
#endif
#ifndef   __PACKED
  #define __PACKED
#endif
#ifndef   __PACKED_STRUCT
  #define __PACKED_STRUCT
#endif
#ifndef   __PACKED_UNION
  #define __PACKED_UNION
#endif
#ifndef   __ALIGNED
  #define __ALIGNED(x)
#endif
#ifndef   __RESTRICT
  #define __RESTRICT                __restrict
#endif

extern void __cmsis_start(void);
extern void __enable_irq(void);
/*lint -esym(534,__disable_irq)  return value of __disable_irq might be ignored, as there is also a void version of this interface */
extern int __disable_irq(void);/*lint !e970 use of modifier or type 'int' outside of a typedef [MISRA 2012 Directive 4.6, required] : 3rd party interface*/
extern uint32_t __get_CONTROL(void);
extern uint32_t __TZ_get_CONTROL_NS(void);
extern void __set_CONTROL(uint32_t control);
extern void __TZ_set_CONTROL_NS(uint32_t control);
extern uint32_t __get_IPSR(void);
extern uint32_t __get_APSR(void);
extern uint32_t __get_xPSR(void);
extern uint32_t __get_PSP(void);
extern uint32_t __TZ_get_PSP_NS(void);
extern void __set_PSP(uint32_t topOfProcStack);
extern void __TZ_set_PSP_NS(uint32_t topOfProcStack);
extern uint32_t __get_MSP(void);
extern uint32_t __TZ_get_MSP_NS(void);
extern void __set_MSP(uint32_t topOfMainStack);
extern void __TZ_set_MSP_NS(uint32_t topOfMainStack);
extern uint32_t __TZ_get_SP_NS(void);
extern void __TZ_set_SP_NS(uint32_t topOfStack);
extern uint32_t __get_PRIMASK(void);
extern uint32_t __TZ_get_PRIMASK_NS(void);
extern void __set_PRIMASK(uint32_t priMask);
extern void __TZ_set_PRIMASK_NS(uint32_t priMask);
extern void __enable_fault_irq(void);
extern void __disable_fault_irq(void);
extern uint32_t __get_BASEPRI(void);
extern uint32_t __TZ_get_BASEPRI_NS(void);
extern void __set_BASEPRI(uint32_t basePri);
extern void __TZ_set_BASEPRI_NS(uint32_t basePri);
extern void __set_BASEPRI_MAX(uint32_t basePri);
extern uint32_t __get_FAULTMASK(void);
extern uint32_t __TZ_get_FAULTMASK_NS(void);
extern void __set_FAULTMASK(uint32_t faultMask);
extern void __TZ_set_FAULTMASK_NS(uint32_t faultMask);
extern uint32_t __get_PSPLIM(void);
extern uint32_t __TZ_get_PSPLIM_NS(void);
extern void __set_PSPLIM(uint32_t ProcStackPtrLimit);
extern void __TZ_set_PSPLIM_NS(uint32_t ProcStackPtrLimit);
extern uint32_t __get_MSPLIM(void);
extern uint32_t __TZ_get_MSPLIM_NS(void);
extern void __set_MSPLIM(uint32_t MainStackPtrLimit);
extern void __TZ_set_MSPLIM_NS(uint32_t MainStackPtrLimit);
extern uint32_t __get_FPSCR(void);
extern void __set_FPSCR(uint32_t fpscr);
extern void __ISB(void);
extern void __DSB(void);
extern void __DMB(void);
extern uint32_t __REV(uint32_t value);
extern uint32_t __REV16(uint32_t value);
extern int16_t __REVSH(int16_t value);
extern uint32_t __ROR(uint32_t op1, uint32_t op2);
extern uint32_t __RBIT(uint32_t value);
extern uint8_t __CLZ(uint32_t value);
extern uint8_t __LDREXB(volatile uint8_t *addr);
extern uint16_t __LDREXH(volatile uint16_t *addr);
extern uint32_t __LDREXW(volatile uint32_t *addr);
extern uint32_t __STREXB(uint8_t value, volatile uint8_t *addr);
extern uint32_t __STREXH(uint16_t value, volatile uint16_t *addr);
extern uint32_t __STREXW(uint32_t value, volatile uint32_t *addr);
extern void __CLREX(void);
extern uint32_t __RRX(uint32_t value);
extern uint8_t __LDRBT(volatile uint8_t *ptr);
extern uint16_t __LDRHT(volatile uint16_t *ptr);
extern uint32_t __LDRT(volatile uint32_t *ptr);
extern void __STRBT(uint8_t value, volatile uint8_t *ptr);
extern void __STRHT(uint16_t value, volatile uint16_t *ptr);
extern void __STRT(uint32_t value, volatile uint32_t *ptr);
extern int32_t __SSAT(int32_t val, uint32_t sat);
extern uint32_t __USAT(int32_t val, uint32_t sat);
extern uint8_t __LDAB(volatile uint8_t *ptr);
extern uint16_t __LDAH(volatile uint16_t *ptr);
extern uint32_t __LDA(volatile uint32_t *ptr);
extern void __STLB(uint8_t value, volatile uint8_t *ptr);
extern void __STLH(uint16_t value, volatile uint16_t *ptr);
extern void __STL(uint32_t value, volatile uint32_t *ptr);
extern uint8_t __LDAEXB(volatile uint8_t *ptr);
extern uint16_t __LDAEXH(volatile uint16_t *ptr);
extern uint32_t __LDAEX(volatile uint32_t *ptr);
extern uint32_t __STLEXB(uint8_t value, volatile uint8_t *ptr);
extern uint32_t __STLEXH(uint16_t value, volatile uint16_t *ptr);
extern uint32_t __STLEX(uint32_t value, volatile uint32_t *ptr);
extern uint32_t __SADD8(uint32_t op1, uint32_t op2);
extern uint32_t __QADD8(uint32_t op1, uint32_t op2);
extern uint32_t __SHADD8(uint32_t op1, uint32_t op2);
extern uint32_t __UADD8(uint32_t op1, uint32_t op2);
extern uint32_t __UQADD8(uint32_t op1, uint32_t op2);
extern uint32_t __UHADD8(uint32_t op1, uint32_t op2);
extern uint32_t __SSUB8(uint32_t op1, uint32_t op2);
extern uint32_t __QSUB8(uint32_t op1, uint32_t op2);
extern uint32_t __SHSUB8(uint32_t op1, uint32_t op2);
extern uint32_t __USUB8(uint32_t op1, uint32_t op2);
extern uint32_t __UQSUB8(uint32_t op1, uint32_t op2);
extern uint32_t __UHSUB8(uint32_t op1, uint32_t op2);
extern uint32_t __SADD16(uint32_t op1, uint32_t op2);
extern uint32_t __QADD16(uint32_t op1, uint32_t op2);
extern uint32_t __SHADD16(uint32_t op1, uint32_t op2);
extern uint32_t __UADD16(uint32_t op1, uint32_t op2);
extern uint32_t __UQADD16(uint32_t op1, uint32_t op2);
extern uint32_t __UHADD16(uint32_t op1, uint32_t op2);
extern uint32_t __SSUB16(uint32_t op1, uint32_t op2);
extern uint32_t __QSUB16(uint32_t op1, uint32_t op2);
extern uint32_t __SHSUB16(uint32_t op1, uint32_t op2);
extern uint32_t __USUB16(uint32_t op1, uint32_t op2);
extern uint32_t __UQSUB16(uint32_t op1, uint32_t op2);
extern uint32_t __UHSUB16(uint32_t op1, uint32_t op2);
extern uint32_t __SASX(uint32_t op1, uint32_t op2);
extern uint32_t __QASX(uint32_t op1, uint32_t op2);
extern uint32_t __SHASX(uint32_t op1, uint32_t op2);
extern uint32_t __UASX(uint32_t op1, uint32_t op2);
extern uint32_t __UQASX(uint32_t op1, uint32_t op2);
extern uint32_t __UHASX(uint32_t op1, uint32_t op2);
extern uint32_t __SSAX(uint32_t op1, uint32_t op2);
extern uint32_t __QSAX(uint32_t op1, uint32_t op2);
extern uint32_t __SHSAX(uint32_t op1, uint32_t op2);
extern uint32_t __USAX(uint32_t op1, uint32_t op2);
extern uint32_t __UQSAX(uint32_t op1, uint32_t op2);
extern uint32_t __UHSAX(uint32_t op1, uint32_t op2);
extern uint32_t __USAD8(uint32_t op1, uint32_t op2);
extern uint32_t __USADA8(uint32_t op1, uint32_t op2, uint32_t op3);
extern uint32_t __UXTB16(uint32_t op1);
extern uint32_t __UXTAB16(uint32_t op1, uint32_t op2);
extern uint32_t __SXTB16(uint32_t op1);
extern uint32_t __SXTB16_RORn(uint32_t op1, uint32_t rotate);
extern uint32_t __SXTAB16(uint32_t op1, uint32_t op2);
extern uint32_t __SMUAD  (uint32_t op1, uint32_t op2);
extern uint32_t __SMUADX (uint32_t op1, uint32_t op2);
extern uint32_t __SMLAD (uint32_t op1, uint32_t op2, uint32_t op3);
extern uint32_t __SMLADX (uint32_t op1, uint32_t op2, uint32_t op3);
extern uint64_t __SMLALD (uint32_t op1, uint32_t op2, uint64_t acc);
extern uint64_t __SMLALDX (uint32_t op1, uint32_t op2, uint64_t acc);
extern uint32_t __SMUSD  (uint32_t op1, uint32_t op2);
extern uint32_t __SMUSDX (uint32_t op1, uint32_t op2);
extern uint32_t __SMLSD (uint32_t op1, uint32_t op2, uint32_t op3);
extern uint32_t __SMLSDX (uint32_t op1, uint32_t op2, uint32_t op3);
extern uint64_t __SMLSLD (uint32_t op1, uint32_t op2, uint64_t acc);
extern uint64_t __SMLSLDX (uint32_t op1, uint32_t op2, uint64_t acc);
extern uint32_t __SEL  (uint32_t op1, uint32_t op2);
extern  int32_t __QADD( int32_t op1,  int32_t op2);
extern  int32_t __QSUB( int32_t op1,  int32_t op2);
extern int32_t __SMMLA (int32_t op1, int32_t op2, int32_t op3);
extern void __WFI(void);
extern void __WFE(void);
extern void __BKPT(uint16_t op1);
extern void    __COMPILER_BARRIER(void);
extern void __NOP(void);
extern void __schedule_barrier(void);

#endif /* __CMSIS_COMPILER_H */

