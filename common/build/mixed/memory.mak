#*******************************************************************************
#  Copyright (c) 2017 BSH Hausgeraete GmbH,
#  Carl-Wery-Str. 34, 81739 Munich, Germany, www.bsh-group.de
#
#  All rights reserved. This program and the accompanying materials
#  are protected by international copyright laws.
#  Please contact copyright holder for licensing information.
#
#*******************************************************************************
#  PROJECT          PP PED Framework
#*******************************************************************************
#  Description      Memory map targets
#*******************************************************************************

# memory map
no_dep_targets += mem_map

# part valid for firmware update 3 (BootManager + lromLoader + programmer + application)
ifeq ($(FW_update_type),FWU3)
    ifneq ($(platform), rx200)
        OTHER_AREA_SIZE      = $(call calcx,0x$(ROM_PHY_END)-0x$(FWU_APP1_END_ADDRESS))
    else
        OTHER_AREA_SIZE      = $(call calcx,0x$(ROM_PHY_START)-0x$(FWU_APP1_START_ADDRESS))
    endif

# part valid for firmware update 4 (gecko_bl application)
else ifeq ($(FW_update_type),FWU4)
    OTHER_AREA_SIZE      = $(call calcx,0x$(ROM_PHY_END)-0x$(FWU_APP1_END_ADDRESS))
endif


ifdef ROM2_PHY_END
    ROM_PHY_END   = $(ROM2_PHY_END)
    ROM_END_LABEL = ROM2_PHY_END
else
    ROM_PHY_END   = $(ROM1_PHY_END)
    ROM_END_LABEL = ROM1_PHY_END
endif


