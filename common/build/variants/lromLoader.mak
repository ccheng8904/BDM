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
#  Description      definition of lromLoader variant
#*******************************************************************************

lromLoader   = true
bootLoader   = true
# set FW-Update type to version 3
FW_update_type=FWU3

common_components = $(msp) firmware_update
#ped_fw_subcomponent_list = 

no_ram_init          = true
no_isr_support       = true
bootloader_merge     = false
ModuleHeader         = firmware_update/BootLoader/ModuleHeader
project_linkerscript = $(msp_path)/cfg_templates/romLoader

crc_calc_start ?= $(FWU_LRO_START_ADDRESS)+$(crc_offset)
crc_store_addr ?= $(FWU_LRO_START_ADDRESS)
crc_calc_end   ?= $(FWU_LRO_END_ADDRESS)


cfg_obj += ped_fw/timer/$(platform)/timerMcXs
ifneq (,$(filter $(platform), ra2 ra4))
    cfg_obj += ped_fw/basic/$(platform)/basicMcXs
endif
