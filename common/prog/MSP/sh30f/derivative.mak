#*******************************************************************************
#  Copyright (c) 2023 BSH Hausgeraete GmbH,
#  Carl-Wery-Str. 34, 81739 Munich, Germany, www.bsh-group.de
#
#  All rights reserved. This program and the accompanying materials
#  are protected by international copyright laws.
#  Please contact copyright holder for licensing information.
#
#*******************************************************************************
#  PROJECT          MCU Framework / SH30F9X71
#*******************************************************************************
#  Description      build include for SH30F9X71 derivative specific definitions
#*******************************************************************************


#Parsing of processor type only for SH30F9X71 e.g. SH30F9871P044PR,SH30F9071P064PR

#   Example:                   SH30 F  9  8  71  P   064PR
#                                |  |  |  |  |   |     |
#   Product Sequence   __________|  |  |  |  |   |     |
#   SH30 = ARM Cortex M0 32bit      |  |  |  |   |     |
#                                   |  |  |  |   |     |
#   Product type        ____________|  |  |  |   |     |
#   F = Flash type,no special power    |  |  |   |     |
#   L = Low power type                 |  |  |   |     |
#                                      |  |  |   |     |
#   Product subsequence________________|  |  |   |     |
#   2 = high-performance/ motor           |  |   |     |
#   7 = low power consumption/ meter      |  |   |     |
#   9 = multifunctional/ home appliances  |  |   |     |
#                                         |  |   |     |
#   ROM        ___________________________|  |   |     |
#   1 = 8KB                                  |   |     |
#   2 = 16KB                                 |   |     |
#   3 = 24KB                                 |   |     |
#   4 = 32KB                                 |   |     |
#   5 = 48KB                                 |   |     |
#   6 = 64KB                                 |   |     |
#   7 = 96KB                                 |   |     |
#   8 = 128KB                                |   |     |
#   9 = 192KB                                |   |     |
#   0 = 256KB                                |   |     |
#   A = 384KB                                |   |     |
#   B = 512KB                                |   |     |
#   C = 768KB                                |   |     |
#   D = 1024KB                               |   |     |
#   E = 1536KB                               |   |     |
#   F = 2048KB                               |   |     |
#                                            |   |     |
#    Serial number __________________________|   |     |
#    ID product serial number                    |     |
#                                                |     |
#    Package type     ___________________________|     |
#    S = LQFP(7X7)                                     |
#    P = LQFP(7X7)                                     |
#    B = LQFP(12X12)                                   |
#    U = TQFP                                          |
#                                                      |
#    Production code     ______________________________|
#    064: pins
#    P  : Package type, same as above
#    R  : Packaging type
#           R: Tray
#           U: Tube
#           Y: Tape & Reel



#**************************************************************************
#********* SH30F9X71 Memory map (start)**************************************
#                                                                        **
#    |----------------------------------- |\                             **
#    |           0x2000 3FFF   RAM1_END   | \                            **
#    | RAM1          SRAM                 |  > RAM1 (SRAM)               **
#    |           0x2000 0000   RAM1_START | /                            **
#    |----------------------------------- |/                             **
#                                                                        **
#    |----------------------------------- |\                             **
#    |           0x0FFF E3FF   ROM3_END   | \                            **
#    |               OTP    BYTES         |  > OTP (FLASH)               **
#    |           0x0FFF E000   ROM3_START | /                            **
#    |----------------------------------- |/                             **
#                                                                        **
#    |----------------------------------- |\                             **
#    |           0x0FFF A3FF   ROM3_END   | \                            **
#    |               Customer Block       |  > Customer Block  (FLASH)   **
#    |           0x0FFF A000   ROM3_START | /                            **
#    |----------------------------------- |/                             **
#                                                                        **
#    |----------------------------------- |\                             **
#    |           0x0FFF 83FF   ROM3_END   | \                            **
#    |               Protect Block        |  > Protect Block (FLASH)     **
#    |           0x0FFF 8000   ROM3_START | /                            **
#    |----------------------------------- |/                             **
#                                                                        **
#    |----------------------------------- |\                             **
#    |           0x0FFE 1FFF   ROM3_END   | \                            **
#    |               Boot Rom             |  > Boot Rom (FLASH)          **
#    |           0x0FFE 0000   ROM3_START | /                            **
#    |----------------------------------- |/                             **
#                                                                        **
#    |----------------------------------- |\                             **
#    |           0x0FFC 0FFF   ROM3_END   | \                            **
#    |               EEPROM Block         |  > EEPROM Block (FLASH)      **
#    |           0x0FFC 0000   ROM3_START | /                            **
#    |----------------------------------- |/                             **
#                                                                        **
#    |----------------------------------- |\                             **
#    |           0x000x xxxx   ROM1_END   | \                            **
#    |               FLASH                |  > ROM1 (FLASH)              **
#    |           0x0000 0000   ROM1_START | /                            **
#    |----------------------------------- |/                             **
#                                                                        **
#********* SH30F9X71 Memory map (end)  **************************************
#**************************************************************************

