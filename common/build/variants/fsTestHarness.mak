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
#  Description      definition for test application of FS test harness
#*******************************************************************************

fsTestHarnessApp    = true
is_testharness     := ENABLED
# set FW-Update type to version 3 (BootManager, or moved application)
FW_update_type = FWU3

defines += TH_FSCOM
defines += TENG_INCLUDED
defines += FSF_CRC_32_BIT
RAMCON_TYPE = NONE

common_components        ?= $(msp) dbus ped_fw firmware_update
ped_fw_subcomponent_list ?= basic schedule stdcrc timer utility testengine 
