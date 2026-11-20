#*******************************************************************************
#  Copyright (c) 2017 BSH Hausgeraete GmbH,
#  Carl-Wery-Str. 34, 81739 Munich, Germany, www.bsh-group.de
#
#  All rights reserved. This program and the accompanying materials
#  are protected by international copyright laws.
#  Please contact copyright holder for licensing information.
#
#*******************************************************************************
#  PROJECT          PP PED Framework
#*******************************************************************************
#  Description      settings and targets for Keil-Arm clang compiler tool chain
#*******************************************************************************

# repeated include protection
ifndef CCTOOLS_MAK
CCTOOLS_MAK   = 1

compiler_vendor = KEIL


# ******************************************************************************
# CC Tool folder settings
# ******************************************************************************

cc_tool_path    ?= $(keil_armclang_path)

# hint: Keil needs the terminating back slash (not a slash) to find its libraries!
cc_lib_path      = $(cc_tool_path)/lib/armlib\\
cc_lib_path2     = $(cc_tool_path)/lib/libcxx\\
cc_inc_path      = $(cc_tool_path)/include
cc_bin_path      = $(cc_tool_path)/bin

cc              = $(cc_bin_path)/armclang.exe
asm             = $(cc_bin_path)/armclang.exe
# not used -> commented, but left
#armasm          = $(cc_bin_path)/armasm.exe
linker          = $(cc_bin_path)/armlink.exe
libm            = $(cc_bin_path)/armar.exe
from_elf        = $(cc_bin_path)/fromelf.exe
# take objdump from gcc as armclang would need a second call of compiler :-(
objdump         = $(gcc_arm_path)\bin\arm-none-eabi-objdump.exe

# additional search pathes for arm cortexMx
search_path += common/prog/MSP/CMSIS
search_path += common/prog/MSP/armCmx


# ******************************************************************************
# Paths for system headers and libraries
# ******************************************************************************
cc_system_path += $(cc_lib_path) $(cc_lib_path2) $(cc_inc_path)

# root region - where it all begin
ROOT_REGION    ?= $(ROM1_START)
ENTRY_ADDRESS  ?= $(ROM1_START)

startvector    ?= ITBL_vResetHandler


# ******************************************************************************
# The core-dependent settings
# ******************************************************************************
# more info can be found in the Table 2. Processor selection in Arm Compiler 5 and Arm Compiler for Embedded 6
# link https://developer.arm.com/documentation/100068/0622/Migrating-from-armcc-to-armclang/Migrating-architecture-and-processor-names-for-command-line-options

ifdef cc_core
    ifeq ($(cc_core),cm0)
        core = cortex-m0
        armlink_cpu = Cortex-M0
        defines += CORTEX_M0
        defines += NO_CORE_DEVIDER
    else ifeq ($(cc_core),cm0p)
        core = cortex-m0plus
        armlink_cpu = Cortex-M0plus
        defines += CORTEX_M0_PLUS
        defines += NO_CORE_DEVIDER
    else ifeq ($(cc_core),cm23)
        core = cortex-m23
        armlink_cpu = Cortex-M23
        defines += CORTEX_M23
    else ifeq ($(cc_core),cm3)
        core = cortex-m3
        armlink_cpu = Cortex-M3
        defines += CORTEX_M3
    else ifeq ($(cc_core),cm4)
        core = cortex-m4
        armlink_cpu = Cortex-M4.no_fp
        defines += CORTEX_M4
    else ifeq ($(cc_core),cm4f)
        core = cortex-m4
        arm_hw_fpu_present = true
        armlink_cpu = Cortex-M4
        arm_fpu_arch = fpv4-sp-d16
        defines += CORTEX_M4
        defines += CORTEX_M4F
    else ifeq ($(cc_core),cm33)
        core = cortex-m33
        defines += ARM_CORTEX_M33
    else ifeq ($(cc_core),cm33f)
        core = cortex-m33
        arm_hw_fpu_present = true
        armlink_cpu = Cortex-M33
        arm_fpu_arch       = fpv5-sp-d16
        defines += ARM_CORTEX_M33
        defines += ARM_CORTEX_M33F
    else ifeq ($(cc_core),cr4)
        core = cortex-r4
        armlink_cpu = Cortex-R4
        defines += CORTEX_R4
    else ifeq ($(cc_core),cm7)
        core = cortex-m7
        defines += CORTEX_M7
        armlink_cpu = Cortex-M7.no_fp
    else ifeq ($(cc_core),cm7f)
        core = cortex-m7
        arm_hw_fpu_present = true
        ifeq ($(fp_precision),dp)
            arm_fpu_arch = fpv5-d16
            armlink_cpu = Cortex-M7
        else
            arm_fpu_arch = fpv5-sp-d16
            armlink_cpu = Cortex-M7.fp.sp
        endif
        defines += CORTEX_M7
        defines += CORTEX_M7F
        
    endif
