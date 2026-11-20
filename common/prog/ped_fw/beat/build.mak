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
#  Description      build include PED Framework, subcomponent: beat
#*******************************************************************************

#protection guard preventing multiple inclusion
ifndef beat_build_mak
beat_build_mak := 1

ifeq ($(marked_comp),ped_fw/beat)
    obj_test = beat beatXS
endif


ifeq ($(std_variant),true)
    obj     += ped_fw/beat/beat
    cfg_obj += ped_fw/beat/beatXS
endif

doxy_image_path_ += beat/doc

#end of protection guard preventing multiple inclusion
endif


