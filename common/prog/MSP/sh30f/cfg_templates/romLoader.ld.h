/*******************************************************************************
 *  Copyright (c) 2023 BSH Hausgeraete GmbH,
 *  Carl-Wery-Str. 34, 81739 Munich, Germany, www.bsh-group.de
 *
 *  All rights reserved. This program and the accompanying materials
 *  are protected by international copyright laws.
 *  Please contact copyright holder for licensing information.
 *
 *******************************************************************************
 *  PROJECT           Generic SW
 *  PLATFORM          sh30f
 ******************************************************************************/

/******************************************************************************/
/* DOCUMENTATION                                                              */
/******************************************************************************/
/*
    This is a GCC scatter file template for platform sh30f.
    It will be processed by a C-Preprocessor before interpreted by the linker
 */



/* Linker script to configure memory regions. */
MEMORY
{ 
  ROM0 (rx) :  ORIGIN = ROOT_REGION, LENGTH = 0x10            // 4 vectors
#ifdef VARIANT_ROMLOADER
  ROM1 (rx) : ORIGIN = ROOT_REGION + 0x10, LENGTH = ROM1_SIZE-0x10
#else
  ROM1 (rx) : ORIGIN = ROM1_START, LENGTH = ROM1_SIZE
#endif
  RAM1 (rwx) : ORIGIN = RAM1_START,  LENGTH = RAM1_SIZE


}
SECTIONS 
{
     
    .boot1 :
    {
        KEEP(*(.vectorsFixed))
    } > ROM0

    .text :
    {
#ifdef VARIANT_LROMLOADER
        KEEP(*(.FWU_SW_MODULE_HEADER))
//        . = VTOR_ALIGN; not done because romloader don't use interrupts (not beautiful because of hard fault and NMI which cannot be disabled)
        KEEP(*(.vectorsCore))
        KEEP(*(.vectors))
#endif
        *(.text*)
        *(.rodata*)
        *(.init)
        *(.fini)
    } > ROM1

    __etext = .;


    .data (NOLOAD) :
    {
        __data_start__ = .;
        *(.FWU_SHARED_DATA)
        *(.data*)
        __data_end__ = .;
    } > RAM1

    .bss :
    { 
        __bss_start__ = .; 
        *(.bss*)
        *(COMMON)
        __bss_end__ = .;         
    } > RAM1
 
    .heap :
    {
        __end__ = .;
        end = __end__;
        *(.heap*)
        __HeapLimit = .;
    } > RAM1

    /* .stack_dummy section doesn't contains any symbols. It is only
     * used for linker to calculate size of stack sections, and assign
     * values to stack symbols later */
    .stack_dummy :
    {
        *(.stack)
    } > RAM1

    /* Set stack top to end of RAM1, and stack limit move down by
     * size of stack_dummy section */
    __StackTop = ORIGIN(RAM1) + LENGTH(RAM1);
    __StackLimit = __StackTop - SIZEOF(.stack_dummy);
    PROVIDE(__stack = __StackTop);
    
    /* Check if data + heap + stack exceeds RAM1 limit */
    ASSERT(__StackLimit >= __HeapLimit, "region RAM1 overflowed with stack") 
} 

