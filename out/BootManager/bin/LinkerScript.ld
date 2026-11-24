MEMORY
{
  ROM0 (rx) : ORIGIN = 0x0, LENGTH = 0x10
  ROM_ANCHOR (rx) : ORIGIN = 0x0 +0x10, LENGTH = 0x10
 ROM1 (rx) : ORIGIN = 0x0 + 0x20, LENGTH = 0xF00 -0x20
  RAM1 (rwx) : ORIGIN = 0x20000000, LENGTH = 0x800
  ROM_FLDRV (rwx) : ORIGIN = 0x200007FF +1 - 0x400 - 0x26C - 0x40, LENGTH = 0x26C
  OTP_MEMORY (rx) : ORIGIN = 0x6000, LENGTH = 0x400
  FS_EXCLUDED_RAM (rwx) : ORIGIN = 0x200007FF +1 - 0x40, LENGTH = 0x40
}
SECTIONS
{
    .boot1 :
    {
        KEEP(*(.vectorsFixed))
    } > ROM0
    .boot2 :
    {
        KEEP(*(.FWU_DATA_ANCHOR))
        KEEP(*(.FWU_OPT_BYTES))
    } > ROM_ANCHOR
    .fwu_otp_memory :
    {
        KEEP(*(.FWU_OTP_MEMORY))
    } > OTP_MEMORY
    .text1 :
    {
        KEEP(*(.vectorsCore))
        KEEP(*(.vectors))
    } > ROM1
    .text2 :
    {
        *(.text*)
        KEEP(*(.init))
        KEEP(*(.fini))
        *crtbegin.o(.ctors)
        *crtbegin?.o(.ctors)
        *(EXCLUDE_FILE(*crtend?.o *crtend.o) .ctors)
        *(SORT(.ctors.*))
        *(.ctors)
        *crtbegin.o(.dtors)
        *crtbegin?.o(.dtors)
        *(EXCLUDE_FILE(*crtend?.o *crtend.o) .dtors)
        *(SORT(.dtors.*))
        *(.dtors)
        *(.rodata*)
        KEEP(*(.eh_frame*))
    } > ROM1
    .ARM.extab :
    {
        *(.ARM.extab* .gnu.linkonce.armextab.*)
    } > ROM1
    __exidx_start = .;
    .ARM.exidx :
    {
        *(.ARM.exidx* .gnu.linkonce.armexidx.*)
    } > ROM1
    __exidx_end = .;
    __etext = .;
    .no_init (NOLOAD) :
    {
        *(.FWU_SHARED_DATA)
        . = 0x40;
        __uninit_data_start = .;
        KEEP(*(.UNINIT_DATA))
        . = __uninit_data_start + 0x40;
*(.START8_1)
*(.TIMER8_10MS)
*(.START8_2)
*(.TIMER8_20MS)
*(.START8_3)
*(.TIMER8_100MS)
*(.START8_4)
*(.TIMER8_200MS)
*(.START8_5)
*(.TIMER8_1S)
*(.START8_6)
*(.TIMER8_2S)
*(.START8_7)
*(.TIMER8_10S)
*(.START8_8)
*(.TIMER8_20S)
*(.START8_9)
*(.TIMER8_1M)
*(.START8_10)
*(.TIMER8_2M)
*(.END8)
*(.START16_1)
*(.TIMER16_10MS)
*(.START16_2)
*(.TIMER16_20MS)
*(.START16_3)
*(.TIMER16_100MS)
*(.START16_4)
*(.TIMER16_200MS)
*(.START16_5)
*(.TIMER16_1S)
*(.START16_6)
*(.TIMER16_2S)
*(.START16_7)
*(.TIMER16_10S)
*(.START16_8)
*(.TIMER16_20S)
*(.START16_9)
*(.TIMER16_1M)
*(.START16_10)
*(.TIMER16_2M)
*(.START16_11)
*(.TIMER16_10M)
*(.START16_12)
*(.TIMER16_20M)
*(.END16)
    } > RAM1
    .data :
    {
        __data_start__ = .;
        *(.data*)
        . = ALIGN(4);
        PROVIDE_HIDDEN (__preinit_array_start = .);
        KEEP(*(.preinit_array))
        PROVIDE_HIDDEN (__preinit_array_end = .);
        . = ALIGN(4);
        PROVIDE_HIDDEN (__init_array_start = .);
        KEEP(*(SORT(.init_array.*)))
        KEEP(*(.init_array))
        PROVIDE_HIDDEN (__init_array_end = .);
        . = ALIGN(4);
        PROVIDE_HIDDEN (__fini_array_start = .);
        KEEP(*(SORT(.fini_array.*)))
        KEEP(*(.fini_array))
        PROVIDE_HIDDEN (__fini_array_end = .);
        . = ALIGN(4);
        __data_end__ = .;
    } > RAM1 AT>ROM1
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
        . += 0x0;
        __HeapLimit = .;
    } > RAM1
    __StackLimit = ORIGIN(RAM1) + LENGTH(RAM1) - 0x4 - 0x40 - 0x400;
    .stack __StackLimit :
    {
        . += 0x400 ;
        __StackTop = .;
    } > RAM1
    PROVIDE(__stack = __StackTop);
 .fs_excluded_ram (NOLOAD):
    {
        KEEP(*(.RAMCONT_EXCLUDED_DATA))
    } > FS_EXCLUDED_RAM
    ASSERT(__StackLimit >= __HeapLimit, "region RAM1 overflowed with stack")
}
