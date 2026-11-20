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
#  Description      build include for firmware update
#*******************************************************************************

# The following variants are handled here:
# - bootManager
# - programmer
# - lromLoader (LRO)
# - ramLoader  (BRA)
# - romLoader  (BRO)

# Configuration of FWU opt. bytes
# RELEASE_KEY  - check is enabled, RELEASE key is used for Auth. boot
# DEVELOP_KEY  - check is enabled, DEVELOP key is used for Auth. boot
# ENABLED      - check is enabled
# IGNORE_ERROR - check errors is ignored but calculation is executed
# DISABLE_CALC - check is complete disabled (errors + calculation)
# RESERVED     - reserved byte for future use (+ alignment struct. to 4bytes)

RELEASE_KEY  = 0x5E
DEVELOP_KEY  = 0xDE
ENABLED      = 0xEA
IGNORE_ERROR = 0x1E
DISABLE_CALC = 0xDC
RESERVED     = 0xFF

defines += MAL_RELEASE_KEY_DEF=$(RELEASE_KEY)
defines += MAL_DEVELOP_KEY_DEF=$(DEVELOP_KEY)
defines += MAL_ENABLE_DEF=$(ENABLED)
defines += MAL_IGNORE_ERROR_DEF=$(IGNORE_ERROR)
defines += MAL_DISABLE_CALC_DEF=$(DISABLE_CALC)
defines += MAL_RESERVED_DEF=$(RESERVED)

ifeq ($(variant_comp),programmer)
    obj_test = mem_drv mem_drv_config
    axivion_uti_src = firmware_update/mem_drv_config.c firmware_update/mem_drv/mem_drv.c
    stubtrace_user=firmware_update/mem_drv

    add_stubfunc  = uint32_t:DRVIF_checkErase:uint32_t:uint32_t
    add_stubfunc += uint32_t:DRVIF_erase:uint32_t:uint32_t
    add_stubfunc += void:DRVIF_deinitFlash:void
    add_stubfunc += void:DRVIF_initFlash:void
    add_stubfunc += bool:DRVIF_isAddrRangeValid:uint32_t:uint32_t
    add_stubfunc += bool:DRVIF_isBusy:void
    add_stubfunc += bool:DRVIF_isError:void
    add_stubfunc += void:DRVIF_resetFlash:void
    add_stubfunc += uint32_t:DRVIF_write:uint32_t:uint32_t:uint8_t*
    add_stubfunc += bool:DRVIF_read:uint32_t:uint32_t:uint8_t*

    ifeq ($(unit_test),true)
        search_path += common/prog/firmware_update/mem_drv
        defines += COMA_UNIT
        defines += MAL_ENABLE_INT_FLASH_DRIVER
    endif
endif

ifeq ($(FW_update_type),FWU3)
  ifeq (,$(filter $(variant_base),ramLoader romLoader lromLoader))
    fwu_subcomponent_list ?= mal
  endif
else
    # no mal for xxxLoader
    fwu_subcomponent_list ?=
endif
# include the specified subcomponents
include $(call prepost, $(sdk_prog_path)/firmware_update/,$(fwu_subcomponent_list),/build.mak)


cfg_hdr += firmware_update/mal/module_administration_cfg.h
cfg_files  += firmware_update/mem_drv_config.c

ifeq ($(FW_update_type),FWU3)
    ifeq ($(app_variant),true)
        cfg_files += firmware_update/Programmer/prog_user_task_config.c
        defines +=  MAL_ENABLE_INT_FLASH_DRIVER
    endif
endif

ifeq ($(FW_update_type),FWU4)
    #This is the default start address of slot 0 in external SPI memory
    #The slot is used to hold the image of a update image (GBL-package)
    FWU4_GBL_IMAGE_START_ADDRESS ?= 10008
    cfg_files += firmware_update/sw_version.c
    cfg_files += firmware_update/gecko_bl/application_properites.c
    cfg_files += devices/spi_flash/spi_flash_cfg.c
    ifdef app_variant
        obj += firmware_update/sw_version
        obj += firmware_update/gecko_bl/application_properites
    endif
