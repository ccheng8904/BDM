#*******************************************************************************
#  Copyright (c) 2017 BSH Hausgeraete GmbH,
#  Carl-Wery-Str. 34, 81739 Munich, Germany, www.bsh-group.de
#
#  All rights reserved. This program and the accompanying materials
#  are protected by international copyright laws.
#  Please contact copyright holder for licensing information.
#
#*******************************************************************************
#  Project          PED FW
#*******************************************************************************
#  Description      targets for flashProgrammer (WinFlash)
#*******************************************************************************

# command line version of WinFlash
# current version in Framework is v4.1
flash_exe  ?= $(flashprogrammer_path)/Flash.exe

ifeq ($(build_type), RELEASE)
    flash_file ?= $(lastword $(release_bin_output_file))
else
    flash_file ?= $(patchbin_out_mot)
endif


# set RAM-loader file
ifdef testClient
    flash_opt += -m $(subst /,\\,$(CURDIR)\\$(flash_file))
    OFFLINE_TIME = 0
else
    flash_opt += -m $(subst /,\\,$(CURDIR)\\$(RAMLOADER))
    OFFLINE_TIME ?= 60
endif

# set application file - the source of data that are going to be flashed
flash_opt += -a $(subst /,\\,$(CURDIR)\\$(flash_file))

# set communication port type
ifeq ($(UDA_TYPE), UDA2)
    ifeq ($(UDA_INTERFACE), USB)
        flash_opt += -i UDA2_Name:$(UDA_NAME)
    else
        flash_opt += -i UDA2_IpHost:$(UDA_NAME)
    endif
else
    flash_opt += -i $(UDA_NAME)
endif

# checking CRC enabled by default
flash_opt_crc ?= true

# set protocol type
flash_opt += -p $(UDA_BUS_TYPE)
# set address of comm. partner
flash_opt += -cp $(strip $(dbus_node_address))$(strip $(dbus_subnode_address))
# set offline delay for the other, besides comm. partner in seconds
flash_opt += -od $(OFFLINE_TIME)
# set “micro-controller.ini” initialization file
flash_opt += -c $(subst /,\\,$(CURDIR)\\$(winflash_cfg_file))
# specifies optional path for log file
flash_opt += -lPath $(subst /,\\,$(bin_path))
ifdef MY_FLASH_INI
# set optional “userfile.ini” file (string – is name of application file with/without full path)
# go page 29 of FlashProgrammer user manual to see userfile.ini structure
    flash_opt += -cf $(MY_FLASH_INI)
else
    ifdef offNode
    # goOffMode sets GoOffline mode, number – available values: 0-target address, 1-broadcast or 2-bridge
    # sdGoOff sets nodes for direct messaging, number – 2 bytes hexadecimal, every bit represents node
    # for direct message; e.g. 0x0406 -> 00000100 00000110b -> Node 2,3,11)
        flash_opt   += -goOffMode 0 -sdGoOff $(offNode) 
    else
        flash_opt   += -goOffMode 1
    endif
    # other paramters in this order:
    # enable reset after flash process,
    # disable to write to EEPROM area,
    # disable resumption of write process after erase error
    # set read timeout in millisecond
    # set number of repeats if communication error - number of repeats
    # set reset delay for communication partner in milliseconds
    # set number of data bits on serial port [5, 6, 7, 8]
    # set type of parity on serial port [0-none, 1-odd, 2-even]
    # set number of stop bits on serial port [0-one, 1-one and half, 2-two bits]
    # set type of parity when transparent mode on USB/D2Bus box is used, number of stop bits: 0-1, 1-1.5, 2-2
    # disable using NMT Shutdown command
    # use new bootloader protocol (BP2)
    # enable automatic reset
    # set level of logging; 0 - no log, 1 - verbose, 2 - data, 3 - info, 4 - warnings, 5 - errors
    flash_opt   += -sy -en -tn -rt 3000 -rp 3 -rd 50 -db 8 -pr 2 -sb 0 -dp 0 -nmtn -nbt -uy -l 1
    # perform CRC Checksum after write process
ifeq ($(flash_opt_crc), true)
    flash_opt   += -crc
endif

endif



flash: link flash_only
flash_no_link: flash_only

flash_only: $(RAMLOADER)
	@echo Log1:$(subst \,/,$(bin_path))/_log$(log_cnt)_$(UDA_NAME).txt
	@echo Start Flashing
	@$(flash_exe) -f $(flash_opt)
	@$(call prntGr,-------- Flashing done -------------)

verify: $(RAMLOADER)
	@echo Start Flash Verify
	@$(flash_exe) -v $(flash_opt)
	@$(call prntGr,-------- Verify done ---------------)


# -- test target for flash option summary ---
test_flashopt:
	@echo Winflasher option summary:
	@$(call prntBl, $(flash_opt))
	@echo.


