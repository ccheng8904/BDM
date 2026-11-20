#*******************************************************************************
#  Copyright (c) 2024 BSH Hausgeraete GmbH,
#  Carl-Wery-Str. 34, 81739 Munich, Germany, www.bsh-group.de
#
#  All rights reserved. This program and the accompanying materials
#  are protected by international copyright laws.
#  Please contact copyright holder for licensing information.
#
#*******************************************************************************
#  PROJECT          PP PED FW
#*******************************************************************************
#  Description      build include PED Framework, subcomponent: basic
#*******************************************************************************

#protection guard preventing multiple inclusion
ifndef tau_build_mak
tau_build_mak := 1

basicXs = tau/basicXs


#src += tau/systick_stopwatch_utest.cpp
src += tau/systick_stopwatch_isr.c
src += tau/systick_stopwatch.c


#end of protection guard preventing multiple inclusion
endif

