#*******************************************************************************
#  Copyright (c) 2017 BSH Hausgeraete GmbH,
#  Carl-Wery-Str. 34, 81739 Munich, Germany, www.bsh-group.de
#
#  All rights reserved. This program and the accompanying materials
#  are protected by international copyright laws.
#  Please contact copyright holder for licensing information.
#
#*******************************************************************************
#  PROJECT          PP PED Framework
#*******************************************************************************
#  Description      settings and targets for PatchBin
#*******************************************************************************

# JLink flashing for some STM32 platforms requires opt. bytes to be at address 0x06000000
ifeq ($(uc_vendor),ST)
ifneq ($(is_data_variant),true)
    ifeq (,$(filter $(variant_base), romLoader lromLoader ramLoader programmer BootManager))
        ifndef noOptionBytes
            patchbin_opt_uc += -ie()$(OPTION_BYTES_EEP)
        endif
        ifneq (,$(filter $(platform_main_type), STM32F0 STM32F1 STM32F3))
            # patch bin output for JLink flashing (only some STM32 platforms)
            # move option bytes from 0x1FFFF800 to 0x06000000
            #extra binaries will be patched with option bytes at address 0x06000000
            jlink_binary_type := 1
            patchbin_out_jlink      = $(bin_path)/$(variant_base)_jlink.mot
            patchbin_out_hex_jlink  = $(bin_path)/$(variant_base)_jlink.hex
            patchbin_out += $(patchbin_out_jlink)
            bin_gen += jlink_binary
            jlink_binary_suffix = _jlink
            jlink_binary_patchbin_fill = -m(0x1FFFF800:B16)(abs=0x06000000)
            jlink_binary_output_file = $(patchbin_out_jlink) $(patchbin_out_hex_jlink)
        else ifneq (,$(filter $(platform_main_type), STM32F4 STM32L4))
            # extra binaries will be patched without option bytes
            jlink_binary_type := 2
        else ifneq (,$(filter $(platform_main_type), STM32G0 STM32G4))
            # extra binaries not needed
            jlink_binary_type := 3
        endif
    endif
endif
endif


# patch bin output
patchbin_out_mot  = $(bin_path)/$(variant_base).mot
patchbin_out_unsigned_mot  = $(bin_path)/$(variant_base)_unsigned.mot
patchbin_out_hex = $(bin_path)/$(variant_base).hex
patchbin_out_bin = $(bin_path)/$(variant_base).bin
patchbin_out_bin_2 = $(bin_path)/$(variant_base)_2.bin

patchbin_out_bin_pat = $(bin_path)/$(variant_base)_partition.bin
patchbin_log = $(bin_path)/patchbin.log


ifdef patchBin_trash_region
    ifndef exclamation_mark_issue
        ifeq ($(FW_update_type),FWU4)
            patchbin_opt_gen += -i!($(patchBin_trash_region);0x$(FWU_OTP_MEMORY_START_ADDRESS),0x$(FWU_OTP_MEMORY_END_ADDRESS))$(linker_bin)
        else
            patchbin_opt_gen += -i!($(strip $(patchBin_trash_region)))$(linker_bin)
        endif
    else
        # This is a workaround for systems with problem of '!' usage.
        # Disadvantage: Trash area must be continuous using this option
        TRASH_AREA_SIZE  ?= 0x10000
        LAST_POSSIBLE_ADDRESS ?= 0xFFFFFFFF
        TRASH_AREA_BORDER = $(call calcx,($(TRASH_AREA_START) - 1))
        ifeq ($(FW_update_type),FWU4)
        FWU_OTP_MEMORY_BORDER = $(call calcx,0x$(FWU_OTP_MEMORY_START_ADDRESS) - 1)
            patchbin_opt_gen += -i(0x00,0x$(FWU_OTP_MEMORY_BORDER);0x$(FWU_OTP_MEMORY_START_ADDRESS)+0x$(FWU_OTP_MEMORY_SIZE),0x$(TRASH_AREA_BORDER);$(TRASH_AREA_START)+$(TRASH_AREA_SIZE),$(LAST_POSSIBLE_ADDRESS))$(linker_bin)
        else
            patchbin_opt_gen += -i(0x00,0x$(TRASH_AREA_BORDER);$(TRASH_AREA_START)+$(TRASH_AREA_SIZE),$(LAST_POSSIBLE_ADDRESS))$(linker_bin)
        endif
    endif
else
    patchbin_opt_gen += -i()$(linker_bin)
endif

patchbin_opt_gen += -S$(sym_file)

#Silent option
patchbin_silent ?= 1
ifeq ($(patchbin_silent),0)
    #Do not print any information to stdout
    patchbin_opt_gen += -silent0
endif
ifeq ($(patchbin_silent),1)
    #Dot not print any information and warnings to stdout
    patchbin_opt_gen += -silent1
endif

ifdef crc_16bit_width
    crctype = 16
else
    crctype = 32
endif

