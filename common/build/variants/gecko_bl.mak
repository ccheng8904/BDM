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
#  Description      definition of gecko bootloader variant
#*******************************************************************************

gecko_bl    = true
bootLoader  = true
startvector = Reset_Handler


common_components = $(msp) firmware_update ped_fw
ped_fw_subcomponent_list = stdcrc

bootloader_merge = false
project_linkerscript = $(msp_path)/cfg_templates/gecko_bl

cfg_obj += ped_fw/basic/$(platform)/basicMcXs  ped_fw/timer/$(platform)/timerMcXs
