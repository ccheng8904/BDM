#*******************************************************************************
#  Copyright (c) 2022 BSH Hausgeraete GmbH,
#  Carl-Wery-Str. 34, 81739 Munich, Germany, www.bsh-group.de
#
#  All rights reserved. This program and the accompanying materials
#  are protected by international copyright laws.
#  Please contact copyright holder for licensing information.
#
#*******************************************************************************
#  PROJECT          PP PED FW
#*******************************************************************************
#  Description      build include PED Framework, subcomponent: veeprom
#*******************************************************************************

#protection guard preventing multiple inclusion
ifndef veeprom_mem_org_mak
veeprom_mem_org_mak := 1


ifeq ($(feature_veeprom),ENABLED)
    ifneq ($(findstring STM32F4,$(derivative)),STM32F4)
        ifeq ($(veeprom_variant), veepromlarge)
            FLASH_PAGES_PER_UNIT                 ?= 2
        else
            FLASH_PAGES_PER_UNIT                 := 1
        endif
        VIRTUAL_EEPROM_UNIT_SIZE             := $(call calcx, 0x$(FLASH_PAGES_PER_UNIT) * 0x$(PAGE_SIZE))
        defines += VEEPROM_FLASH_BLOCK_SIZE=0x$(PAGE_SIZE)u
    else
        FLASH_PAGES_PER_UNIT := 1
        VIRTUAL_EEPROM_UNIT_SIZE := 4000
        defines += VEEPROM_FLASH_BLOCK_SIZE=0x4000u
    endif
    VIRTUAL_EEPROM_SIZE = $(call calcx, 0x$(VIRTUAL_EEPROM_UNIT_SIZE) * 2)
    VIRTUAL_EEPROM = name=VIRTUAL_EEPROM size=0x$(VIRTUAL_EEPROM_SIZE)
    VIRTUAL_EEPROM_UNIT1_START_ADDRESS = $(VIRTUAL_EEPROM_START_ADDRESS)
    VIRTUAL_EEPROM_UNIT2_START_ADDRESS = $(call calcx, 0x$(VIRTUAL_EEPROM_START_ADDRESS) + 0x$(VIRTUAL_EEPROM_UNIT_SIZE))
    defines += VEEPROM_UNIT1_START_ADDRESS=0x$(VIRTUAL_EEPROM_UNIT1_START_ADDRESS)u
    defines += VEEPROM_UNIT2_START_ADDRESS=0x$(VIRTUAL_EEPROM_UNIT2_START_ADDRESS)u
    defines += FLASH_PAGES_PER_UNIT=$(FLASH_PAGES_PER_UNIT)u
endif



#end of protection guard preventing multiple inclusion
endif
