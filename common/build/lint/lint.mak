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
#  Description      settings and targets for PC-Lint
#*******************************************************************************


# ***************************************************************************************************************
# Settings for Lint
# ***************************************************************************************************************
include common/build/mixed/nonccparsers.mak

# select misra 2012 by default
misraplus ?= BSH_PED_MISRA_LP

empty:=
space:= $(empty) $(empty)

out_lintplus_path = $(out_path)/$(variant_base)/lint/
out_lintplus_file = $(out_path)/$(variant_base)/lint/$(variant_base).lint
out_lintplus_sources_file = $(out_path)/$(variant_base)/lint/sources.lnt
out_lintplus_sources_file_dd = $(out_path)/$(variant_base)/lint/sources_dd.lnt
out_implicit_searchpath_file = $(out_path)/$(variant_base)/lint/$(variant_base)_implicit_paths.lnt
out_implicit_searchpath_file_dd = $(out_path)/$(variant_base)/lint/$(variant_base)_implicit_paths_dd.lnt

#No_code_analysis Implementation
common_rel_path =common/prog/
app_rel_path = $(app_path)/prog/
ext_rel_path = $(ext_prog_path)/
no_code_filter = $(subst $(common_rel_path),,$(no_code_analysis))
mid_no_code_filter = $(subst $(common_rel_path),,$(no_code_filter))
final_no_code_filter = $(subst $(app_rel_path),,$(mid_no_code_filter))
lintplus_src   ?= $(call filter-out,$(addsuffix /%,$(final_no_code_filter)),$(sources))
lintplus_files  = $(call prepost,$(out_prog_path)/,$(basename $(filter %.c %.cpp, $(lintplus_src))),.lin)
lintplus_src_with_app_path := $(addprefix $(app_path)/prog/,$(lintplus_src))
#lint_lobs   = $(lintplus_files:.lin=.lob)


lintplus_build_path = common/build/lint
lintplus_indirect_path_file = $(out_path)/$(variant_base)/search_path_lintplus.lnt

lintplusopt += $(addprefix -D,$(defines_tool)) -D__analysis__
lintplusopt += $(addprefix -i,$(search_path_notcc_cmsis))
lintplusopt += $(addprefix -i,$(search_path_implicit))
lintplus_search_path += common/prog/dbus/cfg_templates
lintplus_search_path += common/prog/ped_fw/basic
lintplus_search_path += common/prog/ped_fw/schedule
lintplus_search_path += common/prog/ped_fw/stdcrc
lintplus_search_path += common/prog/ped_fw/seg
lintplus_search_path += common/prog/firmware_update/BootManager
lintplus_search_path += common/prog/FuncSafe
lintplus_search_path += $(app_path)/prog/FuncSafe
lintplusopt += $(addprefix -i,$(lintplus_search_path))
#lintplusopt += -i$(LINT_PATH) -i$(LINT_PATH)\Lnt

#library classification
lintplusopt += "+libclass(angle, ansi)"
#library folders and modules
lintplusopt += "+libdir(common/prog/MSP/compiler*)"
lintplusopt += "+libdir(common/prog/MSP/CMSIS*)"
# third party
lintplusopt += "+libm(*SEGGER_RTT.c)"
lintplusopt += "+libdir($(ansi_cc_path)*)"

#enable ansi extensions, warning is ansi since C/C++ 2023)
lintplusopt += +ppw(include_next)
lintplusopt += +ppw(warning)

# libraries defined by +libm will processed completely (no implicit issues masking)
lintplusopt += +flf

# enable include behaviour like supported by our compilers
# include file will be searched first in folder of including file
lintplusopt += +fdi

# write Warning, Error and Info with upper case like in former lint versions
lintplusopt += +fcc

# compiler specific stuff
lintplusopt += $(cc_lint_options)
lintplusopt += -unit_check

lintplusopt_c += $(lintplusopt)
lintplusopt_c += "+libh(*$(notdir $(preinclude_c)))" -header($(preinclude_c))
lintplusopt_c += $(lintplus_build_path)/$(misraplus).lnt
lintplusopt_c += $(lintplus_build_path)/GenericAdaptionslp.lnt
lintplusopt_c += $(lintplus_build_path)/globallp.lnt


# no CPP messages for C 
lintplusopt_c += -e1756

#  Warning 613: potential use of null pointer: creates some false positives
lintplusopt_c += -e613

