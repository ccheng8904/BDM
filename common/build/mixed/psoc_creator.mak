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
#  Description      settings and targets for PSoC Creator
#*******************************************************************************

#******************************************************************************
# tools and paths
#******************************************************************************

# psoc_creator_path variable has to be defined externally
psoc_creator_bin_path   = $(psoc_creator_path)/bin


# PSoC Creator tool paths
xcyprjmgr           := "$(psoc_creator_bin_path)/cyprjmgr.exe"
xcydsfit            := "$(psoc_creator_bin_path)/cydsfit.exe"
psoc_checksum_py    := $(local_tool_path)/python/psoc_checksum.py
psoc_prjcpy_py      := $(local_tool_path)/python/psoc_prjcpy.py

#******************************************************************************
# functions
#******************************************************************************

# function to add PSoC project into workspace
# parameter 1: absolute path to project file
define cyprjmgr_add
$(xcyprjmgr) -w $(psoc_wrk_file) -addprj "$1" || $(call tool_error,$(xcydsfit))
endef


# call cydsfit.exe tool with options.
# Source files are generated to current directory
# parameter 1: absolute path to project file
# parameter 2: additional cydsfit.exe command line options
define cydsfit
$(xcydsfit) -p "$1" $2 || $(call tool_error,$(xcydsfit))
endef


# function copying generated sources from temporary directory to destination
# only *.c, *.h, *.txt and *.inc files are copied, just like PSoC Creator does
# parameter 1: path to source directory
# parameter 2: path to destination directory
define cy_copy_src
#TODO
endef


# function copying PSoC project and replace name and references
# parameter 1: path to source project
# parameter 2: path to destination project
# parameter 3: PSoC 4 derivative of new project
define cy_copy_prj
$(python) $(psoc_prjcpy_py) $1 $2 $3
endef


# function for generating PSoC source files
# parameter 1: path to source project
# parameter 2: path to where the generated sources will be placed
define cy_gen_src
@$(call deltree,$2)
@$(call mkdir,$2)
@cd $2 && $(call cydsfit,$1,$(cydsfit_opt))
endef


# PSoC checksum script variable
# parameter 1: path to hex file to be patched
define cy_checksum
@$(python) $(psoc_checksum_py) $(patchbin) $1 >$(null)
endef

#******************************************************************************
# make varibles
#******************************************************************************


# Default cydsfit options for compatibility with PSoC Creator
#cydsfit_pre_opt     = -.appdatapath "M:/Cypress/PsocCreator/4.2_SP0"
cydsfit_opt     = -.fdsnotice
cydsfit_opt    += -.fdswarpdepfile=warp_dependencies.txt
cydsfit_opt    += -.fdselabdepfile=elab_dependencies.txt
cydsfit_opt    += -.fdsbldfile=generated_files.txt
cydsfit_opt    += -.fdsreffile=referenced_files.txt
cydsfit_opt    += -- -yv2 -q10 -ygs -o2 -v3 -.fftcfgtype=LE


# PSoC Creator FWU project - common for bootmanager, bootloader and programmer
#psoc_prj_fwu_name   = project_template_$(platform)
psoc_prj_fwu_path   = $(psoc_wrk_path)/$(psoc_prj_fwu_name).cydsn
psoc_prj_fwu_file   = $(psoc_prj_fwu_path)/$(psoc_prj_fwu_name).cyprj


# PSoC workspace and project template paths and names
psoc_wrk_cfg_file   = $(wildcard $(psoc_wrk_cfg_path)/*.cywrk)
psoc_prj_cfg_file   = $(wildcard $(psoc_wrk_cfg_path)/*/*.cyprj)
psoc_prj_cfg_name   = $(basename $(notdir $(psoc_prj_cfg_file)))
psoc_prj_cfg_path   = $(psoc_wrk_cfg_path)/$(psoc_prj_cfg_name).cydsn


# Get PSoC workspace file name from workspace template
psoc_wrk_file_name  = $(basename $(notdir $(psoc_wrk_cfg_file)))
psoc_wrk_file       = $(psoc_wrk_path)/$(psoc_wrk_file_name).cywrk


psoc_prj_path       = $(psoc_wrk_path)/$(psoc_prj_name).cydsn
psoc_prj_file       = $(psoc_prj_path)/$(psoc_prj_name).cyprj
psoc_prj_files      = $(addprefix $(psoc_prj_path)/,$(psoc_prj_name).cyprj $(psoc_prj_name).cydwr TopDesign/TopDesign.cysch)
psoc_prj_header     = $(psoc_src_path)/project.h


# If PSoC application/fwu project does not exists, add it as init target
ifeq (,$(wildcard $(psoc_prj_file)))
    psoc_init_targets += $(psoc_prj_path)
endif

ifeq (,$(wildcard $(psoc_prj_fwu_file)))
    psoc_init_targets += $(psoc_prj_fwu_path)
endif

patch_psoc_checksum = true
init_targets += $(psoc_init_targets)

#******************************************************************************
# targets
#******************************************************************************

no_dep_targets += psoc_init psoc_gen_src

.PHONY: psoc_init
psoc_init: $(psoc_init_targets)


.PHONY: psoc_gen_src
psoc_gen_src: $(psoc_prj_header)


# Target to copy PSoC workspace file from template
$(psoc_wrk_file):
	@echo Generating PSoC workspace: $(dir $@)
	@$(call copy,$(psoc_wrk_cfg_file),$(psoc_wrk_file))


# Target replacing project name from template, registering it to workspace
# and initial generation of PSoC source files
$(psoc_wrk_path)/%.cydsn: $(psoc_wrk_file)
	@echo Initializing PSoC project $*
	@$(call cy_copy_prj,$(psoc_prj_cfg_path),$@,$(psoc_derivative))
	@echo Registering project $* to workspace
	@$(call cyprjmgr_add,$(CURDIR)/$@/$*.cyprj)
	@echo Generating PSoC sources...
	@$(call cy_gen_src,$(CURDIR)/$@/$*.cyprj,$(psoc_src_path)/../$*)


# target for generating of PSoC source files if project changes
ifeq ($(psoc_disable_auto_gen_src),true)
$(psoc_prj_header):
else
$(psoc_prj_header): $(psoc_prj_files)
endif
	@echo Generating PSoC sources...
	@$(call cy_gen_src,$(CURDIR)/$(psoc_prj_file),$(psoc_src_path))
