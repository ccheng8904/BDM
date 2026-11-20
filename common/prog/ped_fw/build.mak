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
#  Description      build include for PED Framework
#*******************************************************************************

#protection guard preventing multiple inclusion
ifndef pedfw_build_mak
pedfw_build_mak := 1

ifndef core_path
    $(error !!! core_path not defined msp shall be first component in common_components list !!!)
endif

ifdef testOnHW
    ped_fw_subcomponent_list = basic schedule testengine timer utility testsupport
    defines +=  TENG_INCLUDED

else ifdef testServer
    ped_fw_subcomponent_list ?= basic schedule testengine timer utility
    defines +=  TENG_INCLUDED

# non-test variants
else ifeq ($(std_variant),true)
    
    # by default old scheduler is used
    ifneq ($(rtos), TRUE)
        ifeq ($(scheduler), schedule)
            scheduler ?= schedule
        else
            scheduler =
        endif
    endif
    
    # by default all subcomponents except flash veeprom exteeprom, nvmem and testengine
    ped_fw_subcomponent_list ?= basic $(scheduler) stdcrc timer utility watchdogtimer

else ifeq ($(unit_test_variant),true)
# do not set the list here
else
    # definitions for test application
    ped_fw_subcomponent_list := testsupport
endif


# include the specified subcomponents
include $(call prepost, $(sdk_prog_path)/ped_fw/,$(ped_fw_subcomponent_list),/build.mak)

# the following files are needed for special variants and will be copied anyway
ifndef ped_fw_testsupport
    cfg_hdr += $(addprefix ped_fw/testSupport/TestSupportXS, .c .h)
endif
ifndef ped_fw_testengine
    cfg_hdr += $(addprefix ped_fw/testengine/teng_msgsrv_XS., c h)
endif

#end of protection guard preventing multiple inclusion
endif

