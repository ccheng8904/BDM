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
    This is a GCC scatter file template for platform cms32m67
    (Cmsemicon CMS32M67xx, ARM Cortex M0+).
    It will be processed by a C-Preprocessor before interpreted by the linker
 */

#ifdef APP_VARIANT
    #ifdef DMODULE_PRESENT
        #include "dmc/LinkerScriptData.h"
    #endif
#endif

/* Linker script to configure memory regions. */
/*
    CMS32M67xx memory map:

    |----------------------------------- |\
    |       0x2000 2FFF   RAM1_END       | \
    |  SRAM1        8kB                  |  > RAM1
    |       0x2000 1000   RAM1_START     | /
    |----------------------------------- |/
    |       0x2000 0FFF   RAM0_END       | \
    |  SRAM0        4kB                  |  > RAM0
    |       0x2000 0000   RAM0_START     | /
    |----------------------------------- |/
    |       0x0000 01FF   ROM1_START     | \
    |  FLASH        128kB                |  > ROM1
    |       0x0001 FFFF   ROM1_END       | /
    |----------------------------------- |/
    |       0x0000 0000   ROM0           | \
    |  vector table + user option bytes  |  > ROM0
    |       0x0000 00C3                  | /
    |----------------------------------- |/

    RAM0 (SRAM0) is reserved for the RAM flash driver used by the programmer
    (ROM_FLDRV) and stays unused for the standard application. RAM1 (SRAM1)
    is used for the application data. RAM4 (SRAM1) is the RAM image of the
    flash driver, it shares the memory with RAM1 as the linker places it at
    the very end of SRAM1, right below the stack.
*/
MEMORY
{
  ROM0 (rx) :  ORIGIN = ROOT_REGION, LENGTH = 0x10             // 4 vectors
  ROM_ANCHOR (rx) : ORIGIN = ROOT_REGION+0x10, LENGTH = 0x10   // 3 vectors + 4 FWU opt. bytes
#if ROOT_REGION == ROM1_START
  ROM1 (rx) : ORIGIN = ROOT_REGION + 0x20, LENGTH = ROM1_SIZE-0x20
#else
  ROM1 (rx) : ORIGIN = ROM1_START, LENGTH = ROM1_SIZE
#endif
#ifdef ROM2_START
  ROM2 (rx) : ORIGIN = ROM2_START, LENGTH = ROM2_SIZE
#endif

  RAM1 (rwx) : ORIGIN = RAM1_START,  LENGTH = RAM1_SIZE
#ifdef RAM2_START
  RAM2 (rwx) : ORIGIN = RAM2_START, LENGTH = RAM2_SIZE
#endif

  ROM_FLDRV (rwx) : ORIGIN = RAM1_END+1 - STACKSIZE - FLASH_DRIVER_SIZE - RAMCONT_EXCLUDED_DATA_SIZE,  LENGTH = FLASH_DRIVER_SIZE

  /* SRAM0 is used for the RAM resident flash driver of the programmer */
  /* SRAM0 is used for the RAM resident flash driver of the programmer */
  RAM4 (rwx) : ORIGIN = 0x20000000, LENGTH = 0x1000

#ifdef FWU_FLEX_PARTITION_START
  FLEX_PARTITION (rx) : ORIGIN = FWU_FLEX_PARTITION_START, LENGTH = FWU_FLEX_PARTITION_SIZE
#endif

#ifdef FWU_OTP_MEMORY_START_ADDRESS
  OTP_MEMORY (rx) : ORIGIN = FWU_OTP_MEMORY_START_ADDRESS, LENGTH = FWU_OTP_MEMORY_SIZE
#endif

#ifdef FEATURE_FUNCTIONAL_SAFETY_ENABLED
  FS_EXCLUDED_RAM (rwx) : ORIGIN = RAM1_END +1 - RAMCONT_EXCLUDED_DATA_SIZE,  LENGTH = RAMCONT_EXCLUDED_DATA_SIZE
#endif
}

