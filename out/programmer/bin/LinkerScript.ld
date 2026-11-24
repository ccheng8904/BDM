MEMORY
{
  ROM0 (rx) : ORIGIN = 0x0, LENGTH = 0x10
  ROM_ANCHOR (rx) : ORIGIN = 0x0 +0x10, LENGTH = 0x10
  ROM1 (rx) : ORIGIN = 0x1a00, LENGTH = 0x4600
  RAM1 (rwx) : ORIGIN = 0x20000000, LENGTH = 0x4000
  ROM_FLDRV (rwx) : ORIGIN = 0x20003FFF +1 - 0x340 - 0x800 - 0x0, LENGTH = 0x800
  OTP_MEMORY (rx) : ORIGIN = 0x6000, LENGTH = 0x400
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
        KEEP(*(.FWU_SW_MODULE_HEADER))
        *IntTblArmCM.o(.text*)
        *StdCrc.o(.text*)
        . = 0x200;
        KEEP(*(.vectorsCore))
        KEEP(*(.vectors))
    } > ROM1
    .text2 :
    {
        *(EXCLUDE_FILE(*intflash_*.o *mem_drv.o *mwdt_mc.o *mwdt.o) .text*)
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
        *(EXCLUDE_FILE(*intflash_*.o *mem_drv.o *mwdt_mc.o *mwdt.o) .rodata*)
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
*(.START8_1)
*(.TIMER8_10MS)
*(.START8_2)
*(.TIMER8_2S)
*(.END8)
*(.START16_1)
*(.TIMER16_10MS)
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
        *(.text.DRVIF_*)
        *(.rodata.DRVIF_*)
        *(.text.ABSIF_*)
        *(.text.MEMDRV_*)
        *(.text.MWDT_*)
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
    __StackLimit = ORIGIN(RAM1) + LENGTH(RAM1) - 0x4 - 0x340;
    .stack __StackLimit :
    {
        . += 0x340 ;
        __StackTop = .;
    } > RAM1
    PROVIDE(__stack = __StackTop);
    ASSERT(__StackLimit >= __HeapLimit, "region RAM1 overflowed with stack")
}
