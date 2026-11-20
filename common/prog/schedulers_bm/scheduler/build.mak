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
#  Description      build include GED Framework, subcomponent: scheduler
#*******************************************************************************

#protection guard preventing multiple inclusion
ifndef scheduler_build_mak
scheduler_build_mak := 1

defines += SCHEDULER

# Scheduler variant selection
# - manual: Scheduler tasks are added manually
# - auto: Scheduler handle tasks are added automatically based on included components
scheduler_config ?= manual

ifeq ($(marked_comp),schedulers_bm/scheduler)
    obj_test = scheduler_time_monitor scheduler scheduler_cfg
    add_stubfunc += void:TIM_handleTask16:void void:BAL_handleTask:void void:DBPL_handleTask:void void:DLL_handleTask:void void:WDT_handleTask:void HSUP_vEnableInt HSUP_vDisableInt 
    ifeq ($(unit_test),true)
        search_path += common/prog/$(stub_trace)/Stubs
        search_path += common/prog/schedulers_bm/scheduler/cfg_templates
        defines += SCHEDULER_UT_TEST_DRIVER
    endif
endif


search_path += $(sdk_prog_path)/schedulers_bm
search_path += $(cfg_prog_path)/schedulers_bm


ifeq ($(std_variant),true)
    obj     += $(addprefix schedulers_bm/scheduler/, scheduler scheduler_time_monitor)
    
    ifeq ($(scheduler_config),manual)
        cfg_src += schedulers_bm/scheduler/scheduler_cfg.c
    else ifeq ($(scheduler_config),auto)
        defines += SCHEDULER_CONFIG_AUTO
        gen_src += schedulers_bm/scheduler/$(variant)/scheduler_auto.c
        gen_hdr += schedulers_bm/scheduler/$(variant)/scheduler_auto.h
    else
        $(error Unsupported scheduler variant $(scheduler_config))
    endif

    cfg_hdr += schedulers_bm/scheduler/scheduler_cfg.h
endif

doxy_image_path_ += schedulers_bm/doc

dep_component += ped_fw/watchdogtimer

#end of protection guard preventing multiple inclusion
endif

