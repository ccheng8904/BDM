### The example of the file specific_config.mak

```Makefile
#*******************************************************************************
#  Copyright (c) 2017 BSH Hausgeraete GmbH,
#  Carl-Wery-Str. 34, 81739 Munich, Germany, www.bsh-group.de
#
#  All rights reserved. This program and the accompanying materials
#  are protected by international copyright laws.
#  Please contact copyright holder for licensing information.
#
#*******************************************************************************
#  PROJECT          DemoApp
#*******************************************************************************
#  Description      Specific build settings for all variants
#*******************************************************************************
# SW memory module FWU_PROCESS_DATA to be placed before FWU_APP1 (and after FWU_OTP_MEMORY) module
FWU_APP1_SIZE = 19000
FWU_PROCESS_DATA_SIZE = 800

FWU_PROCESS_DATA += name=FWU_PROCESS_DATA size=0x$(FWU_PROCESS_DATA_SIZE)

FWU_APP_MODULE_LIST += FWU_PROCESS_DATA
FWU_APP_MODULE_LIST += FWU_APP1

# patchbin options for FWU_PROCESS_DATA
ifneq (, $(filter $(variant), BootManager lromLoader programmer DemoApp))
patchbin_opt_proj += -f(0x$(FWU_PROCESS_DATA_START_ADDRESS)+$(crc_offset),0x$(FWU_PROCESS_DATA_END_ADDRESS)):0xFF
patchbin_opt_proj += -CRC(0x$(FWU_PROCESS_DATA_START_ADDRESS)+$(crc_offset),0x$(FWU_PROCESS_DATA_END_ADDRESS))(0x$(FWU_PROCESS_DATA_START_ADDRESS)):$(crctype)
endif

```
[Back to page of firmware_update help](../README.md)