# set datasheet and ref. manual file names
af_doc_ds_name ?= DS_SH30F9X71.pdf
af_doc_rm_name ?= RM_SH30F9X71.pdf
af_doc_er_name ?= ER_SH30F9X71.pdf

uc_platform ?= SH30F
uc_platform_teng ?= CoreMX
uc_vendor = SH
defines += UC_VENDOR=$(uc_vendor)

sh30f_subsequence   :=  $(call match,$(derivative),SH30F(\d))
sh30f_flash_type    :=  $(call match,$(derivative),SH30F\d(\d))
sh30f_serial_number :=  $(call match,$(derivative),SH30F\d\d(\d+)P)
sh30f_pinout        :=  $(call match,$(derivative),SH30F\d+P(\d+)PR)
sh30f_type          :=  SH30F$(sh30f_subsequence)$(sh30f_flash_type)$(sh30f_serial_number)
sh30               :=  1


platform_main_type    :=  $(uc_platform)
platform_type_segger  :=  $(sh30f_type)
test_derivative:
	@echo derivative            = $(derivative)
	@echo sh30f_subsequence     = $(sh30f_subsequence)
	@echo sh30f_flash_type      = $(sh30f_flash_type)
	@echo sh30f_serial_number   = $(sh30f_serial_number)
	@echo sh30f_pinout          = $(sh30f_pinout)
	@echo sh30f_type            = $(sh30f_type)
	@echo platform_main_type    = $(platform_main_type)
	@echo platform_type_segger  = $(platform_type_segger)
	@echo ROM1_START            = $(ROM1_START)
	@echo ROM1_END              = $(ROM1_END)
	@echo ROM1_SIZE             = $(ROM1_SIZE)
	@echo ROM3_START            = $(ROM3_START)
	@echo ROM3_END              = $(ROM3_END)
	@echo ROM3_SIZE             = $(ROM3_SIZE)
	@echo RAM1_START            = $(RAM1_START)
	@echo RAM1_END              = $(RAM1_END)
	@echo RAM1_SIZE             = $(RAM1_SIZE)



defines += PROC_FAMILY=SH30F9X
defines += SH30_ARM
defines += SH30F9X71
defines += SH30F$(sh30f_subsequence)$(sh30f_flash_type)$(sh30f_serial_number)


# it is used HUARTx_bIsRxOnGoing instead of IRQ for idle
dbm_use_irq_for_idle_detection = true

cc_core = cm0p

# alignment of vector table offset, aligned to fit Module Header and Flex Table
VTOR_ALIGN = 0x200

# Page size is 1024bytes for SH30F9X71 series
PAGE_SIZE = 400
ifeq ($(sh30f_flash_type),1)
   FLASH_SIZE     ?= 8
   ROM1_PHY_SIZE  ?= 2000
   ROM1_PHY_END   ?= 1FFF
