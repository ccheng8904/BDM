#*******************************************************************************
#  Copyright (c) 2017 BSH Hausgeraete GmbH,
#  Carl-Wery-Str. 34, 81739 Munich, Germany, www.bsh-group.de
#
#  All rights reserved. This program and the accompanying materials
#  are protected by international copyright laws.
#  Please contact copyright holder for licensing information.
#
#*******************************************************************************
#  Project          PED FW
#*******************************************************************************
#  Description      targets for firmware update
#*******************************************************************************

# command line version of PSFlasher
psflasher_exe  ?= $(psflasher_path)/PSFlasher/PSFlasherCmd.exe

ident_py         = $(subst /,\,$(local_tool_path)/psflasher/identqry.py)
devinfo_py       = $(subst /,\,$(local_tool_path)/psflasher/devinfo.py)
prod_msg_test_py = $(subst /,\,$(local_tool_path)/psflasher/prod_msg_test.py)
dbuscan_reset_py = $(subst /,\,$(local_tool_path)/psflasher/dbuscan_reset.py)

dbus_node_address    ?= 1
dbus_subnode_address ?= 0


# UDA setting:
# UDA_NAME      - Name of your UDA box or IP address
# UDA_TYPE      - UDA1/UDA2 (for UDA2.1, use UDA2)
# UDA_INTERFACE - USB/ETH (ETH only in case of UDA2)
# UDA_PASS      - Password in case of ETH interface
# UDA_BUS_TYPE  - Communication protocol
UDA_NAME        ?= $(UDA)
UDA_TYPE        ?= UDA1
UDA_INTERFACE   ?= USB
UDA_PASS        ?= bsh
UDA_BUS_TYPE    ?= DBUS2


# Baudrate setting
psflash_std_baud    ?= $(strip $(dbus_default_baudrate))00
psflash_high_baud   ?= 125000


# Generic PSflash configuration
ifeq ($(UDA_TYPE), UDA1)
    uda_version ?= 1
    psflash_interface  = "$(UDA_TYPE)|$(UDA_BUS_TYPE)" $(UDA_NAME)
else ifeq ($(UDA_TYPE), UDA2)
    uda_version ?= 2
    psflash_interface  = "$(UDA_TYPE)_$(UDA_INTERFACE)|$(UDA_BUS_TYPE)" $(UDA_NAME)
  ifeq ($(UDA_INTERFACE), ETH)
    psflash_interface += $(UDA_PASS)
  endif
else
    $(error Unknown UDA type. Supported options are UDA1 or UDA2.)
endif

psflash_opt  = node=0x$(strip $(dbus_node_address))$(strip $(dbus_subnode_address))
psflash_opt += std_baud=$(psflash_std_baud)

no_dep_targets += identqry devinfo testprodmsg reset_dbuscan

# Identqry specific options
ident_opt = timeout=10
ifeq ($(FW_update_type),FWU3)
    ident_opt += programmer
endif
ifeq ($(dbus_mapping),dbuscan)
    ident_opt += dbuscan
endif

identqry: psflash_check
	$(psflasher_exe) -s $(ident_py) -i $(psflash_interface) -p $(psflash_opt) $(ident_opt)


ifeq ($(FW_update_type),FWU3)
devinfo:
	@$(psflasher_exe) -s $(devinfo_py) -i $(psflash_interface) -p $(psflash_opt)
endif

# production message tests options
timeout = 10
max_silent_mode_time=0.1
production_data_json_file ?= $(subst /,\,$(local_tool_path)/psflasher/production_data.json)
appliance_data_json_file ?= $(subst /,\,$(local_tool_path)/psflasher/appliance_data.json)
ifeq ($(FW_update_type),FWU3)
    prod_msg_test_opt += programmer
    prod_msg_test_opt += pdjf=$(production_data_json_file)
endif
ifeq ($(FW_update_type),FWU1)
    ifeq ($(fwu_production_data), true)
        prod_msg_test_opt += proddata
        prod_msg_test_opt += pdjf=$(production_data_json_file)
        ifeq ($(platform), efr32xg21)
            ifeq ($(fwu_appliance_data), true)
                prod_msg_test_opt += appldata
                prod_msg_test_opt += adjf=$(appliance_data_json_file)
            endif
        endif
    endif
endif
prod_msg_test_opt += timeout=$(timeout)
prod_msg_test_opt += max_silent_mode_time=$(max_silent_mode_time)

# providing OTP area start address and sizes of its items to test script for using it in ReadRequest32 messages
ifdef FWU_OTP_MEMORY_START_ADDRESS
    # Definitions of production data item's sizes (in Hex) for target testprodmsg only
    # date+time > 6+4=10
    prod_time_size=A
    # mat.number+suplierID+TRID > 10+10+9=29
    prod_trid_size=1D
    # crc+ID+major+minor+rev.+buildID > 4+8+2+2+2+4=22
    prod_hwid_size=16
    # FWID table contains 4 bytes of CRC + 10 x 10 bytes of FWIDs > 104
    prod_fwid_table_size=68
    # TODO - define target to check if these sizes of production time, TRID, HWID, FWID are matched with sizes from syc file

    prod_msg_test_opt += fwu_otp_start_addr=0x$(FWU_OTP_MEMORY_START_ADDRESS)
    prod_msg_test_opt += prod_time_size=0x$(prod_time_size)
    prod_msg_test_opt += prod_trid_size=0x$(prod_trid_size)
    prod_msg_test_opt += prod_hwid_size=0x$(prod_hwid_size)
    ifeq ($(FW_update_type),FWU3)
        prod_msg_test_opt += prod_fwid_table_size=0x$(prod_fwid_table_size)
    endif
    prod_msg_test_opt += platform=$(platform)
    #provide of the smallest writting block size to the test script for current platform
    prod_msg_test_opt += write_block_size=0x$(write_block_size)
endif

testprodmsg: psflash_check
	@$(psflasher_exe) -s $(prod_msg_test_py) -i $(psflash_interface) -p $(psflash_opt) $(prod_msg_test_opt)


reset_dbuscan_opt += uda_version=$(uda_version)
reset_dbuscan_opt += uda_name=$(UDA_NAME)

reset_dbuscan: psflash_check
	@$(psflasher_exe) -s $(dbuscan_reset_py) -p $(psflash_opt) $(reset_dbuscan_opt)
