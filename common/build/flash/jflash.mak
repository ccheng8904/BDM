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
#  Description      targets for Segger J-Flash with Segger J-Link
#*******************************************************************************

#|-----------------------------------------------|
#|        Relevant only for JLink target         |
#|-----------------------------------------------|
#|          |          | Flashing  |             |
#| Platform |   BIN    | APP | OB  | OB relocated|
#|-----------------------------------------------|
#| STM32F0  | APP + OB | YES | YES | 0x0600 0000 |
#| STM32F1  | APP + OB | YES | YES | 0x0600 0000 |
#| STM32F3  | APP + OB | YES | YES | 0x0600 0000 |
#| STM32F4  | APP + OB | YES | N/A | ----------- |
#| STM32L4  | APP + OB | YES | N/A | ----------- |
#| STM32G4  | APP + OB | YES | YES | ----------- |
#| STM32G0  | APP + OB | YES | YES | ----------- |
#| MKV5     | APP + OB | YES | YES | ----------- |
#| MKE14Z   | APP + OB | YES | YES | ----------- |
#| PSOC4    |    APP   | YES | N/A | ----------- |
#| EFR32xG21|    APP   | YES | N/A | ----------- |
#| GD32F33  |    APP   | YES | N/A | ----------- |
#| GD32F30  |    APP   | YES | N/A | ----------- |
#| GD32E50  |    APP   | YES | N/A | ----------- |
#|-----------------------------------------------|

# command line version of Segger J-Flash
flash_jflash_exe  ?= $(jflash_path)/JFlash.exe

ifeq ($(build_type), RELEASE)
    ifeq ($(jlink_binary_type),1)
        flash_jlink_file ?= $(firstword $(release_bin_jlink_output_file))
    else ifneq (,$(patchbin_out_hex_rom))
        flash_jlink_file  ?= $(firstword $(release_bin_rom_output_file))
        flash_jflash_file ?= $(firstword $(release_bin_rom_output_file))
    else
        flash_jlink_file ?= $(firstword $(release_bin_output_file))
    endif
else
    ifeq ($(jlink_binary_type),1)
        flash_jlink_file ?= $(patchbin_out_hex_jlink)
    else ifneq (,$(patchbin_out_hex_rom))
        flash_jlink_file  ?= $(patchbin_out_hex_rom)
        flash_jflash_file ?= $(patchbin_out_hex_rom)
    else
        flash_jlink_file  ?= $(patchbin_out_hex)
        flash_jflash_file ?= $(patchbin_out_hex)
    endif
endif


ifdef flash_jflash_set
    flash_jflash_path_set ?= $(CURDIR)/common/build/flash/JLink/$(flash_jflash_set)
else
    flash_jflash_path_set ?= $(CURDIR)/common/build/flash/JLink/nxp_mke14z256xxx7.jflash
endif

flash_jflash_log  ?= $(out_path)/$(variant)/jflash_log.txt
flash_jflash_log2  ?= $(out_path)/$(variant)/jflash_log2.txt

# If more than one J-Link is connected
# flash_jflash_sn   ?= -usb504301435

jlink_speed ?= 4000
jlink_erase_range ?=0x$(ROM1_PHY_START),$(call calcx, 0x$(ROM1_PHY_END)+0x1)
# used by JLink command line interface
flash_jlink_exe  ?= $(jflash_path)/JLink.exe
flash_jlink_path_set ?= $(out_path)/$(variant)/$(variant).jlink
flash_jlink_log   ?= $(out_path)/$(variant)/jlink_$(log_cnt)log.txt
flash_jlink_log2  ?= $(out_path)/$(variant)/jlink_$(log_cnt)log2.txt
jlink_ip ?= 0.0.0.0
jlink_sn ?= 000000000
#temporary target for CI compatibility reasons, will be removed later
flashJLink: JFlash

JFlash: link JFlash_only
JFlash_no_link: JFlash_only

JFlash_only:
	@echo Log1:$(flash_jflash_log)
	@echo Log2:$(flash_jflash_log2)
	@echo Start Flashing file $(flash_jflash_file)
	@$(flash_jflash_exe) -hide -openprj$(flash_jflash_path_set) -open$(flash_jflash_file) -jflashlog$(flash_jflash_log) -jlinklog$(flash_jflash_log2) $(flash_jflash_sn) -auto -disconnect -startapp -exit
	@$(call prntGr,-------- Flashing done -------------)


JLink: link JLink_only
JLink_no_link: JLink_only

JLink_only:
	@echo Creating CommanderScript
	@echo Log1:$(flash_jlink_log)
	@echo Log2:$(flash_jlink_log2)
ifneq ($(jlink_ip),0.0.0.0)
	@echo ip $(jlink_ip) > $(flash_jlink_path_set)
	@echo r>> $(flash_jlink_path_set)
else ifneq ($(jlink_sn),000000000)
	@echo USB $(jlink_sn) > $(flash_jlink_path_set)
	@echo r>> $(flash_jlink_path_set)
else
	@echo r> $(flash_jlink_path_set)
endif
ifeq ($(jlink_flash_exclude_bin), true)
    ifneq (,$(filter $(platform), psoc4))
    # in case of PSoC4, read-modify-write (RMW) operation must be used when flashing binary with excluded areas
	@echo exec SetFlashDLNoRMWThreshold=0x80000>> $(flash_jlink_path_set)
    endif
else
	@echo erase $(jlink_erase_range)>> $(flash_jlink_path_set)
endif
	@echo loadfile $(flash_jlink_file) 0 noreset>> $(flash_jlink_path_set)
	@echo rnh>> $(flash_jlink_path_set)
	@echo r0>> $(flash_jlink_path_set)
	@echo Sleep 10>> $(flash_jlink_path_set)
	@echo r1>> $(flash_jlink_path_set)
	@echo qc>> $(flash_jlink_path_set)
	@echo Start Flashing file $(flash_jlink_file)
ifeq ($(jlink_binary_type),2)
	@$(call prntRd,Hint: JLink cannot flash option bytes for this platform)
else ifneq (,$(filter $(platform), psoc4 ))
	@$(call prntRd,Hint: JLink cannot flash option bytes for this platform)
endif
	@$(flash_jlink_exe) -Device "$(platform_type_segger)" -Speed $(jlink_speed) -If SWD -NoGui 1 -ExitOnError -autoConnect -CommandFile $(flash_jlink_path_set) -Log $(flash_jlink_log) >> $(flash_jlink_log2)
	@$(call prntGr,-------- Flashing done -------------)
