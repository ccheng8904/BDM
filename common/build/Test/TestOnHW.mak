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
#  Description      make include for special variants: testClient_<comp>_<subcomp>
#*******************************************************************************

# Naming for testOnHW variants:
# e.g. mke14z/testOnHW_ped_fw_utility
# Hint: the subcomponent shall not contain 
testOnHW      := $(call match,$(variant_base),^testOnHW$$)

ifdef testOnHW
    std_variant = false
    defines += UNIT_TEST
    bootloader_merge = true
    NotFillRom       = true
    $(info --- Creating HW-test application for Comp: $(marked_comp), platform: $(platform) ---)
    
    hw_test = true

    unit_test = true
    
endif

