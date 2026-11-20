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
/** \file fsRunTimeRam.
 *
 *  \ingroup  FunctionalSafety
 *
 *  \brief    Platform specific implementation Run Time RAM check.
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

#ifndef MARCHBUFLENGTH
#define MARCHBUFLENGTH 32
#endif

#ifndef RAM_BLOCKSIZE
#define RAM_BLOCKSIZE 6
#endif

;
/******************************************************************************/
/* EXTERN FUNCTION DEFINITIONS                                                */
/******************************************************************************/

;
/******************************************************************************/
/* PUBLIC FUNCTION DEFINITIONS                                                */
/******************************************************************************/
    __ASM_EXPORT(FSF_tRunTimeRamBufferCheckMarchC)
    __ASM_EXPORT(FSF_tRunTimeRamCheckMarchC)
    __ASM_EXPORT(FSF_tRunTimeRamCheckMarchX)
    
/** \brief    Run Time RAM check (Partial RAM run mode self test).
 *
 *  \details  This function is doing RAM check in normal operation.  In case
 *            of error retutn 1 othervise 0.
 *            The test covers the part of the RAM containing the class B variables.
 *            One block of #RAM_BLOCKSIZE bytes is tested at every test step.
 *            To guarantee coupling fault coverage, consecutive test steps are 
 *            performed on memory blocks with an overlapping of two adjacent bytes.
 *            During the first phase, the block content is stored in the storage 
 *            buffer. The next phase is to perform the marching destructive tests
 *            on all the bytes in the tested RAM block. Then, the final step is 
 *            to restore the original content. This implementation is based on 
 *            March X algorithm:
  *                   (w0 ↑)(r0,w1 ↑)(r1,w0 ↓)(r0 ↑)
 *
 *  \param    pui32RamPiece [uint32_t *] - pointer on piece of RAM
 *
 *  \param    pui32RamBuf [uint32_t *] - pointer on buffer
 *
 *  \return   0 - success, 1 - failure
 */
__ASM_FUNCTION_L1(FSF_tRunTimeRamCheckMarchX)
__ASM_FUNCTION_L2(FSF_tRunTimeRamCheckMarchX)
__ASM_FUNCTION_L3
__ASM_FUNCTION(FSF_tRunTimeRamCheckMarchX)
    // r0 = pui32RamPiece
    // r1 = pui32RamBuf
    // r2 = BCKGRND
    // r3 = INV_BCKGRND
    // r4 = working register

    // r6 = counter/index into memory/buffer
    // r7 = return value
    
    PUSH   {r4-r7} 
    
    LDR    r2, =BCKGRND         // load pointer to BCKGRND into r2
    MVNS   r3, r2               // Bitwise NOT r2 (INV_BCKGRND) into r3    
    MOVS   r7, #0               // return value = 0 (OK) stored in r7

    /*---------------------------- STEP 1 ---------------------------------- */
    /*    Save the content of the RAM_BLOCKSIZE words to be tested and start */
    /*             Write background with addresses increasing                */
    /*                              (w0 ↑)                                   */

    MOVS   r6, #0x00            // reset counter
__ASM_LABEL(LoopStep1_X)
    LDR    r4, [r0,r6]          // load mem cell into r7
    STR    r4, [r1,r6]          // store mem cell into buff
    STR    r2, [r0,r6]          // put BCKGRND into mem cell
    ADDS   r6, r6, #4           // counter++
    CMP    r6, #(RAM_BLOCKSIZE * 4)
    BCC    LoopStep1_X          // if (counter < RAM_BLOCKSIZE) jump LoopStep1_X

    /*---------------------------- STEP 2 ---------------------------------- */
    /* Verify background and write inverted background addresses increasing  */
    /*                            (r0,w1 ↑)                                  */

    MOVS   r6, #0x00            // reset counter
__ASM_LABEL(LoopStep2_X)
    LDR    r4, [r0,r6]          // load mem cell into r1
    CMP    r4, r2
    BEQ    EquStep2_X           // if (mem cell == BCKGRND) jump EquStep2_X
    MOVS   r7, #1               // otherwise r7 (return value) = error
