#*******************************************************************************
#  Copyright (c) 2017 BSH Hausgeraete GmbH,
#  Carl-Wery-Str. 34, 81739 Munich, Germany, www.bsh-group.de
#
#  All rights reserved. This program and the accompanying materials
#  are protected by international copyright laws.
#  Please contact copyright holder for licensing information.
#
#*******************************************************************************
#  PROJECT          Generic SW
#*******************************************************************************
#  Description      build include for ring buffer component
#*******************************************************************************

#protection guard preventing multiple inclusion
ifndef mem_utility_build_mak
mem_utility_build_mak := 1

ifeq ($(marked_comp),mem_utility)
     obj_test = ring_buffer
    # do no stub local functions
    stub_locals = false
    # enable re-entrance for testing by default
    defines += RBUF_WRITE_REENTRANT=2
    ifeq ($(unit_test),true)
        defines += RING_BUFFER_UT_TEST_DRIVER
    endif
endif


ifneq ($(unit_test_variant), true)
    lib_src += mem_utility/ring_buffer.c
else
    src        += mem_utility/test/ring_buffer.mock.cpp
    ut_lib_src += mem_utility/ring_buffer.c
endif

#end of protection guard preventing multiple inclusion
endif


