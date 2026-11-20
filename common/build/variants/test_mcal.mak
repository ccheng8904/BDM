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
#  Description      definition of testMCAL variant
#*******************************************************************************

testMCAL = true

common_components = $(msp) ped_fw mipy_interface
ped_fw_subcomponent_list = basic timer utility stdcrc
#system_timer_disable = 1
basicXs = mipy_interface/basicXs
MIPY_UART_CHANNEL?=0
bootloader_merge     = false


#defines += NO_PLATFORM_INIT
