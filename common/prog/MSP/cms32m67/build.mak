#*******************************************************************************
#  Copyright (c) 2023 BSH Hausgeraete GmbH,
#  Carl-Wery-Str. 34, 81739 Munich, Germany, www.bsh-group.de
#
#  All rights reserved. This program and the accompanying materials
#  are protected by international copyright laws.
#  Please contact copyright holder for licensing information.
#
#*******************************************************************************
#  PROJECT          MCU Framework / CMS32M67xx
#*******************************************************************************
#  Description      build include for CMS32M67xx (Cmsemicon) board support package
#*******************************************************************************

# default path for cc-build system, application may define gccArm as well
cc_build_path ?= armclang
core_path      = armCmx

# Selector for DBus mapping:
#  - mcal (MUART)
#  - hal  (HUART, not available for CMS32M67xx)
#  - dbuscan (DBusCAN chip)
dbus_mapping ?= mcal

# the CMS32M67xx device and driver headers are part of the platform folder
search_path += $(msp_path)/drv

# add vendor driver sources used by the MCAL and the board support package
cms32m67_drv_path = common/prog/MSP/$(platform)/drv
cms32m67_lib += $(addprefix $(cms32m67_drv_path)/, cgc gpio uart timer wdt flash)
# the MCAL peripheral driver functions (IICA, SSP, ADC, DMA) are provided by the same vendor lib
cms32m67_lib += $(addprefix $(cms32m67_drv_path)/, i2ca ssp adc dma)
# start-up code (SystemInit, SystemCoreClock)
cms32m67_lib += $(msp_path)/system_CMS32M67xx

# vendor libs tested working fine
# disable lint checking for vendor libs
no_code_analysis += $(cms32m67_drv_path)
# dissable axstyle checking for vendor libs

ax_exclude_pattern += "$(cms32m67_drv_path)/*.*"
ax_exclude_pattern += "$(msp_path)/weakvectbl.h"

# I2C configuration
hi2c_variant ?= used_swi2c
defines      += I2C_BY_MCAL
ifdef testServer
# todo: check the relevance of these test variants

else ifdef testClient
    arm_cmx_no_itbl = 1

else ifdef bootManager
    bootloader_merge = false
    obj     += $(msp)/mem_drv/intflash_drv
    obj     += $(cms32m67_lib)

else ifdef bootLoader
    dep_component += MSP/mcal std_lib
    mcal_modules ?= mdio muart mwdt
    obj     += $(cms32m67_lib)

    ifdef ramLoader
        obj += $(msp)/mem_drv/intflash_drv
    endif

else ifdef programmer
    obj     += $(cms32m67_lib)
    obj     += $(addprefix $(msp)/, weakvectbl)
    cfg_obj += $(addprefix $(msp)/, IntTbl int_handle_cfg)
    dep_component += MSP/mcal std_lib
    mcal_modules ?= mdio muart mwdt mexti

# ***** Standard: normal application ***********
else
    # define the compilation units for the micro abstraction layer 
    obj     += $(addprefix $(msp)/,mem_drv/intflash_drv weakvectbl hwdt)
    obj     += $(cms32m67_lib)

    # define the library configuration files which must be provided by application
    cfg_obj += $(addprefix $(msp)/, IntTbl int_handle_cfg)
    cfg_hdr += $(addprefix $(msp)/, IntTbl.h cms32m67_ext_cfg.h)
    dep_component += MSP/mcal
endif

# template for option bytes
no_option_bytes_eep_file = 1

dep_component += math MSP/$(core_path)

doxy_image_path_ += MSP/doc/images
