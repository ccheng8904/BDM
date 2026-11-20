#*******************************************************************************
#  Copyright (c) 2024 BSH Hausgeraete GmbH,
#  Carl-Wery-Str. 34, 81739 Munich, Germany, www.bsh-group.de
#
#  All rights reserved. This program and the accompanying materials
#  are protected by international copyright laws.
#  Please contact copyright holder for licensing information.
#
#*******************************************************************************
#  PROJECT          Generic SW
#*******************************************************************************
#  Description      Unit test execution and test coverage
#*******************************************************************************
ifndef UTEST_COV_MAK
UTEST_COV_MAK = 1

ifeq ($(cc_build_path),gccIntel)

gcovr_exclude  += .*doctest.h
gcovr_exclude  += .*dtest.*
gcovr_exclude  += .*mock.*

gcovr_opt += --gcov-executable "$(gvov) -j 4"
gcovr_opt += $(addprefix --gcov-exclude ,$(gcovr_exclude))
gcovr_opt += $(addprefix --filter ,$(ut_cov_filter))
gcovr_opt += --merge-mode-functions=merge-use-line-min
gcovr_opt += --html-absolute-paths



.PHONY: utest clean_gcov coverage cov-view

# unit test target, just execute the target (recreate if required)
utest: $(linker_bin) clean_gcov
	@echo executing unit test
	@$(linker_bin) $(utest_opt)

clean_gcov:
	@echo removing obsolete coverage results
	@$(call delfiles_recursive, $(out_path)/*.gcda)
	@$(call delfile, *.gcov)


coverage_out = $(out_path)/$(variant)/coverage/$(variant_base)_cov.html
coverage: $(coverage_out)


$(coverage_out): utest
	@$(call prntGr,--------Creating coverage report-------------)
	@$(call mkdir, $(dir $@))
	@gcovr $(gcovr_opt) --html-details $@ $(out_path)/$(variant) -s
	@$(call delfile, *.gcov)


else
utest: $(linker_bin)
endif

cov-view: $(coverage_out)
	@start $(coverage_out) >nul 2>&1

endif #UTEST_COV_MAK