# Warning 523: expression statement involving function 'xxx' lacks side effects: creates some false positives
lintplusopt_c += -e523





lintplus_common_folder_opt += -e2662 
#-e2666

ifdef deprecated_platform
# following messages came with newer lint version and will not be considered for deprecated platforms
# Note 9046: 'ulValue' is typographically ambiguous with respect to 'uiValue' when ignoring case and treating 'l' as 'I' [MISRA 2012 Directive 4.5, advisory]
# Note 9031: cannot assign a composite expression of type 'unsigned8' to an object of wider type 'unsigned32' [MISRA 2012 Rule 10.6, required]
# Note 931: both side have side effects
lintplus_common_folder_opt += -e931 -e9046 -e9031
endif

lintplusopt_cpp += $(lintplusopt)
lintplusopt_cpp += -std=c++17
lintplusopt_cpp += +libh(*$(notdir $(preinclude_cpp))) -header($(preinclude_cpp))
lintplusopt_cpp += $(lintplus_build_path)/au-misra-cpp.lnt
lintplusopt_cpp += $(lintplus_build_path)/GenericAdaptionslp.lnt
lintplusopt_cpp += +rw(__typeof__)
lintplusopt_cpp += $(lintplus_build_path)/globallp.lnt

# this is needed because of mixed code, currently we are using the c-options to link.
lint_link_opt += +rw(__typeof__)
# MISRA 11.9 The macro NULL shall be the only permitted form of integer null pointer constant
# 910 creates false positives even when '--emacro((910),NULL)' is used (linking only)
lint_link_opt += -e910

.PHONY: lintprocess
lintprocess: link
	$(MAKE) -f $(firstword $(MAKEFILE_LIST)) lint $(MFLAGS) -s

lint: $(out_lintplus_file)
lintplus: $(out_lintplus_file)

# target used in Eclipse
.PHONY: lint2console
lint2console: lint
	@type $(subst /,\,$(abspath $(out_lintplus_file)))
	@type $(subst /,\,$(abspath $(lintplus_files)))

# Creation of indirect file containing search paths
# Hint: Lint needs include paths in separate lines, spaces are substituted to new lines.
$(lintplus_indirect_path_file):
	@$(file > $@,$(subst $(space),$(newline),$(addprefix -i,$(search_path) $(ansi_system_paths))))

# Hint: Lint writes infos to stderr, this is why lint target redirect error output to  nul!
#@$(lint) $(lintplusopt) list_of_all_lintplus_srcs -max_threads=0 > $@ 2>nul
$(out_lintplus_file): printstartlp $(lintplus_files) $(lintplus_indirect_path_file)
#	@echo %time%
	@$(call prntGr,------------- Linking lintplus -------------)
	@$(perl) $(local_tool_path)/mixed/deduplication.pl $(out_implicit_searchpath_file) > $(out_implicit_searchpath_file_dd)
	@$(perl) $(local_tool_path)/mixed/deduplication.pl $(out_lintplus_sources_file) > $(out_lintplus_sources_file_dd)
	@$(lint) -zero $(lintplusopt_c) $(lint_link_opt) -max_threads=0  $(lintplus_indirect_path_file) $(out_implicit_searchpath_file_dd) $(out_lintplus_sources_file_dd) > $@ 2>nul
	@$(call copy,$(out_lintplus_sources_file_dd), $(out_lintplus_sources_file))
	@$(call copy,$(out_implicit_searchpath_file_dd), $(out_implicit_searchpath_file))
	@$(call del, $(out_lintplus_sources_file_dd))
	@$(call del, $(out_implicit_searchpath_file_dd))
	@$(call prntGr,--------------- Linting done ---------------)

dep_target_ext += .lin

printstartlp:
	@$(call prntGr,------------ Starting lintplus -------------)
	@echo %time%
	@$(call mkdir, $(dir $(out_lintplus_path)))

.PHONY: lintreport
lintreport: lint
	@$(call prntGr,----------- Parsing lint files -------------)
	@$(perl) $(local_tool_path)/mixed/reportLint.pl $(out_path)/$(variant_base)


# ***************************************************************************************************************
# Implicit rules for lint
# ***************************************************************************************************************
$(out_prog_path)/%.lin : common/prog/%.c $(varHeader) $(lintplus_indirect_path_file) | $(code_gen_log) printstartlp
	$(call mkdir,$(dir $@))
