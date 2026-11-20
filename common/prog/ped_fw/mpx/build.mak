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
#  Description      build include PED Framework, subcomponent: mpx
#*******************************************************************************

#protection guard preventing multiple inclusion
ifndef mpx_build_mak
mpx_build_mak := 1

ifeq ($(marked_comp),ped_fw/mpx)
    obj_test = mpx mpxXs
endif


ifeq ($(std_variant),true)
    obj     += ped_fw/mpx/mpx
    cfg_obj += ped_fw/mpx/mpxXs
    cfg_hdr += ped_fw/mpx/mpxXs.h
endif

doxy_image_path_ += ped_fw/mpx/doc

#end of protection guard preventing multiple inclusion
endif
