#*******************************************************************************
#  Copyright (c) 2024 BSH Hausgeraete GmbH,
#  Carl-Wery-Str. 34, 81739 Munich, Germany, www.bsh-group.de
#
#  All rights reserved. This program and the accompanying materials
#  are protected by international copyright laws.
#  Please contact copyright holder for licensing information.
#
#*******************************************************************************
#  Project          Generic SW
#*******************************************************************************
#  Description      definition for doctest application
#*******************************************************************************
ifndef DTEST_MAK
DTEST_MAK = 1

include common/build/variants/ut_off_target_test.mak

std_variant = false
doctest = true
defines_cl  += DOCTEST

ut_test_file_extension = .dtest
ut_mock_file_extension = .mock

ut_test_fw_component   = doctest
ifndef ut_common_tests
    ut_common_tests       += $(msp)
    ut_common_tests       += math
    ut_common_tests       += ped_fw/stdcrc
    ut_common_tests       += devices/spi_flash
    ut_common_tests       += mem_utility
    ut_common_tests       += MSP/mcal
endif


# options for doctest application
utest_opt += --no-intro

endif #DTEST_MAK
