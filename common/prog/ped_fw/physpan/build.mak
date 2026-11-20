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
#  Description      build include PED Framework, subcomponent: physpan
#*******************************************************************************

#protection guard preventing multiple inclusion
ifndef physpan_build_mak
physpan_build_mak := 1

ifeq ($(marked_comp),ped_fw/physpan)
    obj_test = Phypdigi Phypcapa PhyCapXS PhyDigXS PhyComXS
endif


ifeq ($(std_variant),true)
    obj     += $(addprefix ped_fw/physpan/,Phypdigi Phypcapa)
    cfg_obj += $(addprefix ped_fw/physpan/,PhyCapXS PhyDigXS PhyComXS)
    cfg_hdr += $(addprefix ped_fw/physpan/,PhyXS.h)
endif

doxy_image_path_ += ped_fw/physpan/doc

#end of protection guard preventing multiple inclusion
endif
