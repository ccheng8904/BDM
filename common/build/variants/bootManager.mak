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
#  Description      definition of bootManager variant
#*******************************************************************************

bootManager    = true
# set FW-Update type to version 3 (BootManager, or moved application)
FW_update_type = FWU3
common_components = $(msp) ped_fw firmware_update FuncSafe
ped_fw_subcomponent_list = stdcrc

# include components relevant to Functional Safety
ped_fw_subcomponent_list += timer utility
system_timer_disable = 1

# it shall be checked if we need RAM initialization
no_ram_init 	 = true
no_isr_support   = true

cfg_obj += ped_fw/basic/$(platform)/basicMcXs

