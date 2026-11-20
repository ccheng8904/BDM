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
#  Description      definition for test application
#*******************************************************************************

testInstrument_base := $(notdir $(instrument))
testInstrument_dir  := $(patsubst %/,%,$(dir $(instrument)))

ifdef testInstrument_base
    stub_trace_add  := $(call match,$(notdir $(testInstrument_base)),^UT_(\w+)$$)
    
    testInstrument = true
    
    #$(info --- Creating test instrumentation for Comp: $(variant_comp), Subcomp: $(variant_subcomp), with settings from platform: $(platform) in $(stub_trace_add) ---)
    
endif

ifdef testInstrument_dir
    ifneq ($(testInstrument_dir),.)
        implementation := $(testInstrument_dir)
        testInstrument = true
    endif
endif