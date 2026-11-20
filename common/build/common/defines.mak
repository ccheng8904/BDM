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
#  Description      generic definitions
#*******************************************************************************

# Common memory definitions
ifdef RAM1_PHY_START
    defines += RAM1_PHY_START=UINT32_C(0x$(RAM1_PHY_START)) RAM1_PHY_END=UINT32_C(0x$(RAM1_PHY_END))
    defines += RAM1_PHY_SIZE=UINT32_C(0x$(RAM1_PHY_SIZE))
endif
ifdef RAM2_PHY_START
    defines += RAM2_PHY_START=UINT32_C(0x$(RAM2_PHY_START)) RAM2_PHY_END=UINT32_C(0x$(RAM2_PHY_END))
    defines += RAM2_PHY_SIZE=UINT32_C(0x$(RAM2_PHY_SIZE))
endif
ifdef RAM3_PHY_START
    defines += RAM3_PHY_START=UINT32_C(0x$(RAM3_PHY_START)) RAM3_PHY_END=UINT32_C(0x$(RAM3_PHY_END))
    defines += RAM3_PHY_SIZE=UINT32_C(0x$(RAM3_PHY_SIZE))
endif
ifdef RAM4_PHY_START
    defines += RAM4_PHY_START=UINT32_C(0x$(RAM4_PHY_START)) RAM4_PHY_END=UINT32_C(0x$(RAM4_PHY_END))
    defines += RAM4_PHY_SIZE=UINT32_C(0x$(RAM4_PHY_SIZE))
endif
ifdef RAM5_PHY_START
    defines += RAM5_PHY_START=UINT32_C(0x$(RAM5_PHY_START)) RAM5_PHY_END=UINT32_C(0x$(RAM5_PHY_END))
    defines += RAM5_PHY_SIZE=UINT32_C(0x$(RAM5_PHY_SIZE))
endif
ifdef RAM6_PHY_START
    defines += RAM6_PHY_START=UINT32_C(0x$(RAM6_PHY_START)) RAM6_PHY_END=UINT32_C(0x$(RAM6_PHY_END))
    defines += RAM6_PHY_SIZE=UINT32_C(0x$(RAM6_PHY_SIZE))
endif
ifdef RAM7_PHY_START
    defines += RAM7_PHY_START=UINT32_C(0x$(RAM7_PHY_START)) RAM7_PHY_END=UINT32_C(0x$(RAM7_PHY_END))
    defines += RAM7_PHY_SIZE=UINT32_C(0x$(RAM7_PHY_SIZE))
endif

ifdef ROM1_PHY_START
    defines += ROM1_PHY_START=UINT32_C(0x$(ROM1_PHY_START)) ROM1_PHY_END=UINT32_C(0x$(ROM1_PHY_END))
endif
ifdef ROM2_PHY_START
    defines += ROM2_PHY_START=UINT32_C(0x$(ROM2_PHY_START)) ROM2_PHY_END=UINT32_C(0x$(ROM2_PHY_END))
endif
ifdef ROM3_PHY_START
    defines += ROM3_PHY_START=UINT32_C(0x$(ROM3_PHY_START)) ROM3_PHY_END=UINT32_C(0x$(ROM3_PHY_END)) ROM3_PHY_SIZE=UINT32_C(0x$(ROM3_PHY_SIZE))
endif
ifdef ROM4_PHY_START
    defines += ROM4_PHY_START=UINT32_C(0x$(ROM4_PHY_START)) ROM4_PHY_END=UINT32_C(0x$(ROM4_PHY_END))
endif
ifdef ROM5_PHY_START
    defines += ROM5_PHY_START=UINT32_C(0x$(ROM5_PHY_START)) ROM5_PHY_END=UINT32_C(0x$(ROM5_PHY_END))
endif
ifdef OPTION_BYTES_START
    defines += OPTION_BYTES_START=UINT32_C(0x$(OPTION_BYTES_START))
    defines += OPTION_BYTES_END=UINT32_C(0x$(OPTION_BYTES_END))
    defines += OPTION_BYTES_SIZE=UINT32_C(0x$(OPTION_BYTES_SIZE))
endif

