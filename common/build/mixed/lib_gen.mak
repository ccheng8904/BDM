#*******************************************************************************
#  Copyright (c) 2017 BSH Hausgeraete GmbH,
#  Carl-Wery-Str. 34, 81739 Munich, Germany, www.bsh-group.de
#
#  All rights reserved. This program and the accompanying materials
#  are protected by international copyright laws.
#  Please contact copyright holder for licensing information.
#
#*******************************************************************************
#  PROJECT          PP PED Framework
#*******************************************************************************
#  Description      Library generation makefile
#*******************************************************************************

# This makefile serves for generating libraries. To add new library, append
# library name with relative path to variable "lib_gen". Then create variable
# with <library_name>_src and add source files for library. Library will be
# generated to output folder.
#
# Example:
# lib_gen += <component_name>/<library_name>
# <library_name>_develop = true
# <library_name>_copy = true
# <library_name>_src  = <component_name>/library_src_1.c
# <library_name>_src += <component_name>/library_src_2.cpp
# <library_name>_src += <component_name>/library_src_3.s

# function for returning object files for library generation prerequisites
# parameter 1: library file name
# result: list of object files for given library
define get_lib_gen_obj
$$($$(notdir $(1))_obj)
endef

# function for returning dependencies for library
# parameter 1: library file name
# result: list of dependencies for given library
define get_lib_gen_dep
$$($$(notdir $(1))_dep)
endef

lib_gen_copy_path = lib


define lib_gen_template_1 =
    # Library generation configuration
    $(1)_develop    ?= false
    $(1)_copy       ?= false
    $(1)_name       ?= $(1)_$(platform)_$(cc_build_path)

    # Non configurable parameters
    $(1)_path        = $(subst ./,,$(2))
    $(1)_out         = $(out_prog_path)/$$($(1)_path)$(1).lib

  ifeq ($$($(1)_copy),true)
    $(1)_ext         = $(lib_gen_copy_path)/$$($(1)_name).lib
  else
    $(1)_ext         = $$($(1)_out)
  endif

    # Object files out of sources
    $(1)_obj         = $(call prepost,$(out_prog_path)/,$(basename $($(1)_src)),.o)

    # Append current lib to the list of all generated libraries
    lib_gen_libs    += $$($(1)_ext)
endef


define lib_gen_template_2 =

  # Generate if build type is RELEASE
  ifeq ($(build_type),RELEASE)
    $(1)_gen_flag := 0
  endif

  # Generate if explicit target is called
  ifneq (,$(filter $(1),$(MAKECMDGOALS)))
    $(1)_gen_flag := 1
    MAKEFLAGS += --always-make
  endif

  # Generate if -B option is used
  ifneq (,$(findstring B,$(MAKEFLAGS)))
    $(1)_gen_flag := 2
  endif

  # Generate if output file is not present
  ifeq (,$(wildcard $($(1)_ext)))
    $(1)_gen_flag := 3
  endif

  # Generate if library is  generation is enabled
  ifeq ($($(1)_develop),true)
    $(1)_gen_flag := 4
    
    # Add sources for dependency checking and static analysis
    lib_gen_src += $$($(1)_src)
  endif


# Keep object files
.PRECIOUS: $$($(1)_obj)

# Phony target
no_dep_targets += $(1)
.PHONY: $(1)
$(1): $$($(1)_ext)

# Library copy target (if out does not match copy)
ifneq ($$($(1)_out),$$($(1)_ext))
ifdef $(1)_gen_flag
$$($(1)_ext): $$($(1)_out)
	@echo Copying $$($(1)_out) to $$($(1)_ext)
	@$$(call copy, $$($(1)_out), $$($(1)_ext) /Y)
else
$$($(1)_ext): $$($(1)_src) $$($(1)_dep)
	@$(call prntRd,Library $$($(1)_ext) might be out of date)
	@$(call prntGr,To regenerate library execute \"make $(1) $(project_make_str)\")
endif
endif


.PHONY: $(1)_debug
$(1)_debug:
	@echo Lib name:         $$($(1)_name)
	@echo Develop mode:     $$($(1)_develop)
	@echo Copy library:     $$($(1)_copy)
	@echo Relative path:    $$($(1)_path)
	@echo Out library:      $$($(1)_out)
	@echo External path:    $$($(1)_ext)
	@echo Library sources:  $$($(1)_src)
	@echo Library objects:  $$($(1)_obj)
	@echo Dependencies:     $$($(1)_dep)
endef


$(foreach _lib,$(lib_gen),$(eval $(call lib_gen_template_1,$(notdir $(_lib)),$(dir $(_lib)))))
$(foreach _lib,$(lib_gen),$(eval $(call lib_gen_template_2,$(notdir $(_lib)))))

# ******************************************************************************
# The target for generating libraries
# ******************************************************************************
no_dep_targets += gen_libs
.PHONY: gen_libs
gen_libs: $(lib_gen_libs)
