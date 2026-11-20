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
#  Description      settings and targets for GCC-Arm compiler tool chain
#*******************************************************************************

# repeated include protection
ifndef CCTOOLS_MAK
CCTOOLS_MAK   = 1

# may be used in derivative inc which may support other compiler also
compiler_vendor = GNU


# ******************************************************************************
# CC Tool folder settings
# ******************************************************************************

cc_tool_path ?= $(gcc_arm_path)
cc_lib_path  ?= $(gcc_arm_lib_path)

cc_inc_path   = $(cc_lib_path)/include/
cc_inc_path2  = $(cc_lib_path)/include-fixed/

cc_bin_path     = $(cc_tool_path)/bin

cc              = $(cc_bin_path)/arm-none-eabi-gcc.exe
asm             = $(cc_bin_path)/arm-none-eabi-as.exe
linker          = $(cc_bin_path)/arm-none-eabi-ld.exe
# use gcc-ar.exe to enable LTO (link time optimization)
#libm            = $(libmaker_path)arm-none-eabi-ar.exe
libm            = $(cc_bin_path)/arm-none-eabi-gcc-ar.exe
read_elf        = $(cc_bin_path)/arm-none-eabi-readelf.exe
objcopy         = $(cc_bin_path)/arm-none-eabi-objcopy.exe
objdump         = $(cc_bin_path)/arm-none-eabi-objdump.exe

# additional search pathes for arm cortexMx
search_path += common/prog/MSP/CMSIS
search_path += common/prog/MSP/armCmx


# ******************************************************************************
# Pathes for system headers and libraries and setting of core dependent stuff
# ******************************************************************************
cc_system_path += $(cc_lib_path) $(cc_inc_path) $(cc_inc_path2)

# root region - where it all begin
ROOT_REGION    ?= $(ROM1_START)
ENTRY_ADDRESS  ?= $(ROM1_START)

startvector    ?= ITBL_vResetHandler

# ******************************************************************************
# Setting of core dependent stuff
# ******************************************************************************
ifdef cc_core
    ifeq ($(cc_core),cm0)
        core = cortex-m0
        dev_arch = armv6-m
        defines += CORTEX_M0
        defines += NO_CORE_DEVIDER
    else ifeq ($(cc_core),cm0p)
        core = cortex-m0plus
        dev_arch = armv6-m
        defines += CORTEX_M0_PLUS
        defines += NO_CORE_DEVIDER
    else ifeq ($(cc_core),cm23)
        core = cortex-m23
        dev_arch = armv8-m.base
        defines += CORTEX_M23
    else ifeq ($(cc_core),cm3)
        core = cortex-m3
        dev_arch = armv7-m
        defines += CORTEX_M3
    else ifeq ($(cc_core),cm4)
        core = cortex-m4
        dev_arch = armv7e-m
        defines += CORTEX_M4
    else ifeq ($(cc_core),cm4f)
        core = cortex-m4
        dev_arch = armv7e-m 
        arm_hw_fpu_present  = true
        arm_fpu_arch        = fpv4-sp-d16
        defines += CORTEX_M4
        defines += CORTEX_M4F
    else ifeq ($(cc_core),cm33)
        core = cortex-m33
        ifeq ($(dsp_present),true)
            dev_arch = armv8-m.main+dsp
        else
            dev_arch = armv8-m.main
        endif
        defines += ARM_CORTEX_M33
    else ifeq ($(cc_core),cm33f)
        core = cortex-m33
        ifeq ($(dsp_present),true)
            dev_arch = armv8-m.main+dsp
        else
            dev_arch = armv8-m.main
        endif
        arm_hw_fpu_present  = true
        arm_fpu_arch        = fpv4-sp-d16
        defines += ARM_CORTEX_M33
        defines += ARM_CORTEX_M33F
    else ifeq ($(cc_core),cr4)
        core = cortex-r4
        dev_arch = armv7-r
        defines += CORTEX_R4
    else ifeq ($(cc_core),cm7)
        core = cortex-m7
        dev_arch = armv7e-m
        defines += CORTEX_M7
    else ifeq ($(cc_core),cm7f)
        core = cortex-m7
        dev_arch = armv7e-m 
        # according options described by https://gcc.gnu.org/onlinedocs/gcc-7.3.0/gcc/ARM-Options.html#ARM-Options
        arm_hw_fpu_present  = true
        ifeq ($(fp_precision),dp)
            arm_fpu_arch = fpv5-d16
        else
            arm_fpu_arch = fpv5-sp-d16
        endif
        defines += CORTEX_M7
        defines += CORTEX_M7F
    endif
