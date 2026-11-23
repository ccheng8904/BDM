#*******************************************************************************
#  Copyright (c) 2023 BSH Hausgeraete GmbH,
#  Carl-Wery-Str. 34, 81739 Munich, Germany, www.bsh-group.de
#
#  All rights reserved. This program and the accompanying materials
#  are protected by international copyright laws.
#  Please contact copyright holder for licensing information.
#
#*******************************************************************************
#  PROJECT          MCU Framework / CMS32M67xx
#*******************************************************************************
#  Description      build include for CMS32M67xx (Cmsemicon) derivative specific
#                   definitions
#*******************************************************************************


# Parsing of the processor type, e.g. CMS32M6736EGH48NB, CMS32M6736EGH48FA,
# CMS32M6710GH48FA, CMS32M6734EGH48FA
#
#   Example:                   CMS 32 M 67 36 E GH 48 N B
#                               |   |  | |  |  |  |  | | |
#   Product family        ______|   |  | |  |  |  |  | | |
#   CMS = China Micro Semiconductor  |  |  |  |  |  | | |
#   Core               ____________|  |  |  |  |  |  | | |
#   32 = ARM Cortex M0+ 32bit         |  |  |  |  |  | | |
#   Product line         _____________|  |  |  |  |  | | |
#   67 = motor control / M67xx series    |  |  |  |  | | |
#   Sub sequence         _______________|  |  |  |  | | |
#   36 / 34 / 10 = flash and feature set   |  |  |  | | |
#   Silicon revision / temperature grade __|  |  |  | | |
#   Package code                           ___|  |  | | |
#   GH = QFN, FA = LQFP                       |  | | |
#   Pin count                 _________________|  | | |
#   48 pins                                      | | |
#   Packing / option             _________________| | |
#   N = QFN / tube, B ...                          | |
#   Production code                 _________________| |
#
# Only the memory relevant information is extracted below. Every derivative of
# the CMS32M67xx family provides 128kB program flash, 1kB data flash,
# 4kB SRAM0 and 8kB SRAM1.

##########################################################################
#********* CMS32M67xx Memory map ****************************************
#                                                                      **
#   |----------------------------------- |\                            **
#   |       0x2000 2FFF  RAM1_END        | \                           **
#   |  SRAM1        SRAM1 (8kB)          |  > RAM1 (SRAM1)             **
#   |       0x2000 1000  RAM1_START      | /                           **
#   |----------------------------------- |/                            **
#   |       0x2000 0FFF  RAM0_END        | \                           **
#   |  SRAM0        SRAM0 (4kB)          |  > RAM0 (SRAM0)             **
#   |       0x2000 0000  RAM0_START      | /                           **
#   |----------------------------------- |/                            **
#   |       0x0000 03FF  DFLASH_END      | \                           **
#   |  DATA FLASH   1kB                  |  > data flash               **
#   |       0x0000 0000  DFLASH_START    | /                           **
#   |----------------------------------- |/                            **
#   |       0x0002 00  ..                | \                           **
#   |  FLASH        128kB program flash  |  > ROM1 (FLASH)             **
#   |       0x0000 0200  FLASH_START     | /                           **
#   |----------------------------------- |/                            **
#   |       0x0000 0000  VECTORS         | \                           **
#   |  vector table + option bytes       |  > ROM0                     **
#   |       0x0000 00C3                   | /                           **
#   |----------------------------------- |/                            **
#                                                                      **
#********* CMS32M67xx Memory map (end) **********************************
##########################################################################

# set datasheet and ref. manual file names
af_doc_ds_name ?= DS_CMS32M67xx.pdf
af_doc_rm_name ?= RM_CMS32M67xx.pdf
af_doc_er_name ?= ER_CMS32M67xx.pdf

uc_platform ?= CMS32M67
uc_platform_teng ?= CoreMX
uc_vendor = CMS
defines += UC_VENDOR=$(uc_vendor)

cms32m67_subsequence   :=  $(call match,$(derivative),CMS32M(\d\d))
cms32m67_type          :=  CMS32M$(cms32m67_subsequence)

cms32              :=  1

platform_main_type    :=  $(uc_platform)
platform_type_segger  :=  $(derivative)

# CMS32M67xx family: 128kB flash, 4kB SRAM0 + 8kB SRAM1, 1kB data flash
ifdef FLASH_SIZE
    ifneq ($(FLASH_SIZE),128)
        $(error $(derivative) provides 128kB of flash only, FLASH_SIZE=$(FLASH_SIZE) is not supported)
    endif
