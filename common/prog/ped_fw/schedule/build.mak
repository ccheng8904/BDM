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
#  Description      build include PED Framework, subcomponent: schedule
#*******************************************************************************

#protection guard preventing multiple inclusion
ifndef schedule_build_mak
schedule_build_mak := 1

defines += SCHEDULER_OLD

# Scheduler variant selection
# - manual: Scheduler tasks are added manually
# - auto: Scheduler handle tasks are added automatically based on included components
scheduler_config ?= manual

ifeq ($(marked_comp),ped_fw/schedule)
    obj_test = schedule scheduXS schedDef
    add_stubfunc += BAL_HandleTask DBPL_HandleTask DLL_HandleTask TIM_bHandleTask16
    stub_trace = ped_fw/schedule/
    ifeq ($(unit_test),true)
        defines += STM32F071
        obj += MSP/Intel/CMSIS/core_cm0
    endif
endif


ifeq ($(std_variant),true)
    obj     += ped_fw/schedule/schedule
    cfg_obj += ped_fw/schedule/scheduXS
    
    ifeq ($(scheduler_config),manual)
        cfg_src += ped_fw/schedule/schedDef.c
    else ifeq ($(scheduler_config),auto)
        defines += SCHEDULE_CONFIG_AUTO
        gen_src += ped_fw/schedule/$(variant)/schedDef_auto.c
        gen_hdr += ped_fw/schedule/$(variant)/schedDef_auto.h
    else
        $(error Unsupported scheduler confguration $(scheduler_config))
    endif
    
    cfg_hdr += ped_fw/schedule/schedDef.h
endif


doxy_image_path_ += ped_fw/schedule/doc

#end of protection guard preventing multiple inclusion
endif
