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
#  Description      Handle tasks for automatic scheduler file generation
#*******************************************************************************

variant_export_dictionary += scheduler_tasks

ifeq ($(is_reg_channel),ENABLED)
    scheduler_tasks += fsf_reg_handle_task
    fsf_reg_handle_task  = name=REG
    fsf_reg_handle_task += handle_function=FSF_ucRegHandleTask
    fsf_reg_handle_task += priority=high
    fsf_reg_handle_task += dependency=DEP_REG
endif

ifeq ($(is_ram_con_channel),ENABLED)
    scheduler_tasks += fsf_ram_handle_task
    fsf_ram_handle_task  = name=RAM
    fsf_ram_handle_task += handle_function=FSF_ucRamHandleTask
    fsf_ram_handle_task += priority=high
    fsf_ram_handle_task += dependency=DEP_FSF
endif

ifeq ($(is_rom_channel),ENABLED)
    scheduler_tasks += fsf_rom_handle_task
    fsf_rom_handle_task  = name=ROM
    fsf_rom_handle_task += handle_function=FSF_ucRomHandleTask
    fsf_rom_handle_task += priority=high
    fsf_rom_handle_task += dependency=DEP_ROM
endif

ifeq ($(is_stack_channel),ENABLED)
    scheduler_tasks += fsf_stack_handle_task
    fsf_stack_handle_task  = name=STACK
    fsf_stack_handle_task += handle_function=FSF_ucStackHandleTask
    fsf_stack_handle_task += priority=high
    fsf_stack_handle_task += dependency=DEP_STACK
endif

ifeq ($(is_isr_channel),ENABLED)
    scheduler_tasks += fsf_isr_handle_task
    fsf_isr_handle_task  = name=ISR
    fsf_isr_handle_task += handle_function=FSF_ucIsrHandleTask
    fsf_isr_handle_task += priority=high
    fsf_isr_handle_task += dependency=DEP_ISR
endif

# FS COM always active
scheduler_tasks += fsf_com_handle_task
fsf_com_handle_task  = name=FSF
fsf_com_handle_task += handle_function=FSF_ucComHandleTask
fsf_com_handle_task += priority=high
fsf_com_handle_task += dependency=DEP_FSF
fsf_com_handle_task += include_file=FuncSafe/fSafeXS.h