endif

ifeq ($(dbus_mapping),dbuscan)
    bl_platform = _$(dbus_mapping)
endif

ifeq ($(variant_comp),ramLoader)
    obj_test = BLmessageLoop BLservices BLcom$(bl_platform)
    stub_locals = false
    ifeq ($(unit_test),true)
        search_path += common/prog/firmware_update/BootLoader
        search_path += common/prog/MSP/stm32f0
        search_path += common/prog/ped_fw/stdcrc
        #add_stubfunc += HSUP_vGenerateReset
        defines += PROC_FAMILY=x86 UC_VENDOR=Intel
    endif
endif

ifdef bootManager
    # definitions for bootManager
    firmware_submodules += bm_shared_data mal_module_admin
    obj  += $(addprefix firmware_update/BootManager/,BootManager BootManagerModule BootManagerSharedData $(platform)/BootManagerSharedDataUc)
    obj += firmware_update/otp_mem/fwu_otp_memory
    obj += firmware_update/version/fwu_version

    defines += MAL_ENABLE_INT_FLASH_DRIVER

    bm_external_memory_support     ?= ENABLED
    # external memory simulation used for testing purpose only
    mal_external_memory_simulation ?= DISABLED
    ifeq ($(bm_external_memory_support),ENABLED)
        defines += FWU_MEM_DRV_READ_ONLY
        obj += firmware_update/mem_drv/mem_drv
        cfg_obj += firmware_update/mem_drv_config
        ifeq ($(mal_external_memory_simulation),ENABLED)
            obj += $(msp)/mem_drv/intflash_drv
        endif
    endif


else ifdef romLoader
    # definitions for romLoader
    firmware_submodules += bm_shared_data
    obj  += $(addprefix firmware_update/BootManager/,BootManagerSharedData $(platform)/BootManagerSharedDataUc)
    obj  += $(addprefix firmware_update/BootLoader/,BLmain BLcom$(bl_platform) BLmessageLoop BLservices)

else ifdef gecko_bl
    obj  += $(addprefix firmware_update/BootLoader/,BLmain BLcom$(bl_platform) BLmessageLoop BLservices)
    obj  += firmware_update/BootLoader/BLmal firmware_update/mem_drv/com_mal
    cfg_obj  += firmware_update/mem_drv_config


else ifdef lromLoader
    # definitions for lromLoader
    need_header = true
    obj  += $(addprefix firmware_update/BootManager/,BootManagerSharedData $(platform)/BootManagerSharedDataUc)
    obj  += $(addprefix firmware_update/BootLoader/,BLmain BLcom$(bl_platform) BLmessageLoop BLservices)
    ModuleHeader ?= firmware_update/BootLoader/ModuleHeader

else ifdef ramLoader
    # definitions for ramLoader
    obj  += $(addprefix firmware_update/BootManager/,BootManagerSharedData $(platform)/BootManagerSharedDataUc)
    obj  += $(addprefix firmware_update/BootLoader/,BLmain BLcom$(bl_platform) BLmessageLoop BLservices)
    obj  += firmware_update/BootLoader/BLmal
    cfg_obj  += firmware_update/mem_drv_config
    defines += MAL_ENABLE_INT_FLASH_DRIVER

