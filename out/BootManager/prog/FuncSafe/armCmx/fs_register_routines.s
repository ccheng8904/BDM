    .syntax unified
    .text
    .thumb
    .align 2
;
.set con80, 0x80000000
.set conAA, 0xAAAAAAAA
.set con55, 0x55555555
.set conA8, 0xAAAAAAA8
.set con54, 0x55555554
;
    .extern FSF_vReg_FatalException
;
    .global FSF_do_xPSRandR14LinkRegisterCheck
    .global FSF_doGeneralPurposeR0_R12RegisterCheck
    .global FSF_doStackPointerR13RegisterCheck
;
.func FSF_do_xPSRandR14LinkRegisterCheck, FSF_do_xPSRandR14LinkRegisterCheck
.type FSF_do_xPSRandR14LinkRegisterCheck, "function"
.thumb_func
FSF_do_xPSRandR14LinkRegisterCheck:
    movs r0, #0x00
    uxtb r0,r0
    adds r0,#0
    bne CPUTestFail
    bmi CPUTestFail
    subs r0,#1
    bpl CPUTestFail
    adds r0,#2
    bcc CPUTestFail
    beq CPUTestFail
    bmi CPUTestFail
    ldr r0,=con80
    adds r0, r0, r0
    bvc CPUTestFail
    mov r1, r14
    ldr r0, =conAA
    mov r14,r0
    cmp r0,r14
    bne CPUTestFail
    ldr r0, =con55
    mov r14,r0
    cmp r0,r14
    bne CPUTestFail
    mov r14, r1
    bal CPUTstCont
CPUTestFail:
    blal FSF_vReg_FatalException
CPUTstCont:
    bx lr
    .endfunc
;
.func FSF_doGeneralPurposeR0_R12RegisterCheck, FSF_doGeneralPurposeR0_R12RegisterCheck
.type FSF_doGeneralPurposeR0_R12RegisterCheck, "function"
.thumb_func
FSF_doGeneralPurposeR0_R12RegisterCheck:
    push {r4-r7}
    ldr r0,=conAA
    ldr r1,=conAA
    cmp r0,r1
    bne CPUTestFailGeneralReg
    ldr r0, =con55
    ldr r1, =con55
    cmp r0,r1
    bne CPUTestFailGeneralReg
    movs r1, #0x1
    ldr r0, =conAA
    ldr r2, =conAA
    cmp r0,r2
    bne CPUTestFailGeneralReg
    ldr r0, =con55
    ldr r2, =con55
    cmp r0,r2
    bne CPUTestFailGeneralReg
    movs r2, #0x2
    ldr r0, =conAA
    ldr r3, =conAA
    cmp r0,r3
    bne CPUTestFailGeneralReg
    ldr r0, =con55
    ldr r3, =con55
    cmp r0,r3
    bne CPUTestFailGeneralReg
    movs r3, #0x3
    ldr r0, =conAA
    ldr r4, =conAA
    cmp r0,r4
    bne CPUTestFailGeneralReg
    ldr r0, =con55
    ldr r4, =con55
    cmp r0,r4
    bne CPUTestFailGeneralReg
    movs r4, #0x4
    ldr r0, =conAA
    ldr r5, =conAA
    cmp r0,r5
    bne CPUTestFailGeneralReg
    ldr r0, =con55
    ldr r5, =con55
    cmp r0,r5
    bne CPUTestFailGeneralReg
    movs r5, #0x5
    ldr r0, =conAA
    ldr r6,=conAA
    cmp r0,r6
    bne CPUTestFailGeneralReg
    ldr r0, =con55
    ldr r6, =con55
    cmp r0,r6
    bne CPUTestFailGeneralReg
    movs r6, #0x6
    ldr r0, =conAA
    ldr r7, =conAA
    cmp r0,r7
    bne CPUTestFailGeneralReg
    ldr r0, =con55
    ldr r7, =con55
    cmp r0,r7
    bne CPUTestFailGeneralReg
    movs r7, #0x7
    ldr r0, =conAA
    mov r8,r0
    cmp r0,r8
    bne CPUTestFailGeneralReg
    ldr r0, =con55
    mov r8,r0
    cmp r0,r8
    bne CPUTestFailGeneralReg
    movs r0, #0x08
    mov r8,r0
    bal CPUTstContGeneralReg
