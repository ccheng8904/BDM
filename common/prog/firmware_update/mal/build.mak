#*******************************************************************************
#  Copyright (c) 2017 BSH Hausgeraete GmbH,
#  Carl-Wery-Str. 34, 81739 Munich, Germany, www.bsh-group.de
#
#  All rights reserved. This program and the accompanying materials
#  are protected by international copyright laws.
#  Please contact copyright holder for licensing information.
#
#*******************************************************************************
#  PROJECT          MCU Framework
#*******************************************************************************
#  Description      build include firmwar_update, subcomponent: mal
#*******************************************************************************

#protection guard preventing multiple inclusion
ifndef mal_build_mak
mal_build_mak := 1


ifeq ($(marked_comp),firmware_update/mal)
    obj_test = mal_command moduleAdministration
    axivion_uti_src = firmware_update/mal/mal_command.c firmware_update/mal/moduleAdministration.c
    defines += VARIANT_BOOTMANAGER FWU_FLEX_PARTITION_USED
endif


ifeq ($(std_variant),true)
    ifeq ($(bootManager_backward_compatible), true)
        defines += BM_BACKWARD_COMPATIBILITY
    endif

    ifdef bootManager
        obj  += firmware_update/mal/moduleAdministration firmware_update/mal/dataAnchor 

    else ifdef programmer
        obj  += $(addprefix firmware_update/mal/,moduleAdministration dataAnchor)
        obj  += $(addprefix firmware_update/mal/,mal_command)
        obj  += $(msp)/mem_drv/intflash_drv
        dep_component += mem_utility
        STACKSIZE      = 340

    else ifdef app_variant
        obj  += firmware_update/mal/moduleAdministration firmware_update/mal/dataAnchor
        cfg_obj  += firmware_update/mal/fwu_opt_bytes_cfg
        
    endif
    cfg_obj  += firmware_update/mal/module_administration_cfg
endif




#end of protection guard preventing multiple inclusion
endif