__ASM_LABEL(EquStep2_X)
    STR    r3, [r0,r6]          // store INV_BCKGRND into mem cell
    ADDS   r6, r6, #4           // counter++
    CMP    r6, #(RAM_BLOCKSIZE * 4)
    BCC    LoopStep2_X          // if (counter < RAM_BLOCKSIZE) jump LoopStep2_X

    /*---------------------------- STEP 3 ---------------------------------- */
    /*  Verify inverted background and write background addresses decreasing */    
    /*                            (r1,w0 ↓)                                  */
    MOVS   r6, #((RAM_BLOCKSIZE - 1) * 4)    // reset counter
__ASM_LABEL(LoopStep3_X)
    LDR    r4, [r0,r6]          // load mem cell into r1
    CMP    r4, r3
    BEQ    EquStep3_X           // if (mem cell == INV_BCKGRND) jump EquStep3_X
    MOVS   r7, #1               // otherwise r7 (return value) = error
__ASM_LABEL(EquStep3_X)
    STR    r2, [r0,r6]          // store BCKGRND into mem cell
    SUBS   r6, r6, #4           // counter--
    CMP    r6, #(RAM_BLOCKSIZE * 4)
    BPL    LoopStep3_X          // if (counter < RAM_BLOCKSIZE) jump LoopStep3_X

    /*---------------------------- STEP 4 --------------------------------- */
    /* Verify background with addresses increasing and restore the content  */
    /* of the 6 tested words                                                */
    /*                            (r0 ↑)                                    */

    MOVS   r6, #0x00            // reset RT_RAM_SCRMBL counter
__ASM_LABEL(LoopStep4_X)
    LDR    r4, [r0,r6]          // load mem cell into r7
    CMP    r4, r2
    BEQ    EquStep4_X           // if (mem cell == BCKGRND) jump EquStep6
    MOVS   r7, #1               // otherwise r7 (return value) = error
__ASM_LABEL(EquStep4_X)
    LDR    r4, [r1,r6]          // load stored valu from buffer into r7
    STR    r4, [r0,r6]          // restore tested mem cell
    ADDS   r6, r6, #4           // counter++
    CMP    r6, #(RAM_BLOCKSIZE * 4)
    BCC    LoopStep4_X          // if (counter < RAM_BLOCKSIZE) jump LoopStep6

    MOV    r0,r7                // return value - in register R7

    POP    {r4-r7}

    BX     lr                   // return to the caller

    NOP
    __ASM_END_FUNCTION(FSF_tRunTimeRamCheckMarchX)



/** \brief    Run Time RAM check (Partial RAM run mode self test).
 *
 *  \details  This function is doing RAM check in normal operation.  In case
 *            of error retutn 1 othervise 0.
 *            The test covers the part of the RAM containing the class B variables.
 *            One block of #RAM_BLOCKSIZE bytes is tested at every test step.
 *            To guarantee coupling fault coverage, consecutive test steps are 
 *            performed on memory blocks with an overlapping of two adjacent bytes.
 *            During the first phase, the block content is stored in the storage 
 *            buffer. The next phase is to perform the marching destructive tests
 *            on all the bytes in the tested RAM block. Then, the final step is 
 *            to restore the original content. This implementation is based on 
 *            March C- algorithm:
 *                   (w0 ↑)(r0,w1 ↑)(r1,w0 ↑)(r0,w1 ↓)(r1,w0 ↓)(r0 ↑)
 *
 *  \param    pui32RamPiece [uint32_t *] - pointer on piece of RAM
 *
 *  \param    pui32RamBuf [uint32_t *] - pointer on buffer
 *
 *  \return   0 - success, 1 - failure
 */
__ASM_FUNCTION_L1(FSF_tRunTimeRamCheckMarchC)
__ASM_FUNCTION_L2(FSF_tRunTimeRamCheckMarchC)
__ASM_FUNCTION_L3
__ASM_FUNCTION(FSF_tRunTimeRamCheckMarchC)
    // r0 = pui32RamPiece
    // r1 = pui32RamBuf
    // r2 = BCKGRND
    // r3 = INV_BCKGRND
    // r4 = working register

    // r6 = counter/index into memory/buffer
    // r7 = return value
    
    PUSH   {r4-r7}
    
    MOVS   r7, #0                   // return value = 0 (OK) stored in r8

    LDR    r2, =BCKGRND             // load BCKGRND into r2
    MVNS   r3, r2                   // Bitwise NOT r2 (INV_BCKGRND) into r3

    /*---------------------------- STEP 1 ---------------------------------- */
    /* Save the content of the 6 words to be tested and start MarchC-        */
    /* Write background with addresses increasing                            */
    /*                              (w0 ↑)                                   */

    MOVS   r6, #0x00            // reset counter