ifdef ROM1_START
    ROM1_SIZE := $(call calcx,0x$(ROM1_END)-0x$(ROM1_START)+1)
    defines += ROM1_START=UINT32_C(0x$(ROM1_START))
    defines += ROM1_END=UINT32_C(0x$(ROM1_END))
    defines += ROM1_SIZE=UINT32_C(0x$(ROM1_SIZE))
endif
ifdef ROM2_START
    ROM2_SIZE := $(call calcx,0x$(ROM2_END)-0x$(ROM2_START)+1)
    defines += ROM2_START=UINT32_C(0x$(ROM2_START))
    defines += ROM2_END=UINT32_C(0x$(ROM2_END))
    defines += ROM2_SIZE=UINT32_C(0x$(ROM2_SIZE))
endif
ifdef ROM3_START
    ROM3_SIZE := $(call calcx,0x$(ROM3_END)-0x$(ROM3_START)+1)
    defines += ROM3_START=UINT32_C(0x$(ROM3_START))
    defines += ROM3_END=UINT32_C(0x$(ROM3_END))
    defines += ROM3_SIZE=UINT32_C(0x$(ROM3_SIZE))
endif
ifdef RAM1_START
    RAM1_SIZE := $(call calcx,0x$(RAM1_END)-0x$(RAM1_START)+1)
    defines += RAM1_START=UINT32_C(0x$(RAM1_START))
    defines += RAM1_END=UINT32_C(0x$(RAM1_END))
    defines += RAM1_SIZE=UINT32_C(0x$(RAM1_SIZE))
endif
ifdef RAM2_START
    RAM2_SIZE := $(call calcx,0x$(RAM2_END)-0x$(RAM2_START)+1)
    defines += RAM2_START=UINT32_C(0x$(RAM2_START))
    defines += RAM2_END=UINT32_C(0x$(RAM2_END))
    defines += RAM2_SIZE=UINT32_C(0x$(RAM2_SIZE))
endif
ifdef RAM3_START
    RAM3_SIZE := $(call calcx,0x$(RAM3_END)-0x$(RAM3_START)+1)
    defines += RAM3_START=UINT32_C(0x$(RAM3_START))
    defines += RAM3_END=UINT32_C(0x$(RAM3_END))
    defines += RAM3_SIZE=UINT32_C(0x$(RAM3_SIZE))
endif
ifdef RAM4_START
    RAM4_SIZE := $(call calcx,0x$(RAM4_END)-0x$(RAM4_START)+1)
    defines += RAM4_START=UINT32_C(0x$(RAM4_START))
    defines += RAM4_END=UINT32_C(0x$(RAM4_END))
    defines += RAM4_SIZE=UINT32_C(0x$(RAM4_SIZE))
endif
ifdef RAM5_START
    RAM5_SIZE := $(call calcx,0x$(RAM5_END)-0x$(RAM5_START)+1)
    defines += RAM5_START=UINT32_C(0x$(RAM5_START))
    defines += RAM5_END=UINT32_C(0x$(RAM5_END))
    defines += RAM5_SIZE=UINT32_C(0x$(RAM5_SIZE))
endif
ifdef RAM6_START
    RAM6_SIZE := $(call calcx,0x$(RAM6_END)-0x$(RAM6_START)+1)
    defines += RAM6_START=UINT32_C(0x$(RAM6_START))
    defines += RAM6_END=UINT32_C(0x$(RAM6_END))
    defines += RAM6_SIZE=UINT32_C(0x$(RAM6_SIZE))
endif
ifdef RAM7_START
    RAM7_SIZE := $(call calcx,0x$(RAM7_END)-0x$(RAM7_START)+1)
    defines += RAM7_START=UINT32_C(0x$(RAM7_START))
    defines += RAM7_END=UINT32_C(0x$(RAM7_END))
    defines += RAM7_SIZE=UINT32_C(0x$(RAM7_SIZE))
endif

ifdef PAGE_SIZE
    defines += PAGE_SIZE=UINT32_C(0x$(PAGE_SIZE))
endif

# STACK and HEAP sizes
defines += HEAPSIZE=UINT32_C(0x$(HEAPSIZE))
defines += STACKSIZE=UINT32_C(0x$(STACKSIZE))