# mem_map target available only for app variants
ifneq (,$(filter $(variant),$(app_variants)))
.PHONY: mem_map
mem_map:

	@setlocal EnableDelayedExpansion
	@echo --- Start of memory map generation ---
	@echo [91m             $(FW_update_type) flash memory layout for         $(platform)     Hex address/sizes[0m
ifeq ($(FW_update_type),FWU1)
ifneq ($(platform), rx200)
	@echo. %a1%
	@echo [33m FWU_ROMLOADER_START_ADDRESS (ROM1_PHY_START) +-------------+ $(FWU_ROMLOADER_START_ADDRESS)H[0m
	@echo [33m                                              ^|             ^|[0m
	@echo [33m                                              ^| romLoader   ^| $(FWU_ROMLOADER_SIZE)H[0m
	@echo [33m                                              ^|             ^|[0m
	@echo [33m                    FWU_ROMLOADER_END_ADDRESS +-------------+ $(FWU_ROMLOADER_END_ADDRESS)H[0m
ifdef FWU_OTP_MEMORY_START_ADDRESS
	@echo [96m                 FWU_OTP_MEMORY_START_ADDRESS +-------------+ $(FWU_OTP_MEMORY_START_ADDRESS)H[0m
	@echo [96m                                              ^| OTP_MEMORY  ^| $(FWU_OTP_MEMORY_SIZE)H[0m
	@echo [96m                   FWU_OTP_MEMORY_END_ADDRESS +-------------+ $(FWU_OTP_MEMORY_END_ADDRESS)H[0m
endif
	@echo [37m                 FWU_APPLICATION_START_ADDRESS+-------------+ $(FWU_APPLICATION_START_ADDRESS)H[0m
	@echo [37m                                              ^|             ^|[0m
	@echo [37m                                              ^|             ^|[0m
	@echo [37m                                              ^| Application ^| $(FWU_APPLICATION_SIZE)H[0m
	@echo [37m                                              ^|             ^|[0m
	@echo [37m                                              ^|             ^|[0m
	@echo [37m     FWU_APPLICATION_START_ADDRESS (ROM1_END) +-------------+ $(FWU_APPLICATION_END_ADDRESS)H[0m
	@echo [37m                                 $(ROM_END_LABEL) +-------------+ $(ROM_PHY_END)H[0m
else
	@echo. %a1%
	@echo [33m FWU_APPLICATION_START_ADDRESS (ROM1_PHY_START) +-------------+ $(FWU_APPLICATION_START_ADDRESS)H[0m
	@echo [33m                                              ^|             ^|[0m
	@echo [33m                                              ^|             ^|[0m
	@echo [33m                                              ^| Application ^| $(FWU_APPLICATION_SIZE)H[0m
	@echo [33m                                              ^|             ^|[0m
	@echo [33m                                              ^|             ^|[0m
	@echo [33m                  FWU_APPLICATION_END_ADDRESS +-------------+ $(FWU_APPLICATION_END_ADDRESS)H[0m
ifdef FWU_OTP_MEMORY_START_ADDRESS
	@echo [96m                 FWU_OTP_MEMORY_START_ADDRESS +-------------+ $(FWU_OTP_MEMORY_START_ADDRESS)H[0m
	@echo [96m                                              ^| OTP_MEMORY  ^| $(FWU_OTP_MEMORY_SIZE)H[0m
	@echo [96m                   FWU_OTP_MEMORY_END_ADDRESS +-------------+ $(FWU_OTP_MEMORY_END_ADDRESS)H[0m
endif
	@echo [37m                   FWU_ROMLOADER_START_ADDRESS+-------------+ $(FWU_ROMLOADER_START_ADDRESS)H[0m
	@echo [37m                                              ^|             ^|[0m
	@echo [37m                                              ^|             ^|[0m
	@echo [37m                                              ^| romLoader   ^| $(FWU_ROMLOADER_SIZE)H[0m
	@echo [37m                                              ^|             ^|[0m
	@echo [37m                                              ^|             ^|[0m
	@echo [37m         FWU_ROMLOADER_END_ADDRESS (ROM1_END) +-------------+ $(FWU_ROMLOADER_END_ADDRESS)H[0m
	@echo [37m                            VECTORS FIX START +-------------+ $(VECTORS_FIX)H[0m
	@echo [37m                                              ^| Fix Vectors ^|[0m
	@echo [37m                                 $(ROM_END_LABEL) +-------------+ $(ROM_PHY_END)H[0m
endif
else ifeq ($(FW_update_type),FWU3)
ifneq ($(platform), rx200)
	@echo.
	@echo [36m FWU_BOOTMANAGER_START_ADDRESS +-------------+ $(ROM1_PHY_START)H[0m
	@echo [36m                               ^|             ^|[0m
	@echo [36m                               ^| BootManager ^| $(FWU_BOOTMANAGER_SIZE)H[0m
	@echo [36m                               ^|             ^|[0m
	@echo [36m   FWU_BOOTMANAGER_END_ADDRESS +-------------+ $(FWU_BOOTMANAGER_END_ADDRESS)H[0m
	@echo [33m         FWU_LRO_START_ADDRESS +-------------+ $(FWU_LRO_START_ADDRESS)H[0m
	@echo [33m                               ^|             ^|[0m
	@echo [33m                               ^| lromLoader  ^| $(FWU_LRO_SIZE)H[0m
	@echo [33m                               ^|             ^|[0m
	@echo [33m           FWU_LRO_END_ADDRESS +-------------+ $(FWU_LRO_END_ADDRESS)H[0m
	@echo [32m  FWU_PROGRAMMER_START_ADDRESS +-------------+ $(FWU_PROGRAMMER_START_ADDRESS)H[0m
	@echo [32m                               ^|             ^|[0m
	@echo [32m                               ^| programmer  ^| $(FWU_PROGRAMMER_SIZE)H[0m
	@echo [32m                               ^|             ^|[0m
	@echo [32m    FWU_PROGRAMMER_END_ADDRESS +-------------+ $(FWU_PROGRAMMER_END_ADDRESS)H[0m
ifdef FWU_OTP_MEMORY_START_ADDRESS
	@echo [96m  FWU_OTP_MEMORY_START_ADDRESS +-------------+ $(FWU_OTP_MEMORY_START_ADDRESS)H[0m
	@echo [96m                               ^| OTP_MEMORY  ^| $(FWU_OTP_MEMORY_SIZE)H[0m
	@echo [96m    FWU_OTP_MEMORY_END_ADDRESS +-------------+ $(FWU_OTP_MEMORY_END_ADDRESS)H[0m
endif
ifeq ($(fwu_flex_partition_used),ENABLED)
	@echo [95mFWU_FLEX_PARTITION_START_ADDR. +-------------+ $(FWU_FLEX_PARTITION_START_ADDRESS)H[0m
	@echo [95m                               ^| FLEX_PART   ^| $(FWU_FLEX_PARTITION_SIZE)H[0m
	@echo [95m  FWU_FLEX_PARTITION_END_ADDR. +-------------+ $(FWU_FLEX_PARTITION_END_ADDRESS)H[0m
endif
	@echo [37m        FWU_APP1_START_ADDRESS +-------------+ $(FWU_APP1_START_ADDRESS)H[0m
	@echo [37m                               ^|             ^|[0m
	@echo [37m                               ^|             ^|[0m
	@echo [37m                               ^| Application ^| $(FWU_APP1_SIZE)H[0m
	@echo [37m                               ^|             ^|[0m
	@echo [37m                               ^|             ^|[0m
	@echo [37m          FWU_APP1_END_ADDRESS +-------------+ $(FWU_APP1_END_ADDRESS)H[0m
	@echo [90m                               ^| other areas ^| $(OTHER_AREA_SIZE)H[0m
	@echo [90m                  $(ROM_END_LABEL) +-------------+ $(ROM_PHY_END)H[0m
else
	@echo.
	@echo [33m                ROM1_PHY_START +-------------+ $(ROM1_PHY_START)H[0m
	@echo [90m                               ^| other areas ^| $(OTHER_AREA_SIZE)H[0m
	@echo [37m        FWU_APP1_START_ADDRESS +-------------+ $(FWU_APP1_START_ADDRESS)H[0m
	@echo [37m                               ^|             ^|[0m
	@echo [37m                               ^|             ^|[0m
	@echo [37m                               ^| Application ^| $(FWU_APP1_SIZE)H[0m
	@echo [37m                               ^|             ^|[0m
	@echo [37m                               ^|             ^|[0m
	@echo [37m          FWU_APP1_END_ADDRESS +-------------+ $(FWU_APP1_END_ADDRESS)H[0m
ifeq ($(fwu_flex_partition_used),ENABLED)
	@echo [95mFWU_FLEX_PARTITION_START_ADDR. +-------------+ $(FWU_FLEX_PARTITION_START_ADDRESS)H[0m
	@echo [95m                               ^| FLEX_PART   ^| $(FWU_FLEX_PARTITION_SIZE)H[0m
	@echo [95m  FWU_FLEX_PARTITION_END_ADDR. +-------------+ $(FWU_FLEX_PARTITION_END_ADDRESS)H[0m
endif
ifdef FWU_OTP_MEMORY_START_ADDRESS
	@echo [96m  FWU_OTP_MEMORY_START_ADDRESS +-------------+ $(FWU_OTP_MEMORY_START_ADDRESS)H[0m
	@echo [96m                               ^| OTP_MEMORY  ^| $(FWU_OTP_MEMORY_SIZE)H[0m
	@echo [96m    FWU_OTP_MEMORY_END_ADDRESS +-------------+ $(FWU_OTP_MEMORY_END_ADDRESS)H[0m
endif
	@echo [32m  FWU_PROGRAMMER_START_ADDRESS +-------------+ $(FWU_PROGRAMMER_START_ADDRESS)H[0m
	@echo [32m                               ^|             ^|[0m
	@echo [32m                               ^| programmer  ^| $(FWU_PROGRAMMER_SIZE)H[0m
	@echo [32m                               ^|             ^|[0m
	@echo [32m    FWU_PROGRAMMER_END_ADDRESS +-------------+ $(FWU_PROGRAMMER_END_ADDRESS)H[0m
	@echo [33m         FWU_LRO_START_ADDRESS +-------------+ $(FWU_LRO_START_ADDRESS)H[0m
	@echo [33m                               ^|             ^|[0m
	@echo [33m                               ^| lromLoader  ^| $(FWU_LRO_SIZE)H[0m
	@echo [33m                               ^|             ^|[0m
	@echo [33m           FWU_LRO_END_ADDRESS +-------------+ $(FWU_LRO_END_ADDRESS)H[0m
	@echo [36m FWU_BOOTMANAGER_START_ADDRESS +-------------+ $(ROM1_PHY_START)H[0m
	@echo [36m                               ^|             ^|[0m
	@echo [36m                               ^| BootManager ^| $(FWU_BOOTMANAGER_SIZE)H[0m
	@echo [36m                               ^|             ^|[0m
	@echo [36m   FWU_BOOTMANAGER_END_ADDRESS +-------------+ $(FWU_BOOTMANAGER_END_ADDRESS)H[0m
	@echo [90m                  $(ROM_END_LABEL) +-------------+ $(ROM_PHY_END)H[0m
endif
else ifeq ($(FW_update_type),FWU4)
	@echo [36m    FWU_GECKO_BL_START_ADDRESS +-------------+ $(ROM1_PHY_START)H[0m
	@echo [36m                               ^|   Gecko     ^|[0m
	@echo [36m                               ^| Boot-Loader ^| $(FWU_GECKO_BL_SIZE)H[0m
	@echo [36m                               ^|             ^|[0m
	@echo [36m      FWU_GECKO_BL_END_ADDRESS +-------------+ $(FWU_GECKO_BL_END_ADDRESS)H[0m
	@echo [37m        FWU_APP1_START_ADDRESS +-------------+ $(FWU_APP1_START_ADDRESS)H[0m
	@echo [37m                               ^|             ^|[0m
	@echo [37m                               ^|             ^|[0m
	@echo [37m                               ^| Application ^| $(FWU_APP1_SIZE)H[0m
	@echo [37m                               ^|             ^|[0m
	@echo [37m                               ^|             ^|[0m
	@echo [37m          FWU_APP1_END_ADDRESS +-------------+ $(FWU_APP1_END_ADDRESS)H[0m
	@echo [90m                               ^| other areas ^| $(OTHER_AREA_SIZE)H[0m
	@echo [90m                  $(ROM_END_LABEL) +-------------+ $(ROM_PHY_END)H[0m
endif
	@echo.
	@echo --- End of memory map generation ---
endif

