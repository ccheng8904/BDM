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


# EXAMPLE:
# Virtual EEPROM area 'VIRTUAL_EEPROM' is defined after the SW memory module 'FWU_APP1'
# the APP1 size must be provided for correct auto-generation of file fwu_mem_map.mak
# Calculated manually: APP1 size = ROM1_PHY_END - VIRTUAL_EEPROM_SIZE - FWU_APP1_START_ADDRESS + 1
#                                = 801FFFFH - 2000H - 8006800H + 1 = 17800H
override FWU_APP1 = name=FWU_APP1 size=0x17800

FWU_APP_MODULE_LIST  = FWU_APP1
FWU_APP_MODULE_LIST += VIRTUAL_EEPROM
```

[Back to page of firmware_update help](../README.md)