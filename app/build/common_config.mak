#*******************************************************************************
#  Copyright (c) 2017 BSH Hausgeraete GmbH,
#  Carl-Wery-Str. 34, 81739 Munich, Germany, www.bsh-group.de
#
#  All rights reserved. This program and the accompanying materials
#  are protected by international copyright laws.
#  Please contact copyright holder for licensing information.
#
#*******************************************************************************
#  PROJECT          BDM
#*******************************************************************************
#  Description      Common build settings for all variants
#*******************************************************************************
# GCC ARM compiler
gcc_arm_path        = $(CURDIR)\Tools\8_2019-q3-update
#gcc_arm_path        = C:\Sino_GCC_ARM


# Python version 3
python_path         = $(CURDIR)\Tools\Python35-32

# build type (DEVELOP / RELEASE)
# For more details refer to file common/build/help/make_variables.md
# or run 'make help' and click on the link 'List of variables' in main help page
build_type    ?= DEVELOP


# list of application variants, first is default
app_variants   = BDM


# compiler selection
cc_build_path ?= gccArm


# microcontroller configuration
platform      := cms32m67
derivative    := CMS32M6736EGH48NB
core_clock ?= 72
pclock ?= 72
pclock1 ?= 72


# communication configuration
UDA                  ?= UDA-01
dbus_uart_channel     = 0
dbus_node_address     = 2
dbus_default_baudrate = 96
dbus_tx_pin           = A1
dbus_rx_pin           = A0
TESTMSGSRV_SUBNODE_ADDRESS = 10


# firmware update type (FWU1 / FW
FW_update_type = FWU3

# flex partition table settings (ENABLED / DISABLED)
fwu_flex_partition_used = DISABLED

# functional safety settings (ENABLED / DISABLED)
feature_functional_safety ?= DISABLED

# veeprom settings (ENABLED / DISABLED)
feature_veeprom ?= DISABLED

# global definitions
defines += 

# gcc_arm_path = $(tooldrive)/GCC/$(gcc_arm_version)
# gcc_arm_version = 11.3.1

# python_path = $(ext_tool_path)/$(python_trace)/V$(python_version)_32bit
# python_version = 3.5.2


# global build settings
auto_lint = false

scheduler=schedule

ifeq ($(FW_update_type),FWU4)
    # start of GBL image in external flash
    FWU4_GBL_IMAGE_START_ADDRESS ?= 10008

    # FWU4 Version settings (needed for gecko_bl version)
    uid_gbl              = 12345
    version_major_gbl    = 1
    version_minor_gbl    = 2
    version_revision_gbl = 3
endif

# set if tooldrive information output will be suppressed (true) or not (false)
suppress_tooldrive_info ?= false
ifneq ($(suppress_tooldrive_info),true)
$(info Using external tooldrive $(tooldrive))
endif
