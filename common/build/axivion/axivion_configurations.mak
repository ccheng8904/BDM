#*******************************************************************************
#  Copyright (c) 2017 BSH Hausgeraete GmbH,
#  Carl-Wery-Str. 34, 81739 Munich, Germany, www.bsh-group.de
#
#  All rights reserved. This program and the accompanying materials
#  are protected by international copyright laws.
#  Please contact copyright holder for licensing information.
#
#*******************************************************************************
#  Project          MCU-Framework
#*******************************************************************************
#  Description      Configuration for CafeCC/stylecheck options 
#*******************************************************************************

ifdef UTEST_TYPE_SYSTEM
    axcopt += --type_system $(UTEST_TYPE_SYSTEM)
    ax_pstd_gen_opt = -y $(UTEST_TYPE_SYSTEM)
endif
axcopt += --c 
axcopt += --c99
axcppopt += --c++
axcppopt += --c++14
axivionopt += $(addprefix -D,$(defines_tool)) -D__analysis__
# __STDC_VERSION__ defined to be at least C99
axcopt += -D__STDC_VERSION__=199901L
ifdef unit_test_pc
    axcopt += -D UNIT_TEST_PC
    axcppopt += -D UNIT_TEST_PC
endif
# switch off some messages for cafaCC, it does not effect style check!!
# Warning: #550-D: variable "i" was set but never used
# error #230: nonstandard type for a bit field
# error #96: a translation unit must contain at least one declaration
# warning #175-D: subscript out of range, (problems with library buffers)
axcopt += --diag_suppress 1223,550,177,951,230,96,175,147,148,757,28,169,20,59,101,18,65,70
axcopt += --display_error_number 

axivionopt += $(addprefix -I,$(search_path_implicit))
axivionopt += -B$(CURDIR)
axivionopt += $(addprefix --sys_include=,$(search_path_notcc))
axivionopt += $(addprefix -I,$(search_path))
ifeq ($(cc_build_path), gccIntel)
    axivionopt += $(addprefix --sys_include=,$(cc_system_path))
    axcppopt += $(addprefix --sys_include=,$(ccpp_system_path))
endif


ax_exclude_pattern += "$(search_path_notcc)/*.*" "$(app_path)/prog/$(variant).h" common/prog/MSP/compiler/$(cc_build_path)/*.* $(msp_processor_includes) $(addsuffix /*.*,$(no_code_analysis)) $(THIRD_PARTY_LIB) 
ax_exclude_opt = $(addprefix -exclude ,$(ax_exclude_pattern))

ifneq (,$(filter $(MAKECMDGOALS),uti))
    # set switch for target ax_uti
    defines += UNIT_TEST_INST
endif
