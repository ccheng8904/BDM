#*******************************************************************************
#  Copyright (c) 2017 BSH Hausgeraete GmbH,
#  Carl-Wery-Str. 34, 81739 Munich, Germany, www.bsh-group.de
#
#  All rights reserved. This program and the accompanying materials
#  are protected by international copyright laws.
#  Please contact copyright holder for licensing information.
#
#*******************************************************************************
#  Project          Generic SW
#*******************************************************************************
#  Description      definition of romLoader variant
#*******************************************************************************

romLoader    = true
bootLoader   = true
# set FW-Update type to version 1
FW_update_type = FWU1
common_components = $(msp) firmware_update
#ped_fw_subcomponent_list = 

no_ram_init = true
no_isr_support   = true
bootloader_merge = false
project_linkerscript = $(msp_path)/cfg_templates/romLoader

cfg_obj += ped_fw/basic/$(platform)/basicMcXs  ped_fw/timer/$(platform)/timerMcXs

ifeq ($(cortexm_core_present),true)
    patchbin_opt_uc += -c(0x$(ROM1_START):B8)(abs=0x$(BL_APPLICATION_ROM_START))
else ifeq ($(cc_build_path),iarRx)
    #patchbin_opt_uc += -c(0x$(RESET_VECTOR):$(VECTOR_SIZE))(abs=0x$(BL_APPLICATION_ROM_START))
    #as far as i know for rx has nothing to be patched, because we use SW-Reset (functin pointer to main that is written to ROM)
else
    patchbin_opt_uc += -c(0x$(RESET_VECTOR):$(VECTOR_SIZE))(abs=0x$(BL_APPLICATION_ROM_START))
endif