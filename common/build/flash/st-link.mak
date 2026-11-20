#*******************************************************************************
#  Copyright (c) 2017 BSH Hausgeraete GmbH,
#  Carl-Wery-Str. 34, 81739 Munich, Germany, www.bsh-group.de
#
#  All rights reserved. This program and the accompanying materials
#  are protected by international copyright laws.
#  Please contact copyright holder for licensing information.
#
#*******************************************************************************
#  Project          PED FW
#*******************************************************************************
#  Description      targets for ST-Link-Utility
#*******************************************************************************

# command line version of ST-Link-Utility
flash_st_exe  ?= $(st_programmer_path)/bin/STM32_Programmer_CLI.exe
ifeq ($(build_type), RELEASE)
    flash_st_file ?= $(firstword $(release_bin_output_file))
else
    flash_st_file ?= $(patchbin_out_hex)
endif


flash_st_log  ?= $(out_path)/$(variant)/stm32_programmer$(log_cnt).log

flash_st_erase_log  ?= $(out_path)/$(variant)/stm32_eraser$(log_cnt).log

# ID of ST-LINK[0..9] to be used when multiple probes are connected to the host
# Or [SN=<sn>]: Serial Number of the chosen ST-LINK probe.
st_link_id ?= 0

# [index=<index>] Index of the debug probe. Default index value is 0.
# [sn=<serialNumber>]Serial Number of the debug probe.
con ?= index

.PHONY: flashST_check flashST flashST_no_link flashST_only

flashST_check:
ifeq ($(wildcard $(flash_st_exe)),)
	$(call tool_error,$(flash_st_exe))
endif

flashST: link flashST_only
flashST_no_link: flashST_only

flashST_only: flashST_check
	@echo Log1:$(flash_st_log)
	@echo Start Flashing with using $(flash_st_exe)
	@$(flash_st_exe) --connect port=SWD $(con)=$(st_link_id) mode=UR -e all --download $(flash_st_file) --log $(flash_st_log) -hardRst
	@$(call prntGr,-------- Flashing done -------------)
    
flashST_erase: flashST_check
	@echo Start Erasing with using $(flash_st_exe)
	@$(flash_st_exe) --connect port=SWD $(con)=$(st_link_id) mode=UR -e all --log $(flash_st_erase_log) -hardRst
	@$(call prntGr,-------- Erasing done -------------)