else ifeq ($(sh30f_flash_type),2)
   FLASH_SIZE     ?= 16
   ROM1_PHY_SIZE  ?= 4000
   ROM1_PHY_END   ?= 3FFF
else ifeq ($(sh30f_flash_type),3)
   FLASH_SIZE     ?= 24
   ROM1_PHY_SIZE  ?= 6000
   ROM1_PHY_END   ?= 5FFF
else ifeq ($(sh30f_flash_type),4)
   FLASH_SIZE     ?= 32
   ROM1_PHY_SIZE  ?= 8000
   ROM1_PHY_END   ?= 7FFF
else ifeq ($(sh30f_flash_type),5)
   FLASH_SIZE     ?= 48
   ROM1_PHY_SIZE  ?= C000
   ROM1_PHY_END   ?= BFFF
else ifeq ($(sh30f_flash_type),6)
   FLASH_SIZE     ?= 64
   ROM1_PHY_SIZE  ?= 10000
   ROM1_PHY_END   ?= FFFF
else ifeq ($(sh30f_flash_type),7)
   FLASH_SIZE     ?= 96
   ROM1_PHY_SIZE  ?= 18000
   ROM1_PHY_END   ?= 17FFF
else ifeq ($(sh30f_flash_type),8)
   FLASH_SIZE     ?= 128
   ROM1_PHY_SIZE  ?= 20000
   ROM1_PHY_END   ?= 1FFFF
else ifeq ($(sh30f_flash_type),9)
   FLASH_SIZE     ?= 192
   ROM1_PHY_SIZE  ?= 30000
   ROM1_PHY_END   ?= 2FFFF
else ifeq ($(sh30f_flash_type),0)
   FLASH_SIZE     ?= 256
   ROM1_PHY_SIZE  ?= 40000
   ROM1_PHY_END   ?= 3FFFF
else ifeq ($(sh30f_flash_type),A)
   FLASH_SIZE     ?= 384
   ROM1_PHY_SIZE  ?= 60000
   ROM1_PHY_END   ?= 5FFFF
else ifeq ($(sh30f_flash_type),B)
   FLASH_SIZE     ?= 512
   ROM1_PHY_SIZE  ?= 80000
   ROM1_PHY_END   ?= 7FFFF
else ifeq ($(sh30f_flash_type),C)
   FLASH_SIZE     ?= 768
   ROM1_PHY_SIZE  ?= C0000
   ROM1_PHY_END   ?= BFFFF
else ifeq ($(sh30f_flash_type),D)
   FLASH_SIZE     ?= 1024
   ROM1_PHY_SIZE  ?= 100000
   ROM1_PHY_END   ?=  FFFFF
else ifeq ($(sh30f_flash_type),E)
   FLASH_SIZE     ?= 1536
   ROM1_PHY_SIZE  ?= 180000
   ROM1_PHY_END   ?= 17FFFF
else ifeq ($(sh30f_flash_type),F)
   FLASH_SIZE     ?= 2048
   ROM1_PHY_SIZE  ?= 200000
   ROM1_PHY_END   ?= 1FFFFF
else
    $(error Unknown derivative $(derivative))
endif

RAM1_PHY_SIZE  ?= 4000
RAM1_PHY_END   ?= 20003FFF
# OTP block area
ROM3_PHY_START    ?= FFFE000
ROM3_PHY_END      ?= FFFE3FF
ROM3_PHY_SIZE     ?= 400
ROM3_START        ?= $(ROM3_PHY_START)
ROM3_END          ?= $(ROM3_PHY_END)
ROM3_SIZE         ?= $(ROM3_PHY_SIZE)


STACKSIZE         ?= 400
HEAPSIZE          ?= 0

# size of bytes of flash driver located in RAM
FLASH_DRIVER_SIZE ?= 26C

#**** Boot-Loader Definitions ***
BL_VECTOR_START   ?= 0
ROOT_REGION       ?= 0