endif

# ******************************************************************************
# ARM specific definitions
# ******************************************************************************
defines += ALIGNMENT=4
defines += __LITTLE_ENDIAN_ADAPTATION
defines += ROOT_REGION=UINT32_C(0x$(ROOT_REGION))


# compiler personality for lint
cc_lint_options = common/build/lint/CO-RVlp.LNT


# TODO Axivion
UTEST_TYPE_SYSTEM = KEIL_ARM
# disable microsoft language extensions
AXIVIONOPT += --no_microsoft
AXIVIONOPT += --strict


#****** Watch symbols ***********************
watch_Symbols += ITBL_fixedVectors ITBL_coreVectors ITBL_peripheralVectors

# ******************************************************************************
# Settings for Compiler
# ******************************************************************************

# range: [0, 1, 2, 3, s - speed, z - size] (default is maximum optim. for size and speed).
# For debugging use 0.
# Beware that -O3 option may produce much bigger code in some cases than -O2
# In case of using link time optimization, disable generation of system library
# by setting make variable system_lib_generation = DISABLED
fwu_optim_level  ?= z
optim_level      ?= z
cc_link_time_opt ?= DISABLED
cc_rtti          ?= DISABLED
cpp_std_version  ?= 14
cc_ver_query     ?= --version

copt_d += $(addprefix -D,$(defines_cl))
copt   += $(addprefix -I,$(search_path_implicit))
copt   += $(addprefix -I,$(search_path))
copt   += $(addprefix -I,$(cc_system_path))
copt   += -O$(optim_level)
copt   += -mthumb
copt   += -ffunction-sections
copt   += -fdata-sections
copt   += --target=arm-arm-none-eabi
copt   += -mcpu=$(core)

ifeq ($(arm_hw_fpu_present),true)
    copt    += -mfloat-abi=hard
    copt    += -mfpu=$(arm_fpu_arch)
    defines += ARM_HW_FPU_PRESENT
else
    copt    += -mfloat-abi=soft
    copt    += -mfpu=none
endif

copt   += -c -gdwarf-3
copt   += -fshort-enums

# LTO (link time optimization) not recommended
ifeq ($(cc_link_time_opt),ENABLED)
    copt    += -flto
endif

# disable warning for case sensitive includes
copt   += -Wno-nonportable-include-path

# disable warning: call to function without interrupt attribute could clobber interruptee's VFP registers
copt   += -Wno-extra

# disable warning: expression result unused; creates false positives with volatile variables
copt   += -Wno-unused-value

copt   += -Wno-typedef-redefinition

# C compiler options
ccopt   += -std=c11

# C++ compiler options
cppcopt += -std=c++$(cpp_std_version)
# RTTI (Real Time Type Identification) not recommended
ifeq ($(cc_rtti),DISABLED)
    cppcopt += -fno-rtti
endif


# *****************************************************************************
# Settings for Libmaker
# *****************************************************************************
#always create new library
libopt   = --create


# *****************************************************************************
# Settings for Assembler armclang.exe
# *****************************************************************************
asmopt   += $(addprefix -D,$(defines_cl))
asmopt   += $(addprefix -I,$(search_path))
asmopt   += -mthumb
asmopt   += --target=arm-arm-none-eabi
asmopt   += -Wno-unused-command-line-argument
ifeq ($(arm_hw_fpu_present),true)
    asmopt += -mfpu=$(arm_fpu_arch)
    asmopt += -mfloat-abi=hard
