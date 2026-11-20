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
#  Description      build include for math component
#*******************************************************************************

#protection guard preventing multiple inclusion
ifndef math_build_mak
math_build_mak := 1


ut_lib_src += math/mathDiv.c


#end of protection guard preventing multiple inclusion
endif
