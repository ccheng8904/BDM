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

# Naming for testClient variants:
# e.g. mke14z/testClient_ped_fw_utility
# Hint: the subcomponent shall not contain 
testClient      := $(call match,$(variant_base),^testClient(\w+)$$)

ifdef testClient
    std_variant = false
    BIN_OUT_ADRESSES = 0x$(ROM1_START),0x$(ROM1_END)
    
    defines += UNIT_TEST
    no_isr_support   = true
    bootloader_merge = false
    NotFillRom       = true
    $(info --- Creating test application for Comp: $(marked_comp), platform: $(platform) ---)
    
    
    ifeq ($(platform),Intel)
        pc_test = true
        search_path += common\prog\MSP\Intel
        search_path += common\prog\MSP\Intel\CMSIS
    else
        defines += TESTCLIENT_FOR_TARGET
        hw_test = true
    endif
    unit_test = true
    
endif

