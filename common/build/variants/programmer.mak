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
#  Description      definition of programmer variant
#*******************************************************************************

#set stack_monitor_programmer to ENABLED to use stack monitor in programmer
stack_monitor_programmer ?= DISABLED
programmer    = true

# set FW-Update type to version 3
FW_update_type = FWU3

# Clock configuration structure must be set because
# this structure is used for MUART baud rate calculation
clock_config_platforms = ra4 ra2 mspm0
# VLCA: added temporary solution for RA2 & RA4.
# Clock configuration is needed in Programmer built-in variant,
# otherwise, the DBus is not working. 
ifneq ($(platform),$(filter $(platform),$(clock_config_platforms)))
    defines += NO_PLATFORM_INIT
    common_components = $(msp) ped_fw dbus firmware_update
else
    # TOHR: RA2,RA4 system timer is not disabled, because it is needed by MCAL / STD_Callback
    common_components = $(msp) ped_fw dbus firmware_update MSP/mcal 
endif

ifeq ($(stack_monitor_programmer),ENABLED)
    common_components += stack_monitor
endif

ped_fw_subcomponent_list = basic timer utility stdcrc watchdogtimer

basicXs = firmware_update/Programmer/basicXs
ifeq ($(cortexm_core_present),true)
    # supported for keil and gcc only
    timerXs = firmware_update/Programmer/timerXS
endif


bootloader_merge     = false
#project_linkerscript = $(msp_path)/cfg_templates/programmer

dbus_receive_buffer_size  ?= 201
dbus_transmit_buffer_size ?= 131

defines +=  MAL_INTFL_DRIVER_TIMEOUT=6U
defines += MAL_RAM_DRIVER_WRITABLE_SIZE=0x01U
defines += MAL_RAM_DRIVER_ERASABLE_SIZE=0x01U


crc_calc_start ?= $(FWU_PROGRAMMER_START_ADDRESS)+$(crc_offset)
crc_store_addr ?= $(FWU_PROGRAMMER_START_ADDRESS)
crc_calc_end   ?= $(FWU_PROGRAMMER_END_ADDRESS)


#***** VERSIONING *************************************
MAJOR_VERSION = 3
MINOR_VERSION = 3
PATCH_VERSION = 0

defines += MAJOR_VERSION=UINT16_C($(MAJOR_VERSION))
defines += MINOR_VERSION=UINT16_C($(MINOR_VERSION))
defines += PATCH_VERSION=UINT16_C($(PATCH_VERSION))
