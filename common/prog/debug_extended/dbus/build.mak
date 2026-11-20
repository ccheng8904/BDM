#*******************************************************************************
#  Copyright (c) 2017 BSH Hausgeraete GmbH,
#  Carl-Wery-Str. 34, 81739 Munich, Germany, www.bsh-group.de
#
#  All rights reserved. This program and the accompanying materials
#  are protected by international copyright laws.
#  Please contact copyright holder for licensing information.
#
#*******************************************************************************
#  PROJECT          Debug_extended component
#*******************************************************************************
#  Description      DBus component for debug_extended component
#*******************************************************************************

#protection guard preventing multiple inclusion
ifndef dbgx_dbus_build_mak
dbgx_dbus_build_mak := 1

# adding source modules
src += debug_extended/dbus/dbgx_msg_handlers.c
src += debug_extended/dbus/dbgx_msg_lists.c

# link DBGX Transmit and Receive Objects to BAL 
defines += DBGX_INCLUDED

#end of protection guard preventing multiple inclusion
endif