ifdef crc_start1
  CRC_TMO ?= 5
    patchbin_crc = -CRC($(crc_start1),$(crc_end1))(v=CRC1):$(crctype)
    patchbin_crc_out1 = \nCRCstart1=$(crc_start1)\nCRCend1=$(crc_end1)\nCRC1=%%CRC1\n
endif
ifdef crc_start2
    patchbin_crc += -CRC($(crc_start2),$(crc_end2))(v=CRC2):$(crctype)
    patchbin_crc_out2 += \nCRCstart2=$(crc_start2)\nCRCend2=$(crc_end2)\nCRC2=%%CRC2\n
endif
ifdef crc_start3
    patchbin_crc += -CRC($(crc_start3),$(crc_end3))(v=CRC3):$(crctype)
    patchbin_crc_out3 += \nCRCstart3=$(crc_start3)\nCRCend3=$(crc_end3)\nCRC3=%%CRC3\n
endif
ifdef crc_start4
    patchbin_crc += -CRC($(crc_start4),$(crc_end4))(v=CRC4):$(crctype)
    patchbin_crc_out4 += \nCRCstart4=$(crc_start4)\nCRCend4=$(crc_end4)\nCRC4=%%CRC4\n
endif
ifdef crc_start5
    patchbin_crc += -CRC($(crc_start5),$(crc_end5))(v=CRC5):$(crctype)
    patchbin_crc_out5 += \nCRCstart5=$(crc_start5)\nCRCend5=$(crc_end5)\nCRC5=%%CRC5\n
endif
ifdef crc_start6
    patchbin_crc += -CRC($(crc_start6),$(crc_end6))(v=CRC6):$(crctype)
    patchbin_crc_out6 += \nCRCstart6=$(crc_start6)\nCRCend6=$(crc_end6)\nCRC6=%%CRC6\n
endif

patchbin_out_comment_crc ?= $(patchbin_crc_out1)$(patchbin_crc_out2)$(patchbin_crc_out3)$(patchbin_crc_out4)$(patchbin_crc_out5)$(patchbin_crc_out6)CRCtmo=$(CRC_TMO)\n

#Output of PatchBin can be redefined in project.inc
patchbin_out_format_mot ?= -omS0multi($(BIN_OUT_ADRESSES))
patchbin_out_format_hex ?= -oh($(BIN_OUT_ADRESSES))

patchbin_out_comment ?= $(variant_base)

ifeq ($(is_data_variant),true)
    patchbin_opt       += $(patchbin_opt_gen) $(patchbin_opt_proj) -el
else
    patchbin_opt       += $(patchbin_opt_gen) $(patchbin_opt_uc) $(patchbin_opt_proj) $(patchbin_move_app) $(patchbin_fwids_crc) $(patchbin_crc_app)
endif


# patchbin output options
ifdef patchbin_out_comment_crc
    patchbin_out_opt += $(patchbin_crc) $(patchbin_out_format_mot)$(patchbin_out_mot)\#"$(patchbin_out_comment_crc)$(patchbin_out_comment)"
else
    patchbin_out_opt += $(patchbin_out_format_mot)$(patchbin_out_mot)\#"$(patchbin_out_comment)"
endif
ifdef patchbin_out_format_hex
    patchbin_out_opt += $(patchbin_out_format_hex)$(patchbin_out_hex)
endif

patchbin_out_opt += $(patchbin_opt_out)

crc_offset = 0x04

ifeq ($(FW_update_type),FWU3)
    ifeq ($(fwu_flex_partition_used),ENABLED)
        ifneq ($(variant_base), ramLoader)
            pat_crc_calc_start ?= $(FWU_FLEX_PARTITION_START_ADDRESS)+$(crc_offset)
            pat_crc_calc_end   ?= $(FWU_FLEX_PARTITION_END_ADDRESS)
            pat_crc_store_addr ?= $(FWU_FLEX_PARTITION_START_ADDRESS)
            patchbin_binary_out_opt = -ob()$(patchbin_out_bin_pat)
            patchbin_crc_app   += -CRC(0x$(pat_crc_calc_start),0x$(pat_crc_calc_end))(0x$(pat_crc_store_addr)):$(crctype)
        endif
    endif
    ifeq ($(app_variant), true)
        patchbin_fwids_crc      = -CRC(OTP_Data_fwIdArray_value_entries)(OTP_Data_fwIdArray_value_CRC):$(crctype)
        crc_calc_start ?= $(FWU_APP1_START_ADDRESS)+$(crc_offset)
        crc_calc_end   ?= $(FWU_APP1_END_ADDRESS)
        crc_store_addr ?= $(FWU_APP1_START_ADDRESS)
        bin_end_addr   ?= $(crc_calc_end)
        patchbin_binary_out_opt += -ob()$(patchbin_out_bin)
        ifdef FWU_APP1_START_ADDRESS2
            # create additional binary for second application fragment if present
            crc_calc_start2 ?= $(FWU_APP1_START_ADDRESS2)+$(crc_offset)
            crc_calc_end2   ?= $(FWU_APP1_END_ADDRESS2)
            crc_store_addr2 ?= $(FWU_APP1_START_ADDRESS2)
            bin_end_addr2   := $(crc_calc_end2)
            patchbin_binary_out_opt2 = -ob()$(patchbin_out_bin_2)
        endif
    endif
