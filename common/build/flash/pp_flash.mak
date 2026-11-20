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
#  Description      targets for firmware update
#*******************************************************************************

# possible interface protocols are listed below, normaly we are using SWD
#    JTAG
#    ISSP
#    I2C
#    SWD
#    SPI
#    SWD_SWV
pp_interface ?= SWD
# possible values for frequency applicable only to the MiniProg3
# device in SWD/JTAG mode
#    FREQ_48_0
#    FREQ_24_0
#    FREQ_16_0
#    FREQ_03_2
#    FREQ_06_0
#    FREQ_12_0
#    FREQ_08_0
#    FREQ_01_6
#    FREQ_01_5
#    FREQ_03_0
pp_frequency ?= FREQ_03_0
# possible values for MiniProg3 in SWD mode are:
#    FIVE_PINS – 5-pin connector
#    TEN_PINS – 10-pin connector
pp_connector ?= FIVE_PINS
# command line version of PSoC Programmer
ppflash_pl  = $(local_tool_path)/perl/Perl_Ex.pl

#ppflash_perl_lib_path = $(ppflash_perl_path)/lib/
ppflash_perl_lib_path = $(local_tool_path)/perl/lib/

flash_pp_hex_file ?= $(patchbin_out_hex)
flash_pp_log  ?= $(out_path)/$(variant)/bin/pp_log.txt

pp_flash: link pp_flash_only
pp_flash_no_link: pp_flash_only

pp_flash_only:
	@$(call prntGr,-------- Start PSoC Flashing by using perl script and PSoCProgrammerCOM.exe -------------)
# the order of script parameter is fixed: perlscript hexfile logfile interface frequency connector
	@$(perl) -I $(ppflash_perl_lib_path) -I $(local_tool_path)/perl  $(ppflash_pl) $(CURDIR)/$(flash_pp_hex_file) $(CURDIR)/$(flash_pp_log) $(pp_interface) $(pp_frequency) $(pp_connector)
	@$(call prntGr,-------- Flashing done -------------)