# smallest writable block
write_block_size   = 4

# Common Settings for SH30
ROM_FILL          ?= 00
defines           += COMA_ERASE_BYTE=0x00u

ROM1_PHY_START    ?= 0
RAM1_PHY_START    ?= 20000000
RESET_VECTOR      ?= 4
VECTOR_SIZE        = B4

VECTOR_TABLE_SIZE  = C0

winflash_cfg_template ?= $(msp_path)/bootloader/SH30F9X71.bp2.h

jlink_speed = 1000
#--------------- Logical memory mapping ----------------------------------------
bootloader_merge  ?= true
# -- test server / testClient: for internal use only
ifdef testServer
    FW_update_type = FWU1
    ROM1_END      ?= $(ROM1_PHY_END)
    RAM1_END      ?= 20000FFF
    # RAM behind is used for ROM and RAM1 of testClient
endif

ifdef testClient
    FW_update_type = FWU1
    ROM1_START    ?= 20001000
    ROM1_END      ?= 20002FFF
    RAM1_START    ?= 20003000
    RAM1_END      ?= $(RAM1_PHY_END)
    #Stack not needed for testClient, Stack is provided by test-server
    STACKSIZE      = 10
endif

FWU_OTP_MEMORY_SIZE ?= $(PAGE_SIZE)

ifeq ($(FW_update_type),FWU1)
    # old style update (no BootManager, no MiniLoader)
    defines+=RESET_VECTOR=0x$(RESET_VECTOR)
    ifeq ($(dbus_mapping),dbuscan)
        FWU_ROMLOADER_SIZE ?= 1C00
    else
        FWU_ROMLOADER_SIZE ?= C00
    endif

    ifdef romLoader
    #---- ROM-Loader settings
        ROM1_END ?= $(FWU_ROMLOADER_END_ADDRESS)
        RAM1_END ?= 200007FF
        ifeq ($(cc_build_path),gccArm)
            # disable selfident to fit into memory using GCC
            defines += SELF_IDENT_DISABLED
        endif
    else
        ifeq ($(bootloader_merge),true)
            ROM1_START ?= $(BL_APPLICATION_ROM_START)
            patchbin_opt_uc += -i(0x$(BL_VECTOR_START)+8,0x$(FWU_ROMLOADER_END_ADDRESS))$(BOOTLOADER)
            # Currently ROM is filled so this is the only solution
            patchbin_opt_uc += -I(0x$(BL_VECTOR_START):B8)$(BOOTLOADER)
            patchbin_dep    += $(BOOTLOADER)
        else
            ROM1_START ?= $(ROM1_PHY_START)
        endif
        ROM1_END       ?= $(ROM1_PHY_END)
    endif
