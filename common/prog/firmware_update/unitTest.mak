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
#  Description      build include firmware update unit test
#*******************************************************************************

ifeq ($(var_subcomp),BLcom)
    #$(error so far test working)
    obj  += firmware_update/BootLoader/stubs/BLcom_testframe
    obj  += firmware_update/BootLoader/stubs/BLcomMap
    search_path += common/prog/firmware_update/BootLoader/stubs
    
else ifeq ($(var_subcomp),BLmessageLoop)
    obj  += firmware_update/BootLoader/stubs/BLmessageLoop_testframe
    search_path += common/prog/firmware_update/BootLoader/stubs
    defines += ROMLOADER

else ifeq ($(var_subcomp),BLservices)
    obj  += firmware_update/BootLoader/stubs/BLservices_testframe
    search_path += common/prog/firmware_update/BootLoader/stubs
    defines += PROC_FAMILY=x86 UC_VENDOR=Intel
    
else
    $(error test-unit curently not supported)
endif

