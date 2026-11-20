#*******************************************************************************
#  Copyright (c) 2017 BSH Hausgeraete GmbH,
#  Carl-Wery-Str. 34, 81739 Munich, Germany, www.bsh-group.de
#
#  All rights reserved. This program and the accompanying materials
#  are protected by international copyright laws.
#  Please contact copyright holder for licensing information.
#
#*******************************************************************************
#  Project          Generic SW
#*******************************************************************************
#  Description      path defines for external tools
#*******************************************************************************

# Most tools paths are compatible with forward and back slash separator, use
# forward slash symbol if possible to eliminate problem with escape sequences.
# Every path variable should end without final slash symbol, to unify chaining
# of paths, e.g.: $(gcc_intel_path)/bin


#*******************************************************************************
#  Generic settings
#*******************************************************************************
local_tool_path = common\tools

tooldrive   ?= M:
ext_tool_path    ?= $(tooldrive)/SwTools

#*******************************************************************************
#  Compilers
#*******************************************************************************

# GCC intel compiler
gcc_intel_version   ?= 10.3.0
gcc_intel_path      ?= $(tooldrive)/MinGW/$(gcc_intel_version)
gcc_intel_lib_path  ?= $(gcc_intel_path)/lib/gcc/x86_64-w64-mingw32/$(gcc_intel_version)
choco_packages      += id=MinGW_gcc;version=$(gcc_intel_version);allowMultipleVersions=true;make=gcc_intel_path


# GCC ARM compiler
gcc_arm_version     ?= 11.3.1
gcc_arm_path        ?= $(tooldrive)/GCC/$(gcc_arm_version)
gcc_arm_lib_path    ?= $(gcc_arm_path)/lib/gcc/arm-none-eabi/$(gcc_arm_version)
choco_packages      += id=gcc_arm;version=$(gcc_arm_version);allowMultipleVersions=true;make=gcc_arm_path

# Keil ARMCC and ARMCLANG compiler
keil_version        ?= 5.37
keil_path           ?= $(tooldrive)/Keil/MDK_ARM_$(keil_version)
keil_armcc_path     ?= $(keil_path)/ARM/ARMCC
keil_armclang_path  ?= $(keil_path)/ARM/ARMCLANG
choco_packages      += id=keil;version=$(keil_version);allowMultipleVersions=true;make=keil_path id=license.Keil.ext

# IAR RX compiler
iar_rx_version      ?= 4.12.1
iar_rx_path         ?= $(tooldrive)/IAR/Renesas/RX/V$(iar_rx_version)/Rx
choco_packages      += id=IAR_RX;version=$(iar_rx_version);allowMultipleVersions=true;make=iar_rx_path

#*******************************************************************************
#  Flashing tools
#*******************************************************************************

# STM32 Cube Programmer
st_programmer_version ?= 2.10.0
st_programmer_path    ?= $(ext_tool_path)/STM32CubeProg/V$(st_programmer_version)
choco_packages        += id=STM32CubeProg;version=$(st_programmer_version);allowMultipleVersions=true;make=st_programmer_path

# GD Link Cli
gdlink_programmer_version ?= 4.8.6.20664
gdlink_programmer_path    ?= $(ext_tool_path)/GDLinkProgrammer/$(gdlink_programmer_version)
choco_packages            += id=GDLinkProgrammer;version=$(gdlink_programmer_version);allowMultipleVersions=true;make=gdlink_programmer_path

# Renesas Flash Programmer
flash_rfp_version   ?= 3.05
flash_rfp_path      ?= $(ext_tool_path)/Renesas_Flash_Programmer/V$(flash_rfp_version)
choco_packages      += id=RenesasFlashProgrammer;version=$(flash_rfp_version);allowMultipleVersions=true;make=flash_rfp_path

# PS Flasher
psflasher_version   ?= 3.0.9
psflasher_path      ?= $(ext_tool_path)/PSFlasher/$(psflasher_version)
choco_packages      += id=psflasher;version=$(psflasher_version);allowMultipleVersions=true;make=psflasher_path

