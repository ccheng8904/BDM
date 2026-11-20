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
#  Description      build include PED Framework, subcomponent: stdcrc
#*******************************************************************************

#protection guard preventing multiple inclusion
ifndef funcsafe_build_mak
funcsafe_build_mak := 1

implementation ?= $(platform)

fsf_supported_platforms = stm32g0 stm32g4 stm32f0 stm32f3 stm32f4 mkv5x psoc4 mke14z stm32h7 ra2 sh30f gd32f33 mspm0 gd32e50 gd32f30

# include fsafe.mak in case target is not init target
#ifeq (,$(filter $(MAKECMDGOALS), init))
    -include $(app_path)/prog/FuncSafe/fsafe.mak
#endif

ifeq ($(marked_comp),FuncSafe)
    obj_test = fsReg fsRam fsRom fsStack fsIsr fsCom fsramcon
    add_stubfunc = uint:HSUP_ucGetHighByte:uint uint:HSUP_ucGetLowByte:uint
    ifeq ($(unit_test),true)
        defines_cl +=__NVIC_PRIO_BITS=4
        search_path += common/prog/ped_fw/timer
    endif
endif


#FuncSafe/,fsCom fsRam fsReg fsramcon
ifeq ($(std_variant),true)
    ifeq ($(platform),$(filter $(platform),$(fsf_supported_platforms)))
        # Functional Safety is relevant only for BootManager and User Application
        ifneq ($(variant),$(filter $(variant),romLoader ramLoader lromLoader programmer testApp))
            # Generic part of Functional Safety (independed from core and platform)
            defines += FEATURE_FUNCTIONAL_SAFETY_ENABLED
            obj += $(addprefix FuncSafe/,fsReg fsRam fsRom fsStack fsIsr fsCom fsramcon fs_psm)
            cfg_obj += $(addprefix FuncSafe/,fSafeXS)
            cfg_hdr += $(addprefix FuncSafe/,fSafeXS.h)
            cfg_files += FuncSafe/fsafe.mak

            # Include scheduler task configuration for automatic code generation
            include $(sdk_prog_path)/FuncSafe/scheduler_tasks.mak

            # Platform or core specific part of Functional Safety
            ifneq (,$(filter cm%,$(cc_core)))
                obj += $(addprefix FuncSafe/armCmx/, fs_registers_checks_arm_cmx)
                lib_src += $(addprefix FuncSafe/armCmx/, fs_Start_Up_Ram.S)
                lib_src += $(addprefix FuncSafe/armCmx/, fs_Run_Time_Ram.S)
                lib_src += $(addprefix FuncSafe/armCmx/, fs_register_routines.S)
            endif
        endif
    else
        ifneq ($(FW_update_type)_$(MAKECMDGOALS), FWU3_init)
            ifndef bootManager
                $(error ERROR: functional safety module is not implemented for platform $(platform)! ***)
            endif
        endif
    endif
endif

# define excluded data section from Ram Continuous (RunTime) check (end must be aligned to 0x40)
# must be set because "MarchDataBuffer" is located there
RAMCONT_EXCLUDED_DATA_SIZE ?= 40

#end of protection guard preventing multiple inclusion
endif
