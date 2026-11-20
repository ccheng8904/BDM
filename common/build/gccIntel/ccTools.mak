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
#  Description      settings and targets for MinGW/GCC x86 compiler tool chain
#*******************************************************************************

ifndef CCTOOLS_MAK
CCTOOLS_MAK   = 1              # repeated include protection

do_not_perform_patchbin = true
# ******************************************************************************
# CC Tool folder settings
# ******************************************************************************
UTEST_TYPE_SYSTEM = Windows32
compiler_vendor   = GCC

cc_tool_path ?= $(gcc_intel_path)
cc_inc_path2 ?= $(gcc_intel_lib_path)/include
cc_lib_path  ?= $(cc_tool_path)/lib
cc_inc_path  ?= $(cc_tool_path)/include
gcc_32bit_dll_path ?= $(cc_tool_path)/x86_64-w64-mingw32/lib32/
cpp_inc_path ?= $(cc_inc_path2)/c++
cpp_inc_path_mingw ?= $(cpp_inc_path)/mingw32

cc_bin_path   = $(cc_tool_path)/bin
cc            = $(cc_bin_path)/gcc.exe
ccpp          = $(cc_bin_path)/g++.exe
conv_elf      = $(cc_bin_path)/objcopy
libm          = $(cc_bin_path)/ar.exe
objdump       = $(cc_bin_path)/objdump.exe
cpp           = $(cc_bin_path)/cpp.exe
gvov          = $(cc_bin_path)/gcov.exe

# prepared for the future if any dll file will be needed to run exe file
# fill variable with a list of dll files to be copied to the application out bin folder
gcc_32bit_dll_files ?= 

#export the compiler path to the environment for all sub processes!! 
export PATH +=;$(subst /,\,$(cc_bin_path))


# ******************************************************************************
# Pathes for system headers and libraries
# ******************************************************************************
cc_system_path += $(cc_lib_path) $(cc_inc_path) $(cc_inc_path2) 

ccpp_system_path += $(cc_lib_path) $(cc_inc_path) $(cc_inc_path2) $(cpp_inc_path) $(cpp_inc_path_mingw)

linkerScriptTemplate  ?= $(app_path)/prog/$(msp)/LinkerScript.ld.h
# If linker script template is missing, use default from compiler
ifneq ($(wildcard $(linkerScriptTemplate)),)
    linkerScript    ?= $(bin_path)/LinkerScript.ld
    linkopt         += -T $(linkerScript)
endif

# ******************************************************************************
# GCC specific definitions
# ******************************************************************************
defines += ALIGNMENT=4
defines += __LITTLE_ENDIAN_ADAPTATION 


# compiler personality for lint
cc_lint_options = common/build/lint/co-gcclp.LNT

# ******************************************************************************
# Settings for Compiler(gcc)
# ******************************************************************************
defines += __PCGCC__
defines_gcc += defines_cl
# usually the GCC x86 compiler is not used as target compiler
gcc_is_target_compiler ?= false
ifeq ($(gcc_is_target_compiler), false)
    defines_gcc +=  __NOT_CC__  __NOT_TARGET_CC__
endif

ifeq ($(unit_test_variant), true)
    # add compiler options for code coverage instrumentation for test sources
    $(foreach file,$(ut_test_src),$(eval $(basename $(notdir $(file)))_copt=--coverage))
endif

copt   += $(addprefix -D,$(defines_cl))
copt   += $(addprefix -I,$(search_path_implicit))
copt   += $(addprefix -I,$(search_path))
copt   += $(addprefix -I,$(cc_system_path))
copt  += -m32 -og
copt  += -c
copt  += -g
copt  += -Wbuiltin-macro-redefined
copt  += -gdwarf-4

ccopt  += -std=c99

# ******************************************************************************
# Settings for Compiler(g++)
# ******************************************************************************

cc_ver_query ?= --version

ccppopt  += -std=c++14

# ***************************************************************************************************************
# Settings for COFF to ELF converter
# ***************************************************************************************************************
convopt += -I pe-i386
convopt += -O elf32-i386


# *****************************************************************************
# Settings for Libmaker
# *****************************************************************************
#always create new library
libopt   = rs



# *****************************************************************************
# Settings for C++ compiler
# *****************************************************************************
# TODO


