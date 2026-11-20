#*******************************************************************************
#  Copyright (c) 2017 BSH Hausgeraete GmbH,
#  Carl-Wery-Str. 34, 81739 Munich, Germany, www.bsh-group.de
#
#  All rights reserved. This program and the accompanying materials
#  are protected by international copyright laws.
#  Please contact copyright holder for licensing information.
#
#*******************************************************************************
#  PROJECT          PP PED FW / GCC / Mingw
#*******************************************************************************
#  Description      build include for gccX86 (intel)
#*******************************************************************************

# default path for cc-build system
cc_build_path ?= gccIntel
core_path      = intel

ifdef testClient
    python_lib_path = $(python_path)/Libs
    search_path    += $(python_path)/Include
    obj     += $(msp)/TEngAppl/TEngAppl
    lib     += $(python_lib_path)/python35.lib
endif


ifeq ($(unit_test_variant), true)
    lib_src    += $(msp)/test/hsup.mock.cpp
    lib_src    += $(msp)/hsup.c
    ut_lib_src += $(msp)/hsup.c
    # must be done here additionally, because hsup is tested in global namespace
    # the common mechanism to set the coverage 
    hsup_copt    =--coverage
    
endif


# currently no HW specific stuff
