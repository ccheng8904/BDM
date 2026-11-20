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
#  Description      build include PED Framework, subcomponent: timer
#*******************************************************************************

#protection guard preventing multiple inclusion
ifndef timer_build_mak
timer_build_mak := 1

# Timer handle task configuration
code_gen_cfg += ped_fw/timer/config.json

ifeq ($(marked_comp),ped_fw/timer)
    obj_test = timerlib timerXS
    ifeq ($(unit_test),true)
        system_timer_disable := 1
    endif
endif


ifeq ($(std_variant),true)
    obj     += $(addprefix ped_fw/timer/,timerlib)

    ifdef timerXs
        obj     += $(timerXs)
    else
        # default timerXS
        cfg_obj += ped_fw/timer/timerXS
    endif
    cfg_obj += $(addprefix ped_fw/timer/,$(platform)/timerMcXs)
    cfg_hdr += $(addprefix ped_fw/timer/,timer_seg.h)
    # not all platforms have timerMcXs.h implemented yet. 
    # It will be added in the future depending on the platform is supporting 32bit HW timer or not.
    ifeq ($(supports_32bit_hwtimer),true)
        cfg_hdr += $(addprefix ped_fw/timer/,$(platform)/timerMcXs.h)
    endif
    search_path += $(app_path)/prog/ped_fw/timer/$(platform)
endif


ifneq (,$(filter $(platform),rx100))
    system_timer_disable := 1
endif


ifndef system_timer_disable
    lib_src += $(addprefix ped_fw/timer/,system_timer.c)

    #System timer default period is 1 ms
    system_timer_period     ?= 1
    defines += SYSTEM_TIMER_PERIOD=$(strip $(system_timer_period))U SYSTEM_TIMER_ENABLED

    #In case of Cortex-M, use SysTick otherwise use microcontroller peripheral timer
    ifneq (,$(filter $(cc_core), cm0 cm0p cm23 cm3 cm33 cm33f cm4 cm4f cm7f))
        defines += SYSTEM_TIMER_SYSTICK
    else
        defines += SYSTEM_TIMER_MC_PERIPH
        cfg_src += $(addprefix ped_fw/timer/$(platform)/,system_timer_cfg.c)
        cfg_hdr += $(addprefix ped_fw/timer/$(platform)/,system_timer_cfg.h)
    endif

    dep_component += std_lib
endif

#end of protection guard preventing multiple inclusion
endif
