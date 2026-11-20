#*******************************************************************************
#  Copyright (c) 2021 BSH Hausgeraete GmbH,
#  Carl-Wery-Str. 34, 81739 Munich, Germany, www.bsh-group.de
#
#  All rights reserved. This program and the accompanying materials
#  are protected by international copyright laws.
#  Please contact copyright holder for licensing information.
#
#*******************************************************************************
#  PROJECT          Generic SW
#*******************************************************************************
#  Description      Memory map generation makefile
#*******************************************************************************

# This section is used for fwu_mem_map.mak generation
# Definitions of sizes and start/end address have following priority:
# common_config.mak > derivative.mak > specific_config.mak > fwu_mem_org.mak (default)

ifeq ($(feature_veeprom),ENABLED)
    _FWU_APP_MODULE_LIST = VIRTUAL_EEPROM
endif

define check_dmodule_size
$(eval dmodule_name_upper = FWU_$(call upper,$1))
$(origin $(dmodule_name_upper)_SIZE)
endef

define process_dmodules
$(eval dmodule_name_upper = FWU_$(call upper,$1))
$(eval $(dmodule_name_upper) += name=$(dmodule_name_upper) size=0x$($(dmodule_name_upper)_SIZE))
$(eval _FWU_APP_MODULE_LIST += $(dmodule_name_upper))
endef

ifdef ramLoader
    # No mamory map generation for ramLoader
    
else ifeq ($(FW_update_type),FWU1)

    # following asignment is valid for all platform except RX200
    ifneq ($(platform), rx200)
        FWU_ROMLOADER_START_ADDRESS ?= $(ROM1_PHY_START)
    endif
    FWU_OTP_MEMORY_SIZE         ?= 800
    FWU_ROMLOADER_SIZE          ?= 800
    ifneq ($(platform), rx200)
        FWU_APPLICATION_END_ADDRESS ?= $(ROM1_PHY_END)
        FWU_ROMLOADER           += name=FWU_ROMLOADER size=0x$(FWU_ROMLOADER_SIZE) start_address=0x$(FWU_ROMLOADER_START_ADDRESS)
        FWU_APPLICATION         += name=FWU_APPLICATION end_address=0x$(FWU_APPLICATION_END_ADDRESS)
    else
        # end address of romLoader is needed for RX200 platform instead of start address
        FWU_ROMLOADER           += name=FWU_ROMLOADER end_address=0x$(FWU_ROMLOADER_END_ADDRESS) size=0x$(FWU_ROMLOADER_SIZE)
        FWU_APPLICATION         += name=FWU_APPLICATION start_address=0x$(FWU_APPLICATION_START_ADDRESS) end_address=0x$(FWU_APPLICATION_END_ADDRESS)
    endif
    FWU_OTP_MEMORY          += name=FWU_OTP_MEMORY size=0x$(FWU_OTP_MEMORY_SIZE)

    FWU_COMMON_MODULE_LIST   = FWU_ROMLOADER

    ifeq ($(fwu_production_data),true)
        _FWU_APP_MODULE_LIST    += FWU_OTP_MEMORY
    endif
    _FWU_APP_MODULE_LIST        += FWU_APPLICATION
    
    BL_APPLICATION_ROM_START = $(FWU_APPLICATION_START_ADDRESS)
    defines += BL_APPLICATION_ROM_START=UINT32_C(0x$(FWU_APPLICATION_START_ADDRESS))

    ifneq (,$(strip $(app_dmodules)))
        $(foreach dmodulename,$(app_dmodules),$(eval fwu_dmodules+=$(call upper,$(dmodulename))))
        $(foreach dmodulename,$(app_dmodules),$(if $(findstring undefined,$(call check_dmodule_size,$(dmodulename))),$(error Missing define $(dmodule_name_upper)_SIZE, please add it in common_config.mak)))
        $(foreach dmodulename,$(app_dmodules),$(eval $(call process_dmodules,$(dmodulename))))
    endif
    
