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
#  Description      build include PED Framework, subcomponent: testSupport
#*******************************************************************************

#protection guard preventing multiple inclusion
ifndef testsupport_build_mak
testsupport_build_mak := 1


obj     += ped_fw/testSupport/TestSupport
cfg_obj += ped_fw/testSupport/TestSupportXS
cfg_hdr += ped_fw/testSupport/TestSupportXS.h
ped_fw_testsupport = true

#end of protection guard preventing multiple inclusion
endif
