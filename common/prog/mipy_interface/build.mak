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
#  Description      build include for the micropython interface
#*******************************************************************************

#protection guard preventing multiple inclusion
ifndef mipy_interface_build_mak
mipy_interface_build_mak := 1

defines += MIPY_INTERFACE
defines += MIPY_UART_CHANNEL=$(MIPY_UART_CHANNEL)

cfg_hdr += $(addprefix mipy_interface/, testdriver_table_config.h)
cfg_src += $(addprefix mipy_interface/, testdriver_table_config.c)

src += $(addprefix mipy_interface/, testdriver_interface.c)

#end of protection guard preventing multiple inclusion
endif
