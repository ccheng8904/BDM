#*******************************************************************************
#  Copyright (c) 2017 BSH Hausgeraete GmbH,
#  Carl-Wery-Str. 34, 81739 Munich, Germany, www.bsh-group.de
#
#  All rights reserved. This program and the accompanying materials
#  are protected by international copyright laws.
#  Please contact copyright holder for licensing information.
#
#*******************************************************************************
#  Project          MCU Framework
#*******************************************************************************
#  Description      definitions for release target
#*******************************************************************************

uid              ?= 0
version_major    ?= 0
version_minor    ?= 0
version_revision ?= 0

defines +=FW_UID=UINT64_C($(call calc,$(uid)))
defines +=VERSION_MAJOR=UINT16_C($(call calc,$(version_major)))
defines +=VERSION_MINOR=UINT16_C($(call calc,$(version_minor)))
defines +=VERSION_REVISION=UINT16_C($(call calc,$(version_revision)))

ifeq ($(FW_update_type),FWU4)
    uid_gbl              ?= 0
    version_major_gbl    ?= 0
    version_minor_gbl    ?= 0
    version_revision_gbl ?= 0
    version_build_gbl    ?= 0x$(project_rev_hash)
    
    defines +=UID_GBL=UINT64_C($(call calc,$(uid_gbl)))
    defines +=VERSION_MAJOR_GBL=UINT16_C($(call calc,$(version_major_gbl)))
    defines +=VERSION_MINOR_GBL=UINT16_C($(call calc,$(version_minor_gbl)))
    defines +=VERSION_REVISION_GBL=UINT16_C($(call calc,$(version_revision_gbl)))
    defines +=VERSION_BUILD_GBL=UINT32_C($(call calc,$(version_build_gbl)))
endif


ifdef dbus_target_node
    defines +=DBUS_TARGET_NODE=$(call calc,$(dbus_target_node))
endif

release_path := $(bin_path)/release
#optional application specific release definitions (e.g. for additional partitions)
-include $(app_path)/build/release_cfg.mak

# files needed only in case of FWU3 build RELEASE (not needed for FWU1 build RELEASE)
ifeq ($(FW_update_type),FWU3)
    release_bin_file_in := $(patchbin_out_bin)
    release_bin_file    := $(release_path)/Application
    release_file_list_non_copy += $(release_bin_file)
endif

ifdef FWU_APP1_START_ADDRESS2
    release_bin_file2_in := $(patchbin_out_bin_2)
    release_bin_file2    := $(release_path)/App_part2
    release_file_list_non_copy += $(release_bin_file2)
endif

release_file_list_default ?= $(sym_file) $(syc_file)

# main release binary
release_binary_targets = release_bin
release_bin_output_file = $(release_path)/$(variant_base).hex $(release_path)/$(variant_base).mot

# Exclude FWU OTP section from release binaries
ifdef FWU_OTP_MEMORY_START_ADDRESS
    release_bin_exclude_range += 0x$(FWU_OTP_MEMORY_START_ADDRESS),0x$(FWU_OTP_MEMORY_END_ADDRESS)
endif

# for jlink release binary
ifeq ($(jlink_binary_type),1)
    release_binary_targets += release_bin_jlink
    release_bin_jlink_input_file = $(patchbin_out_jlink)
    release_bin_jlink_output_file = $(release_path)/$(variant_base)_jlink.hex $(release_path)/$(variant_base)_jlink.mot
    release_bin_jlink_exclude_range = $(release_bin_exclude_range)
endif

#similar for _rom binary
ifdef patchbin_out_hex_rom
    release_binary_targets += release_bin_rom
    release_bin_rom_input_file = $(patchbin_out_hex_rom)
    release_bin_rom_output_file = $(release_path)/$(variant_base)_rom.hex $(release_path)/$(variant_base)_rom.mot
    release_bin_rom_exclude_range = $(release_bin_exclude_range)
endif

bin_gen += $(release_binary_targets)

# project may append to release_file_list or define own $(release_file_list_default)
release_file_list += $(release_file_list_default)


release: $(release_binary_targets) $(release_dep) release_copy


.PHONY: release_copy
release_copy: $(release_file_list)
	@echo --- copy release files ---
	@$(call mkdir, $(release_path))
	@$(foreach file,$(release_file_list) $(release_bin_output_file),$(call copy,$(file),$(release_path) /Y))
	@$(call copy,$(release_bin_file_in),$(release_bin_file) /Y)
ifdef FWU_APP1_START_ADDRESS2
	@$(call copy,$(release_bin_file2_in),$(release_bin_file2))
endif