# SiLabs commander (custom jlink)
sl_commander_version    ?= 1.12.0
sl_commander_path       ?= $(ext_tool_path)/SiLabs_commander/v$(sl_commander_version)
choco_packages          += id=SimplicityCommander;version=$(sl_commander_version);allowMultipleVersions=true;make=sl_commander_path

# FlashProgrammer (WinFlash)
flashprogrammer_version ?= 4.2.1
flashprogrammer_path    ?= $(CURDIR)/$(local_tool_path)/Flashprogrammer/$(flashprogrammer_version)
choco_packages          += id=FlashProgrammer;version=$(flashprogrammer_version);allowMultipleVersions=true;make=flashprogrammer_path

# SEGGER J-Flash/J-Link
jflash_version         ?= 7.96.0
jflash_path            ?= $(CURDIR)/$(local_tool_path)/jlink/$(jflash_version)
choco_packages         += id=J-Link;version=$(jflash_version);allowMultipleVersions=true;make=jflash_path id=J-Link.driver.ext

#*******************************************************************************
#  Testing / documentation
#*******************************************************************************

# Doxygen
doxygen_version     ?= 1.8.11
doxy_path           ?= $(ext_tool_path)/Doxygen/V$(doxygen_version)
choco_packages      += id=Doxygen;version=$(doxygen_version);allowMultipleVersions=true;make=doxy_path

# Axivion
axivion_version     ?= 6.9.3
axivion_path        ?= $(tooldrive)/Axivion/V$(subst .,_,$(axivion_version))
choco_packages      += id=Axivion;version=$(axivion_version);allowMultipleVersions=true;make=axivion_path id=license.Axivion.ext

# Tex
tex_version         ?= 3.1415926
tex_path            ?= $(ext_tool_path)/TeXLive2012/Bin/win32
choco_packages      += id=texlive2012;version=$(tex_version);allowMultipleVersions=true;make=tex_path

# Ghost script
gs_version          ?= 8.64
gs_path             ?= $(ext_tool_path)/gs/gs$(gs_version)/bin

#Vectorcast
vectorcast_version  ?= V_2023_SP3
vectorcast_dir      ?= C:\Programs\Vectorcast\$(vectorcast_version)
choco_packages      += id=VectorCast;version=$(subst V_20,,$(subst _SP,.,$(vectorcast_version)));allowMultipleVersions=true;make=vectorcast_dir

#*******************************************************************************
#  Python
#*******************************************************************************

# Python version 3
python_version      ?= 3.5.2
python_trace        ?= Python
python_path         ?= $(ext_tool_path)/$(python_trace)/V$(python_version)_32bit
choco_packages      += id=Python;version=$(python_version);forceX86=true;allowMultipleVersions=true;make=python_path
pip_packages        += PyYAML setuptools openpyxl pyserial jdcal six
#ToDo: Axivion 7.x need black, but its possibly not compatible with python 3.5.2_32bit
#pip_packages       += black

# Python version 2
python2_version     ?= 2.7.3
python2_path        ?= $(ext_tool_path)/$(python_trace)/V$(python2_version)
choco_packages      += id=Python2;version=$(python2_version);allowMultipleVersions=true;make=python2_path


#*******************************************************************************
#  Other
#*******************************************************************************

# PSoC Creator
psoc_creator_major      ?= 4.3
psoc_creator_minor      ?= 0
psoc_creator_path       ?= $(tooldrive)/Cypress/PsocCreator/$(psoc_creator_major)_SP$(psoc_creator_minor)/PSoC Creator
choco_packages          += id=Psoc_Creator;version=$(psoc_creator_major);allowMultipleVersions=true;make=psoc_creator_path


# Curl
curl_version        ?= 7.64.0
curl_path           ?= $(ext_tool_path)/curl/V$(curl_version)
curl                ?= $(curl_path)/curl.exe
choco_packages      += id=Curl;version=$(curl_version);allowMultipleVersions=true;make=curl_path


