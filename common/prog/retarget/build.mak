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
#  Description      build include for retarget component
#*******************************************************************************

#protection guard preventing multiple inclusion
ifndef retarget_build_mak
retarget_build_mak := 1

nolib_src += retarget/retarget.c

ifneq (,$(filter $(cc_build_path), armclang keilArm))
    nolib_src += retarget/retarget_ARM.c
else ifeq ($(cc_build_path), gccArm)
    nolib_src += retarget/retarget_GCC.c
else ifneq (,$(filter $(cc_build_path), iarRx))
    nolib_src += retarget/retarget_IAR.c
else
    $(error Unsupported compiler for component "retarget")
endif

#end of protection guard preventing multiple inclusion
endif