__ASM_LABEL(LoopStep1_C)
    LDR    r4, [r0,r6]          // load mem cell into r4
    STR    r4, [r1,r6]          // store mem cell into buff
    STR    r2, [r0,r6]          // put BCKGRND into mem cell
    ADDS   r6, r6, #4           // counter++
    CMP    r6, #(RAM_BLOCKSIZE * 4)
    BCC    LoopStep1_C          // if (counter < RAM_BLOCKSIZE) jump LoopStep1_C

    /*---------------------------- STEP 2 ---------------------------------- */
    /* Verify background and write inverted background addresses increasing  */
    /*                              (r0,w1 ↑)                                */

    MOVS   r6, #0x00            // reset counter
__ASM_LABEL(LoopStep2_C)
    LDR    r4, [r0,r6]          // load mem cell into r4
    CMP    r4, r2
    BEQ    EquStep2_C           // if (mem cell == BCKGRND) jump EquStep2_C
    MOVS   r7, #1               // otherwise r8 (return value) = error
__ASM_LABEL(EquStep2_C)
    STR    r3, [r0,r6]          // store INV_BCKGRND into mem cell
    ADDS   r6, r6, #4           // counter++
    CMP    r6, #(RAM_BLOCKSIZE * 4)
    BCC    LoopStep2_C          // if (counter < RAM_BLOCKSIZE) jump LoopStep2_C

    /*---------------------------- STEP 3 ---------------------------------- */
    /* Verify inverted background and write background addresses increasing  */
    /*                            (r1,w0 ↑)                                  */

    MOVS   r6, #0x00            // reset counter
__ASM_LABEL(LoopStep3_C)
    LDR    r4, [r0,r6]          // load mem cell into r7
    CMP    r3, r4
    BEQ    EquStep3_C           // if (mem cell == INV_BCKGRND) jump EquStep3_C
    MOVS   r7, #1               // otherwise r8 (return value) = error
__ASM_LABEL(EquStep3_C)
    STR    r2, [r0,r6]          // store BCKGRND into mem cell
    ADDS   r6, r6, #4           // counter++
    CMP    r6, #(RAM_BLOCKSIZE * 4)
    BCC    LoopStep3_C          // if (counter < RAM_BLOCKSIZE) jump LoopStep3_C

    /*---------------------------- STEP 4 ---------------------------------- */
    /* Verify background and write inverted background addresses decreasing  */
    /*                              (r0,w1 ↓)                                */

    MOVS   r6, #((RAM_BLOCKSIZE - 1) * 4)    // reset counter
__ASM_LABEL(LoopStep4_C)
    LDR    r4, [r0,r6]          // load mem cell into r7
    CMP    r4, r2
    BEQ    EquStep4_C           // if (mem cell == BCKGRND) jump EquStep4_C
    MOVS   r7, #1               // otherwise r8 (return value) = error
__ASM_LABEL(EquStep4_C)
    STR    r3, [r0,r6]          // store INV_BCKGRND into mem cell
    SUBS   r6, r6, #4           // counter--
    BPL    LoopStep4_C          // if (counter >= 0) jump LoopStep4_C

    /*---------------------------- STEP 5 --------------------------------- */
    /* Verify inverted background and write background addresses decreasing */
    /*                              (r1,w0 ↓)                               */

    MOVS   r6, #((RAM_BLOCKSIZE - 1) * 4)    // reset counter
__ASM_LABEL(LoopStep5_C)
    LDR    r4, [r0,r6]          // load mem cell into r7
    CMP    r3, r4
    BEQ    EquStep5_C           // if (mem cell == INV_BCKGRND) jump EquStep5_C
    MOVS   r7, #1               // otherwise r8 (return value) = error
