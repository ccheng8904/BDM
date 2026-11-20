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
#  Description      build include PED Framework, subcomponent: veeprom
#*******************************************************************************

#protection guard preventing multiple inclusion
ifndef veeprom_build_mak
veeprom_build_mak := 1


# possible variants are veeprom and veepromlarge
veeprom_variant ?= veeprom


# is_veeprom_supported_platform contains platform name if platform is supported
veeprom_supported_platforms := stm32f0 stm32f1 stm32f3 stm32f4 stm32l4 stm32g0 stm32g4 mkv5x ra2 sh30f gd32f33 gd32e50 gd32f30 mspm0
is_veeprom_supported_platform = $(filter %$(platform),$(veeprom_supported_platforms))


ifeq ($(marked_comp),ped_fw/veeprom)
    obj_test = veeprom crc VeepromXs
    stub_trace = ped_fw/$(variant_subcomp)/
endif


ifeq ($(std_variant),true)
    
# Test if is_veeprom_supported_platform is not empty --> platform is supported by veeprom
ifneq ($(strip $(is_veeprom_supported_platform)),)
    ifeq ($(findstring STM32F401,$(derivative)),STM32F401)
        obj += $(addprefix ped_fw/veeprom/stm32f401/,veepromMc )
    else ifeq ($(findstring STM32F0,$(derivative)),STM32F0)
        obj += $(addprefix ped_fw/veeprom/stm32f0/,veepromMc )
    else ifeq ($(findstring STM32G4,$(derivative)),STM32G4)
        obj += $(addprefix ped_fw/veeprom/cellsize_8/,veepromMc )
    else ifeq ($(findstring STM32G0,$(derivative)),STM32G0)
        obj += $(addprefix ped_fw/veeprom/cellsize_8/,veepromMc )
    else ifeq ($(findstring STM32L4,$(derivative)),STM32L4)
        obj += $(addprefix ped_fw/veeprom/cellsize_8/,veepromMc )
    else ifeq ($(findstring MKV5,$(derivative)),MKV5)
        obj += $(addprefix ped_fw/veeprom/cellsize_8/,veepromMc )
    else ifeq ($(findstring GD32F303,$(derivative)),GD32F303)
        obj += $(addprefix ped_fw/veeprom/stm32f401/,veepromMc )
    else ifeq ($(findstring GD32F330,$(derivative)),GD32F330)
        obj += $(addprefix ped_fw/veeprom/stm32f401/,veepromMc )
    else ifeq ($(findstring GD32E50,$(derivative)),GD32E50)
        obj += $(addprefix ped_fw/veeprom/cellsize_4/,veepromMc )
    else ifeq ($(findstring MSPM0,$(derivative)),MSPM0)
        obj += $(addprefix ped_fw/veeprom/cellsize_8/,veepromMc )
    else
        obj += $(addprefix ped_fw/veeprom/,veeprom )
    endif
    
    obj     += $(addprefix ped_fw/veeprom/,crc)
    cfg_obj += $(addprefix ped_fw/veeprom/,veepromXs)
    cfg_obj += $(addprefix ped_fw/veeprom/,$(platform)/veepromMcXs)
    cfg_hdr += $(addprefix ped_fw/veeprom/,veepromXs.h testModuleXs.h)
    cfg_hdr += $(addprefix ped_fw/veeprom/,$(platform)/veepromMcXs.h)
    
    search_path += $(app_path)/prog/ped_fw/veeprom/$(platform)
    doxy_image_path_ += ped_fw/veeprom/doc

    ifeq ($(veeprom_variant), veeprom)
        #Size of Drawer ID is 1 byte, standard VEEPROM
    else ifeq ($(veeprom_variant), veepromlarge)
        #Drawer ID is 2 bytes long, more pages used as a one memory unit
        defines += VEEPROM_LARGE
    else
        $(error Invalid veeprom_variant=$(veeprom_variant))
    endif
    
    # to enable debug of veeprom, define veeprom_debug=true
    ifeq ($(veeprom_debug),true)
        debug    = true
        # assert is included
        defines += ASSERT_INCLUDED
        # Include 16-Bit Realtime Measurement ( comment line out if no)
        defines += TM_RTIME16_INCLUDED
        # Include Test Points ? ( comment line out if no)
        defines += TM_TESTPOINTS_INCLUDED
        # Include Trigger Settings support ( comment line out if no)
        defines += TM_TRIGGER_INCLUDED
        cfg_obj += $(addprefix ped_fw/veeprom/,assertXs)
        cfg_hdr += $(addprefix ped_fw/veeprom/,assertXs.h)
        obj_test += AssertXs
    endif
    
    ifndef unit_test
        # include dependency components
        include $(sdk_prog_path)/ped_fw/flash/build.mak
    endif 

else
    $(error VEEPROM component is not supported for platform $(platform))
endif

endif

#end of protection guard preventing multiple inclusion
endif