CPUTestFailGeneralReg:
    blal FSF_vReg_FatalException
CPUTstContGeneralReg:
    ldr r0, =conAA
    mov r9,r0
    cmp r0,r9
    bne CPUTestFailGeneralReg
    ldr r0, =con55
    mov r9,r0
    cmp r0,r9
    bne CPUTestFailGeneralReg
    movs r0, #0x09
    mov r9,r0
    ldr r0, =conAA
    mov r10,r0
    cmp r0,r10
    bne CPUTestFailGeneralReg
    ldr r0, =con55
    mov r10,r0
    cmp r0,r10
    bne CPUTestFailGeneralReg
    movs r0, #0x0A
    mov r10,r0
    ldr r0, =conAA
    mov r11,r0
    cmp r0,r11
    bne CPUTestFailGeneralReg
    ldr r0, =con55
    mov r11,r0
    cmp r0,r11
    bne CPUTestFailGeneralReg
    movs r0, #0x0B
    mov r11,r0
    ldr r0, =conAA
    mov r12,r0
    cmp r0,r12
    bne CPUTestFailGeneralReg
    ldr r0, =con55
    mov r12,r0
    cmp r0,r12
    bne CPUTestFailGeneralReg
    movs r0, #0x0C
    mov r12,r0
    movs r0, #0x0
    cmp r0, #0x00
    bne CPUTestFailGeneralReg
    cmp r1, #0x01
    bne CPUTestFailGeneralReg
    cmp r2, #0x02
    bne CPUTestFailGeneralReg
    cmp r3, #0x03
    bne CPUTestFailGeneralReg
    cmp r4, #0x04
    bne CPUTestFailGeneralReg
    cmp r5, #0x05
    bne CPUTestFailGeneralReg
    cmp r6, #0x06
    bne CPUTestFailGeneralReg
    cmp r7, #0x07
    bne CPUTestFailGeneralReg
    movs r0, #0x08
    cmp r0,r8
    bne CPUTestFailGeneralReg
    movs r0, #0x09
    cmp r0,r9
    bne CPUTestFailGeneralReg
    movs r0, #0x0A
    cmp r0,r10
    bne CPUTestFailGeneralReg
    movs r0, #0x0B
    cmp r0,r11
    bne CPUTestFailGeneralReg
    movs r0, #0x0C
    cmp r0,r12
    bne CPUTestFailGeneralReg
    pop {r4-r7}
    bx lr
    .endfunc
;
.func FSF_doStackPointerR13RegisterCheck, FSF_doStackPointerR13RegisterCheck
.type FSF_doStackPointerR13RegisterCheck, "function"
.thumb_func
FSF_doStackPointerR13RegisterCheck:
    mrs r0,psp
    ldr r1, =conA8
    msr psp,r1
    mrs r2,psp
    cmp r2,r1
    bne CPUTestFailStackReg
    ldr r1, =con54
    msr psp,r1
    mrs r2,psp
    cmp r2,r1
    bne CPUTestFailStackReg
    msr psp, r0
    mrs r0,msp
    ldr r1, =conA8
    msr msp,r1
    mrs r2,msp
    cmp r2,r1
    bne CPUTestFailStackReg
    ldr r1, =con54
    msr msp,r1
    mrs r2,msp
    cmp r2,r1
    bne CPUTestFailStackReg
    msr msp,r0
    BAL CPUTestStackRegOK
CPUTestFailStackReg:
    BLAL FSF_vReg_FatalException
CPUTestStackRegOK:
    bx lr
    .endfunc
    nop
    .end