__ASM_LABEL(EquStep5_C)
    STR    r2, [r0,r6]          // store BCKGRND into mem cell
    SUBS   r6, r6, #4           // counter--
    BPL    LoopStep5_C          // if (counter >= 0) jump LoopStep5_C

    /*---------------------------- STEP 6 --------------------------------- */
    /* Verify background with addresses increasing and restore the content  */
    /* of the 6 tested words                                                */
    /*                              (r0 ↑)                                  */

    MOVS   r6, #0x00             // reset counter
__ASM_LABEL(LoopStep6_C)
    LDR    r4, [r0,r6]           // load mem cell into r7
    CMP    r4, r2
    BEQ    EquStep6_C            // if (mem cell == BCKGRND) jump EquStep6_C
    MOVS   r7, #1                // otherwise r8 (return value) = error
__ASM_LABEL(EquStep6_C)
    LDR    r4, [r1,r6]           // load stored valu from buffer into r7
    STR    r4, [r0,r6]           // restore tested mem cell
    ADDS   r6, r6, #4            // counter++
    CMP    r6, #(RAM_BLOCKSIZE * 4)
    BCC    LoopStep6_C           // if (counter < RAM_BLOCKSIZE) jump LoopStep6_C

    MOV    r0,r7                 // return value - in register R7

    POP    {r4-r7}
    
    BX     lr                    // return to the caller

    NOP                          // alignment
    __ASM_END_FUNCTION(FSF_tRunTimeRamCheckMarchC)

/** \brief    Run Time RAM Buffer check .
 *
 *  \details  This function is doing check of buffer, before test of piece of RAM.
 *            In case of error retutn 1 othervise 0.
 *            This implementation is based on March C- algorithm:
 *                   (w0 ↑)(r0,w1 ↑)(r1,w0 ↑)(r0,w1 ↓)(r1,w0 ↓)(r0 ↑)
 *
 *  \param    pui32RamBuf [uint32_t *] - pointer on buffer
 *
 *  \return   0 - success, 1 - failure
 */
__ASM_FUNCTION_L1(FSF_tRunTimeRamBufferCheckMarchC)
__ASM_FUNCTION_L2(FSF_tRunTimeRamBufferCheckMarchC)
__ASM_FUNCTION_L3
__ASM_FUNCTION(FSF_tRunTimeRamBufferCheckMarchC)
    // r0 = pui32RamPiece
    // r1 = pui32RamBuf
    // r2 = BCKGRND
    // r3 = INV_BCKGRND
    // r4 = working register

    // r6 = counter/index into memory/buffer
    // r7 = return value

    PUSH   {r4-r7}

    LDR    r2, =BCKGRND             // load pointe to BCKGRND into r2
    MVNS   r3, r2                   // load INV_BCKGRND to r3

    MOVS   r7, #0                   // return value = 0 (OK) stored in r2

    /*---------------------------- STEP 1 -----------------------------------*/
    /*      Save the content of the 6 words to be tested and start MarchC-   */
    /*          Write background with addresses increasing                   */
    /*                              (w0 ↑)                                   */

    MOVS   r6, #0x00                // reset counter
__ASM_LABEL(LoopBuffStep1)
    STR    r2, [r0,r6]              // store BCKGRND into buff-> buf[r6]=BCKGRND
    ADDS   r6, r6, #4               // counter++
    CMP    r6, #MARCHBUFLENGTH      // compare with buff lengh 
    BCC    LoopBuffStep1            // if (counter < MARCHBUFLENGTH)
                                    // jump LoopBuffStep1

    /*---------------------------- STEP 2 -----------------------------------*/
    /* Verify background and write inverted background addresses increasing  */
    /*                              (r0,w1 ↑)                                */

    MOVS   r6, #0x00                // reset counter
__ASM_LABEL(LoopBuffStep2)
    LDR    r4, [r0,r6]              // read value from buff r4 = buff[r6]
    CMP    r4, r2
    BEQ    EquBuffStep2             // if (r4 == BCKGRND) jump EquBuffStep2
    MOVS   r7, #1                   // if not set error into return value
