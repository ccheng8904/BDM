#*******************************************************************************
#  Copyright (c) 2017 BSH Hausgeraete GmbH,
#  Carl-Wery-Str. 34, 81739 Munich, Germany, www.bsh-group.de
#
#  All rights reserved. This program and the accompanying materials
#  are protected by international copyright laws.
#  Please contact copyright holder for licensing information.
#
#*******************************************************************************
#  PROJECT          PP PED FW
#*******************************************************************************
#  Description      build include PED Framework, subcomponent: utility
#*******************************************************************************

#protection guard preventing multiple inclusion
ifndef utility_build_mak
utility_build_mak := 1

ifeq ($(marked_comp),ped_fw/utility)
    obj_test = utility
endif


ifeq ($(std_variant),true)
    obj += ped_fw/utility/utility
endif

#end of protection guard preventing multiple inclusion
endif
