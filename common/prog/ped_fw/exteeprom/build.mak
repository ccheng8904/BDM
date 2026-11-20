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
#  Description      build include PED Framework, subcomponent: exteeprom
#*******************************************************************************

#protection guard preventing multiple inclusion
ifndef exteeprom_build_mak
exteeprom_build_mak := 1


# possible variants exteeprom and exteepromlarge
exteeprom_variant ?= exteeprom

# possible interfaces are used_swi2c and used_hwi2c
exteeprom_interface ?= $(hi2c_variant)

# is_exteeprom_supported_platform contains platform name if platform is supported
exteeprom_supported_platforms := stm32f0 stm32f1 stm32f3 stm32f4 stm32l4 stm32g0 stm32g4 mkv5x efr32xg21 ra4 ra2 stm32h7 gd32f33 sh30f mspm0 gd32e50 gd32f30
is_exteeprom_supported_platform = $(filter %$(platform),$(exteeprom_supported_platforms))


ifeq ($(std_variant),true)
    
# Test if is_exteeprom_supported_platform is not empty --> platform is supported by exteeprom
ifneq ($(strip $(is_exteeprom_supported_platform)),)
    
    ifeq ($(exteeprom_interface), used_hwi2c)
        obj     += $(addprefix ped_fw/exteeprom/$(platform)/,ExtEEPlibMc )
        cfg_obj += $(addprefix ped_fw/exteeprom/,$(platform)/ExtEEPlibMcXS )
        cfg_hdr += $(addprefix ped_fw/exteeprom/,$(platform)/ExtEEPlibMcXS.h)
    else ifeq ($(exteeprom_interface), used_swi2c)
        obj     += $(addprefix ped_fw/exteeprom/,ExtEEPlib )
        cfg_obj += $(addprefix ped_fw/exteeprom/,ExtEEPlibXS )
        cfg_hdr += $(addprefix ped_fw/exteeprom/,ExtEEPlibXS.h)
    else
        $(error Invalid exteeprom_interface=$(exteeprom_interface))
    endif
    
    doxy_image_path_ += ped_fw/exteeprom/doc
    
    ifeq ($(exteeprom_variant), exteeprom)
        # standard exteeprom - no options needed
    else ifeq ($(exteeprom_variant), exteepromlarge)
        defines += EXTERNAL_LARGE
    else
        $(error Invalid exteeprom_variant=$(exteeprom_variant))
    endif

else
    $(error EXTEEPROM component is not supported for platform $(platform))
endif

endif

#end of protection guard preventing multiple inclusion
endif
