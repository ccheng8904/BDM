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
/** \file fs_Start_Up_Ram.S
 *
 *  \ingroup  FunctionalSafety
 *
 *  \brief    Implementation of startup RAM check for ARM CortexMx derivates.
 *
 *  This implementation is based on self test library from STMicrolectronics.
 *  Source file is stm32f0xx_STLfullRamMc.c version V1.0.0 from February 2012
 *
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
__ASM_MACRO_DEF(BCKGRND,0x00000000)
;
/******************************************************************************/
/* EXTERN FUNCTION DEFINITIONS                                                */
/******************************************************************************/
    __ASM_IMPORT(FSF_vRam_FatalException)
;
/******************************************************************************/
/* PUBLIC FUNCTION DEFINITIONS                                                */
/******************************************************************************/
    __ASM_EXPORT(FSF_vRamcheck)

/** \brief    StartUp RAM check.
 *
 *  \details  This function is doing RAM check after reset. In case
 *            of error jump to Fattal error reaction function. 
 *            This implementation is based on March C- algorithm.
 *            (w0 ↑)(r0,w1 ↑)(r1,w0 ↑)(r0,w1 ↓)(r1,w0 ↓)(r0 ↑)
 *
 *  \note     This test is destructive -> must be called immediately ater reset
 *
 *  \param    none
 *  \return   Branch directly to a Fail Safe routine in case of failure -> never
 *            return back
 */
__ASM_FUNCTION_L1(FSF_vRamcheck)
__ASM_FUNCTION_L2(FSF_vRamcheck)
__ASM_FUNCTION_L3
__ASM_FUNCTION(FSF_vRamcheck)
    // r0, r1, r2 are input arguments
    // r0 = pucRamSegments
    // r1 = NumberOfRamSegments
    // r2 = u32ExcludedData

    // r0 = working register
    // r1 = working register
    // r2 = address counter
    // r4 = BCKGRND
    // r5 = INV_BCKGRND
    // r7 = NumberOfRamSegments
    // r8 = RamStart
    // r9 = RamEnd
    // r11 = pucRamSegments

#ifdef TH_FSCOM                 // Start UP RAM test is not possible
                                // in Test Harrnes mode (RAM variables are
                                // not initialized )
    BX     lr                   // return to the caller
#else  //#ifdef TH_FSCOM

    MOV    r11, r0              // save pucRamSegments pointer to r11
    MOV    r7, r1               // save NumberOfRamSegments to r7

    LDR    r0, [r0]             // load RamStart addr. from pucRamSegments to r0
    ADDS   r0, r0, r2           // SharedData are excluded
    MOV    r8, r0               // load RamStart to r8

    MOV    r0, r11              // load pucRamSegments pointer to r0
    ADDS   r0, r0, #4           // move pointer to next element (uint32) - RamEnd
    LDR    r0, [r0]             // load RamEnd addr. from pucRamSegments to r0
    ADDS   r0, r0, #1           // RamEnd + 1 =>  RAM_END must be aligned to 4

    MOV    r9, r0               // load RamEnd to r9

    LDR    r4, =BCKGRND         // load pointer to BCKGRND to r4
    MVNS   r5, r4               // Bitwise NOT r4 (INV_BCKGRND) to r5

__ASM_LABEL(MainLoop)

    /* ---------------------------- STEP 1 ------------------------------------- */
    /*           Write background with addresses increasing                      */
    /*                               (w0 ↑)                                      */
    MOV    r2, r8               // load RamStart(r8) to act. address(r2)
__ASM_LABEL(LoopStep1)
    STR    r4, [r2]             // write one uint32 by patern BCKGRND(r4)
    ADDS   r2, r2, #4           // increment act. address(r2) (#4 - used uint32)
    CMP    r2, r9               // compare act. address(r2) with RamEnd(r9)
    BCC    LoopStep1

    /* ---------------------------- STEP 2 ------------------------------------- */
    /* Verify background and write inverted background with addresses increasing */
    /*                              (r0,w1 ↑)                                    */

    MOV    r2, r8               // load RamStart(r8) to act. address(r2)
__ASM_LABEL(LoopStep2)
    LDR    r0, [r2]                 // load pattern from act. addr(r2) to r0
    CMP    r0, r4                   // compare act. pattern with pattern BCKGRND(r4)
    BNE    TestFail

    STR    r5, [r2]                 // write pattern INV_BCKGRND(r5) to act. addr(r2)

    ADDS   r2, r2, #4               // increment act. address(r2) + 4 bytes
    CMP    r2, r9                   // compare act. address(r2) with RamEnd(r9)
    BCC    LoopStep2

    /* ---------------------------- STEP 3 ------------------------------------- */
    /* Verify inverted background and write background with addresses increasing */
    /*                              (r1,w0 ↑)                                    */

    MOV    r2, r8                   // load RamStart(r8) to act. address(r2)