endif
FLASH_SIZE        ?= 128
ROM1_PHY_START    ?= 200
ROM1_PHY_SIZE     ?= 1FE00
ROM1_PHY_END      ?= 1FFFF

test_derivative:
	@echo derivative            = $(derivative)
	@echo cms32m67_subsequence  = $(cms32m67_subsequence)
	@echo cms32m67_type         = $(cms32m67_type)
	@echo platform_main_type    = $(platform_main_type)
	@echo platform_type_segger  = $(platform_type_segger)
	@echo ROM1_START            = $(ROM1_START)
	@echo ROM1_END              = $(ROM1_END)
	@echo ROM1_SIZE             = $(ROM1_SIZE)
	@echo RAM1_START            = $(RAM1_START)
	@echo RAM1_END              = $(RAM1_END)
	@echo RAM1_SIZE             = $(RAM1_SIZE)


defines += PROC_FAMILY=CMS32M67XX
defines += CMS32M67XX
defines += $(cms32m67_type)

# CMS32M67xx header selection, the device header is chosen by these defines
ifneq (,$(findstring M6736,$(derivative)))
    defines += CMS32M6736
else ifneq (,$(findstring M6734,$(derivative)))
    defines += CMS32M6734
else ifneq (,$(findstring M6710,$(derivative)))
    defines += CMS32M6710
endif

# it is used HUARTx_bIsRxOnGoing instead of IRQ for idle
dbm_use_irq_for_idle_detection = true

cc_core = cm0p

# alignment of vector table offset, aligned to fit Module Header and Flex Table
VTOR_ALIGN = 0x200

# Page size (sector size) is 512 bytes for CMS32M67xx series
PAGE_SIZE = 200

RAM0_PHY_START    ?= 20000000
RAM0_PHY_SIZE     ?= 1000
RAM0_PHY_END      ?= 20000FFF

RAM1_PHY_START    ?= 20001000
RAM1_PHY_SIZE     ?= 2000
RAM1_PHY_END      ?= 20002FFF

# Data flash (1kB) used e.g. for user option/trim data
ROM3_PHY_START    ?= 0
ROM3_PHY_END      ?= 3FF
ROM3_PHY_SIZE     ?= 400
ROM3_START        ?= $(ROM3_PHY_START)
ROM3_END          ?= $(ROM3_PHY_END)
ROM3_SIZE         ?= $(ROM3_PHY_SIZE)


STACKSIZE         ?= 400
HEAPSIZE          ?= 0

# size of bytes of flash driver located in RAM
FLASH_DRIVER_SIZE ?= 200

#**** Boot-Loader Definitions ***
BL_VECTOR_START   ?= 0
ROOT_REGION       ?= 0

# smallest writable block
write_block_size   = 4

# Common Settings for CMS32M67xx
ROM_FILL          ?= 00
defines           += COMA_ERASE_BYTE=0x00u

ROM1_PHY_START    ?= 200
RAM1_PHY_START    ?= 20001000
RESET_VECTOR      ?= 4
VECTOR_SIZE        = B4

VECTOR_TABLE_SIZE  = C0

winflash_cfg_template ?= $(msp_path)/bootloader/CMS32M67xx.bp2.h

jlink_speed = 1000
#--------------- Logical memory mapping ----------------------------------------
bootloader_merge  ?= true
# -- test server / testClient: for internal use only
ifdef testServer
    FW_update_type = FWU1
    ROM1_END      ?= $(ROM1_PHY_END)
    RAM1_END      ?= 20001FFF
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
        RAM1_END ?= 200017FF
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
        RAM1_END         ?= 20002FFF
        ROM1_START       ?= $(ROM1_PHY_START)
        ROM1_END         ?= $(FWU_BOOTMANAGER_END_ADDRESS)

    else ifdef lromLoader
        # -- ROM-Loader --
        ROM1_START       ?= $(FWU_LRO_START_ADDRESS)
        ROM1_END         ?= $(FWU_LRO_END_ADDRESS)
        RAM1_END         ?= 20002FFF

    else ifdef ramLoader
        # -- RAM-Loader settings
        RAM1_END         ?= 20002FFF
        ROM1_START       ?= 20001000
        ROM1_END         ?= 20002FFF
        RESET_VECTOR     ?= $(ROM1_START)

    else ifdef programmer
        FLASH_DRIVER_SIZE = 200
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