else ifeq ($(FW_update_type),FWU3)
    # overloaded value from fwu_mem_org.mak
    FWU_BOOTMANAGER_SIZE    ?= F00
    ifeq ($(dbus_mapping),dbuscan)
        FWU_LRO_SIZE        ?= 1900
        FWU_PROGRAMMER_SIZE ?= 4C00
    else
        FWU_LRO_SIZE        ?= B00
        FWU_PROGRAMMER_SIZE ?= 4600
    endif
    # end
    FWU_APP1_END_ADDRESS ?= $(ROM1_PHY_END)

    ifdef bootManager
        BIN_OUT_ADRESSES ?= 0x$(ROM1_START),0x$(ROM1_END)
        RAM1_END         ?= 200007FF
        ROM1_START       ?= $(ROM1_PHY_START)
        ROM1_END         ?= $(FWU_BOOTMANAGER_END_ADDRESS)

    else ifdef lromLoader
        # -- ROM-Loader --
        ROM1_START       ?= $(FWU_LRO_START_ADDRESS)
        ROM1_END         ?= $(FWU_LRO_END_ADDRESS)
        RAM1_END         ?= 200007FF

    else ifdef ramLoader
        # -- RAM-Loader settings
        RAM1_END         ?= 200007FF
        ROM1_START       ?= 20000800
        ROM1_END         ?= 20001FFF
        RESET_VECTOR     ?= $(ROM1_START)

    else ifdef programmer
        FLASH_DRIVER_SIZE = 800
        ROM1_START       ?= $(FWU_PROGRAMMER_START_ADDRESS)
        ROM1_END         ?= $(FWU_PROGRAMMER_END_ADDRESS)
        defines          += COPY_FLASH_DRV_TO_RAM
        crc_start1       ?= 0x$(FWU_PROGRAMMER_START_ADDRESS)
        crc_end1         ?= 0x$(FWU_PROGRAMMER_END_ADDRESS)
        defines          += ABSOLUTE_ADDR_IN_VTOR
    else
        # -- settings for relocated application
        ROM1_START       ?= $(FWU_APP1_START_ADDRESS)
        ROM1_END         ?= $(FWU_APP1_END_ADDRESS)

        patchbin_dep     += $(BOOTLOADER) $(BOOTMANAGER) $(PROGRAMMER)
        vectorNanchorSize = 32
        patchbin_opt_uc  += -i(0x$(FWU_BOOTMANAGER_START_ADDRESS)+$(vectorNanchorSize),0x$(FWU_BOOTMANAGER_END_ADDRESS))$(BOOTMANAGER) -I(0x$(FWU_BOOTMANAGER_START_ADDRESS):B$(vectorNanchorSize))$(BOOTMANAGER)
        patchbin_opt_uc  += -i(0x$(FWU_LRO_START_ADDRESS),0x$(FWU_LRO_END_ADDRESS))$(BOOTLOADER)
        patchbin_opt_uc  += -i(0x$(FWU_PROGRAMMER_START_ADDRESS),0x$(FWU_PROGRAMMER_END_ADDRESS))$(PROGRAMMER)
        ifeq ($(fwu_flex_partition_used),ENABLED)
            patchbin_opt_uc += -f(0x$(FWU_FLEX_PARTITION_START_ADDRESS),0x$(FWU_FLEX_PARTITION_END_ADDRESS)):0x$(ROM_FILL)
        endif
        # fill possible gap between programmer and app
        patchbin_opt_uc  += -f(0x$(FWU_PROGRAMMER_END_ADDRESS),0x$(FWU_APP1_START_ADDRESS)):0x$(ROM_FILL)
        crc_start1       ?= 0x$(ROM1_PHY_START)
    endif
endif
#------------------------------------------------------------------------------

# if not already set, define RAM/ROM start/end
ROM1_START ?= $(ROM1_PHY_START)
ROM1_END   ?= $(ROM1_PHY_END)
RAM1_START ?= $(RAM1_PHY_START)
RAM1_END   ?= $(RAM1_PHY_END)

RAM1        = $(RAM1_START)-$(RAM1_END)

defines    += RAM0START=0x$(RAM1_START)UL
defines    += RAM0TOP=0x$(RAM1_END)UL
defines    += ROM1START=0x$(ROM1_START)UL
defines    += ROM1END=0x$(ROM1_END)UL
# defines FLASH_SIZE in kB as decimal number
defines    += FLASH_SIZE=$(FLASH_SIZE)UL

#************************************************************************************************
#*********************************** Settings for Patchbin **************************************
#************************************************************************************************
patchbin_opt_uc += -el  #Endianness

#Fill unused memory
ifndef NotFillRom
    ifdef ramLoader
        patchbin_opt_uc += -f(0x$(ROM1_START),?0x$(ROM1_END)):0x$(ROM_FILL)
    else
        patchbin_opt_uc += -f(0x$(ROM1_START),0x$(ROM1_END)):0x$(ROM_FILL)
    endif
    crc_start1 ?= 0x$(ROM1_START)
    crc_end1   ?= 0x$(ROM1_END)
endif
