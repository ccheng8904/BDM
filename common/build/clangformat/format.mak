#*******************************************************************************
#  Copyright (c) 2017 BSH Hausgeraete GmbH,
#  Carl-Wery-Str. 34, 81739 Munich, Germany, www.bsh-group.de
#
#  All rights reserved. This program and the accompanying materials
#  are protected by international copyright laws.
#  Please contact copyright holder for licensing information.
#
#*******************************************************************************
#  Project          MCU FW
#*******************************************************************************
#  Description      settings and targets for clang-format
#*******************************************************************************

# ***************************************************************************************************************
# Settings for Format
# ***************************************************************************************************************
format_script = $(python) $(CURDIR)/common/build/clangformat/format.py
no_code_filter_format = $(subst $(common_rel_path),,$(no_code_analysis_format))
mid_no_code_filter_format = $(subst $(common_rel_path),,$(no_code_filter_format))
final_no_code_filter_format = $(subst $(app_rel_path),,$(mid_no_code_filter_format))
file_name = NONE
ifdef filename
    file_name = $(filename)
endif
ifeq ($(final_no_code_filter_format),$(empty))
    final_no_code_filter_format = NONE
endif
format_search_path = $(CURDIR)/common/prog	

format : 
	@$(call prntGr,******************  Formatting  started  ********************)  	
	@$(format_script) --filename $(file_name) --platform_name $(platform) --format_tool $(clang_formatter) --formatsearchpath $(format_search_path) --no_code_analysis $(final_no_code_filter_format)
	
	@$(call prntGr,******************  Formatting  Completed  ******************)
		

		
	