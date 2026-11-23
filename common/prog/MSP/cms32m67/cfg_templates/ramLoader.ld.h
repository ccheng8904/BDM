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
 *  PLATFORM          cms32m67
 ******************************************************************************/

/******************************************************************************/
/* DOCUMENTATION                                                              */
/******************************************************************************/
/*
    This is a GCC scatter file template for platform cms32m67 ramLoader.
    It will be processed by a C-Preprocessor before interpreted by the linker
 */



/* Linker script to configure memory regions. */
MEMORY
{ 
  ROM0 (rx) :  ORIGIN = ROOT_REGION, LENGTH = 0x10            // 4 vectors
  ROM1 (rx) :  ORIGIN = ROM1_START,  LENGTH = ROM1_SIZE
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
        KEEP(*(.vectorsCore))
        *(.text*)
        *(.rodata*)
        *(.init)
        *(.fini)
    } > ROM1

    __etext = .;


    .data (NOLOAD) :
    {
        __data_start__ = .;
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

