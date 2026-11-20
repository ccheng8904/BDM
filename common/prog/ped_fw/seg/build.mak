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
#  Description      build include PED Framework, subcomponent: seg
#*******************************************************************************

#protection guard preventing multiple inclusion
ifndef seg_build_mak
seg_build_mak := 1

ifeq ($(marked_comp),ped_fw/seg)
    obj_test = seg segDflt segXs
endif


ifeq ($(std_variant),true)
    obj     += $(addprefix ped_fw/seg/,seg segDflt)
    cfg_obj += $(addprefix ped_fw/seg/,segXs)
    cfg_hdr += $(addprefix ped_fw/seg/,segXs.h)
endif

doxy_image_path_ += ped_fw/seg/doc

#end of protection guard preventing multiple inclusion
endif
