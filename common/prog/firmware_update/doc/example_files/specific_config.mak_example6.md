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
#         FWU_APP1_START_ADDRESS +-------------+ 8006000H
#                                |             |
#                                |             |
#                                | Application | 19800H
#                                |             |
#                                |             |
#           FWU_APP1_END_ADDRESS +-------------+ 801F7FFH
#   FWU_OTP_MEMORY_START_ADDRESS +-------------+ 801f800H
#                                | OTP_MEMORY  | 800H
#     FWU_OTP_MEMORY_END_ADDRESS |             |
#                   ROM1_PHY_END +-------------+ 801FFFFH

override FWU_COMMON_MODULE_LIST = FWU_BOOTMANAGER FWU_LRO FWU_PROGRAMMER
FWU_APP1_END_ADDRESS = 801F7FF
FWU_OTP_MEMORY_SIZE  = 800
FWU_APP_MODULE_LIST += FWU_APP1
FWU_APP_MODULE_LIST += FWU_OTP_MEMORY
```

[Back to page of firmware_update help](../README.md)