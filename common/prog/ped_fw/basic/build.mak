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
#  Description      build include PED Framework, subcomponent: basic
#*******************************************************************************

#protection guard preventing multiple inclusion
ifndef basic_build_mak
basic_build_mak := 1

ifdef basicXs
    obj     += $(basicXs)
else
    # default basicXS
    cfg_obj += ped_fw/basic/basicXs
endif

ifdef main_src
    src     += $(main_src)
else
    # default main
    obj     += $(addprefix ped_fw/basic/,main)
endif

cfg_obj += ped_fw/basic/$(platform)/basicMcXs

#end of protection guard preventing multiple inclusion
endif