# GNU awk
# TBD - current version on M drive is old v3.1.3.1614 from link date: 2.6.2004 and 32-bit app.
# after updgrade to 64-bit and placement to M:/SwTools/Gawk/version, we can change it.
gawk_path           ?= $(tooldrive)/batch
gawk                ?= $(gawk_path)/gawk.exe
choco_packages      += id=gawk;allowMultipleVersions=true;make=gawk_path

# Clang format
clang_formatter_version     ?= 10.0.0
clang_formatter_path        ?= $(ext_tool_path)/clang-format/V$(subst .,_,$(clang_formatter_version))
clang_formatter             ?= $(clang_formatter_path)/bin/clang-format
choco_packages              += id=ClangFormat;version=$(clang_formatter_version);allowMultipleVersions=true;make=clang_formatter_path

# Lintplus
lintplus_version    ?= 1.3.5
lintplus_path       ?= $(ext_tool_path)\lintplus\V$(lintplus_version)\windows
lint                ?= $(lintplus_path)\pclp64.exe
choco_packages      += id=LintPlus;version=$(lintplus_version);allowMultipleVersions=true;make=lintplus_path

# C preprocessor
# default is version 10.3.0 (still 32bit), set cpp_version = 4.9.3 if you want to use an older version
cpp_version         ?= 10.3.0
cpp_path            ?= $(CURDIR)/$(local_tool_path)/cpp/$(cpp_version)
cpp                 ?= $(cpp_path)/cpp.exe
cpp_generic         ?= $(cpp)
cpp_depend          ?= $(cpp)
choco_packages      += id=cpp;version=$(cpp_version);allowMultipleVersions=true;make=cpp_path
export PATH        := $(cpp_path);$(PATH)

# ccache - a fast C/C++ compiler cache
ccache_version      ?= 3.7.12
ccache_path         ?= $(CURDIR)/$(local_tool_path)/ccache/$(ccache_version)
ccache              ?= $(ccache_path)/ccache.exe
choco_packages      += id=ccache;version=$(ccache_version);allowMultipleVersions=true;make=ccache_path

# PatchBin - a binary manipulation tool
patchbin_version    ?= 1.60
patchbin_path       ?= $(CURDIR)/$(local_tool_path)/patchbin/$(patchbin_version)
patchbin            ?= $(patchbin_path)/patchbin.exe
choco_packages      += id=PatchBin;version=$(patchbin_version);allowMultipleVersions=true;make=patchbin_path

# Perl
perl_version        ?= 5.32.0
perl_path           ?= $(CURDIR)/$(local_tool_path)/perl/$(perl_version)
perl                ?= $(perl_path)/perl.exe
choco_packages      += id=perl;version=$(perl_version);allowMultipleVersions=true;make=perl_path

# Report Generator
repgen_version      ?= 1.1
repgen_path         ?= $(CURDIR)/$(local_tool_path)/repgen/$(repgen_version)
repgen              ?= $(repgen_path)/ReportGen.exe
choco_packages      += id=ReportGen;version=$(repgen_version);allowMultipleVersions=true;make=repgen_path

# Git 64-bit client
# currently the local installation is done by user manually to the folder C:\Program Files,
# the path contains a space and cannot be used in make scripts, therefore the git_path is not defined, it is empty
git_version         ?= 2.37.0
git_path            ?=
ifeq (,$(git_path))
    git             ?= git.exe
else
    git             ?= $(git_path)/bin/git.exe
endif
choco_packages      += id=Git;make=git_path

# Printf utility (made in 2007 32-bit) was removede because of obsolete, it was replacesd by ANSI Escape Sequences


# MakSym - make symbol's file tool
maksym_version      ?= 3.0.2
maksym_path         ?= $(CURDIR)/$(local_tool_path)/maksym/$(maksym_version)
maksym              ?= $(maksym_path)/maksym.exe

