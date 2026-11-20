#*******************************************************************************
#  Copyright (c) 2023 BSH Hausgeraete GmbH,
#  Carl-Wery-Str. 34, 81739 Munich, Germany, www.bsh-group.de
#
#  All rights reserved. This program and the accompanying materials
#  are protected by international copyright laws.
#  Please contact copyright holder for licensing information.
#
#*******************************************************************************
#  PROJECT          MCU Framework / SH30F9X71
#*******************************************************************************
#  Description      build include for SH30F9X71 board support package
#*******************************************************************************

# default path for cc-build system, application may define gccArm as well
cc_build_path ?= armclang
core_path      = armCmx

# Selector for DBus mapping:
#  - mcal (MUART)
#  - hal (HUART, not available for SH30F9X71)
#  - dbuscan (DBusCAN chip)
dbus_mapping ?= mcal

# add search_path for sh30f peripherals headers
search_path += $(msp_path)/lib

# add vendor libs for some of the peripherals
sh30f_lib += $(addprefix $(msp)/lib/sh30f9xx_sa0_, dma exti flash rcc syscfg timer twi)
 
# vendor libs tested working fine
# disable lint checking for vendor libs
no_code_analysis += $(msp)/lib
# dissable axstyle checking for vendor libs
ax_exclude_pattern += "$(msp_path)/lib/*.*"
ax_exclude_pattern += "$(msp_path)/sh30f9xx_sa0.h" 
ax_exclude_pattern += "$(msp_path)/system_sh30f9xx_sa0.h" 
ax_exclude_pattern += "$(msp_path)/weakvectbl.h"

# I2C configuration
hi2c_variant ?= used_swi2c
defines      += I2C_BY_MCAL

ifdef testServer
# todo: check the relevance of these test variants
#    obj     += $(addprefix $(msp)/,hint01 hint01i huart01 huart01i hwdt01 weakvectbl)
#    cfg_obj += $(addprefix $(msp)/,hint01xs huart01xs hwdt01xs IntTbl int_handle_cfg)    

else ifdef testClient
    arm_cmx_no_itbl = 1

else ifdef bootManager
    bootloader_merge = false
    obj     += $(msp)/mem_drv/intflash_drv
    obj     += $(addprefix $(msp)/lib/sh30f9xx_sa0_,rcc syscfg timer exti)

else ifdef bootLoader
    dep_component += MSP/mcal std_lib
    mcal_modules ?= mdio muart mwdt
    obj     += $(addprefix $(msp)/lib/sh30f9xx_sa0_,rcc syscfg timer exti)

    ifdef ramLoader
        obj += $(msp)/mem_drv/intflash_drv
        #obj += $(msp)/mem_drv/optflash_drv
        #defines += MAL_ENABLE_OPT_FLASH_DRIVER
    endif

else ifdef programmer
    obj     += $(sh30f_lib)
    obj     += $(addprefix $(msp)/, weakvectbl)
    cfg_obj += $(addprefix $(msp)/, IntTbl int_handle_cfg)
    dep_component += MSP/mcal std_lib
    mcal_modules ?= mdio muart mwdt mexti

# ***** Standard: normal application ***********
else
    # define the compilation units for the micro abstraction layer 
    obj     += $(addprefix $(msp)/,mem_drv/intflash_drv weakvectbl)
    obj += $(sh30f_lib)

    # define the library configuration files which must be provided by application
    cfg_obj += $(addprefix $(msp)/, IntTbl int_handle_cfg)
    cfg_hdr += $(addprefix $(msp)/, IntTbl.h sh30f_ext_cfg.h)
    dep_component += MSP/mcal
endif

# template for option bytes
no_option_bytes_eep_file = 1

dep_component += math MSP/$(core_path)

doxy_image_path_ += MSP/doc/images
