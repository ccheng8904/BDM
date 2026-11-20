#*******************************************************************************
#  Copyright (c) 2017 BSH Hausgeraete GmbH,
#  Carl-Wery-Str. 34, 81739 Munich, Germany, www.bsh-group.de
#
#  All rights reserved. This program and the accompanying materials
#  are protected by international copyright laws.
#  Please contact copyright holder for licensing information.
#
#*******************************************************************************
#  PROJECT          PP GED FW
#*******************************************************************************
#  Description      build include GED Framework, subcomponent: watchdogtimer
#*******************************************************************************

#protection guard preventing multiple inclusion
ifndef watchdogtimer_build_mak
watchdogtimer_build_mak := 1

# Watchdog handle function configuration
code_gen_cfg += ped_fw/watchdogtimer/config.json

obj     += $(addprefix ped_fw/watchdogtimer/, watchdogtimer)

defines += WATCHDOG_INCLUDED
doxy_image_path_ += ped_fw/watchdogtimer/doc

#end of protection guard preventing multiple inclusion
endif
