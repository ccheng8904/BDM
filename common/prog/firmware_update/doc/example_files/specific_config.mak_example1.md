### The example of the specific_config.mak file related to example in the chapter [3.2 How-to add a custom SW memory module](../README.md#32-how-to-add-a-custom-sw-memory-module)

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
# FWU3 flash memory layout for stm32g0           Hex address/sizes
#  FWU_BOOTMANAGER_START_ADDRESS +-------------+ 8000000H
#                                |             |
#                                | BootManager | 1800H
#                                |             |
#    FWU_BOOTMANAGER_END_ADDRESS +-------------+ 80017ffH
#          FWU_LRO_START_ADDRESS +-------------+ 8001800
#                                |             |
#                                | lromLoader  | 800H
#                                |             |
#            FWU_LRO_END_ADDRESS +-------------+ 8001fffH
#   FWU_PROGRAMMER_START_ADDRESS +-------------+ 8002000H
#                                |             |
#                                | programmer  | 4000H
#                                |             |
#     FWU_PROGRAMMER_END_ADDRESS +-------------+ 8005fffH
#   FWU_OTP_MEMORY_START_ADDRESS +-------------+ 8006000H
#                                | OTP_MEMORY  | 800H
#     FWU_OTP_MEMORY_END_ADDRESS +-------------+ 80067ffH
#         FWU_APP1_START_ADDRESS +-------------+ 8006800H
#                                |             |
#                                |             |
#                                | Application | 19000H
#                                |             |
#                                |             |
#           FWU_APP1_END_ADDRESS +-------------+ 801F7FFH
# FWU_PROCESS_DATA_START_ADDRESS +-------------+ 801F800H
#                                | FWU_PROCESS | 800H
#   FWU_PROCESS_DATA_END_ADDRESS |  _DATA      |
#                   ROM1_PHY_END +-------------+ 801FFFFH

# EXAMPLE: SW memory module 'FWU_PROCESS_DATA' is defined after the module 'FWU_APP1'
FWU_PROCESS_DATA_SIZE = 800
FWU_PROCESS_DATA += name=FWU_PROCESS_DATA size=0x$(FWU_PROCESS_DATA_SIZE)

FWU_APP1_END_ADDRESS := $(call calcx,0x$(ROM1_PHY_END)-0x$(FWU_PROCESS_DATA_SIZE))

FWU_APP_MODULE_LIST += FWU_APP1
FWU_APP_MODULE_LIST += FWU_PROCESS_DATA

# patchbin options for FWU_PROCESS_DATA
ifneq (, $(filter $(variant), BootManager lromLoader programmer DemoApp))
patchbin_opt_proj += -f(0x$(FWU_PROCESS_DATA_START_ADDRESS)+$(crc_offset),0x$(FWU_PROCESS_DATA_END_ADDRESS)):0xFF
patchbin_opt_proj += -CRC(0x$(FWU_PROCESS_DATA_START_ADDRESS)+$(crc_offset),0x$(FWU_PROCESS_DATA_END_ADDRESS))(0x$(FWU_PROCESS_DATA_START_ADDRESS)):$(crctype)
endif
```
[Back to page of firmware_update help](../README.md)
