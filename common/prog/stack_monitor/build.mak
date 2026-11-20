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
#  Description      Build include for stack monitor component.
#                   This component is used for monitoring stack usage at runtime.
#*******************************************************************************

#protection guard preventing multiple inclusion
ifndef stack_monitor_build_mak
stack_monitor_build_mak := 1

ifneq ($(uc_platform),RL78)
    defines += STACK_MONITOR_ENABLED
    src     += stack_monitor/stack_monitor.c
    code_gen_cfg += stack_monitor/config.json
else
    $(info Stack monitor is not available for RL78)
endif

# end of protection guard preventing multiple inclusion
endif
