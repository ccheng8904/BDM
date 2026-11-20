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
#  Description      general definitions for off target unit tests
#*******************************************************************************
ifndef UT_OFF_TARGET_TEST_MAK
UT_OFF_TARGET_TEST_MAK = 1

defines_cl  += MOCK_IT

unit_test_variant = true
linker_scripts = $(empty)


cc_build_path := gccIntel

# microcontroller configuration
platform      := Intel
derivative    := NONE


# firmware update type: no FWU for off-target unit testing
FW_update_type = FWU0

# re-define the default target
default_targets = utest


ut_test_src   += $(foreach f,$(ut_lib_src) $(ut_src),$(dir $(f))test/$(notdir $(basename $(f)))$(ut_test_file_extension).cpp)
ut_cov_filter += $(sort $(addprefix $(app_prog_path)/,$(dir $(ut_src))) $(addprefix $(sdk_prog_path)/,$(dir $(ut_lib_src))))


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

# maksym running with issues using unit test frameworks
no_maksym  ?= true
# this speeds up build tremendously
no_objdump ?= true
linkerScriptTemplate = $(empty)

endif # UT_OFF_TARGET_TEST_MAK
