/*******************************************************************************
 *  Copyright (c) 2017 BSH Hausgeraete GmbH,
 *  Carl-Wery-Str. 34, 81739 Munich, Germany, www.bsh-group.de
 *
 *  All rights reserved. This program and the accompanying materials
 *  are protected by international copyright laws.
 *  Please contact copyright holder for licensing information.
 *
 *******************************************************************************
 *  PROJECT          GED-Framework NG
 *  PROCESSOR        ARM CortexMx - Keil Arm Compiler
 *  COMP_ABBREV      FSF
 *
 ******************************************************************************/


/******************************************************************************/
/* DOCUMENTATION                                                              */
/******************************************************************************/
/** \file
 *
 *  \brief    Implementation of CortexMx specific register check functions for
 *            Keil assembler.
 *
 *  \details  This file contains the Cortex-Mx CPU test routines executed
 *            at start-up and during run time.
 */

/******************************************************************************/
/* INCLUDES                                                                   */
/******************************************************************************/
#include "fs_asm_macros.h"
    __ASM_DIRECTIVE1 
    __ASM_DIRECTIVE2
    __ASM_DIRECTIVE_THUMB
    __ASM_DIRECTIVE_ALIGN(2)
; 
/******************************************************************************/
/* PRIVATE DEFINITIONS                                                        */
/******************************************************************************/
__ASM_MACRO_DEF(con80,0x80000000)
__ASM_MACRO_DEF(conAA,0xAAAAAAAA)
__ASM_MACRO_DEF(con55,0x55555555)
__ASM_MACRO_DEF(conA8,0xAAAAAAA8)
__ASM_MACRO_DEF(con54,0x55555554)
;
/******************************************************************************/
/* EXTERN FUNCTION DEFINITIONS                                                */
/******************************************************************************/
    __ASM_IMPORT(FSF_vReg_FatalException)
;
/******************************************************************************/
/* PUBLIC FUNCTION DEFINITIONS                                                */
/******************************************************************************/
    __ASM_EXPORT(FSF_do_xPSRandR14LinkRegisterCheck)
    __ASM_EXPORT(FSF_doGeneralPurposeR0_R12RegisterCheck)
    __ASM_EXPORT(FSF_doStackPointerR13RegisterCheck)
;

/** \brief    Program Status (xPSR) and Link (R14) Registers  check.
 *
 *  \details  This function is doing PSR and Link (R14) registers check. In case
 *            of error jump to Fattal error reaction function
 *            This implementation is based on Self Test Library from
 *            STMicroelectronics stm32f0xx_STLcpurunKEIL.s file V1.0.0 from
 *            February 2012
 *
 *  \note     When possible, BRANCH are 16-bit only (depending on relative offset
 *            to final BL instruction)
 *
 *  \param    none
 *  \return   Branch directly to a Fail Safe routine in case of failure -> never
 *            return back
 */
__ASM_FUNCTION_L1(FSF_do_xPSRandR14LinkRegisterCheck)
__ASM_FUNCTION_L2(FSF_do_xPSRandR14LinkRegisterCheck)
__ASM_FUNCTION_L3
__ASM_FUNCTION(FSF_do_xPSRandR14LinkRegisterCheck)
    movs r0, #0x00
    uxtb r0,r0
    adds r0,#0                // Set Z(ero) Flag
    bne  CPUTestFail          // Fails if Z clear
    bmi  CPUTestFail          // Fails if N is set
    subs  r0,#1               // Set N(egative) Flag
    bpl  CPUTestFail          // Fails if N clear
    adds r0,#2                // Set C(arry) Flag and do not set Z
    bcc  CPUTestFail          // Fails if C clear
    beq  CPUTestFail          // Fails if Z is set
    bmi  CPUTestFail          // Fails if N is set

    ldr  r0,=con80            // Prepares Overflow test
    adds r0, r0, r0           // Set V(overflow) Flag
    bvc CPUTestFail           // Fails if V clear

