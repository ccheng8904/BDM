#*******************************************************************************
#  Copyright (c) 2017 BSH Hausgeraete GmbH,
#  Carl-Wery-Str. 34, 81739 Munich, Germany, www.bsh-group.de
#
#  All rights reserved. This program and the accompanying materials
#  are protected by international copyright laws.
#  Please contact copyright holder for licensing information.
#
#*******************************************************************************
#  Project          PP PED FW
#*******************************************************************************
#  Description      Top level generic build
#*******************************************************************************

# the default definiton of colored output
mcufw_colored_output ?= true

-include environment.mak
-include ci/ci_tools/make/extension.mak
include common/build/common/chocolatey.mak
include common/build/common/paths.mak
include common/build/common/ext_tools.mak
include common/build/common/functions.mak

ifndef project
    app_path = app
    out_path = out
else
    app_path = app/$(project)
    out_path = out/$(project)
endif

# abs_path contains the absolute path to the root of a project folder
# drive's substitutions are expanded to give physical path of working directory
abs_path := $(shell $(python) $(py_modules)/pwd.py)
sdk_path = common
ext_path = ext

app_prog_path   = $(app_path)/prog
sdk_prog_path   = $(sdk_path)/prog
ext_prog_path  ?= $(ext_path)
cfg_prog_path  ?= $(app_prog_path)

ifdef marked_comp
    marked_comp_subst ?= $(subst /,_,$(marked_comp))
    ifdef variant_comp
        override variant := $(variant)_$(marked_comp_subst)_$(variant_comp)
    else
        override variant := $(variant)_$(marked_comp_subst)
    endif

    $(info +++++ building variant $(variant) +++++)
endif


bin_path        = $(out_path)/$(variant)/bin
out_prog_path   = $(out_path)/$(variant)/prog

nullstring :=

# CRC (32bit) size definition
crc32_size ?= 4
defines += CRC32_SIZE=UINT32_C(0x$(strip $(crc32_size)))

# STACK magic size definition for FS stack check
#For allocating space in stack for STACK magic pattern
stack_magic_size ?= 4
defines += STACK_MAGIC_SIZE=UINT32_C(0x$(strip $(stack_magic_size)))


# Make will search for prerequisites in following order:
make_search_path ?= $(call unique, $(cfg_prog_path) $(app_prog_path) $(ext_prog_path) $(sdk_prog_path))

# For these targets the memory map auto-generation will be suppressed
no_memory_map_gen_targets += init initcfg choco%