endif

ifdef app_variant
    ifeq ($(FW_update_type),FWU3)
        ifeq ($(strip $(build_type)),DEVELOP)
            # used opt. bytes defined by user (fwu_opt_bytes_cfg.c)
            patchbin_opt_uc += -I(MAL_FwuOptBytes)$(linker_bin)
        endif
        ifneq ($(platform),rx200)
            # CRC recalculation for BootManager. Rx200 is excluded because FS is missing.
            FWU_BOOTMANAGER_END_ADDRESS_FS ?= $(call calcx,(0x$(FWU_BOOTMANAGER_END_ADDRESS) - 0x$(crc32_size)))
            FWU_BOOTMANAGER_CRC_FS ?= $(call calcx,(0x$(FWU_BOOTMANAGER_END_ADDRESS_FS) + 1))
            patchbin_crc_app += -crc32(0x$(FWU_BOOTMANAGER_START_ADDRESS),0x$(FWU_BOOTMANAGER_END_ADDRESS_FS))(0x$(FWU_BOOTMANAGER_CRC_FS)):0x4C11DB7,0xFFFFFFFF
        endif
    endif
endif

ifdef crc_store_addr
    # CRC for first fragment of application (also for programmer and lromLoader)
    patchbin_crc_app += -CRC(0x$(crc_calc_start),0x$(crc_calc_end))(0x$(crc_store_addr)):$(crctype)
    ifdef crc_calc_start2
        # separate CRC for second fragment of application if present
        patchbin_crc_app += -CRC(0x$(crc_calc_start2),0x$(crc_calc_end2))(0x$(crc_store_addr2)):$(crctype)
    endif
endif


# ************************ patch bin main recipe *******************************
.PHONY: patchbin
patchbin: $(sym_file) $(patchbin_out) $(patchbin_out_mot)

$(patchbin_out_mot): $(sym_file) $(patchbin_dep) $(MAKEFILE_LIST)
	@echo patching binaries with PatchBin...
	@$(patchbin) -L$(patchbin_log) $(patchbin_opt) $(patchbin_out_opt)
ifeq ($(app_variant), true)
ifeq ($(FW_update_type),FWU3)
ifeq ($(is_data_variant),true)
	@$(patchbin) -silent1 -i(0x$(FWU_DATA_VARIANT_START_ADDRESS),0x$(FWU_DATA_VARIANT_END_ADDRESS))(abs=0x00)$(patchbin_out_mot) $(patchbin_binary_out_opt)
else
	@$(patchbin) -silent1 -i(0x$(FWU_APP1_START_ADDRESS),0x$(bin_end_addr))(abs=0x00)$(patchbin_out_mot) $(patchbin_binary_out_opt)
ifdef patchbin_binary_out_opt2
	@$(patchbin) -silent1 -i(0x$(FWU_APP1_START_ADDRESS2),0x$(bin_end_addr2))(abs=0x00)$(patchbin_out_mot) $(patchbin_binary_out_opt2)
endif
endif
endif
ifeq ($(patch_psoc_checksum), true)
	@echo applying PSoC checksum...
	@$(call cy_checksum,$(patchbin_out_hex))
endif
endif
ifeq ($(FW_update_type),FWU4)
	$(call copy,$(patchbin_out_mot),$(patchbin_out_unsigned_mot))
	$(call silabs_sign,$(patchbin_out_mot),$(patchbin_out_mot))
ifeq ($(app_variant), true)
	$(call silabs_create_gbl,$(bin_path)/$(variant_base).gbl,$(patchbin_out_mot))
	$(call silabs_create_bl_gbl,$(bin_path)/$(variant_base)_bl.gbl,$(patchbin_out_mot),$(GECKO_BL))
	@$(python) $(local_tool_path)/python/create_gbl_mot.py -infile $(bin_path)/$(variant_base).gbl -offset 0x$(FWU4_GBL_IMAGE_START_ADDRESS)
	@$(python) $(local_tool_path)/python/create_gbl_mot.py -infile $(bin_path)/$(variant_base)_bl.gbl -offset 0x$(FWU4_GBL_IMAGE_START_ADDRESS)

endif
ifdef patchbin_opt_fwu4_merge
	@$(call prntGr,-------- merging BTM Application -------------)
	@$(patchbin) -silent1 -i()$(patchbin_out_mot) $(patchbin_opt_fwu4_merge) $(patchbin_out_opt)
endif
endif


# Obsolete patchbin targets
obsolete_targets += extract_nvmem_area extract_config_area extract_touch_area
obsolete_targets += patch_nvmem_area patch_config_area patch_touch_area

$(obsolete_targets):
	$(error Target $@ is obsolete. Please use binary generator instead, or contact MCU Framework support)
