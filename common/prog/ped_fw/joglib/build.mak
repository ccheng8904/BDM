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
#  Description      build include PED Framework, subcomponent: joglib
#*******************************************************************************

#protection guard preventing multiple inclusion
ifndef joglib_build_mak
joglib_build_mak := 1

ifeq ($(marked_comp),ped_fw/joglib)
    obj_test = joglib joglibXS
endif


ifeq ($(std_variant),true)
    obj     += ped_fw/joglib/joglib
    cfg_obj += ped_fw/joglib/joglibXS
    cfg_hdr += ped_fw/joglib/joglibXS.h
endif

doxy_image_path_ += ped_fw/joglib/doc

#end of protection guard preventing multiple inclusion
endif
