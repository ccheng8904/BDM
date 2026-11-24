    .syntax unified
    .text
    .thumb
    .align 2
;
.set BCKGRND, 0x00000000
;
    .extern FSF_vRam_FatalException
;
    .global FSF_vRamcheck
.func FSF_vRamcheck, FSF_vRamcheck
.type FSF_vRamcheck, "function"
.thumb_func
FSF_vRamcheck:
    MOV r11, r0
    MOV r7, r1
    LDR r0, [r0]
    ADDS r0, r0, r2
    MOV r8, r0
    MOV r0, r11
    ADDS r0, r0, #4
    LDR r0, [r0]
    ADDS r0, r0, #1
    MOV r9, r0
    LDR r4, =BCKGRND
    MVNS r5, r4
MainLoop:
    MOV r2, r8
LoopStep1:
    STR r4, [r2]
    ADDS r2, r2, #4
    CMP r2, r9
    BCC LoopStep1
    MOV r2, r8
LoopStep2:
    LDR r0, [r2]
    CMP r0, r4
    BNE TestFail
    STR r5, [r2]
    ADDS r2, r2, #4
    CMP r2, r9
    BCC LoopStep2
    MOV r2, r8
LoopStep3:
    LDR r0, [r2]
    CMP r0, r5
    BNE TestFail
    STR r4, [r2]
    ADDS r2, r2, #4
    CMP r2, r9
    BCC LoopStep3
    MOV r2, r9
LoopStep4:
    SUBS r2, r2, #4
    LDR r0, [r2]
    CMP r0, r4
    BNE TestFail
    STR r5, [r2]
    CMP r2, r8
    BHI LoopStep4
    MOV r2, r9
LoopStep5:
    SUBS r2, r2, #4
    LDR r0, [r2]
    CMP r0, r5
    BNE TestFail
    STR r4, [r2]
    CMP r2, r8
    BHI LoopStep5
    MOV r2, r8
LoopStep6:
    LDR r0, [r2]
    CMP r0, r4
    BNE TestFail
    ADDS r2, r2, #4
    CMP r2, r9
    BCC LoopStep6
    SUBS r7, r7, #1
    CMP r7, #0
    BNE InitOfNextCheck
    BX lr
TestFail:
    BL FSF_vRam_FatalException
InitOfNextCheck:
    MOV r0, r11
    ADDS r0, r0, #8
    MOV r11, r0
    LDR r0, [r0]
    MOV r8, r0
    MOV r0, r11
    ADDS r0, r0, #4
    LDR r0, [r0]
    ADDS r0, r0, #1
    MOV r9, r0
    B MainLoop
    .endfunc
;
    NOP
    .end
