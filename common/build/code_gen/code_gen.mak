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
#  Description      Code generation makefile
#*******************************************************************************


# By default automatic generation of files is enabled
# Set this variable to "false" only if build files are not changed
auto_code_gen  ?= true

# File for storing code generator configuration
code_gen_pkl    = $(out_path)/$(variant)/code_gen.pkl
# If any config template override path, add it first for code generator.
code_gen_search_path += $(cfg_override_path)
code_gen_search_path += $(make_search_path)

# Include exported make variables in XML format
code_gen_cfg   += $(common_export_xml) $(variant_export_xml)
code_gen_cfg   += common/build/code_gen/code_gen.json

# Helper variables for configuration files
cfg_files_full = $(addprefix $(cfg_prog_path)/,$(cfgFiles))
cfg_files_missing = $(filter-out $(wildcard $(cfg_files_full)), $(cfg_files_full))

gen_files_full = $(addprefix $(cfg_prog_path)/,$(genFiles))
gen_files_missing = $(filter-out $(wildcard $(gen_files_full)), $(gen_files_full))

# export list of config files to export xml file
variant_export_variable += cfg_files_full gen_files_full


# Warning message if some configuration file is missing
ifneq ($(cfg_files_missing),)
  ifeq (,$(filter $(MAKECMDGOALS), init))
    $(warning Some configuration files are missing, run "make init" to copy them.)
    $(info Missing files $(cfg_files_missing))
  endif
endif



# Create code generator configuration file out of all config inputs
$(code_gen_pkl): export PYTHONHASHSEED=0
$(code_gen_pkl): $(code_gen_cfg)
	@$(python) $(code_gen_config_py) $(addprefix -i , $^) -o $@


# Target for generating configuration files out of templates in cfg_templates folder
# execute "make initcfg cfg_overwrite=true" if you want to overwrite config files
ifeq ($(cfg_overwrite),true)
    cfg_overwrite_opt = -overwrite
endif

# Common code generator options
code_gen_opt += $(addprefix -s , $(code_gen_search_path))
code_gen_opt += $(addprefix -p , $(code_gen_params))

# Target for copying of configuration files (usually once on setup of project)
.PHONY: initcfg
initcfg: $(variant_export_xml)
	@$(python) $(code_gen_files_py) -c $(variant_export_xml) $(code_gen_opt) -a $(cfg_prog_path)/ -d cfg_files_full $(cfg_overwrite_opt)

# Switch to enable/disable automatic generation of files
ifneq ($(auto_code_gen),true)
$(gen_files_full): $(code_gen_pkl)
	@echo $@ is out of date. Run "make gencfg" to regeneratate file
else
$(gen_files_full): $(code_gen_log)
endif

# Target generating configuration files (checked before every build)
.PHONY: gencfg
gencfg: $(code_gen_log)
# If any generated file is missing, force code generation
ifneq ($(gen_files_missing),)
    $(code_gen_log): FORCE
endif
$(code_gen_log): export PYTHONHASHSEED=0
$(code_gen_log): $(code_gen_pkl)
	@$(python) $(code_gen_files_py) -v $(variant) -c $(code_gen_pkl) $(code_gen_opt) -a $(cfg_prog_path)/ -d gen_files_full -overwrite
	@echo DUMMY LOG FILE > $@


# Target for generating of files based on template/output variables
define code_gen_target_template =
    gen_targets += $(1)
    
    # Default variable definitions
    $(1)_config     ?= $(code_gen_pkl)
    $(1)_overwrite  ?= true
    $(1)_search_path += $(code_gen_search_path)
    $(1)_params     += $(code_gen_params)

    $(1)_opt        += $$(addprefix -s , $$($(1)_search_path))
    $(1)_opt        += $$(addprefix -p , $$($(1)_params))
  ifeq ($$($(1)_overwrite), true)
    $(1)_opt        += -overwrite
  endif

.PHONY: $(1)
$(1): export PYTHONHASHSEED=0
$(1): export PYTHONDONTWRITEBYTECODE=1
$(1): $$($(1)_output)
$$($(1)_output): $$($(1)_template) $$($(1)_config)
	@$(python) $(code_gen_files_py) $(addprefix -c ,$$($(1)_config)) $$($(1)_opt) -i $$< -o $$@
endef

$(foreach cg,$(code_gen),$(eval $(call code_gen_target_template,$(cg))))


# Make variables and target for testing of code generator
code_gen_test_path = common/build/code_gen/test_files
code_gen_test_tpl  = $(code_gen_test_path)/template.c
code_gen_test_out  = $(out_path)/output.c
code_gen_test_cmp  = $(code_gen_test_path)/compare.c
code_gen_test_cfg += 

$(code_gen_test_out): $(code_gen_test_tpl) $(code_gen_test_cfg)
	@$(python) $(code_gen_files_py) -i $< -o $@ $(addprefix -c, $(code_gen_test_cfg)) -overwrite

.PHONY: code_gen_test
code_gen_test: $(code_gen_test_out)
	@echo comparing output file $(code_gen_test_out) with $(code_gen_test_cmp)
	@$(eval _file_diff = $(call compare_files, $(code_gen_test_cmp), $(code_gen_test_out)))
	@$(if $(filter $(_file_diff), True),$(call prntGr,"Files are equal"), $(call prntRd,"Files are NOT equal!"))

