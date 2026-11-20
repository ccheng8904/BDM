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
#  Description      generic makefile, please do no change!
#*******************************************************************************
.SUFFIXES:
SHELL = cmd

# enable double expansion of make variables
.SECONDEXPANSION:

# variable for adding make search path for prerequisites
VPATH = $(make_search_path)

# synchronization of console output for parallel processing
# options: none, line, target, recurse
# none - This is the default: all output is sent directly as it is generated and no synchronization is performed
# line - destroys colored output when -j is used
# target - reorganizes output in a bad way and destroys colored output when -j is used
MAKEFLAGS += --output-sync=none


# include common build settings
include common/build/common/common.mak
ifeq ($(makefile_debug),true)
    $(warning $(call debug_time_stamp))
    $(info makefile recursion level $(MAKELEVEL))
endif

# supported build-in variants (needed already in application build.mak)
built_in_variants  = romLoader ramLoader lromLoader BootManager programmer fsTestHarness
# BTM variants
built_in_variants += gecko_bl
test_variants = testVariant testServer testClient testOnHW plainBuild HALTest

ifneq (,$(filter $(MAKECMDGOALS),release))
    build_type = RELEASE
endif

ifneq (,$(filter $(MAKECMDGOALS),all))
    .NOTPARALLEL:
endif

# include common build definitions
ifeq ($(wildcard $(app_path)/build/common_config.mak),)
    # Add target to the filter list if the target is independent of setting up a project
    ifeq (,$(filter $(MAKECMDGOALS),choco_export choco_install choco_pkg choco_python debug pip_update))
        $(info not existing: $(app_path)/build/common_config.mak)
        $(error The specified project $(project) does not exist!)
    endif
endif

-include $(app_path)/build/common_config.mak
ifdef app_variants
    # only added in case of existing common_config.mak
    app_variants += tautest doctest
endif

ifeq ($(cc_build_path),keilArm)
    ifndef force_keilArm
        # keilArm not supported in newer toolchain
        $(info Hint: ++++ The present Keil MDK does not support armcc anymore, armclang used instead +++)
        $(info +          To force using keilArm you need to select an older Keil MDK:)
        $(info +          keil_version  = 5.29)
        $(info +          force_keilArm = true)
        $(info +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++)
        cc_build_path = armclang
    endif
endif


variant ?= $(firstword $(app_variants))

ifneq (,$(filter $(variant),$(app_variants)))
    # include settings for application
    variant_mak = $(app_path)/build/$(variant).mak
    include $(variant_mak)

# TODO: Uncomment following after test scripts are updated
# else ifeq (,$(filter $(variant),$(built_in_variants) $(test_variants)))
    # $(error Unknown variant $(variant))
endif

# default FW update type FWU0 (not valid FWU) solves skipping memory calculation when it is not needed
FW_update_type ?= FWU0

# include settings for common variants
include common/build/variants/variants.mak

# Release build evaluation
_release_lock_file := $(out_path)/$(variant_base)/release.lock

# Release configuraiton
ifeq ($(strip $(build_type)),RELEASE)
    
    # Disable dependency checking
    auto_dep ?= false
    
    # Check release lock
    ifeq (,$(filter clean%,$(MAKECMDGOALS)))
        ifneq (,$(wildcard $(_release_lock_file)))
            $(error Some of the build performed without release configuration. Perform "make clean" to proceed)
        endif
    endif
    
    ifeq ($(app_variant),true)
        # disable parallel execution (-j option) in release builds
        $(info This is release build of $(variant), parallel build (-j option) is suppressed)
        .NOTPARALLEL:
    endif
else
    # Create release lock  
    $(call fprintf,$(_release_lock_file),LOCKED)
endif


# create the desired out put folder using a shell macro
dummy_out_path := $(shell $(python) $(mkfolders) -folders $(out_path)/$(variant))

msp := MSP/$(platform)
msp_path := common/prog/$(msp)

# set the common components (if no already done)
common_components += $(add_components)

# include Functional Safety component by default for FWU3
ifeq ($(FW_update_type)_$(MAKECMDGOALS), FWU3_init)
    feature_functional_safety = ENABLED
endif

# include Functional Safety component
ifdef app_variant
    ifeq ($(feature_functional_safety),ENABLED)
        common_components += FuncSafe
    endif
endif

# included here to use its definitions in common build.mak files
include $(msp_path)/derivative.mak


#include build files for the application components

app_include_components = $(call unique,$(app_components) $(test_app_components))
include $(addsuffix /build.mak,$(addprefix $(app_path)/prog/,$(app_include_components)))



#include build files for the external components
ext_include_components = $(call unique,$(ext_components) $(test_ext_components))
include $(addsuffix /build.mak,$(addprefix $(ext_path)/,$(ext_include_components)))



#out      := out/$(variant)
# add prefix and postfix to object list
_objects  := $(call prepost, $(out_prog_path)/,$(obj),.o)
_sources  := $(addsuffix .c,$(obj))

obj      := $(nullstring)

#include build files for the common components
common_include_components = $(call unique,$(common_components) $(test_components))
include $(call prepost,common/prog/,$(common_include_components),/build.mak)


# include specific build definitions only if file is present
-include $(app_path)/build/specific_config.mak

# include data variants
include common/prog/dmc/dmc.mak

# include veeprom memory calculation
ifeq ($(feature_veeprom),ENABLED)
    include common/prog/ped_fw/veeprom/veeprom_mem_org.mak
endif

# include memory calculation
include common/build/mixed/fwu_mem_org.mak

# include dependencies
ifdef dep_component
    include $(addsuffix /build.mak,$(addprefix common/prog/,$(dep_component)))
endif

ifneq ($(unit_test_variant),true)
    src += $(ut_src)
    lib_src += $(ut_lib_src)
endif
system_lib_generation ?= ENABLED
ifeq ($(system_lib_generation),ENABLED)
    # System library generated by library generator
    systemlib :=  $(out_prog_path)/ped_fw.lib
    lib_gen += ped_fw
    ped_fw_src = $(lib_src) $(addsuffix .c,$(obj))
    ped_fw_develop = true
else
    # System library not generated
    _objects += $(call prepost, $(out_prog_path)/,$(basename $(lib_src)),.o)
    _objects += $(call prepost, $(out_prog_path)/,$(obj),.o)
    _sources += $(lib_src) $(addsuffix .c,$(obj))
endif


# library generation makefile
include common/build/mixed/lib_gen.mak

# Sources from library generation
_objects   += $(call prepost, $(out_prog_path)/,$(nolib_obj) $(cfg_obj),.o)
_sources   +=  $(addsuffix .c, $(nolib_obj) $(cfg_obj))


# New type of defining object and source files
_objects   += $(call prepost, $(out_prog_path)/,$(basename $(cfg_src) $(gen_src) $(src) $(nolib_src)),.o)
_sources   += $(nolib_src) $(cfg_src) $(gen_src) $(src) $(lib_gen_src)


# Application libraries
lib        += $(addprefix $(app_path)/prog/,$(app_libs))


# Filter out duplicate entries from temporary variables
nocc_libobj = $(call unique,$(_nocc_libobj))
objects = $(call unique,$(_objects))
sources = $(call unique,$(_sources))


doxy_image_path += $(addprefix common/prog/,$(doxy_image_path_))

ifeq ($(auto_lint),false)
    default_targets ?= link
else
    default_targets ?= link lintprocess
endif

# include the generic part of build process
include common/build/common/generic.mak

# default target
.DEFAULT_GOAL := DEF_GOAL

.PHONY: DEF_GOAL
DEF_GOAL: $(default_targets)
