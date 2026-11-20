#*******************************************************************************
#  Copyright (c) 2023 BSH Hausgeraete GmbH,
#  Carl-Wery-Str. 34, 81739 Munich, Germany, www.bsh-group.de
#
#  All rights reserved. This program and the accompanying materials
#  are protected by international copyright laws.
#  Please contact copyright holder for licensing information.
#
#*******************************************************************************
#  Project          PED FW
#*******************************************************************************
#  Description      targets for GD_LINK_CLI.exe
#*******************************************************************************


# command line version of GD_LINK_CLI
flash_gd_exe ?= $(gdlink_programmer_path)\GD_Link_CLI.exe

ifeq ($(build_type), RELEASE)
    flash_gd_file ?= $(firstword $(release_bin_output_file))
else
    flash_gd_file ?= $(patchbin_out_hex)
endif


flash_gd_commandfile  ?= $(CURDIR)\$(out_path)\$(variant)\flash.gdlink
erase_gd_commandfile  ?= $(CURDIR)\$(out_path)\$(variant)\erase.gdlink

flash_gd_log  ?= $(CURDIR)/$(out_path)/$(variant)/gd32_programmer$(log_cnt).log
erase_gd_log  ?= $(CURDIR)/$(out_path)/$(variant)/gd32_erase$(log_cnt).log

# ID of GD-LINK[0..9] to be used when multiple probes are connected to the host
# Or [SN=<sn>]: Serial Number of the chosen GD-LINK probe.
gd_link_device_id ?= 0

.PHONY: flashGD_check flashGD flashGD_no_link flashGD_only flashGD_erase

flashGD_check:
ifeq ($(wildcard $(flash_gd_exe)),)
	$(call tool_error,$(flash_gd_exe))
endif

flashGD: link flashGD_only
flashGD_no_link: flashGD_only

# connect->halt->flash->reset
# quiting with q means failure, errorlevel==2
#FIXME but if no q in commandfile, alway errorlevel==0 evenif there are some errors
flashGD_only: flashGD_check
	@$(call prntBl,-------- Flashing started---------------------)
	@echo c $(gd_link_device_id) > $(flash_gd_commandfile)
	@echo h >> $(flash_gd_commandfile)
	@echo load $(flash_gd_file) 0x$(ROM1_PHY_START) >> $(flash_gd_commandfile)
	@echo r >> $(flash_gd_commandfile)
	@$(call prntGr,-------- Created: $(out_path)/$(variant)/flash.gdlink ------------)
	@$(flash_gd_exe) -commandfile $(flash_gd_commandfile)
	@$(call prntGr,-------- Flashing done------------------------)
    
# connect->halt->erase->reset
flashGD_erase: flashGD_check
	@$(call prntBl,-------- Erasing started-------------)
	@echo c $(gd_link_device_id) > $(erase_gd_commandfile)
	@echo h >> $(erase_gd_commandfile)
	@echo erase >> $(erase_gd_commandfile)
	@echo r >> $(erase_gd_commandfile)
	@$(call prntGr,-------- Created: $(out_path)/$(variant)/erase.gdlink ------------)
	@$(flash_gd_exe) -commandfile $(erase_gd_commandfile)
	@$(call prntGr,-------- Erasing done----------------)
