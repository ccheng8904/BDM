#*******************************************************************************
#  Copyright (c) 2017 BSH Hausgeraete GmbH,
#  Carl-Wery-Str. 34, 81739 Munich, Germany, www.bsh-group.de
#
#  All rights reserved. This program and the accompanying materials
#  are protected by international copyright laws.
#  Please contact copyright holder for licensing information.
#
#*******************************************************************************
#  PROJECT          Generic SW
#*******************************************************************************
#  Description      build include for component: standard library
#*******************************************************************************

#start of include protection guard
ifndef std_lib_build_mak
std_lib_build_mak := 1

lib_src    += std_lib/std_version.c
ut_lib_src += std_lib/std_callback.c


ifeq ($(unit_test_variant), true)
    src    += std_lib/test/std_callback.mock.cpp
endif


#end of include protection guard
endif
