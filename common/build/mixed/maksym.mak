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
#  Description      settings and targets for PatchBin
#*******************************************************************************

sym_base = $(bin_path)/$(variant_base)
sym_file ?= $(sym_base).sym
syc_file ?= $(sym_base).syc
maksym_psyd = $(sym_base)_psyd.xml
metric_path = $(bin_path)/metric

ifdef no_maksym
maksym: $(linker_bin)

$(sym_file):

else
maksym: $(sym_file)

$(sym_file): $(linker_bin)
	@echo creating metric file...
	@$(maksym) -METRIC $(dwarf_file) >$(sym_base).mtr
	@echo creating symbols and registry...
	@$(maksym) -TENG=Register $(dwarf_file) $(test_modules) >$(sym_base).syr
	@$(maksym) -PSYD=SIGN $(dwarf_file) > $(maksym_psyd)
	@$(maksym) -TENG=Release $(dwarf_file) $(test_modules) >$(sym_base).rel
	@$(maksym) -TENG=Register $(dwarf_file) $(sym_modules) -SIGNVER 2008F >$(sym_base).sym 
	@$(maksym) -SYC $(dwarf_file) $(sym_modules) > $(sym_base).syc
	@echo creating usage report...
	@$(call mkdir, $(metric_path))
	@$(maksym) -METRIC=XML $(dwarf_file) > $(metric_path)/$(variant_base).xml
	@$(repgen) -L$(metric_path)/ReportGen.log -silent1 -t$(repgen_path)\metric_v4.xslt -i$(metric_path)\$(variant_base).xml -o$(metric_path)/$(variant_base)_memory_usage.html

endif