# ******************************************************************************
# Settings for Linker
# ******************************************************************************
linker_out = $(bin_path)/$(variant_base)
linker_bin = $(linker_out).exe
linker_lst = $(linker_out).lst
dwarf_file = $(linker_out).elf
map_file   = $(linker_out).map
linker_bat = $(linker_out).bat

lib_path += $(python_lib_path)
lib_path += $(msp_path)
linkopt  += $(addprefix -L,$(lib_path))
ifeq ($(unit_test_variant), true)
    linkopt  += -lgcov --coverage
endif
# ******************************************************************************
# linker target
# ******************************************************************************

$(linker_bin):  $(objects) $(lib_gen_libs) $(linkerScript)
	@echo linking $(variant)
	$(call mkdir, $(dir $@))
	@$(ccpp) $(linkopt) $(objects) $(lib_gen_libs) $(lib) -o $(linker_bin) -Wl,-Map=$(map_file) -m32
ifndef no_objdump
	@$(conv_elf) $(convopt) $@ $(dwarf_file)
	@$(objdump) $(dwarf_file) --source > $(linker_lst)
endif
ifdef $(gcc_32bit_dll_files)
	@$(foreach file,$(addprefix $(gcc_32bit_dll_path),$(gcc_32bit_dll_files)),$(call copy,$(file),$(subst /,\,$(CURDIR)/$(dir $(linker_out))/) /Y))
endif
ifdef testClient
	@echo @set PYTHONHOME=$(python_path)> $(subst /,\,$(dir $@)/testClient.bat)
	@echo @set PYTHONPATH=$(python_path);$(subst /,\,$(CURDIR)/$(local_tool_path)/PythonTEng);\
$(subst /,\,$(CURDIR)/$(local_tool_path)/Python)>> $(subst /,\,$(dir $@)/testClient.bat)
	@echo @set PATH=$(python_path);$(subst /,\,$(CURDIR)/$(local_tool_path)/PythonTEng);%%PATH%%>> $(subst /,\,$(dir $@)/testClient.bat)
	@echo @call $(subst /,\,$(notdir $@)) %%*>> $(subst /,\,$(dir $@)/testClient.bat)
endif
	@$(call prntGr,-------- Linking done -------------)
	@$(call prntnl)
	@$(call prntnl)

# ******************************************************************************
# Target for generating libraries
# ******************************************************************************
$(out_prog_path)/%.lib: $(call get_lib_gen_obj,%) $(call get_lib_gen_dep,%)
	@echo generating $@
	@$(call mkdir, $(dir $@))
	@$(libm) $(libopt) $@ $($(*F)_obj)


#################################################################################################################
# Implicit rules
#################################################################################################################

# function for getting specific options for compilation unit
# parameter 1: file name of compilation unit
define gccintel_copt
$(addprefix -I,$($(basename $1)_search_path)) $(addprefix -O,$($(basename $1)_optim_level)) $($(basename $1)_copt)
endef

# ***************************************************************************************************************
# Implicit rules for compiler
# ***************************************************************************************************************
$(out_prog_path)/%.o : %.c $(varHeader) | $(code_gen_log)
	@$(call prntlineWhiteNCyanSCyan,compiling ,$(dir $<),$(notdir $<))
	$(call mkdir, $(dir $@))
ifdef preproc
	@$(cc) $(copt) $(ccopt) $(call gccintel_copt,$(@F)) -E -x c -C $< >$(basename $@).i
endif
	@$(cc) $(copt) $(ccopt) $(call gccintel_copt,$(@F)) $< -o $@
ifndef no_objdump
	@$(objdump) -S --disassemble $@ > $(basename $@).lst
endif

$(out_prog_path)/%.o : %.cpp $(varHeader) | $(code_gen_log)
	@$(call prntlineWhiteNCyanSCyan,compiling ,$(dir $<),$(notdir $<))
	$(call mkdir, $(dir $@))
ifdef preproc
	@$(ccpp) $(copt) $(cppcopt) $(call gccintel_copt,$(@F)) -E -x c++ -C $< >$(basename $@).i
endif
	@$(ccpp) $(copt) $(cppcopt) $(call gccintel_copt,$(@F)) $< -o $@
ifndef no_objdump
	@$(objdump) -S --disassemble $@ > $(basename $@).lst
endif

endif #CCTOOLS_MAK
