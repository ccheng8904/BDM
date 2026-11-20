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
#  Description      handling of testincludes and stub_path
#*******************************************************************************

ifdef test_comp_kind
    ifeq ($(test_comp_kind), ext)
        stub_pre ?= ext/
        test_ext_components += $(marked_comp)
    else ifeq ($(test_comp_kind), app)
        stub_pre ?= $(app_path)/prog/
        test_app_components += $(marked_comp)
    else
        stub_pre ?= common/prog
        test_components += $(marked_comp)
    endif
else
    stub_pre ?= common/prog
    test_components += $(marked_comp)
endif


ifdef stubtrace_user
    stub_trace ?= $(stubtrace_user)
else
    stub_trace ?= $(marked_comp)
endif


stub_trace_add ?= $(platform)

stub_path ?= $(stub_pre)/$(stub_trace)/$(stub_trace_add)/Stubs

ifeq ($(unit_test),true)
    $(info --- Using stub_path: $(stub_path) ---)
    lib_src += $(call prepost, $(stub_path)/Testframes/,$(obj_test),_testframe.c)
    lib_src += $(addprefix $(stub_path)/,GenericStub.c Common/CommonStub.c)
    search_path += $(stub_path)
    search_path += common/prog/$(marked_comp)
    search_path += common/prog/$(marked_comp)/cfg_templates
    ifdef implementation
        search_path += common/prog/$(marked_comp)/$(implementation)
        search_path += common/prog/$(marked_comp)/cfg_templates/$(implementation)
    endif
    
    ifeq ($(platform), Intel)
        ifneq ($(platform),$(implementation))
            search_path += $(msp_path)/register_stubs/$(implementation)
            search_path += $(msp_path)/CMSIS/
            lib_src += $(addprefix $(msp_path)/CMSIS/,$(core_stubfiles))
            lib_src += $(addprefix $(msp_path)/register_stubs/$(implementation)/,$(register_stubfile))
        endif
    endif
endif
