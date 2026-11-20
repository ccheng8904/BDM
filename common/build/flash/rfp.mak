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
#  Description      targets for Renesas Flash Programmer with E1 Programmer
#*******************************************************************************

# command line version of Renesas Flash Programmer
flash_rfp_exe  ?= $(flash_rfp_path)/RFPV3.exe

ifeq ($(build_type), RELEASE)
    flash_rfp_file ?= $(lastword $(release_bin_output_file))
else
    flash_rfp_file ?= $(patchbin_out_mot)

endif
flash_rfp_log  ?= $(out_path)/$(variant)/rfp_log.txt

# If more than one E1 is connected
# flash_rfp_sn   ?= 2LS040071A

ifdef flash_rfp_sn
    flash_rfp_sn_cmd = /tool $(flash_rfp_sn)
else
    flash_rfp_sn_cmd =
endif



flashE1_check:
ifeq ($(wildcard $(flash_rfp_exe)),)
	$(call tool_error,$(flash_rfp_exe))
endif

flashE1: link flashE1_only
flashE1_no_link: flashE1_only

flashE1_only: flashE1_check
	@echo Log1:$(flash_rfp_log)
	@echo Start Flashing
	@echo cmd-options: /silent $(flash_rfp_set) /file $(flash_rfp_file) /log $(flash_rfp_log) $(flash_rfp_sn_cmd)
	@$(flash_rfp_exe) /silent $(flash_rfp_set) /file $(flash_rfp_file) /log $(flash_rfp_log) $(flash_rfp_sn_cmd)
	@$(call prntGr,-------- Flashing done -------------)
