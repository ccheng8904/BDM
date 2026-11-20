#*******************************************************************************
#  Copyright (c) 2017 BSH Hausgeraete GmbH,
#  Carl-Wery-Str. 34, 81739 Munich, Germany, www.bsh-group.de
#
#  All rights reserved. This program and the accompanying materials
#  are protected by international copyright laws.
#  Please contact copyright holder for licensing information.
#
#*******************************************************************************
#  PROJECT          Generic SW
#*******************************************************************************
#  Description      Binary generation makefile
#*******************************************************************************

# This makefile serves for generating binary files. To add new binary to
# generate, append target name to variable "bin_gen". Then create variables
# with <binary_target>_XXX based on following list of variables:

# Input/output/log files:
# <binary_target>_input_file    - optional input file for generating binary
#                                 (default $(patchbin_out_hex))
# <binary_target>_output_file   - output file(s) with path. By default, .hex
#                                 and .mot files are created in the folder
#                                 where input file is located, with
#                                 <binary_target> suffix.
# <binary_target>_patch_file    - optional file used to patch/rewrite sections
#                                 in specified <binary_target>_input_file
#
# <binary_target>_log_file      - log file name with path (default no log)
#                                 By default, .log file is created in the
#                                 folder where input file is located, with
#                                 <binary_target> suffix.
# <binary_target>_sym_file      - input symbol file (default $(sym_file))
# <binary_target>_suffix        - suffix added to output and log files
#                                 (default <binary_target>)
#
# Include/exclude range (optional):
# <binary_target>_exclude_range - range of memory that will be excluded from
#                                 input file. This variable can be also valid
#                                 symbol from .sym file. There can be multiple
#                                 exclude ranges separated by space.
# <binary_target>_output_range  - range of memory for output file(s). This
#                                 variable can be also valid symbol from .sym
#                                 file. If range is specified as start/end
#                                 address then CRC for WinFlash is calculated.
#
# CRC calculation (optional):
# <binary_target>_crc_range     - range of memory for calculating CRC
# <binary_target>_crc_addr      - address of where to place calculated CRC
# <binary_target>_crc_type      - type of CRC (default $(crctype))
#
# Pattern patching (optional):
# <binary_target>_fill_range    - range of memory that will be filled with pattern
# <binary_target>_fill_pattern  - pattern for filling memory (default 0x$(ROM_FILL))
#
# Output info severity (optional):
# <binary_target>_silent        - Suppression of output (default $(patchbin_silent))
#                                 0: Do not print any information to stdout.
#                                 1: Do not print any information and warnings to stdout.
#
# Direct patchbin options (optional, patchbin syntax required):
# <binary_target>_patchbin_in   - Input file option
# <binary_target>_patchbin_fill - Memory filling
# <binary_target>_patchbin_crc  - CRC calculation
# <binary_target>_patchbin_out  - Output file(s) option
# <binary_target>_patchbin_msg  - Message to save to S0-record. Message from
#                                 original file is discarded.


# ******************************************************************************
# Examples
# ******************************************************************************

# Example 1 - extracting range (0x1000 - 0x1FFF) of memory from binary:
# bin_gen += extract_bin
# extract_bin_output_range  = 0x1000,0x1FFF

# Example 2 - exclude range (0x1000 - 0x1FFF) of memory from binary and save output
# to hex and mot files:
# bin_gen += exclude_bin
# exclude_bin_exclude_range = 0x1000,0x1FFF

# Example 3 - extract structure MDHD_ModuleHeader from binary and save as .eep file:
# bin_gen += extract_symbol
# extract_symbol_output_file = $(basename $(patchbin_out_hex))_extract_symbol.eep
# extract_symbol_output_range = MDHD_ModuleHeader

# Example 4 - fill range 0x1000,0x1FFF with pattern 0xABCD, overwriting original data:
# bin_gen += fill_area
# fill_area_exclude_range = 0x1000,0x1FFF
# fill_area_fill_range = 0x1000,0x1FFF
# fill_area_fill_pattern = 0xABCD


# ******************************************************************************
# Template for binary generator variables and targets
# ******************************************************************************

coma := ,
empty:=
space:= $(empty) $(empty)

