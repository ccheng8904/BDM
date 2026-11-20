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
#  Description      targets for SiLabs commander flash-tool (custom jlink)
#*******************************************************************************

# command line version of Silabs Commander
flash_commander_exe  ?= $(sl_commander_path)/commander.exe

flash_commander_file ?= $(patchbin_out_hex)

flash_jflash_log  ?= $(out_path)/$(variant)/commander_log.txt

ifdef sl_cmd_sn
sl_cmd_serial_no = --serialno $(sl_cmd_sn)
endif

sl_cmd_device ?= $(derivative)
sl_cmd_device = --device $(sl_cmd_device)

sl_flash: link sl_flash_only
sl_flash_no_link: sl_flash_only

sl_flash_only:
	@echo Start Flashing
	@$(flash_commander_exe) flash $(flash_commander_file) $(sl_cmd_serial_no) $(sl_cmd_device)
	@$(call prntGr,-------- Flashing done -------------)
