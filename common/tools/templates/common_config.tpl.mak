#*******************************************************************************
#  Copyright (c) 2017 BSH Hausgeraete GmbH,
#  Carl-Wery-Str. 34, 81739 Munich, Germany, www.bsh-group.de
#
#  All rights reserved. This program and the accompanying materials
#  are protected by international copyright laws.
#  Please contact copyright holder for licensing information.
#
#*******************************************************************************
#  PROJECT          <$PROJECT_NAME>
#*******************************************************************************
#  Description      Common build settings for all variants
#*******************************************************************************
<$INCLUDES>
# build type (DEVELOP / RELEASE)
# For more details refer to file common/build/help/make_variables.md
# or run 'make help' and click on the link 'List of variables' in main help page
build_type    ?= DEVELOP


# list of application variants, first is default
app_variants   = <$VARIANT>


# compiler selection
cc_build_path ?= <$COMPILER>


# microcontroller configuration
platform      := <$PLATFORM>
derivative    := <$DERIVATIVE>
<$CLOCK_CONFIG>


# communication configuration
UDA                  ?= UDA-01
dbus_uart_channel     = <$DBUS_UART_CHANNEL>
dbus_node_address     = <$DBUS_NODE_ADDRESS>
dbus_default_baudrate = <$BAUDRATE>
dbus_tx_pin           = <$DBUS_TX_PIN>
dbus_rx_pin           = <$DBUS_RX_PIN>
TESTMSGSRV_SUBNODE_ADDRESS = 10


# firmware update type (FWU1 / FWU3)
FW_update_type = <$FWUTYPE>

# flex partition table settings (ENABLED / DISABLED)
fwu_flex_partition_used = <$FLEXPART>

# functional safety settings (ENABLED / DISABLED)
feature_functional_safety ?= <$FUNCSAFE>

# veeprom settings (ENABLED / DISABLED)
feature_veeprom ?= DISABLED

# global definitions
defines += <$DEFINITIONS>


# global build settings
<$MACROS>
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