else
    asmopt += -mfpu=none
    asmopt += -mfloat-abi=soft
endif
asmopt   += -mcpu=$(core)
asmopt   += -c -gdwarf-3
asmopt   += -O$(optim_level)

# *****************************************************************************
# Settings for linker/scatter file
# *****************************************************************************
RELOC_ADDR ?= 0

# to locate memory which will not be part of output
TRASH_AREA_START = 0x1000000
TRASH_AREA ?= $(TRASH_AREA_START):B65536

defines += TRASH_AREA_START=$(TRASH_AREA_START)
defines += RELOC_ADDR=0x$(RELOC_ADDR)


# reserve this space in highest prio any regions
any_contingency ?=50
defines += ANY_CONTINGENCY=0x$(any_contingency)

patchBin_trash_region = $(TRASH_AREA)

# ******************************************************************************
# Settings for Linker
# ******************************************************************************
linker_out = $(bin_path)/$(variant_base)
project_lib = $(linker_out).lib
linker_bin = $(linker_out).s37
linker_lst = $(linker_out).lst
dwarf_file = $(linker_out).elf
map_file   = $(linker_out).map
cll_grph   = $(linker_out)_CG.html
global_sym_def = $(linker_out).gsd

# location of linker script and linker script template
ifdef project_linkerscript
    linkerScriptTemplate  = $(project_linkerscript).sct.h
endif

ifdef testClient
    linkerScriptTemplate  ?= $(msp_path)/cfg_templates/LinkerScripttestClient.sct.h
else ifdef bootManager
    linkerScriptTemplate  ?= $(msp_path)/cfg_templates/LinkerScriptBM.sct.h
endif
linkerScript          ?= $(bin_path)/LinkerScript.sct
linkerScriptTemplate  ?= $(app_path)/prog/$(msp)/LinkerScript.sct.h

# ***********   general settings ******
linkopt  += --cpu=$(armlink_cpu)
linkopt  += --libpath $(cc_lib_path),$(cc_lib_path2)
# linkopt  += --userlibpath
ifndef no_startvector
  linkopt  += --entry=$(startvector)
else
  #suppress warning: no L6305W entry point
  linkopt +=  --diag_suppress L6305W
endif
linkopt  += --verbose --map --list_mapping_symbols --symbols --xref
linkopt  += --info common,debug,exceptions,inline,inputs,libraries,sizes,stack,summarysizes,summarystack,tailreorder,totals,unused,veneers,veneercallers
# display image load vs. execution address
linkopt  += --load_addr_map_info
# if object comes from lib show it with full path
linkopt  += --show_parent_lib --show_full_path

linkopt  += --list $(map_file)
linkopt  += --symdefs $(global_sym_def)
linkopt  += --callgraph --callgraph_file=$(cll_grph)
linkopt  += -o $(dwarf_file)
#suppress linker warning: L6314W: No section matches pattern xxxxxxxxx - occures with timer segments
linkopt  += --diag_suppress L6314
#suppress linker warning: L6314W: Pattern  xxxx only matches removed unused sections
linkopt  += --diag_suppress L6329
linkopt  += --scatter $(linkerScript)
linkopt  += --any_placement=worst_fit
linkopt  += --any_contingency
# LTO (link time optimization) not recommended
ifeq ($(cc_link_time_opt),ENABLED)
    linkopt += --lto
endif

# ******************************************************************************
# linker target
# ******************************************************************************
$(linker_bin): $(objects) $(lib_gen_libs) $(linkerScript) $(winflash_cfg_file)
	@echo linking $(variant)
	$(call mkdir, $(dir $@))
	@$(linker) $(linkopt) $(objects) $(lib_gen_libs) $(lib) 2>&1
	@$(from_elf) --m32combined --output $@ $(dwarf_file)
	@$(from_elf) -c -s --output $(linker_lst) $(dwarf_file)
	@$(call prntGr,-------- Linking done -------------)
	@$(call prntnl)
	@$(call prntnl)


# ******************************************************************************
# The target for creating project library
# ******************************************************************************
.PHONY: plib
plib: $(project_lib)
$(project_lib): $(objects) $(lib_gen_libs) create_dir($(project_lib))
	@echo generating project library $@
	@$(file >$@.in,$(objects) $(lib_gen_libs))
	@$(libm) $(libopt) $@ --via=$@.in
	@$(call delfile,$@.in)


