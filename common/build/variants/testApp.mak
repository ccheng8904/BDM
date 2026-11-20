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
#  Description      definition for test application
#*******************************************************************************

testApp    = true

common_components         = $(msp) dbus ped_fw testApp firmware_update MSP/mcal retarget
HEAPSIZE=100

ped_fw_subcomponent_list  = basic schedule timer utility stdcrc
ModuleHeader              = testApp/ModuleHeader
scheduler_config          = auto
