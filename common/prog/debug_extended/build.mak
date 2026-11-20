#*******************************************************************************
#  Copyright (c) 2017 BSH Hausgeraete GmbH,
#  Carl-Wery-Str. 34, 81739 Munich, Germany, www.bsh-group.de
#
#  All rights reserved. This program and the accompanying materials
#  are protected by international copyright laws.
#  Please contact copyright holder for licensing information.
#
#*******************************************************************************
#  PROJECT          Generic SW
#*******************************************************************************
#  Description      Build include for debug_extended component (debug_extended in / out and bug preventions using exceptional errors).
#*******************************************************************************

ifndef debug_extended_build_mak
debug_extended_build_mak := 1

# Include the test application
# ============================
devices_subcomponent_list ?= test_app test_app_error test_app_isr dbus

# include the specified subcomponents
include $(call prepost, $(sdk_prog_path)/debug_extended/,$(devices_subcomponent_list),/build.mak)

# Composing the test suite for integration test:
# ==============================================

ifeq ($(marked_comp),debug_extended)
    defines_gcc += STS_SIM_TEST_CI
    defines_gcc += SUSPEND_STUBBING_CALLS_TO_PROTOCOL
    defines_gcc += SUSPEND_STUBBING_CALLS_TO_INTERFACE

    defines_gcc += DBGX_TEST_API_CALLBACK
    defines_gcc += DBGX_TEST_API_CALLOUT
    defines_gcc += DBGX_USR_TEXT_DAT_FUNCTIONS_ENABLED
    defines_gcc += DBGX_USR_BIN_DAT_FUNCTIONS_ENABLED
    defines_gcc += DBGX_INPUT_FUNCTIONS_ENABLED

    obj_test = api protocol interface
    stub_locals = false

    lib_src += debug_extended/test_sts/api_sts.c
    lib_src += debug_extended/test_sts/protocol_sts.c
    lib_src += debug_extended/test_sts/main.c
    lib_src += debug_extended/test_sts/app_dbgx_test_api_callback.c
    lib_src += debug_extended/test_sts/app_dbgx_test_api_callout.c
    lib_src += debug_extended/test_sts/sts_sim_test_uty.c
    lib_src += debug_extended/error.c
    lib_src += mem_utility/ring_buffer.c
    lib_src += MSP/mcal/mcal.c
    lib_src += ped_fw/stdcrc/CrcCom.c
    
    search_path += $(app_prog_path)/MSP/mcal/stm32g0
    
    code_gen_cfg += debug_extended/test_sts/dbgx_test_sts_filters.json
    dbgx_activated_filters = DBGX_TEST_STS_FILTERS
endif 

search_path += common/prog/debug_extended/test_sts
search_path += common/prog/debug_extended/dbus

# Composing the source files:
# ===========================

lib_src += debug_extended/api.c
lib_src += debug_extended/error.c
lib_src += debug_extended/protocol.c
lib_src += debug_extended/interface.c

debug_extended_cfg  = debug_extended/api_cfg.c
debug_extended_cfg += debug_extended/api_error_cfg.c
debug_extended_cfg += debug_extended/protocol_cfg.c
debug_extended_cfg += debug_extended/interface_cfg.c
cfg_src   += $(debug_extended_cfg)

cfg_hdr += debug_extended/user_cfg.h
cfg_hdr += debug_extended/api_cfg.h
cfg_hdr += debug_extended/api_error_cfg.h
cfg_hdr += debug_extended/protocol_cfg.h
cfg_hdr += debug_extended/interface_cfg.h

# All files of cfg_src and of cfg_hdr will be copied to the application
# directory by setup.bat, if the files do not exist there yet.

# Generating the filter definitions:
# ==================================

dbgx_activated_filters ?= DBGX_FILTERS
common_export_variable += dbgx_activated_filters

cfg_hdr += debug_extended/dbgx_filters.json
code_gen_cfg += $(app_prog_path)/debug_extended/dbgx_filters.json
variant_export_dictionary += dbgx_filters

code_gen_cfg += debug_extended/dbgx_api_functions.json
variant_export_dictionary += dbgx_api_functions

gen_hdr += debug_extended/filters_aut.h


# Generating the objects, lint files and dependency files for the sources in the configuration area:
# ==================================================================================================

$(call prepost,$(out_prog_path)/,$(basename $(debug_extended_cfg)),.o) : $(dbgx_filters_target_file)
$(call prepost,$(out_prog_path)/,$(basename $(debug_extended_cfg)),.lin) : $(dbgx_filters_target_file)
$(call prepost,$(out_prog_path)/,$(basename $(debug_extended_cfg)),.ddd) : $(dbgx_filters_target_file)

# Deleting the automatically generated files of debug_extended component
# =============================================================
define deldebug_extended
@$(call delfile,$(app_prog_path)/debug_extended/filters_aut.h)
endef

endif