ifdef preproc
	@$(lint) -va -p $(lintplusopt_c) $(lintplus_common_folder_opt) $(lintplus_indirect_path_file) $(lintopt_component) $< > $(basename $@).lin.p 2>nul
endif
	@$(lint) $(lintplusopt_c) $(lintplus_common_folder_opt) $(c_lint_predef) $(lintplus_indirect_path_file) $(lintopt_component) $< > $(basename $@).lin 2>nul
	@$(perl) $(local_tool_path)/mixed/ParseLint.pl $@
	@$(file >> $(out_lintplus_sources_file), $<)
	@$(file >> $(out_implicit_searchpath_file), $(addprefix -i,$(search_path_implicit)))
ifeq ($(dc_component_present), true)
ifeq ($(approbation), true)
	@$(python) $(dc_mixed_path)/python_scripts/ParseLint.py $@ $(shell $(git) hash-object $<) $(out_prog_path)/lint_hash.txt
endif
endif
ifeq ($(LintOutput), true)
	@$(perl) $(local_tool_path)/mixed/ParseLint.pl $@ >> $(out_prog_path)/LintOut.txt
endif

$(out_prog_path)/%.lin : %.c $(varHeader) $(lintplus_indirect_path_file) | $(code_gen_log) printstartlp
	$(call mkdir,$(dir $@))
ifdef preproc
	@$(lint) -va -p $(lintplusopt_c) $(lintplus_indirect_path_file) $(lintopt_component) $< > $(basename $@).lin.p 2>nul
endif
	@$(lint) $(lintplusopt_c) $(c_lint_predef) $(lintplus_indirect_path_file) $(lintopt_component) $< > $(basename $@).lin 2>nul
	@$(perl) $(local_tool_path)/mixed/ParseLint.pl $@
	@$(file >> $(out_lintplus_sources_file), $<)
	@$(file >> $(out_implicit_searchpath_file), $(addprefix -i,$(search_path_implicit)))
ifeq ($(dc_component_present), true)
ifeq ($(approbation), true)
	@$(python) $(dc_mixed_path)/python_scripts/ParseLint.py $@ $(shell $(git) hash-object $<) $(out_prog_path)/lint_hash.txt
endif
endif
ifeq ($(LintOutput), true)
	@$(perl) $(local_tool_path)/mixed/ParseLint.pl $@ >> $(out_prog_path)/LintOut.txt
endif

$(out_prog_path)/%.lin : %.cpp $(varHeader) $(lintplus_indirect_path_file) | $(code_gen_log) printstartlp
	$(call mkdir, $(dir $@))
ifdef preproc
	@$(lint) -va -p $(lintplusopt_cpp) $(lintplus_indirect_path_file) $(lintopt_component) $< > $(basename $@).lin.p 2>nul
endif
	@$(lint) $(lintplusopt_cpp) $(lintplus_indirect_path_file) $(lintopt_component) $< > $(basename $@).lin 2>nul
	@$(perl) $(local_tool_path)/mixed/ParseLint.pl $@ 
	@$(file >> $(out_lintplus_sources_file), $<)
	@$(file >> $(out_implicit_searchpath_file), $(addprefix -i,$(search_path_implicit)))
ifeq ($(dc_component_present), true)
ifeq ($(approbation), true)
	@$(python) $(dc_mixed_path)/python_scripts/ParseLint.py $@ $(shell $(git) hash-object $<) $(out_prog_path)/lint_hash.txt
endif
endif
ifeq ($(LintOutput), true)
	@$(perl) $(local_tool_path)/mixed/ParseLint.pl $@ >> $(out_prog_path)/LintOut.txt
endif

ifeq ($(dc_component_present), true)
$(out_prog_path)/%.h.lin : %.h $(varHeader)
	$(call mkdir, $(dir $@))
	@echo Header > $(basename $@).lin
	@$(python) $(dc_mixed_path)/python_scripts/ParseLint.py $@ $(shell $(git) hash-object $<) $(out_prog_path)/lint_hash.txt

$(out_prog_path)/%.S.lin : %.S $(varHeader)
	$(call mkdir, $(dir $@))
	@echo Header > $(basename $@).lin
	@$(python) $(dc_mixed_path)/python_scripts/ParseLint.py $@ $(shell $(git) hash-object $<) $(out_prog_path)/lint_hash.txt
endif

