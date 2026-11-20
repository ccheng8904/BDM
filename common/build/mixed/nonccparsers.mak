#*******************************************************************************
#  Copyright (c) 2017 BSH Hausgeraete GmbH,
#  Carl-Wery-Str. 34, 81739 Munich, Germany, www.bsh-group.de
#
#  All rights reserved. This program and the accompanying materials
#  are protected by international copyright laws.
#  Please contact copyright holder for licensing information.
#
#*******************************************************************************
#  Project          Generic
#*******************************************************************************
#  Description      common settings for code parsers (not the compiler itself)
#*******************************************************************************

#protection guard preventing multiple inclusion
ifndef nonccparsers_mak
nonccparsers_mak := 1

compiler_for_ansi_includes ?= gccArm


ifeq ($(compiler_for_ansi_includes), gccArm)

ansi_cc_path = $(gcc_arm_path)
ansi_system_paths += $(gcc_arm_path)/arm-none-eabi/include/c++/$(gcc_arm_version)
ansi_system_paths += $(gcc_arm_path)/arm-none-eabi/include
ansi_system_paths += $(gcc_arm_path)/lib/gcc/arm-none-eabi/$(gcc_arm_version)/include-fixed
ansi_system_paths += $(gcc_arm_lib_path)/include
ansi_system_paths += $(gcc_arm_path)/arm-none-eabi/include/c++/$(gcc_arm_version)/arm-none-eabi

else
    $(error currently only the ansi includes of gccArm are supported)
endif

# The predefined macros for the selected ANSI platform
preinclude_c   = common/build/$(compiler_for_ansi_includes)/predef_c.h
preinclude_cpp = common/build/$(compiler_for_ansi_includes)/predef_cpp.h

#end of protection guard preventing multiple inclusion
endif