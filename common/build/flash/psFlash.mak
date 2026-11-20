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
#  Description      targets for firmware update
#*******************************************************************************


# Applicaion module IDs and binary files
APP_MODULE_ID   ?= 0x2200
APP_MODULE_ID2  ?= 0x2210
ps_bin_file     ?= $(patchbin_out_bin)
ps_bin_file2    ?= $(patchbin_out_bin_2)

# First flash second fragment of app in case app is fragmented
ifdef FWU_APP1_START_ADDRESS2
    psflash_flash_opt += module=$(APP_MODULE_ID2):$(ps_bin_file2)
endif

psflash_flash_opt += module=$(APP_MODULE_ID):$(ps_bin_file)
psflash_flash_opt += high_baud=$(psflash_high_baud)

# For CI test do not perform reset after flashing
ifeq ($(variant), CiTestVariant)
    psflash_flash_opt += noreset
endif


# Psflash targets
psflash_check:
ifeq ($(wildcard $(psflasher_exe)),)
	$(call tool_error,$(psflasher_exe))
endif

psflash: link psflash_only
psflash_no_link: psflash_only

psflash_only: psflash_check
	@echo Start remote flashing
	@$(psflasher_exe) -s $(psflash_py) -i $(psflash_interface) -p $(psflash_opt) $(psflash_flash_opt)
	@$(call prntGr,-------- Flashing done -------------)


psverify: $(RAMLOADER)
	@echo Start Flash Verify
	@$(flash_exe) -v $(flash_opt)
	@$(call prntGr,-------- Verifying done -------------)