ifeq ($(FW_update_type),FWU4)
    FWU_SHARED_DATA_SIZE ?= 4
    defines += FWU4_GBL_IMAGE_START_ADDRESS=UINT32_C(0x$(FWU4_GBL_IMAGE_START_ADDRESS))
else ifeq ($(platform),psoc4)
    # shared data and vectors segment size are combined for PSoC4
    FWU_SHARED_DATA_SIZE ?= 100
else
    FWU_SHARED_DATA_SIZE ?= 40
endif
defines += FWU_SHARED_DATA_SIZE=UINT32_C(0x$(FWU_SHARED_DATA_SIZE))

# define uninitialized data section (end must be aligned to 0x40)
UNINIT_DATA_SIZE ?= 40
defines += UNINIT_DATA_SIZE=UINT32_C(0x$(UNINIT_DATA_SIZE))

# define excluded data section from Ram Continuous (RunTime) check (end must be aligned to 0x40)
# default is 0 when FS is disabled. It's overloaded in build.mak of FS 
RAMCONT_EXCLUDED_DATA_SIZE ?= 0
defines += RAMCONT_EXCLUDED_DATA_SIZE=UINT32_C(0x$(strip $(RAMCONT_EXCLUDED_DATA_SIZE)))

# Clock configuration definitions
ifdef core_clock
    defines += CORE_CLOCK=$(strip $(core_clock))U CORE_CLOCK_$(strip $(core_clock))M
endif
ifdef core_clock_khz
    #needed for fractal core clock 38.4M is 38400UL
    #note: maximum allowed frequency specified in KHZ is 500MHz
    defines += CORE_CLOCK=$(strip $(core_clock_khz))UL
    defines += CLOCK_IN_KHZ
endif

ifdef pclock
    defines       += PCLOCK=$(strip $(pclock))U
endif
ifdef pclock1
    defines       += PCLOCK1=$(strip $(pclock1))U
endif
ifdef pclock2
    defines       += PCLOCK2=$(strip $(pclock2))U
endif
ifdef pclock3
    defines       += PCLOCK3=$(strip $(pclock3))U
endif
ifdef pclock4
    defines       += PCLOCK4=$(strip $(pclock4))U
endif
ifdef clockADC
    defines       += CLOCK_ADC=$(strip $(clockADC))U
endif
ifdef ext_osc
    defines       += EXT_CRYSTAL_OSC=$(strip $(ext_osc))U
endif
# RA4 specific
ifdef pclockA
    defines       += PCLOCKA=$(strip $(pclockA))U
endif
ifdef pclockB
    defines       += PCLOCKB=$(strip $(pclockB))U
endif
ifdef pclockC
    defines       += PCLOCKC=$(strip $(pclockC))U
endif
ifdef pclockD
    defines       += PCLOCKD=$(strip $(pclockD))U
endif
ifdef fclock
    defines       += FCLOCK=$(strip $(fclock))U
endif
# MSPM0G specific
ifdef f_vco
    defines       += F_VCO=$(strip $(f_vco))U
endif
ifdef pdiv
    defines       += PDIV=$(strip $(pdiv))
endif


# VTOR alignment
ifdef VTOR_ALIGN
    defines += VTOR_ALIGN=UINT32_C($(VTOR_ALIGN))
endif
ifdef VECTOR_TABLE_SIZE
    defines += VECTOR_TABLE_SIZE=UINT32_C(0x$(VECTOR_TABLE_SIZE))
endif
ifdef FLASH_DRIVER_SIZE
    defines += FLASH_DRIVER_SIZE=UINT32_C(0x$(FLASH_DRIVER_SIZE))
endif


# In case of release build, disable asserts
ifeq ($(strip $(build_type)),RELEASE)
    defines += NDEBUG
    debug = false
endif


# Debug definitions
ifeq ($(debug),true)
    defines += DEBUG=0x01U
else
    defines += DEBUG=0x00U
endif


ifeq ($(no_isr_support), true)
    defines += NO_ISR_SUPPORT
endif

ifeq ($(no_ram_init), true)
    defines += NO_RAM_INIT
endif

ifneq ($(variant_base), ramLoader)
    ifneq ($(FW_update_type),FWU4)
    ifneq ($(FW_update_type),FWU0)
        defines+= need_fixed_vectors
    endif
    endif