define bin_gen_variable_template =
    # Default variable definitions
    $(1)_input_file     ?= $(patchbin_out_mot)
    $(1)_suffix         ?= $(1)
    $(1)_output_base     = $$(basename $$($(1)_input_file))$$($(1)_suffix)
    $(1)_output_file    ?= $$($(1)_output_base).hex $$($(1)_output_base).mot
    $(1)_log_file       ?= $$($(1)_output_base).log
    $(1)_sym_file       ?= $(sym_file)
    $(1)_silent         ?= $(patchbin_silent)
    $(1)_crc_type       ?= $(crctype)
    $(1)_crc_range      ?= $$($(1)_output_range)
    $(1)_fill_pattern   ?= 0x$(ROM_FILL)
    $(1)_patchbin_msg   += \n
    
    # Log file
    ifneq (,$$($(1)_log_file))
        $(1)_patchbin_log = -L$$($(1)_log_file)
    endif
    
    # SYM file
    $(1)_patchbin_in += -S$$($(1)_sym_file)
    
    # Input file
    $(1)_patchbin_in += -i!($(subst $(space),;,$($(1)_exclude_range)))$$($(1)_input_file)
    
    # Patch file (optional)
    ifdef $(1)_patch_file
        $(1)_patchbin_in += -I()$$($(1)_patch_file)
    endif
    
    # Filling (optional)
    ifdef $(1)_fill_range
        $(1)_patchbin_fill += -f($$($(1)_fill_range)):$$($(1)_fill_pattern)
    endif
    
    # User CRC (optional)
    ifdef $(1)_crc_addr
        $(1)_patchbin_crc += -CRC($$($(1)_crc_range))($$($(1)_crc_addr)):$$($(1)_crc_type)
    endif
    
    # CRC calculation and S0-record message for WinFlash (if output_range is defined using start,end address)
    ifdef $(1)_output_range
        # Split output range with coma
        $(1)_ors = $$(subst $$(coma), , $$($(1)_output_range))
        # Check if there are two parameters (start,end address)
        ifeq ($$(words $$($(1)_ors)),2)
            # Fill empty area with $(ROM_FILL) pattern
            $(1)_patchbin_fill += -f($$($(1)_output_range)):0x$(ROM_FILL)
            # Calculate CRC for range and add message
            $(1)_patchbin_crc += -CRC($$($(1)_output_range))(v=CRC):$(crctype)
            $(1)_patchbin_msg += CRCstart1=$$(firstword $$($(1)_ors))\nCRCend1=$$(lastword $$($(1)_ors))\nCRC1=%%CRC\n
        endif
    endif
    
    # Output file(s)
    $(1)_output_file_hex ?= $$(filter %.hex, $$($(1)_output_file))
    $(1)_output_file_mot ?= $$(filter %.mot, $$($(1)_output_file))
    $(1)_output_file_eep ?= $$(filter %.eep, $$($(1)_output_file))
    $(1)_output_file_bin ?= $$(filter %.bin, $$($(1)_output_file))
    ifneq (,$$($(1)_output_file_hex))
        $(1)_patchbin_out += -oh($$($(1)_output_range))$$($(1)_output_file_hex)
    endif
    ifneq (,$$($(1)_output_file_mot))
        $(1)_patchbin_out += -om($$($(1)_output_range))$$($(1)_output_file_mot)\#"$$($(1)_patchbin_msg)"
    endif
    ifneq (,$$($(1)_output_file_eep))
        $(1)_patchbin_out += -oe($$($(1)_output_range))$$($(1)_output_file_eep)
    endif
    ifneq (,$$($(1)_output_file_bin))
        $(1)_patchbin_out += -ob($$($(1)_output_range))$$($(1)_output_file_bin)
    endif
    
    # All options together
    $(1)_patchbin_opt += $$($(1)_patchbin_log) $$($(1)_patchbin_in)
    $(1)_patchbin_opt += $$($(1)_patchbin_fill) $$($(1)_patchbin_crc)
    $(1)_patchbin_opt += $$($(1)_patchbin_out)
    
    # Append silent config (possible settings 0, 1, NO)
    ifeq ($$($(1)_silent),0)
        $(1)_patchbin_opt += -silent0
    else ifeq ($$($(1)_silent),1)
        $(1)_patchbin_opt += -silent1
    endif
endef


define bin_gen_target_template =
    # Target template for patching binaries
.PHONY: $(1)
$(1): $$(firstword $$($(1)_output_file))

$$($(1)_output_file): $$($(1)_input_file) $$($(1)_sym_file)
	@echo generating binary $$($(1)_output_file)
	$(call mkdir, $$(dir $$($(1)_output_file)))
	@$(patchbin) $$($(1)_patchbin_opt)
endef


# ******************************************************************************
# Expansion of all binary generation inputs
# ******************************************************************************

# Create variables for all bin_gen entries
$(foreach bin,$(bin_gen),$(eval $(call bin_gen_variable_template,$(bin))))

# Create targets for all bin_gen entries
$(foreach bin,$(bin_gen),$(eval $(call bin_gen_target_template,$(bin))))
