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
#  Description      Preprocessing makefile
#*******************************************************************************

# This makefile serves for preprocessing of files. To add file to
# preprocess, append target name to variable "preproc_file". Then create
# variables with <preproc_target>_XXX based on following list of variables:

# <preproc_target>_template     - input template file to be preprocessed
# <preproc_target>_output       - output where to store preprocessed file
# <preproc_target>_defines      - optional definitions for preprocessing
# <preproc_target>_depend       - optional dependencies for output file
# <preproc_target>_copt         - optional compiler options for preprocessor


# Target for preprocessing of files based on template/output variables
define preproc_target_template =
.PHONY: $(1)
$(1): $($(1)_output)
$($(1)_output): $($(1)_template) $($(1)_depend)
	@echo preprocessing file $($(1)_output)
	$(call mkdir, $(dir $($(1)_output)))
	@$(cpp_generic) -P $($(1)_copt) $(addprefix -D,$($(1)_defines)) $($(1)_template) > $($(1)_output)
endef

$(foreach pre,$(preproc_file),$(eval $(call preproc_target_template,$(pre))))