SECTIONS
{
#ifdef need_fixed_vectors
    .boot1 :
    {
        KEEP(*(.vectorsFixed))
    } > ROM0
#endif

#ifdef need_fixed_option_bytes
    /* CMS32M67xx user option bytes (0xC0 - 0xC3) have to be part of every
       image, otherwise the option byte block would be erased/undefined */
    .option_byte :
    {
        KEEP(*(.option_byte))
    } > ROM0
#endif

#ifdef REMOTE_FIRMWARE_UPDATE
    .boot2 :
    {
        KEEP(*(.FWU_DATA_ANCHOR))
        KEEP(*(.FWU_OPT_BYTES))
    } > ROM_ANCHOR
#endif


#ifdef FWU_FLEX_PARTITION_START
    .flexpartition :
    {
        KEEP(*(.FWU_FLEX_PARTITION_HEADER))
        KEEP(*(.FWU_FLEX_PARTITION))
    } > FLEX_PARTITION
#endif

#ifdef FWU_OTP_MEMORY_START_ADDRESS
    .fwu_otp_memory :
    {
        KEEP(*(.FWU_OTP_MEMORY))
    } > OTP_MEMORY
#endif

    .text1 :
    {
#ifdef need_ModuleHeader
        KEEP(*(.FWU_SW_MODULE_HEADER))
        *IntTblArmCM.o(.text*)
        *StdCrc.o(.text*)
        . = VTOR_ALIGN;
#endif
        KEEP(*(.vectorsCore))
        KEEP(*(.vectors))
    } > ROM1


    .text2 :
    {
      #if defined(APP_VARIANT)
        *(EXCLUDE_FILE(*intflash_*.o *flash.o) .text*) *(.text.*)
      #elif defined(COPY_FLASH_DRV_TO_RAM)
        *(EXCLUDE_FILE(*intflash_*.o *flash.o) .text*) *(.text.*)
      #else
        *(.text*)
      #endif

        KEEP(*(.init))
        KEEP(*(.fini))

        /* .ctors */
        *crtbegin.o(.ctors)
        *crtbegin?.o(.ctors)
        *(EXCLUDE_FILE(*crtend?.o *crtend.o) .ctors)
        *(SORT(.ctors.*))
        *(.ctors)

        /* .dtors */
        *crtbegin.o(.dtors)
        *crtbegin?.o(.dtors)
        *(EXCLUDE_FILE(*crtend?.o *crtend.o) .dtors)
        *(SORT(.dtors.*))
        *(.dtors)

      #if defined(APP_VARIANT)
        *(EXCLUDE_FILE(*intflash_*.o *flash.o) .rodata*)
      #elif defined(COPY_FLASH_DRV_TO_RAM)
        *(EXCLUDE_FILE(*intflash_*.o *flash.o) .rodata*)
      #else
        *(.rodata*)
      #endif
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

#if defined(COPY_FLASH_DRV_TO_RAM)
    /* Flash driver executed from SRAM0, copied by the start-up code */
    .fdrv_res (NOLOAD) :
    {
        __fdrv_start__ = .;
        KEEP(*(.FWDRV_RAM))
        . = ALIGN(4);
        __fdrv_end__ = .;
    } > RAM4
#endif

    .no_init (NOLOAD) :
    {
        *(.FWU_SHARED_DATA)
        . = FWU_SHARED_DATA_SIZE;
#if defined(VARIANT_BOOTMANAGER) || defined(APP_VARIANT)
        __uninit_data_start = .;
        KEEP(*(.UNINIT_DATA))
        . = __uninit_data_start + UNINIT_DATA_SIZE;
#endif
        // Include all timer segments
        #include "timer/timer_seg.h"
    } > RAM1


    .data :
    {
        __data_start__ = .;
        *(.data*)

        . = ALIGN(4);
        /* preinit data */
        PROVIDE_HIDDEN (__preinit_array_start = .);
        KEEP(*(.preinit_array))
        PROVIDE_HIDDEN (__preinit_array_end = .);

        . = ALIGN(4);
        /* init data */
        PROVIDE_HIDDEN (__init_array_start = .);
        KEEP(*(SORT(.init_array.*)))
        KEEP(*(.init_array))
        PROVIDE_HIDDEN (__init_array_end = .);


        . = ALIGN(4);
        /* finit data */
        PROVIDE_HIDDEN (__fini_array_start = .);
        KEEP(*(SORT(.fini_array.*)))
        KEEP(*(.fini_array))
        PROVIDE_HIDDEN (__fini_array_end = .);

        . = ALIGN(4);
    #if defined(APP_VARIANT) || defined(COPY_FLASH_DRV_TO_RAM)
        // Flash routines to be loaded into RAM
        *(.text.DRVIF_*)
        *(.rodata.DRVIF_*)
        *(.text.ABSIF_*)
      #if defined(COPY_FLASH_DRV_TO_RAM)
        *(.text.MEMDRV_*)
        *(.text.MWDT_*)
        /* the vendor flash routines are executed from the RAM together with the
         * DRVIF driver routines of the other platforms */
        *(.text.EraseChip)
        *(.text.EraseSector)
        *(.text.ProgramPage)
        *(.text.flash_read)
        *(.text.flashRead)
        *(.text.flash_write)
      #endif
        . = ALIGN(4);
    #endif
        /* All data end */
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
        . += HEAPSIZE;
        __HeapLimit = .;
    } > RAM1

    /* Set stack limit */
    #ifndef FEATURE_FUNCTIONAL_SAFETY_ENABLED
    __StackLimit = ORIGIN(RAM1) + LENGTH(RAM1) - STACK_MAGIC_SIZE - STACKSIZE;
    #else
    __StackLimit = ORIGIN(RAM1) + LENGTH(RAM1) - STACK_MAGIC_SIZE - RAMCONT_EXCLUDED_DATA_SIZE - STACKSIZE;
   #endif

    /* .stack section doesn't contain any symbols. It is only
     * used to reserve memory for stack, and assign values to
     * stack symbols later. It sets stack top to end of RAM1 */
    .stack __StackLimit :
    {
        . += STACKSIZE  ;
        __StackTop = .;
    } > RAM1


    PROVIDE(__stack = __StackTop);

    /* Excluded region -  excluded data from RAM Continuous (RunTime) check */
#ifdef FEATURE_FUNCTIONAL_SAFETY_ENABLED
 .fs_excluded_ram (NOLOAD): //No load region
    {
        KEEP(*(.RAMCONT_EXCLUDED_DATA))
    } > FS_EXCLUDED_RAM
#endif

    /* Check if data + heap + stack exceeds RAM1 limit */
    ASSERT(__StackLimit >= __HeapLimit, "region RAM1 overflowed with stack")
    /* The RAM resident flash driver must fit into SRAM0 */
    /* ROM_FLDRV and RAM4 must not overlap, both start at the bottom of SRAM0 */
#if defined(COPY_FLASH_DRV_TO_RAM)
    ASSERT(SIZEOF(.fdrv_res) <= LENGTH(RAM4), "region SRAM0 overflowed by flash driver")
#endif
}
