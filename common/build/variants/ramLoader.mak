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
#  Description      definition of ramLoader variant
#*******************************************************************************

ramLoader    = true
bootLoader   = true
# set FW-Update type to version 1
ifeq ($(cc_build_path),iarRx)
FW_update_type = FWU1
else
FW_update_type = FWU3
endif
common_components = $(msp) firmware_update ped_fw
ped_fw_subcomponent_list = stdcrc

no_ram_init      = true
no_isr_support   = true
bootloader_merge = false
project_linkerscript = $(msp_path)/cfg_templates/ramLoader

cfg_obj += ped_fw/timer/$(platform)/timerMcXs
ifneq (,$(filter $(platform), ra2 ra4))
    cfg_obj += ped_fw/basic/$(platform)/basicMcXs
endif
