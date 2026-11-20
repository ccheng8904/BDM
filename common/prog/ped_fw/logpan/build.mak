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
#  Description      build include PED Framework, subcomponent: logpan
#*******************************************************************************

#protection guard preventing multiple inclusion
ifndef logpan_build_mak
logpan_build_mak := 1

ifeq ($(marked_comp),ped_fw/logpan)
    obj_test = logPan logPanXS
endif


ifeq ($(std_variant),true)
    obj     += ped_fw/logpan/logPan
    cfg_obj += ped_fw/logpan/logPanXS
    cfg_hdr += ped_fw/logpan/logPanXS.h
endif

doxy_image_path_ += ped_fw/logpan/doc

#end of protection guard preventing multiple inclusion
endif