__ASM_LABEL(LoopStep3)
    LDR    r0, [r2]                 // load pattern from act. addr(r2) to r0
#ifdef SYSTEMTEST_F11_RAM
    SUBS   r0, r0, #1               // provide error (0xFFFFFFFE) for system test
#endif
    CMP    r0, r5                   // compare act. pattern with pattern INV_BCKGRND(r5)
    BNE    TestFail

    STR    r4, [r2]                 // write pattern BCKGRND(r4) to act. addr(r2)

    ADDS   r2, r2, #4               // increment act. address(r2) + 4 bytes
    CMP    r2, r9                   // compare act. address(r2) with RamEnd(r9)
    BCC    LoopStep3

    /* ---------------------------- STEP 4 ------------------------------------- */
    /* Verify background and write inverted background with addresses decreasing */
    /*                              (r0,w1 ↓)                                    */

    MOV    r2, r9                   // load RamEnd(r9) to act. address(r2)
__ASM_LABEL(LoopStep4)
    SUBS   r2, r2, #4               // decrement act. address(r2) - 0x4 bytes

    LDR    r0, [r2]                 // load pattern from act. addr(r2) to r0
    CMP    r0, r4                   // compare act. pattern with pattern BCKGRND(r4)
    BNE    TestFail

    STR    r5, [r2]                 // write pattern INV_BCKGRND(r5) to address(r2) 

    CMP    r2, r8                   // compare act. address(r2) with RamStart(r8)
    BHI    LoopStep4

    /* ---------------------------- STEP 5 ------------------------------------- */
    /* Verify inverted background and write background with addresses decreasing */
    /*                              (r1,w0 ↓)                                    */

    MOV    r2, r9                   // load RamEnd(r9) to act. address(r2)
__ASM_LABEL(LoopStep5)
    SUBS   r2, r2, #4              // decrement act. address(r2) -> 4 bytes

    LDR    r0, [r2]                 // load pattern from act. addr(r2) to r0
    CMP    r0, r5                   // compare act. pattern with pattern INV_BCKGRND(r5)
    BNE    TestFail

    STR    r4, [r2]                 // write pattern BCKGRND(r4) to address(r2)

    CMP    r2, r8                   // compare act. address(r2) with RamStart(r8)
    BHI    LoopStep5

    /* ---------------------------- STEP 6 ------------------------------------- */
    /*              Verify background with addresses increasing                  */
    /*                              (r0 ↑)                                       */

    MOV    r2, r8                   // load RamStart(r8) to act. address(r2)
__ASM_LABEL(LoopStep6)
    LDR    r0, [r2]                 // load pattern from act. addr(r2) to r0
    CMP    r0, r4                   // compare act. pattern with pattern BCKGRND(r4)
    BNE    TestFail

    ADDS   r2, r2, #4               // increment act. address(r2) (#4 - used uint32)
    CMP    r2, r9                   // compare act. address(r2) with RamEnd(r9)
    BCC    LoopStep6

    SUBS   r7, r7, #1               // decrement NumberOfRamSegments
    CMP    r7, #0                   // all Ram segments are checked?
    BNE    InitOfNextCheck          // initialization of Ram check for next segment

    BX     lr                       // return to the caller

__ASM_LABEL(TestFail)
    BL      FSF_vRam_FatalException // BL used instead of B because jump limitation C-M0+ (Armv6)

__ASM_LABEL(InitOfNextCheck)
    MOV    r0, r11                  // load pucRamSegments pointer to r0
    ADDS   r0, r0, #8               // move pointer to next RamSegment(Ram2Start),
                                    // #8 because Ram1Start-uint32 Ram1End-uint32
    MOV    r11, r0                  // save pointer to pucRamSegments to r11
    LDR    r0, [r0]                 // load Ram2Start addr. from pucRamSegments to r0
    MOV    r8, r0                   // load RamStart to r8

    MOV    r0, r11                  // load pucRamSegments pointer to r0
    ADDS   r0, r0, #4               // move pointer to next element (uint32) - Ram2End
    LDR    r0, [r0]                 // load Ram2End addr. from pucRamSegments to r0
    ADDS   r0, r0, #1               // Ram2End + 1 =>  RAM2_END must be aligned to 4

    MOV    r9, r0                   // load RamEnd to r9
    B      MainLoop                 // checking next segment of Ram memory
#endif  //#ifdef TH_FSCOM
    __ASM_END_FUNCTION(FSF_vRamcheck)
;
    NOP
    __ASM_END_FILE