endif


ifeq ($(arm_hw_fpu_present),true)
    copt    += -mfloat-abi=hard -mfpu=$(arm_fpu_arch)
    linkopt += -mfloat-abi=hard -mfpu=$(arm_fpu_arch)
    defines += ARM_HW_FPU_PRESENT
else
    copt    += -mfloat-abi=soft
    linkopt += -mfloat-abi=soft
endif


# ******************************************************************************
# ARM specific definitions
# ******************************************************************************
defines += ALIGNMENT=4
defines += __LITTLE_ENDIAN_ADAPTATION 
defines += ROOT_REGION=UINT32_C(0x$(ROOT_REGION))


# compiler personality for lint
cc_lint_options = common/build/lint/co-gcclp.LNT

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

# highest optimization (possible values: 0,1,2,3,s)
fwu_optim_level = s
optim_level ?= s
cc_link_time_opt ?= DISABLED
cc_inlining      ?= ENABLED
cc_rtti          ?= DISABLED
cpp_std_version  ?= 14
cc_ver_query     ?= --version

copt_d    += $(addprefix -D,$(defines_cl))
copt    += $(addprefix -I,$(search_path_implicit))
copt    += $(addprefix -I,$(search_path))
copt    += $(addprefix -I,$(cc_system_path))
copt    += -ffunction-sections -fdata-sections
ifeq ($(cc_inlining),DISABLED)
    copt    += -fno-inline
endif
copt    += -Wall
copt    += -Wno-main
copt    += -Wno-unused-but-set-variable
# warning for multi line comment
copt    += -Wno-comment
copt    +=  -c
copt    += -mlittle-endian
copt    += -gdwarf-2
copt    += -mthumb
copt    += -mcpu=$(core)
# LTO (link time optimization) not recommended
ifeq ($(cc_link_time_opt),ENABLED)
    copt    += -flto
endif
copt    += -O$(optim_level)
copt    += -Wa,-adlh=$(basename $@).lsst

# C compiler options
ccopt   += -std=c11

# Compiler option for gecko_bl rollback protection
ifeq ($(variant_base), gecko_bl)
    copt    += -mcmse
endif

# C++ compiler options
cppcopt += -std=c++$(cpp_std_version)
# RTTI (Real Time Type Identification) not recommended
ifeq ($(cc_rtti),DISABLED)
    cppcopt += -fno-rtti
endif


# ******************************************************************************
# Settings for Libmaker
# ******************************************************************************
libopt   = -rc


# ******************************************************************************
# Settings for Assembler
# ******************************************************************************

asmopt += $(addprefix -I,$(search_path))
asmopt += -gdwarf2 
asmopt += -meabi=4
asmopt += -mcpu=$(core)
asmopt += -mthumb
asmopt += -g
asmopt += -alh=$(basename $@).lst -L

# ******************************************************************************
# Settings for linker/scatter file
# ******************************************************************************
RELOC_ADDR ?= 0

# to locate memory which will not be part of output
TRASH_AREA_START ?= 0x1000000
TRASH_AREA_SIZE  ?= 0x1000
TRASH_AREA ?= $(TRASH_AREA_START):B4096
patchBin_trash_region = $(TRASH_AREA)

defines += TRASH_AREA_START=$(TRASH_AREA_START)
defines += TRASH_AREA_SIZE=$(TRASH_AREA_SIZE)
defines += RELOC_ADDR=0x$(RELOC_ADDR)


# location of linker script and linker script template
ifdef project_linkerscript
    linkerScriptTemplate  = $(project_linkerscript).ld.h
endif

ifdef testClient
linkerScriptTemplate  ?= $(msp_path)/cfg_templates/LinkerScripttestClient.ld.h
endif
linkerScript          ?= $(bin_path)/LinkerScript.ld
linkerScriptTemplate  ?= $(app_path)/prog/$(msp)/LinkerScript.ld.h


# ******************************************************************************
# Settings for Linker
# ******************************************************************************
linker_out = $(bin_path)/$(variant_base)
project_lib = $(linker_out).lib
linker_bin = $(linker_out).s37
linker_lst = $(linker_out).lst
dwarf_file = $(linker_out).elf

