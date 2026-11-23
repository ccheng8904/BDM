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
ifndef stdcrc_build_mak
stdcrc_build_mak := 1

ifeq ($(marked_comp),ped_fw/stdcrc)
    obj_test = CrcCom StdCrc
    ifeq ($(unit_test),true)
        defines += TEST_STDCRC=stdcrc/$(stdcrc_folder)StdCrc
        defines += STDCRC_UT_TEST_DRIVER
    endif
endif

ifeq ($(unit_test_variant), true)
# needed for off-target testing with unit test framework
    ut_test_src   += ped_fw/stdcrc/test/StdCrc$(ut_test_file_extension).cpp
    ifeq ($(platform),Intel)
        ut_cov_filter += $(sdk_prog_path)/ped_fw/stdcrc
        StdCrc_copt    =--coverage
        CrcCom_copt    =--coverage
    endif
    lib_src     += $(addprefix ped_fw/stdcrc/,CrcCom.c $(stdcrc_folder)StdCrc.c)
endif

ifneq (,$(filter $(platform),rx100 rx200 efr32xg21 gd32e23 ra4 ra2 gd32f30 cms32m67))
    # use generic sw crc, as hw unit is not much faster and complicate in case of far memory
    stdcrc_folder = Intel/
else ifeq ($(platform),psoc4)
    # temporary solution for PSoC4
    stdcrc_folder = Intel/
else ifeq ($(platform),mke14z)
    # currently HW-CRC not working correctly for unaligned data
    stdcrc_folder = $(platform)/
    defines += __TRIGGER_WDG
else ifeq ($(platform),Intel)
    stdcrc_folder = $(platform)/
else
    stdcrc_folder = $(platform)/
endif

ifdef implementation
    stdcrc_folder = $(implementation)/
endif

ifeq ($(std_variant),true)
    obj     += $(addprefix ped_fw/stdcrc/,CrcCom $(stdcrc_folder)StdCrc)
endif

#end of protection guard preventing multiple inclusion
endif
