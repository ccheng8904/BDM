    .syntax unified
    .text
    .thumb
    .align 2
;
.set BCKGRND, 0x00000000
;
;
    .global FSF_tRunTimeRamBufferCheckMarchC
    .global FSF_tRunTimeRamCheckMarchC
    .global FSF_tRunTimeRamCheckMarchX
.func FSF_tRunTimeRamCheckMarchX, FSF_tRunTimeRamCheckMarchX
.type FSF_tRunTimeRamCheckMarchX, "function"
.thumb_func
FSF_tRunTimeRamCheckMarchX:
    PUSH {r4-r7}
    LDR r2, =BCKGRND
    MVNS r3, r2
    MOVS r7, #0
    MOVS r6, #0x00
LoopStep1_X:
    LDR r4, [r0,r6]
    STR r4, [r1,r6]
    STR r2, [r0,r6]
    ADDS r6, r6, #4
    CMP r6, #(0x00000006 * 4)
    BCC LoopStep1_X
    MOVS r6, #0x00
LoopStep2_X:
    LDR r4, [r0,r6]
    CMP r4, r2
    BEQ EquStep2_X
    MOVS r7, #1
EquStep2_X:
    STR r3, [r0,r6]
    ADDS r6, r6, #4
    CMP r6, #(0x00000006 * 4)
    BCC LoopStep2_X
    MOVS r6, #((0x00000006 - 1) * 4)
LoopStep3_X:
    LDR r4, [r0,r6]
    CMP r4, r3
    BEQ EquStep3_X
    MOVS r7, #1
EquStep3_X:
    STR r2, [r0,r6]
    SUBS r6, r6, #4
    CMP r6, #(0x00000006 * 4)
    BPL LoopStep3_X
    MOVS r6, #0x00
LoopStep4_X:
    LDR r4, [r0,r6]
    CMP r4, r2
    BEQ EquStep4_X
    MOVS r7, #1
EquStep4_X:
    LDR r4, [r1,r6]
    STR r4, [r0,r6]
    ADDS r6, r6, #4
    CMP r6, #(0x00000006 * 4)
    BCC LoopStep4_X
    MOV r0,r7
    POP {r4-r7}
    BX lr
    NOP
    .endfunc
.func FSF_tRunTimeRamCheckMarchC, FSF_tRunTimeRamCheckMarchC
.type FSF_tRunTimeRamCheckMarchC, "function"
.thumb_func
FSF_tRunTimeRamCheckMarchC:
    PUSH {r4-r7}
    MOVS r7, #0
    LDR r2, =BCKGRND
    MVNS r3, r2
    MOVS r6, #0x00
LoopStep1_C:
    LDR r4, [r0,r6]
    STR r4, [r1,r6]
    STR r2, [r0,r6]
    ADDS r6, r6, #4
    CMP r6, #(0x00000006 * 4)
    BCC LoopStep1_C
    MOVS r6, #0x00
LoopStep2_C:
    LDR r4, [r0,r6]
    CMP r4, r2
    BEQ EquStep2_C
    MOVS r7, #1
EquStep2_C:
    STR r3, [r0,r6]
    ADDS r6, r6, #4
    CMP r6, #(0x00000006 * 4)
    BCC LoopStep2_C
    MOVS r6, #0x00
LoopStep3_C:
    LDR r4, [r0,r6]
    CMP r3, r4
    BEQ EquStep3_C
    MOVS r7, #1
EquStep3_C:
    STR r2, [r0,r6]
    ADDS r6, r6, #4
    CMP r6, #(0x00000006 * 4)
    BCC LoopStep3_C
    MOVS r6, #((0x00000006 - 1) * 4)
LoopStep4_C:
    LDR r4, [r0,r6]
    CMP r4, r2
    BEQ EquStep4_C
    MOVS r7, #1
EquStep4_C:
    STR r3, [r0,r6]
    SUBS r6, r6, #4
    BPL LoopStep4_C
    MOVS r6, #((0x00000006 - 1) * 4)
LoopStep5_C:
    LDR r4, [r0,r6]
    CMP r3, r4
    BEQ EquStep5_C
    MOVS r7, #1
EquStep5_C:
    STR r2, [r0,r6]
    SUBS r6, r6, #4
    BPL LoopStep5_C
    MOVS r6, #0x00
LoopStep6_C:
    LDR r4, [r0,r6]
    CMP r4, r2
    BEQ EquStep6_C
    MOVS r7, #1
EquStep6_C:
    LDR r4, [r1,r6]
    STR r4, [r0,r6]
    ADDS r6, r6, #4
    CMP r6, #(0x00000006 * 4)
    BCC LoopStep6_C
    MOV r0,r7
    POP {r4-r7}
    BX lr
    NOP
    .endfunc
.func FSF_tRunTimeRamBufferCheckMarchC, FSF_tRunTimeRamBufferCheckMarchC
.type FSF_tRunTimeRamBufferCheckMarchC, "function"
.thumb_func
FSF_tRunTimeRamBufferCheckMarchC:
    PUSH {r4-r7}
    LDR r2, =BCKGRND
    MVNS r3, r2
    MOVS r7, #0
    MOVS r6, #0x00
LoopBuffStep1:
    STR r2, [r0,r6]
    ADDS r6, r6, #4
    CMP r6, #0x20
    BCC LoopBuffStep1
    MOVS r6, #0x00
LoopBuffStep2:
    LDR r4, [r0,r6]
    CMP r4, r2
    BEQ EquBuffStep2
    MOVS r7, #1
EquBuffStep2:
    STR r3, [r0,r6]
    ADDS r6, r6, #4
    CMP r6, #0x20
    BCC LoopBuffStep2
    MOVS r6, #0x00
LoopBuffStep3:
    LDR r4, [r0,r6]
    CMP r4, r3
    BEQ EquBuffStep3
    MOVS r7, #1
EquBuffStep3:
    STR r2, [r0,r6]
    ADDS r6, r6, #4
    CMP r6, #0x20
    BCC LoopBuffStep3
    MOVS r6, #(0x20 - 4)
LoopBuffStep4:
    LDR r4, [r0,r6]
    CMP r4, r2
    BEQ EquBuffStep4
    MOVS r7, #1
EquBuffStep4:
    STR r3, [r0,r6]
    SUBS r6, r6, #4
    BPL LoopBuffStep4
    MOVS r6, #(0x20 - 4)
LoopBuffStep5:
    LDR r4, [r0,r6]
    CMP r4, r3
    BEQ EquBuffStep5
    MOVS r7, #1
EquBuffStep5:
    STR r2, [r0,r6]
    SUBS r6, r6, #4
    BPL LoopBuffStep5
    MOVS r6, #0x00
LoopBuffStep6:
    LDR r4, [r0,r6]
    CMP r4, r2
    BEQ EquBuffStep6
    MOVS r7, #1
EquBuffStep6:
    ADDS r6, r6, #4
    CMP r6, #0x20
    BCC LoopBuffStep6
    MOV r0, r7
    POP {r4-r7}
    BX lr
    .endfunc
;
    .end