#if !defined(CORTEX_M0) && !defined(CORTEX_M0_PLUS) && !defined(CORTEX_M23)
    //ARMv7-M extension (Cortex M3,4,7)
    movs r0, #0xFFFFFFFF       // Prepares Saturation test -> mov must be used
    usat r1, #10,R0            // Set Q(saturation) Flag
    mrs  r0, APSR              // Get Status register
    cmp  r0, #0xB8000000       // Verifies that N=C=V=Q=1
    bne  CPUTestFail           // Fails if Q+N+C=V clear
#endif

//    // Link register (Register R14)
    mov  r1, r14              // Contains the return address -> must be saved
    ldr  r0, =conAA           // Load addres of constant into R0
    mov  r14,r0               // Load test value from R0 into R14
    cmp  r0,r14               // Compare R0 and R14
    bne  CPUTestFail          // Fails if not equal
    ldr  r0, =con55           // Load addres of constant into R0
    mov  r14,r0               // Load test value from R0 into R14
    cmp  r0,r14               // Compare R0 and R14
    bne  CPUTestFail          // Fails if not equal
    mov  r14, r1              // Restore LR register value
    bal CPUTstCont

__ASM_LABEL(CPUTestFail)
    blal FSF_vReg_FatalException// never return back
                                // -> fatal error reaction
__ASM_LABEL(CPUTstCont)
    bx lr                     // return to the caller
    __ASM_END_FUNCTION(FSF_do_xPSRandR14LinkRegisterCheck)
;

/** \brief    General Purpose Registers (R0-R12) check.
 *
 *  \details  This function is doing General Purpose Registers (R0-R12) register
 *            check. In case of error jump to Fattal error reaction function
 *            This implementation is based on Self Test Library from
 *            STMicroelectronics stm32f0xx_STLcpurunKEIL.s file V1.0.0 from
 *            February 2012
 *
 *  \note     When possible, BRANCH are 16-bit only (depending on relative offset
 *            to final BL instruction)
 *
 *  \param    none
 *  \return   Branch directly to a Fail Safe routine in case of failure -> never
 *            return back
 */
__ASM_FUNCTION_L1(FSF_doGeneralPurposeR0_R12RegisterCheck)
__ASM_FUNCTION_L2(FSF_doGeneralPurposeR0_R12RegisterCheck)
__ASM_FUNCTION_L3
__ASM_FUNCTION(FSF_doGeneralPurposeR0_R12RegisterCheck)

#if defined(CORTEX_M0) || defined(CORTEX_M0_PLUS) || defined(CORTEX_M23)
    push {r4-r7}              // Safe critical registers
#else
    stmdb sp!, {r4-r11}
#endif

    // Register R1
    ldr  r0,=conAA            // Load address of conAA into R0
    ldr  r1,=conAA            // Load address of conAA into R1
    cmp  r0,r1                // compare R0 with R1
    bne  CPUTestFailGeneralReg// if it is not equal jumt to error
    ldr  r0, =con55           // Load address of con55 into R0
    ldr  r1, =con55           // Load address of con55 into R1
    cmp  r0,r1                // compare R0 with R1
    bne  CPUTestFailGeneralReg// if it is not equal jumt to error
    movs r1, #0x1             // For ramp test

    // Register R2
    ldr  r0, =conAA
    ldr  r2, =conAA
    cmp  r0,r2
    bne  CPUTestFailGeneralReg
    ldr  r0, =con55
    ldr  r2, =con55
    cmp  r0,r2
    bne  CPUTestFailGeneralReg
    movs r2, #0x2             // For ramp test

    // Register R3
    ldr  r0, =conAA
    ldr  r3, =conAA
#ifdef SYSTEMTEST_F11_REG
    adds r3, r3, #1           // provide error (0xAAAAAAAB) for system test