# ******************************************************************************
# Target for generating libraries
# ******************************************************************************
$(out_prog_path)/%.lib: $(call get_lib_gen_obj,%) $(call get_lib_gen_dep,%) create_dir($(out_prog_path)/%.lib)
	@echo generating $@
	@$(file >$@.in,$($(*F)_obj))
	@$(libm) $(libopt) $@ --via=$@.in
	@$(call delfile,$@.in)

create_dir(%):
	@$(call mkdir, $(dir $*))


#################################################################################################################
# Implicit rules
#################################################################################################################

# function for getting specific options for compilation unit
# parameter 1: file name of compilation unit
define armclang_copt
$(addprefix -I,$($(basename $1)_search_path)) $(addprefix -O,$($(basename $1)_optim_level)) $($(basename $1)_copt)
endef

# ***************************************************************************************************************
# Implicit rules for C compiler
# ***************************************************************************************************************
$(out_prog_path)/%.o : %.c $(varHeader) | $(code_gen_log)
	@$(call prntlineWhiteNCyanSCyan,compiling ,$(dir $<),$(notdir $<))
	@$(call mkdir, $(dir $@))
ifdef preproc
	@$(cc) $(copt_d) $(copt) $(ccopt) $(call armclang_copt,$(@F)) -E $< >$(basename $@).i
endif
	@$(ccache_opt) $(cc) $(copt_d) $(copt) $(ccopt) $(call armclang_copt,$(@F)) $< -o $@
	@$(objdump) -S --disassemble $@ > $(basename $@).lst


# ***************************************************************************************************************
# Implicit rules for C++ compiler
# ***************************************************************************************************************
$(out_prog_path)/%.o : %.cpp $(varHeader) | $(code_gen_log)
	@$(call prntlineWhiteNCyanSCyan,compiling ,$(dir $<),$(notdir $<))
	@$(call mkdir, $(dir $@))
ifdef preproc
	@$(cc) $(copt_d) $(copt) $(cppcopt) $(call armclang_copt,$(@F)) -E $< >$(basename $@).i
endif
	@$(ccache_opt) $(cc) $(copt_d) $(copt) $(cppcopt) $(call armclang_copt,$(@F)) $< -o $@
	@$(objdump) -S --disassemble $@ > $(basename $@).lst


# ***************************************************************************************************************
# Implicit rules for assembler
# ***************************************************************************************************************
$(out_prog_path)/%.o : %.asm
	@$(call prntlineYellowNCyanSCyan,assembling ,$(dir $<),$(notdir $<))
	@$(call mkdir, $(dir $@))
	@$(asm) $(asmopt) $< -o $@ --list=$(basename $@).lst

$(out_prog_path)/%.o : %.s
	@$(call prntlineYellowNCyanSCyan,assembling ,$(dir $<),$(notdir $<))
	@$(call mkdir, $(dir $@))
	@$(asm) $(asmopt) $< -o $@
	@$(objdump) -S --disassemble $@ > $(basename $@).lst

$(out_prog_path)/%.o : %.S $(varHeader) | $(code_gen_log)
	@$(call prntlineYellowNCyanSCyan,assembling ,$(dir $<),$(notdir $<))
	@$(call mkdir, $(dir $@))
	@$(asm) $(asmopt) -imacros $(varHeader) $<  -o $@
	@$(objdump) -S --disassemble $@ > $(basename $@).lst


# ***************************************************************************************************************
# Internal special targets to crate predefined macro definitions
# ***************************************************************************************************************

# internal use: Create predefined macros for C
show_cmacros:
	@echo create compiler predefined C macros
	@echo > dummy.c
	@$(cc) $(copt) $(ccopt) -dM -E -xc dummy.c > predef_c.h
	@del dummy.c

# internal use: Create predefined macros for CPP
show_cppmacros:
	@echo create compiler predefined CPP macros
	@echo > dummy.cpp
	@$(cc) $(copt) $(cppcopt) -dM -E -xc++ dummy.cpp > predef_cpp.h
	@del dummy.cpp


endif #CCTOOLS_MAK
