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
#  Description      settings and targets for doxygen
#*******************************************************************************

auto_dep ?= true

depopt  += -E -MM
depopt  += $(addprefix -D,$(defines_tool)) -D__analysis__
depopt  += $(addprefix -I,$(search_path_implicit))
depopt  += $(addprefix -I,$(search_path))
depopt  += $(addprefix -I,$(search_path_notcc))
depopt  += -D __GNUC__=4
depopt_c    += -std=c99
depopt_cpp  += -std=c++14

dep_out_file    = $(out_path)/$(variant)/$(variant_base).dep

#General extensions to create dependencies for
dep_target_ext += .o .obj
ifeq (,$(filter $(no_dep_targets),$(MAKECMDGOALS) $(MFLAGS) $(MAKEFLAGS)))
    # do not include dependency in case of some targets like e.g. clean
    # so in case of defect dependency file at least clean is a solution
    ifeq ($(auto_dep), true)
        dep_target_ext += .ddd
        
        #only include if we are not building dep directly
        ifeq (,$(filter $(MAKECMDGOALS) $(MFLAGS),dep))
            include $(dep_out_file)
        endif
    else
        # Hint: path written different to prevent automatic generation
        #       When the file is available it will be included
        -include $(out_path)\\$(variant)\\$(variant_base).dep
    endif
endif

.PHONY: dep
dep: $(dep_out_file)


# list of dependecy files made from sources with extension .c and .cpp
dep_files = $(call prepost,$(out_prog_path)/,$(basename $(filter %.c %.cpp, $(sources))),.ddd)


# ******************************************************************************
# Local functions
# ******************************************************************************

# function for getting specific options for compilation unit
# parameter 1: file name of compilation unit
define dep_copt
$(addprefix -I,$($(basename $1)_search_path))
endef

# function for deleting file and throwing error
# parameter 1: file to delete
define dep_del_and_error
$(perl) -e "unlink (\"$1\"); exit -1;"
endef

# function for creating dependency file
# parameter 1: compiler options
# parameter 2: source file
# parameter 3: output file
define dep_create
$(cpp_depend) $1 $2 $3.tmp && $(perl) $(local_tool_path)/mixed/depend.pl $3.tmp >>$3 || $(call dep_del_and_error,$3)
$(call dospath,if exist $3.tmp del $3.tmp)
endef

# ******************************************************************************
# Make targets
# ******************************************************************************

# Rule for generating .dep file 
$(dep_out_file): $(dep_files)
	@$(call concat_files,$(@D)/,$@,*.ddd)
	@$(call prntGr,-------- Dependencies calculated -----------)

# Implicit rule for preprocessing
$(out_prog_path)/%.ddd : %.c $(varHeader) | $(code_gen_log)
	@$(call prntlineWhiteCyan,Scanning dependency of $(dir $<),$(notdir $<))
	@$(call mkdir, $(@D))
	@$(call prntnnl,$(addprefix $(basename $@),$(dep_target_ext)) : ) > $@
	@$(call dep_create,$(depopt) $(depopt_c) $(call dep_copt,$(@F)),$<,$@)

$(out_prog_path)/%.ddd : %.cpp $(varHeader) | $(code_gen_log)
	@$(call prntlineWhiteCyan,Scanning dependency of $(dir $<),$(notdir $<))
	@$(call mkdir, $(@D))
	@$(call prntnnl,$(addprefix $(basename $@),$(dep_target_ext)) : ) > $@
	@$(call dep_create,$(depopt) $(depopt_cpp) $(call dep_copt,$(@F)),$<,$@)
