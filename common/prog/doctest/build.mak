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
#  Description      build include unit test framework doctest
#*******************************************************************************

#protection guard preventing multiple inclusion
ifndef doctest_build_mak
doctest_build_mak := 1

no_code_analysis += doctest

src += doctest/main.cpp


#end of protection guard preventing multiple inclusion
endif

