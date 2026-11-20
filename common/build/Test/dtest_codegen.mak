#*******************************************************************************
#  Copyright (c) 2017 BSH Hausgeraete GmbH,
#  Carl-Wery-Str. 34, 81739 Munich, Germany, www.bsh-group.de
#
#  All rights reserved. This program and the accompanying materials
#  are protected by international copyright laws.
#  Please contact copyright holder for licensing information.
#
#*******************************************************************************
#  Project          Generic SW
#*******************************************************************************
#  Description      targets for doctest code generator
#*******************************************************************************
# usage of doctest code generator:
# make gen_doc_test header=<header_file> inline_mock=true/false
# in case of inline_mock=true, mock functions are generated for inline functions
# the given header file is searched in the defined search_folders
#*******************************************************************************


inline_mock ?= false
header ?= not_specified
doctest_gen_files_py = $(local_tool_path)/python/generate_doctest_files.py

ifeq ($(inline_mock), true)
    gen_doc_test_opt  = -s
endif

search_path_doc_test = $(sdk_prog_path) $(sdk_prog_path)/firmware_update $(search_path) $(sdk_prog_path)/MSP/mcal

gen_doc_test_opt += -i $(header)
gen_doc_test_opt +=  $(addprefix -f ,$(search_path_doc_test))

gen_doc_test:
	@echo Generating doctest files for header=$(header)...
	@$(choco_python) $(doctest_gen_files_py) $(gen_doc_test_opt)


