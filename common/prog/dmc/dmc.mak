#*******************************************************************************
#  Copyright (c) 2022 BSH Hausgeraete GmbH,
#  Carl-Wery-Str. 34, 81739 Munich, Germany, www.bsh-group.de
#
#  All rights reserved. This program and the accompanying materials
#  are protected by international copyright laws.
#  Please contact copyright holder for licensing information.
#
#*******************************************************************************
#  PROJECT          Generic SW
#*******************************************************************************
#  Description      Recipes for creating data component
#*******************************************************************************

ifeq ($(is_data_variant),true)
    defines += IS_DATA_VARIANT
endif

#*******************************************************************************
# pathes
#*******************************************************************************
dmc_template_path = common/prog/dmc/cfg_templates
dmc_target_path = $(app_path)/prog/$(dmodule)_$(dvariant)
dmc_target_path2 = $(app_path)/prog/dmc

#*******************************************************************************
# linker settings
#*******************************************************************************
ifeq ($(is_data_variant),true)
    linkerScriptTemplate = $(app_path)/prog/dmc/LinkerScriptData.h
    bootloader_merge     = false
    system_lib_generation = DISABLED
    no_startvector = true
endif

#*******************************************************************************
# code generator settings
#*******************************************************************************

# Generate data component files
common_export_variable += dvariant dmodule

code_gen += dmc_variant_mak
dmc_variant_mak_template     = $(template_folder)/dmc_variant.tpl.mak
dmc_variant_mak_output       = $(app_path)/build/$(dmodule)_$(dvariant).mak

code_gen += dmc_content_c
dmc_content_c_template     = $(dmc_template_path)/dmc_content.c
dmc_content_c_output       = $(dmc_target_path)/$(dmodule)_$(dvariant).c

code_gen += dmc_content_h
dmc_content_h_template     = $(dmc_template_path)/dmc_content.h
dmc_content_h_output       = $(dmc_target_path)/$(dmodule)_$(dvariant).h

code_gen += dmc_moduleheader
dmc_moduleheader_template     = $(dmc_template_path)/ModuleHeader.c
dmc_moduleheader_output       = $(dmc_target_path)/ModuleHeader_$(dvariant).c

code_gen += dmc_build_mak
dmc_build_mak_template     = $(template_folder)/dmc_build.tpl.mak
dmc_build_mak_output       = $(dmc_target_path)/build.mak

code_gen += dmc_linkerscript
dmc_linkerscript_template     = $(dmc_template_path)/LinkerScript.tpl.h
dmc_linkerscript_output       = $(dmc_target_path2)/LinkerScriptData.h

#*******************************************************************************
# make target to initialize data component
#*******************************************************************************
no_dep_targets += init_dmc init_dmc_checks

# prepare variant mak, ModuleHeader, source/header file and LinkerScript for data component
.PHONY: init_dmc
init_dmc: init_dmc_checks $(dmc_linkerscript_output) $(dmc_variant_mak_output) $(dmc_content_c_output) $(dmc_content_h_output) $(dmc_moduleheader_output) $(dmc_build_mak_output)
	@echo Init finished

init_dmc_checks:
ifeq (,$(app_dmodules))
	@$(error app_dmodules is not defined, please add it in common_config.mak)
endif
ifeq (,$(filter $(dmodule),$(app_dmodules)))
	@$(error app_dmodules are '$(app_dmodules)', $(dmodule) is not one of them. Please add it in common_config.mak)
endif
	@echo Check finished

