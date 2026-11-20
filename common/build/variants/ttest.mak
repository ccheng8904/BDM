#*******************************************************************************
#  Copyright (c) 2024 BSH Hausgeraete GmbH,
#  Carl-Wery-Str. 34, 81739 Munich, Germany, www.bsh-group.de
#
#  All rights reserved. This program and the accompanying materials
#  are protected by international copyright laws.
#  Please contact copyright holder for licensing information.
#
#*******************************************************************************
#  Project          Generic SW
#*******************************************************************************
#  Description      definition for tautest variant
#*******************************************************************************
ifndef TAUTEST_MAK
TAUTEST_MAK = 1

ut_common_tests       += ped_fw/stdcrc
tautest = true

std_variant = false
defines_cl  += TAUTEST
HEAPSIZE = 1000

ut_test_file_extension = .ttest

ut_test_fw_component   = tau $(msp) ped_fw debug


ifeq ($(ut_do_app_ttest), true)
    ped_fw_subcomponent_list = basic
else
    ped_fw_subcomponent_list = basic stdcrc
endif

unit_test_variant = true
linker_scripts = $(empty)

# firmware update type: no FWU for on-target unit testing
FW_update_type = FWU0

# re-define the default target
default_targets = utest


ut_test_src   += $(foreach f,$(ut_lib_src) $(ut_src),$(dir $(f))test/$(notdir $(basename $(f)))$(ut_test_file_extension).cpp)


ifeq ($(MAKECMDGOALS),lint)
    ut_static_code_analysis ?= true
else
    ut_static_code_analysis ?= false
endif

ifeq ($(ut_static_code_analysis), false)
    ifeq ($(ut_do_app_test), true)
        common_components = $(ut_test_fw_component)
    else
        # do the common unit tests
        common_components = $(ut_test_fw_component) $(ut_common_tests)
    endif
    src += $(ut_test_src)
else
    ifeq ($(ut_do_app_test), true)
        common_components = $(empty)
    else
        # do the common static unit tests
        common_components =  $(ut_common_tests)
    endif
    src += $(ut_src)
    lib_src += $(ut_lib_src)
endif

user_systick_handler = true

endif #TAUTEST_MAK