__ASM_LABEL(EquBuffStep2)
    STR    r3, [r0,r6]              // store INV_BCKGRND into buff-> buf[r6]=INV_BCKGRND
    ADDS   r6, r6, #4               // counter++
    CMP    r6, #MARCHBUFLENGTH      // compare with buff lengh 
    BCC    LoopBuffStep2            // if (counter < MARCHBUFLENGTH)
                                    // jump LoopBuffStep2

    /*---------------------------- STEP 3 -----------------------------------*/
    /* Verify inverted background and write background addresses increasing  */
    /*                            (r1,w0 ↑)                                  */

    MOVS   r6, #0x00                // reset counter
__ASM_LABEL(LoopBuffStep3)
    LDR    r4, [r0,r6]              // read value from buff r4 = buff[r6]
    CMP    r4, r3
    BEQ    EquBuffStep3             // if (r4 == INV_BCKGRND) jump EquBuffStep3
    MOVS   r7, #1                   // if not set error into return value
__ASM_LABEL(EquBuffStep3)
    STR    r2, [r0,r6]              // store BCKGRND into buff-> buff[r6]=BCKGRND
    ADDS   r6, r6, #4               // counter++
    CMP    r6, #MARCHBUFLENGTH      // compare with buff lengh 
    BCC    LoopBuffStep3            // if (counter < MARCHBUFLENGTH)
                                    // jump LoopBuffStep3

    /*---------------------------- STEP 4 -----------------------------------*/
    /* Verify background and write inverted background addresses decreasing  */
    /*                              (r0,w1 ↓)                                */

    MOVS   r6, #(MARCHBUFLENGTH - 4)// counter = MARCHBUFLENGTH
__ASM_LABEL(LoopBuffStep4)
    LDR    r4, [r0,r6]              // read value from buff r4 = buff[r6]
    CMP    r4, r2
    BEQ    EquBuffStep4             // if (r4 == BCKGRND) jump EquBuffStep4
    MOVS   r7, #1                   // if not set error into return value
__ASM_LABEL(EquBuffStep4)
    STR    r3, [r0,r6]              // store INV_BCKGRND into buff-> buff[r6]=INV_BCKGRND
    SUBS   r6, r6, #4               // counter--
    BPL    LoopBuffStep4            // if (counter >= 0) jump LoopBuffStep4

    /*---------------------------- STEP 5 --------------------------------- */
    /* Verify inverted background and write background addresses decreasing */
    /*                              (r1,w0 ↓)                               */

    MOVS   r6, #(MARCHBUFLENGTH - 4)// counter = MARCHBUFLENGTH
__ASM_LABEL(LoopBuffStep5)
    LDR    r4, [r0,r6]              // read value from memory r4 = memory[r6]
    CMP    r4, r3
    BEQ    EquBuffStep5             // if (r4 == INV_BCKGRND) jump EquBuffStep5
    MOVS   r7, #1                   // if not set error into return value
__ASM_LABEL(EquBuffStep5)
    STR    r2, [r0,r6]              // store BCKGRND into memmory-> mem[r6]=BCKGRND
    SUBS   r6, r6, #4               // counter--
    BPL    LoopBuffStep5            // if (counter >= 0) jump LoopBuffStep5

    /*---------------------------- STEP 6 --------------------------------- */
    /*              Verify background with addresses increasing             */
    /*              and restore the content of the 6 tested words           */
    /*                              (r0 ↑)                                  */

    MOVS   r6, #0x00                // reset counter
__ASM_LABEL(LoopBuffStep6)
    LDR    r4, [r0,r6]              // read value from memory r4 = memory[r6]
    CMP    r4, r2
    BEQ    EquBuffStep6             // if (r4 == BCKGRND) jump EquBuffStep6
    MOVS   r7, #1                   // if not set error into return value
__ASM_LABEL(EquBuffStep6)
    ADDS   r6, r6, #4               // counter++
    CMP    r6, #MARCHBUFLENGTH      // compare with buff lengh 
    BCC    LoopBuffStep6            // if (counter < MARCHBUFLENGTH)
                                    // jump LoopBuffStep6

    MOV    r0, r7                   // return value - in register R7

    POP    {r4-r7}
    
    BX     lr                       // return to the caller

    __ASM_END_FUNCTION(FSF_tRunTimeRamBufferCheckMarchC)
;
    __ASM_END_FILE
