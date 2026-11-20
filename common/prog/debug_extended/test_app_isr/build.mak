#*******************************************************************************
#  Copyright (c) 2017 BSH Hausgeraete GmbH,
#  Carl-Wery-Str. 34, 81739 Munich, Germany, www.bsh-group.de
#
#  All rights reserved. This program and the accompanying materials
#  are protected by international copyright laws.
#  Please contact copyright holder for licensing information.
#
#*******************************************************************************
#  PROJECT          Generic SW
#*******************************************************************************
#  Description      build include for dbus_board component
#*******************************************************************************

#protection guard preventing multiple inclusion
ifndef dbgx_test_app_isr_mak
dbgx_test_app_isr_mak := 1

ifdef dbgx_test_app_isr
    obj          += debug_extended/test_app_isr/dbgx_test_app_isr
    code_gen_cfg += debug_extended/test_app_isr/config.json
endif

#end of protection guard preventing multiple inclusion
endif
