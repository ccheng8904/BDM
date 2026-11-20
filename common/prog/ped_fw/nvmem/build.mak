#*******************************************************************************
#  Copyright (c) 2017 BSH Hausgeraete GmbH,
#  Carl-Wery-Str. 34, 81739 Munich, Germany, www.bsh-group.de
#
#  All rights reserved. This program and the accompanying materials
#  are protected by international copyright laws.
#  Please contact copyright holder for licensing information.
#
#*******************************************************************************
#  PROJECT          PP PED FW
#*******************************************************************************
#  Description      build include PED Framework, subcomponent: nvmem
#*******************************************************************************

#protection guard preventing multiple inclusion
ifndef nvmem_build_mak
nvmem_build_mak := 1

# possible variants are nvmem, nvmemext, nvmemextlarge, nvmemveep, nvmemveeplarge
nvmem_variant ?= nvmemveep


# is_nvmem_supported_platform contains platform name if platform is supported
nvmem_supported_platforms := stm32f0 stm32f1 stm32f3 stm32f4 stm32l4 stm32g0 stm32g4 mkv5x efr32xg21 ra4 ra2 stm32h7 gd32f33 sh30f mspm0 gd32e50 gd32f30
is_nvmem_supported_platform = $(filter %$(platform),$(nvmem_supported_platforms))


ifeq ($(marked_comp),ped_fw/nvmem)
    obj_test = nvmem cache cacheXS NvmemXs
    defines += CACHE_INCLUDE_EEPROM
    defines += CACHE_INCLUDE_VEEPROM
    ifeq ($(unit_test),true)
        search_path += $(sdk_prog_path)/ped_fw/flash/cfg_templates/stm32f0
        search_path += $(sdk_prog_path)/ped_fw/veeprom/cfg_templates/stm32f0
        search_path += $(sdk_prog_path)/ped_fw/veeprom/cfg_templates/
        search_path += $(sdk_prog_path)/ped_fw/veeprom
    endif
endif


# Test if is_nvmem_supported_platform is not empty --> platform is supported by nvmem
ifeq ($(std_variant),true)
ifneq ($(strip $(is_nvmem_supported_platform)),)
    obj     += $(addprefix ped_fw/nvmem/,nvmem cache)
    cfg_obj += $(addprefix ped_fw/nvmem/,cacheXS NvmemXs)
    cfg_hdr += $(addprefix ped_fw/nvmem/,cacheXS.h  NvmemXs.h TestModuleXs.h)
    
    doxy_image_path_ += ped_fw/nvmem/doc
    
    ifeq ($(nvmem_variant), nvmem)
        defines += CACHE_INCLUDE_EEPROM
        defines += CACHE_INCLUDE_VEEPROM
        nvmem_dep_component := veeprom exteeprom
        
    else ifeq ($(nvmem_variant), nvmemext)
        defines += CACHE_INCLUDE_EEPROM
        nvmem_dep_component := exteeprom
        
    else ifeq ($(nvmem_variant), nvmemextlarge)
        defines += CACHE_INCLUDE_EEPROM
        defines += EXTERNAL_LARGE
        nvmem_dep_component := exteeprom
        
    else ifeq ($(nvmem_variant), nvmemveep)
        defines += CACHE_INCLUDE_VEEPROM
        nvmem_dep_component := veeprom
        
    else ifeq ($(nvmem_variant), nvmemveeplarge)
        defines += CACHE_INCLUDE_VEEPROM
        defines += VEEPROM_LARGE
        nvmem_dep_component := veeprom
        
    else
        $(error Invalid nvmem_variant=$(nvmem_variant))
    endif
    
    # to enable debug of nvmem, define nvmem_debug=true
    ifeq ($(nvmem_debug),true)
        debug    = true
        defines += ASSERT_INCLUDED
        # Include 16-Bit Realtime Measurement ( comment line out if no)
        defines += TM_RTIME16_INCLUDED
        # Include Test Points ? ( comment line out if no)
        defines += TM_TESTPOINTS_INCLUDED
        # Include Trigger Settings support ( comment line out if no)
        defines += TM_TRIGGER_INCLUDED
        cfg_obj += $(addprefix ped_fw/nvmem/,AssertXs)
        cfg_hdr += $(addprefix ped_fw/nvmem/,AssertXs.h)
        obj_test += AssertXs
    endif
    
    ifndef unit_test
        # include dependency components
        include $(call prepost, $(sdk_prog_path)/ped_fw/,$(nvmem_dep_component),/build.mak)
    endif
    
else
    $(error NVMEM component is not supported for platform $(platform))
endif
endif

#end of protection guard preventing multiple inclusion
endif