endif

ifeq ($(need_vectors_in_ram), true)
    defines+= need_vectors_in_ram
endif

# Default baud rate and UART channel definitions for DBus and Bootloader
ifneq (dbuscan,$(dbus_mapping))
    ifdef bootLoader
        defines += BP2_UART_CHANNEL=$(strip $(dbus_uart_channel))
        defines += BP2_MUART_CHANNEL=MUART$(dbus_uart_channel)_RX_P$(dbus_rx_pin)_TX_P$(dbus_tx_pin)
    endif

    ifneq (,$(dbus_rx_pin))
        defines += HUART$(dbus_uart_channel)_RX_P$(dbus_rx_pin)
        defines += HUART$(dbus_uart_channel)_TX_P$(dbus_tx_pin)
        defines += HUART$(dbus_uart_channel)_RX_PIN=$(dbus_rx_pin)
        defines += HUART$(dbus_uart_channel)_TX_PIN=$(dbus_tx_pin)

    ifeq ($(dbus_mapping),mcal)
            defines += DBUS_MUART_PORT=MUART$(call calc,$(dbus_uart_channel))_RX_P$(dbus_rx_pin)_TX_P$(dbus_tx_pin)
            defines += MUART$(dbus_uart_channel)_USED
            defines += DBM_BREAK_MDIO_CHANNEL=MDIO$(dbus_break_pin)
    ifeq ($(platform), mspm0)
            defines += DBM_BREAK_MDIO_CHANNEL_PF=MDIO$(dbus_break_pin)_UART$(dbus_uart_channel)_TX_OUT
    endif
            defines += DBM_IDLE_MDIO_CHANNEL=MDIO$(dbus_rx_pin)
            defines += DBM_IDLE_MEXTI_CHANNEL=MEXTI$(dbus_rx_pin)
        endif
    endif

    defines += HUART$(dbus_uart_channel)_USED
endif

defines += DBUS_DEFAULT_BAUDRATE=$(strip $(dbus_default_baudrate))U

# Definition needed for flash and veeprom
defines += $(subst -,_,$(derivative))


# Command line definitions
defines_cl += VARIANT=$(variant) VARIANT_$(call upper,$(variant_base))
defines_cl += $(call upper,$(cc_build_path))

work_with_variant_include ?= true
ifneq ($(work_with_variant_include), true)
    defines_cl   += $(defines)
endif

# Definition is needed for logging information about interrupt exception type
ifeq ($(logging_interrupt_exception), true)
    defines += LOG_EXCEPTION_ENABLED
endif

# Other tool definitions
defines_tool += $(defines_cl) __NOT_CC__  __NOT_TARGET_CC__ UTEST_TYPE_SYSTEM_$(UTEST_TYPE_SYSTEM)
defines_gcc  += $(defines_cl) __NOT_CC__  __NOT_TARGET_CC__

defines+= MIN_WRITEBLOCK_SIZE=UINT32_C(0x$(write_block_size))

# Firmware update type definition
ifeq ($(FW_update_type),FWU4)
    defines += BTM_FIRMWARE_UPDATE
else ifeq ($(FW_update_type),FWU3)
    defines += REMOTE_FIRMWARE_UPDATE
else ifeq ($(FW_update_type),FWU0)
    defines += NO_FIRMWARE_UPDATE
else
    ifeq ($(bootloader_merge),true)
        defines += WIRED_FIRMWARE_UPDATE
    endif
endif


# Date definitions
defines_date += DBUS2_NODE_DATE=date:$(call dateString)

# Overall ECU ID count, Application and programmer ID number definitions
ifeq (,$(filter  drives_control, $(ext_components)))
    ifdef ecu_id_count
        defines += ECU_ID_COUNT=$(strip $(ecu_id_count))U
    endif

    ifdef app_id_no
        defines += APP_ID_NUMBER=$(strip $(app_id_no))U
    endif

    ifdef programmer_id_no
        defines += PROGRAMMER_ID_NUMBER=$(strip $(programmer_id_no))U
    endif
endif

ifeq ($(supports_32bit_hwtimer),true)
    defines += PLATFORM_SUPPORTS_32BIT_HWTIMER
endif