#endif
    cmp  r0,r3
    bne  CPUTestFailGeneralReg
    ldr  r0, =con55
    ldr  r3, =con55
    cmp  r0,r3
    bne  CPUTestFailGeneralReg
    movs r3, #0x3             // For ramp test

    // Register R4
    ldr  r0, =conAA
    ldr  r4, =conAA
    cmp  r0,r4
    bne  CPUTestFailGeneralReg
    ldr  r0, =con55
    ldr  r4, =con55
    cmp  r0,r4
    bne  CPUTestFailGeneralReg
    movs r4, #0x4             // For ramp test

    // Register R5
    ldr  r0, =conAA
    ldr  r5, =conAA
    cmp  r0,r5
    bne  CPUTestFailGeneralReg
    ldr  r0, =con55
    ldr  r5, =con55
    cmp  r0,r5
    bne  CPUTestFailGeneralReg
    movs r5, #0x5             // For ramp test

    // Register R6
    ldr  r0, =conAA
    ldr  r6,=conAA
    cmp  r0,r6
    bne  CPUTestFailGeneralReg
    ldr  r0, =con55
    ldr  r6, =con55
    cmp  r0,r6
    bne  CPUTestFailGeneralReg
    movs r6, #0x6             // For ramp test

    // Register R7
    ldr  r0, =conAA
    ldr  r7, =conAA
    cmp  r0,r7
    bne  CPUTestFailGeneralReg
    ldr  r0, =con55
    ldr  r7, =con55
    cmp  r0,r7
    bne  CPUTestFailGeneralReg
    movs r7, #0x7             // For ramp test

    // Register R8
    ldr  r0, =conAA
    mov  r8,r0
    cmp  r0,r8
    bne  CPUTestFailGeneralReg
    ldr  r0, =con55
    mov  r8,r0
    cmp  r0,r8
    bne  CPUTestFailGeneralReg
    movs r0, #0x08            // For ramp test
    mov  r8,r0

    bal  CPUTstContGeneralReg

__ASM_LABEL(CPUTestFailGeneralReg)
    blal FSF_vReg_FatalException// never return back
                                // -> fatal error reaction

__ASM_LABEL(CPUTstContGeneralReg)
    // Register R9
    ldr  r0, =conAA
    mov  r9,r0
    cmp  r0,r9
    bne  CPUTestFailGeneralReg
    ldr  r0, =con55
    mov  r9,r0
    cmp  r0,r9
    bne  CPUTestFailGeneralReg
    movs r0, #0x09            // For ramp test
    mov  r9,r0

    // Register R10
    ldr  r0, =conAA
    mov  r10,r0
    cmp  r0,r10
    bne  CPUTestFailGeneralReg
    ldr  r0, =con55
    mov  r10,r0
    cmp  r0,r10
    bne  CPUTestFailGeneralReg
    movs r0, #0x0A            // For ramp test
    mov  r10,r0

    // Register R11
    ldr  r0, =conAA
    mov  r11,r0
    cmp  r0,r11
    bne  CPUTestFailGeneralReg
    ldr  r0, =con55
    mov  r11,r0
    cmp  r0,r11
    bne  CPUTestFailGeneralReg
    movs r0, #0x0B            // For ramp test
    mov  r11,r0

    // Register R12
    ldr  r0, =conAA
    mov  r12,r0
    cmp  r0,r12
    bne  CPUTestFailGeneralReg
    ldr  r0, =con55
    mov  r12,r0
    cmp  r0,r12
    bne  CPUTestFailGeneralReg
    movs r0, #0x0C            // For ramp test
    mov  r12,r0
    movs r0, #0x0             // For ramp test

    // Ramp pattern verification
    cmp  r0, #0x00
    bne  CPUTestFailGeneralReg
    cmp  r1, #0x01
    bne  CPUTestFailGeneralReg
    cmp  r2, #0x02
    bne  CPUTestFailGeneralReg
    cmp  r3, #0x03
    bne  CPUTestFailGeneralReg
    cmp  r4, #0x04
    bne  CPUTestFailGeneralReg
    cmp  r5, #0x05
    bne  CPUTestFailGeneralReg
    cmp  r6, #0x06
    bne  CPUTestFailGeneralReg
    cmp  r7, #0x07
    bne  CPUTestFailGeneralReg
    movs r0, #0x08
    cmp  r0,r8
    bne  CPUTestFailGeneralReg
    movs r0, #0x09
    cmp  r0,r9
    bne  CPUTestFailGeneralReg
    movs r0, #0x0A
    cmp  r0,r10
    bne  CPUTestFailGeneralReg
    movs r0, #0x0B
    cmp  r0,r11
    bne  CPUTestFailGeneralReg
    movs r0, #0x0C
    cmp  r0,r12
    bne  CPUTestFailGeneralReg