linkopt_spec  ?= nano.specs rdimon.specs
syslib        ?= -lc -lm -lstdc++

linkopt += -T $(linkerScript)
linkopt += -march=$(dev_arch)
linkopt += -mthumb
linkopt += -Wl,-Map=$(basename $@).map
linkopt += $(addprefix -specs=, $(linkopt_spec))
linkopt += -Wl,--gc-sections
# LTO (link time optimization) not recommended
ifeq ($(cc_link_time_opt),ENABLED)
    linkopt += -flto
endif
linkopt += -O$(optim_level)
linkopt += -g
linkopt += -o $(basename $@).elf
ifndef no_startvector
    linkopt += --entry $(startvector)
endif

# ******************************************************************************
# linker target
# ******************************************************************************

$(linker_bin): $(objects) $(lib_gen_libs) $(linkerScript) $(winflash_cfg_file)
	@echo linking $(variant)
	$(call mkdir, $(dir $@))    
	@$(cc) $(linkopt) -Wl,--start-group $(objects) $(lib_gen_libs) $(lib) $(syslib) -Wl,--end-group 2>&1
	@$(objcopy) -O srec $(dwarf_file) $@
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
	@$(libm) $(libopt) $@ @$@.in
	@$(call delfile,$@.in)


# ******************************************************************************
# Target for generating libraries
# ******************************************************************************
$(out_prog_path)/%.lib: $(call get_lib_gen_obj,%) $(call get_lib_gen_dep,%) create_dir($(out_prog_path)/%.lib)
	@echo generating $@
	@$(file >$@.in,$($(*F)_obj))
	@$(libm) $(libopt) $@ @$@.in
	@$(call delfile,$@.in)

create_dir(%):
	@$(call mkdir, $(dir $*))


#################################################################################################################
# Implicit rules
#################################################################################################################

# function for getting specific options for compilation unit
# parameter 1: file name of compilation unit
define gccarm_copt
$(addprefix -I,$($(basename $1)_search_path)) $(addprefix -O,$($(basename $1)_optim_level)) $($(basename $1)_copt)
endef

# ***************************************************************************************************************
# Implicit rules for C compiler
# ***************************************************************************************************************
$(out_prog_path)/%.o : %.c $(varHeader) | $(code_gen_log)
	@$(call prntlineWhiteNCyanSCyan,compiling ,$(dir $<),$(notdir $<))
	$(call mkdir, $(dir $@))
ifdef preproc
	@$(cc) $(copt_d) $(copt) $(ccopt) $(call gccarm_copt,$(@F)) -E -x c -P -C $< >$(basename $@).i
endif
	@$(ccache_opt) $(cc) $(copt_d) $(copt) $(ccopt) $(call gccarm_copt,$(@F)) $< -o $@
	@$(objdump) -S --disassemble $@ > $(basename $@).lst


# ***************************************************************************************************************
# Implicit rules for C++ compiler
# ***************************************************************************************************************
$(out_prog_path)/%.o : %.cpp $(varHeader) | $(code_gen_log)
	@$(call prntlineWhiteNCyanSCyan,compiling ,$(dir $<),$(notdir $<))
	$(call mkdir, $(dir $@))
ifdef preproc
	@$(cc) $(copt_d) $(copt) $(cppcopt) $(call gccarm_copt,$(@F)) -E -x c -P -C $< >$(basename $@).i
endif
	@$(ccache_opt) $(cc) $(copt_d) $(copt) $(cppcopt) $(call gccarm_copt,$(@F)) $< -o $@
	@$(objdump) -S --disassemble $@ > $(basename $@).lst


# ***************************************************************************************************************
# Implicit rules for assembler
# ***************************************************************************************************************
$(out_prog_path)/%.o : %.s
	@$(call prntlineYellowNCyanSCyan,assembling ,$(dir $<),$(notdir $<))
	$(call mkdir, $(dir $@))
	@$(asm) $(asmopt) $< -o $(basename $@).o

$(out_prog_path)/%.o : %.S $(varHeader) | $(code_gen_log)
	@$(call prntlineYellowNCyanSCyan,assembling ,$(dir $<),$(notdir $<))
	$(call mkdir, $(dir $@))
	@$(cpp_generic) $(addprefix -D,$(defines_cl)) -imacros $(varHeader) -P $< -o $(basename $@).s
	@$(asm) $(asmopt) $(basename $@).s -o $(basename $@).o


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
