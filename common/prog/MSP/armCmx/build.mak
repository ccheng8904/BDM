#*******************************************************************************
#  Copyright (c) 2017 BSH Hausgeraete GmbH,
#  Carl-Wery-Str. 34, 81739 Munich, Germany, www.bsh-group.de
#
#  All rights reserved. This program and the accompanying materials
#  are protected by international copyright laws.
#  Please contact copyright holder for licensing information.
#
#*******************************************************************************
#  PROJECT        Generic SW
#*******************************************************************************
#  Description    build include for ARM Cortex-M microcontroller support package
#*******************************************************************************

#start of include protection guard
ifndef arm_cmx_build_mak
arm_cmx_build_mak := 1

ifndef arm_cmx_no_itbl
    nolib_src   += $(addprefix MSP/armCmx/,IntTblArmCM.c)
endif

lib_src     += MSP/armCmx/hsupArmCM.c
cfg_src   +=$(addprefix MSP/armCmx/,int_exc_cfg.c)
# the following configuration files are copied to app folder,
# but compilation is done in case of ISR-support only
cfg_files   +=$(addprefix MSP/armCmx/,int_prio_cfg.c systick_cfg.c)

ifndef no_isr_support
    lib_src     += $(addprefix MSP/armCmx/,systick.c int_prio.c)
    src         += $(addprefix MSP/armCmx/,int_prio_cfg.c)

    ifndef user_systick_handler
        src += $(addprefix MSP/armCmx/,systick_cfg.c)
    else
        defines += USER_SYSTICK_HANDLER
    endif
endif

#end of include protection guard
endif