#if defined(CORTEX_M0) || defined(CORTEX_M0_PLUS) || defined(CORTEX_M23)
    pop {r4-r7}               // Restore critical registers
#else
    ldmia sp!, {r4-r11}
#endif

    bx lr                     // return to the caller
    __ASM_END_FUNCTION(FSF_doGeneralPurposeR0_R12RegisterCheck)
;

/** \brief    Stack Pointer (R13) Register  check.
 *
 *  \details  This function is doing Stack Pointer (R13) register check. In case
 *            of error jump to Fattal error reaction function
 *            This implementation is based on Self Test Library from
 *            STMicroelectronics stm32f0xx_STLcpurunKEIL.s file V1.0.0 from
 *            February 2012
 *  \note     When possible, BRANCH are 16-bit only (depending on relative
 *            offset to final BL instruction)
 *
 *            A called routine need not preserve the values of r0-r3, IP (r12)
 *            and LR (r14) -> see Procedure Call Standard for the ARM
 *            Architecture (page.15)
 *
 *            The first four registers r0-r3 are used to pass parameter values
 *            into a routine and result values out of a routine and to hold
 *            intermediate values within a routine (but, in general, only
 *            between subroutine calls). In ARM-state, register r12 also
 *            called IP can also be used to hold intermediate values between
 *            subroutine calls.
 *
 *            Typically, the registers from r4 to r11 are used to hold the
 *            values of a routine's local variables. They are also labeled
 *            v1-v8. Only v1-v4 can be used uniformly by the whole Thumb
 *            instruction set (shown emboldened).
 *
 *            In all variants of the procedure call standard, registers r12-r15
 *            have special roles. In these roles they are labeled IP, SP, LR
 *            and PC (or ip, sp, lr, and pc, but this standard uses the upper
 *            case name for the special role).
 *
 *  \param    none
 *  \return   Branch directly to a Fail Safe routine in case of failure -> never
 *            return back
 */
__ASM_FUNCTION_L1(FSF_doStackPointerR13RegisterCheck)
__ASM_FUNCTION_L2(FSF_doStackPointerR13RegisterCheck)
__ASM_FUNCTION_L3
__ASM_FUNCTION(FSF_doStackPointerR13RegisterCheck)
   // Process Stack pointer (banked Register R13)
    mrs r0,psp          // save process stack value
    ldr r1, =conA8      // Test is different (PSP is word aligned, 2 LSB cleared)
    msr psp,r1          // load process stack value
    mrs r2,psp          // Get back process stack value
    cmp r2,r1           // Verify value
    bne CPUTestFailStackReg
    ldr r1, =con54      // Test is different (PSP is word aligned, 2 LSB cleared)
    msr psp,r1          // load process stack value
    mrs r2,psp          // Get back process stack value
    cmp r2,r1           // Verify value
    bne CPUTestFailStackReg
    msr psp, r0         // Restore process stack value

    // Main Stack pointer (banked Register R13)
    mrs r0,msp          // Save stack pointer value
    ldr r1, =conA8      // Test is different (SP is word aligned, 2 LSB cleared)
    msr msp,r1          // load SP value
    mrs r2,msp          // Get back SP value
    cmp r2,r1           // Verify value
    bne CPUTestFailStackReg
    ldr r1, =con54
    msr msp,r1          // Get back SP value
    mrs r2,msp          // Verify value
    cmp r2,r1
    bne CPUTestFailStackReg

    msr msp,r0          // Restore stack pointer value

    BAL CPUTestStackRegOK

__ASM_LABEL(CPUTestFailStackReg)
    BLAL FSF_vReg_FatalException// never return back
                                // -> fatal error reaction
__ASM_LABEL(CPUTestStackRegOK)
    bx lr                     // return to the caller
    __ASM_END_FUNCTION(FSF_doStackPointerR13RegisterCheck)
    nop
    __ASM_END_FILE
