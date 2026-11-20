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
#  Description      Build include for debug component.
#                   This component is used for debug purposes only and must not
#                   be included in production release. Object files for this
#                   component are not part of system library.
#*******************************************************************************

#protection guard preventing multiple inclusion
ifndef debug_build_mak
debug_build_mak := 1

#for debug interface configuration, see "debug_api_config.h" file

nolib_src   += debug/debug_api.c
cfg_hdr     += debug/debug_api_config.h

ifneq ($(platform), Intel)
    nolib_src   += debug/RTT/SEGGER_RTT.c
    cfg_hdr     += debug/RTT/SEGGER_RTT_Conf.h
endif


# Self installation for debug component
# This is done here instead of explaining how to install debug component for tau-testing
$(sdk_prog_path)/debug/debug_api.c: $(app_prog_path)/debug/debug_api_config.h

$(app_prog_path)/debug/debug_api_config.h:
	@if not exist "$@" $(call copy,$(sdk_prog_path)/debug/cfg_templates/debug_api_config.h,$@)

common/prog/debug/RTT/SEGGER_RTT.c: $(app_prog_path)/debug/RTT/SEGGER_RTT_Conf.h

$(sdk_prog_path)/debug/RTT/SEGGER_RTT.c: $(app_prog_path)/debug/RTT/SEGGER_RTT_Conf.h

$(app_prog_path)/debug/RTT/SEGGER_RTT_Conf.h:
	@if not exist "$@" $(call copy,$(sdk_prog_path)/debug/RTT/cfg_templates/SEGGER_RTT_Conf.h,$@)


# end of protection guard preventing multiple inclusion
endif
