MEMORY
{
  ROM0 (rx) : ORIGIN = 0x0, LENGTH = 0x10
  ROM1 (rx) : ORIGIN = 0xf00, LENGTH = 0xB00
  RAM1 (rwx) : ORIGIN = 0x20000000, LENGTH = 0x800
}
SECTIONS
{
    .boot1 :
    {
        KEEP(*(.vectorsFixed))
    } > ROM0
    .text :
    {
        KEEP(*(.FWU_SW_MODULE_HEADER))
        KEEP(*(.vectorsCore))
        KEEP(*(.vectors))
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
    .stack_dummy :
    {
        *(.stack)
    } > RAM1
    __StackTop = ORIGIN(RAM1) + LENGTH(RAM1);
    __StackLimit = __StackTop - SIZEOF(.stack_dummy);
    PROVIDE(__stack = __StackTop);
    ASSERT(__StackLimit >= __HeapLimit, "region RAM1 overflowed with stack")
}