else ifdef programmer
    # definitions for programmer
    need_header = true
    obj  += $(addprefix firmware_update/BootManager/,BootManagerModule BootManagerSharedData $(platform)/BootManagerSharedDataUc)
    obj  += $(addprefix firmware_update/map/SRC/,common/map_cmd common/map_entry config/ped_framework/map_mem_access_interface_XS)
    obj      += firmware_update/mem_drv/mem_drv
    obj      += firmware_update/mem_drv/intflash_abs
    cfg_obj  += firmware_update/mem_drv_config
    cfg_obj  += firmware_update/Programmer/prog_user_task_config
    obj  += firmware_update/hsi/hsi_basic
    obj  += firmware_update/otp_mem/fwu_otp_memory
    obj  += firmware_update/version/fwu_version
    ModuleHeader ?= firmware_update/Programmer/ModuleHeader

    search_path += common/prog/firmware_update/map/SRC/common
    search_path += common/prog/firmware_update/Programmer
    search_path += common/prog/firmware_update/hsi
    defines += MAL_ENABLE_INT_FLASH_DRIVER

else ifeq ($(FW_update_type),FWU3)
    ifneq ($(unit_test),true)
        # any other moved application
        need_header = true
        obj  += $(addprefix firmware_update/BootManager/,BootManagerModule BootManagerSharedData $(platform)/BootManagerSharedDataUc)
        obj  += firmware_update/otp_mem/fwu_otp_memory
        obj  += firmware_update/version/fwu_version
        # size of Module header for update-able application (with HW ID array)
        ifndef additional_app
            #main application that is started after reset
            #include Flex Table
            defines += need_Flex_PAT
        endif   #main_app
        obj      += firmware_update/mem_drv/intflash_abs
        obj      += firmware_update/mem_drv/mem_drv
        cfg_obj  += firmware_update/mem_drv_config
    endif
else ifndef testClient
    obj  += $(addprefix firmware_update/BootManager/,BootManagerSharedData $(platform)/BootManagerSharedDataUc)
endif

ifeq ($(FW_update_type),FWU1)

    ifeq ($(fwu_production_data),true)
        cfg_obj  += firmware_update/sw_version
    endif

    ifdef app_variant
        cfg_obj  += firmware_update/mal/fwu_opt_bytes_cfg
        obj += firmware_update/mem_drv/intflash_abs
        obj += firmware_update/otp_mem/fwu_otp_memory
        obj += firmware_update/version/fwu_version
    endif
endif

obj  += firmware_update/mem_drv/com_mal

ifdef need_header
    defines += need_ModuleHeader
    ifdef ModuleHeader
        nolib_obj += $(ModuleHeader)
    else
        cfg_obj  += firmware_update/ModuleHeader
    endif
endif
ifeq ($(FW_update_type),FWU4)
    search_path += $(msp_path)/gecko_bl/api
    search_path += $(msp_path)/gecko_bl
    obj  += $(addprefix firmware_update/gecko_bl/,gecko_bl_api)
    ifdef gecko_bl_api_test
        obj  += $(addprefix firmware_update/gecko_bl/,gecko_bl_api_test)
    endif
    ifdef app_variant
        obj += firmware_update/mem_drv/intflash_abs
        obj  += firmware_update/otp_mem/fwu_otp_memory
        obj  += firmware_update/version/fwu_version
    endif
endif


watch_Symbols += BMDAT_sharedData MDHD_ModuleHeader MDHD_ModuleHeader2 MAL_partitionFlex MAL_partitionFlexHeader MAL_DataAnchor MAL_hw_version

# Settings to generate latex/pdf specification for Bootloader

latex  ?= $(tex_path)\latex
dvips  ?= $(tex_path)\dvips
ps2pdf ?= $(tex_path)\ps2pdf

btl_doc_src_path = $(sdk_prog_path)/firmware_update/BootLoader/doc/src

btlspec:
	@echo Generating BootLoader specification
	@cd $(btl_doc_src_path) && $(latex) BL_SPEC.tex
	@cd $(btl_doc_src_path) && $(latex) BL_SPEC.tex
	@cd $(btl_doc_src_path) && $(dvips) BL_SPEC.dvi
	@cd $(btl_doc_src_path) && $(ps2pdf) BL_SPEC.ps
	@cd $(btl_doc_src_path) && copy /y BL_SPEC.pdf ..\BL_SPEC.pdf
	@$(call prntGr,-------- Generation done -------------)
