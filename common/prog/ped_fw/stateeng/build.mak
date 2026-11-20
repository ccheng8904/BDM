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
#  Description      build include PED Framework, subcomponent: stateeng
#*******************************************************************************

#protection guard preventing multiple inclusion
ifndef stateeng_build_mak
stateeng_build_mak := 1

ifeq ($(marked_comp),ped_fw/stateeng)
    obj_test = stateeng stateXS
endif


ifeq ($(std_variant),true)
    ifdef tinyState
        obj += ped_fw/stateeng/tiny_stateeng
    else
        obj += ped_fw/stateeng/stateeng
    endif

    cfg_obj += ped_fw/stateeng/stateXS
endif

doxy_image_path_ += ped_fw/stateeng/doc

#end of protection guard preventing multiple inclusion
endif