else ifeq ($(FW_update_type),FWU3)

    # following asignment is valid for all platform except RX200
    ifneq ($(platform), rx200)
        FWU_BOOTMANAGER_START_ADDRESS   ?= $(ROM1_PHY_START)
    endif

    FWU_BOOTMANAGER_SIZE    ?= 1800
    FWU_LRO_SIZE            ?= 800
    FWU_PROGRAMMER_SIZE     ?= 4000
    FWU_OTP_MEMORY_SIZE     ?= 800
    FWU_FLEX_PARTITION_SIZE ?= 800

    # Built in modules in ascending order in memory
    FWU_COMMON_MODULE_LIST = FWU_BOOTMANAGER FWU_LRO FWU_PROGRAMMER FWU_OTP_MEMORY
    ifneq ($(platform), rx200)
        FWU_BOOTMANAGER += name=FWU_BOOTMANAGER start_address=0x$(FWU_BOOTMANAGER_START_ADDRESS) size=0x$(FWU_BOOTMANAGER_SIZE)
        FWU_APP1        += name=FWU_APP1 end_address=0x$(FWU_APP1_END_ADDRESS)
        ifdef FWU_APP1_START_ADDRESS
            FWU_APP1    += start_address=0x$(FWU_APP1_START_ADDRESS)
        endif
    else
        # end address of BootManager is needed for RX200 platform instead of start address
        FWU_BOOTMANAGER += name=FWU_BOOTMANAGER end_address=0x$(FWU_BOOTMANAGER_END_ADDRESS) size=0x$(FWU_BOOTMANAGER_SIZE)
        FWU_APP1        += name=FWU_APP1 start_address=0x$(FWU_APP1_START_ADDRESS)  end_address=0x$(FWU_APP1_END_ADDRESS)
    endif
    FWU_LRO             += name=FWU_LRO size=0x$(FWU_LRO_SIZE)
    FWU_PROGRAMMER      += name=FWU_PROGRAMMER size=0x$(FWU_PROGRAMMER_SIZE)
    FWU_OTP_MEMORY      += name=FWU_OTP_MEMORY size=0x$(FWU_OTP_MEMORY_SIZE)

    ifeq ($(fwu_flex_partition_used),ENABLED)
        _FWU_APP_MODULE_LIST += FWU_FLEX_PARTITION
        FWU_FLEX_PARTITION ?= name=FWU_FLEX_PARTITION size=0x$(FWU_FLEX_PARTITION_SIZE)

        defines += FWU_FLEX_PARTITION_USED
        defines += FWU_FLEX_PARTITION_START=FWU_FLEX_PARTITION_START_ADDRESS
        defines += FWU_FLEX_PARTITION_END=FWU_FLEX_PARTITION_END_ADDRESS
    endif

    # Application must be after all built in modules
    _FWU_APP_MODULE_LIST += FWU_APP1
    
    # Not sure if used anywhere
    ifdef FWU_APP1_START_ADDRESS2
        defines += FWU_APP1_START_ADDRESS2=UINT32_C(0x$(FWU_APP1_START_ADDRESS2))
        defines += FWU_APP1_END_ADDRESS2=UINT32_C(0x$(FWU_APP1_END_ADDRESS2))
    endif
    ifdef FWU_APP2_START_ADDRESS
        defines += FWU_APP2_START_ADDRESS=UINT32_C(0x$(FWU_APP2_START_ADDRESS))
        defines += FWU_APP2_END_ADDRESS=UINT32_C(0x$(FWU_APP2_END_ADDRESS))
    endif
    
    ifneq (,$(strip $(app_dmodules)))
        $(foreach dmodulename,$(app_dmodules),$(eval fwu_dmodules+=$(call upper,$(dmodulename))))
        $(foreach dmodulename,$(app_dmodules),$(if $(findstring undefined,$(call check_dmodule_size,$(dmodulename))),$(error Missing define $(dmodule_name_upper)_SIZE, please add it in common_config.mak)))
        $(foreach dmodulename,$(app_dmodules),$(eval $(call process_dmodules,$(dmodulename))))
    endif
    ifeq (,$(filter  drives_control, $(ext_components)))
        ecu_id_count ?= 1
        ifeq ($(variant_base), programmer)
            programmer_id_no = 1
            app_id_no = 2
        else
            ifeq ($(app_variant),true)
                app_id_no = 1
                programmer_id_no = $(call calc, $(ecu_id_count)+1)
            endif
        endif
    endif

else ifeq ($(FW_update_type),FWU4)
    
    FWU_GECKO_BL_START_ADDRESS ?= $(ROM1_PHY_START)
    
    FWU_GECKO_BL        += name=FWU_GECKO_BL size=0x$(FWU_GECKO_BL_SIZE) start_address=0x$(FWU_GECKO_BL_START_ADDRESS)
    FWU_APP1            += name=FWU_APP1 start_address=0x$(FWU_APP1_START_ADDRESS) end_address=0x$(FWU_APP1_END_ADDRESS)
    FWU_OTP_MEMORY      += name=FWU_OTP_MEMORY size=0x$(FWU_OTP_MEMORY_SIZE)

    FWU_COMMON_MODULE_LIST  = FWU_GECKO_BL
    
    ifeq ($(fwu_production_data),true)
        FWU_COMMON_MODULE_LIST += FWU_OTP_MEMORY
    endif
    
    # Application must be after all built in modules
    _FWU_APP_MODULE_LIST += FWU_APP1
    
endif

ifneq ($(FW_update_type),FWU0)
  ifndef ramLoader
    FWU_APP_MODULE_LIST ?= $(_FWU_APP_MODULE_LIST)

    FWU_MODULE_LIST = $(FWU_COMMON_MODULE_LIST) $(FWU_APP_MODULE_LIST)

    # Add fwu_mem_map for code generation
    common_export_dictionary += FWU_MODULE_LIST
    common_export_variable += fwu_dmodules
    
    ifeq (,$(filter $(MAKECMDGOALS),$(no_memory_map_gen_targets)))
        code_gen += fwu_mem_map
    endif
    ifneq ($(platform), rx200)
        fwu_mem_map_template = $(template_folder)/fwu_mem_map.tpl.mak
    else
        # upside down calculation is needed for RX200, therefore there is special memory map template
        fwu_mem_map_template = $(template_folder)/fwu_rx_mem_map.tpl.mak
    endif
    fwu_mem_map_output = $(app_path)/build/fwu_mem_map.mak
    fwu_mem_map_opt += -silent

    # Include oputut file containing addresses
    -include $(fwu_mem_map_output)

  endif
endif
