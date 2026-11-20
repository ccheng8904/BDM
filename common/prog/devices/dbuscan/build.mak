#*******************************************************************************
#  Copyright (c) 2022 BSH Hausgeraete GmbH,
#  Carl-Wery-Str. 34, 81739 Munich, Germany, www.bsh-group.de
#
#  All rights reserved. This program and the accompanying materials
#  are protected by international copyright laws.
#  Please contact copyright holder for licensing information.
#
#*******************************************************************************
#  PROJECT          Generic SW
#*******************************************************************************
#  Description      build include for devices, subcomponent: dbuscan
#*******************************************************************************

# protection guard preventing multiple inclusion
ifndef dbuscan_build_mak
    dbuscan_build_mak := 1

search_path += common/prog/devices/dbuscan

code_gen_cfg += devices/dbuscan/config.json

ifeq ($(std_variant),true)
    obj += devices/dbuscan/dbuscan_drv
    ifdef bootLoader
        obj += devices/dbuscan/dbuscan_bl
    else
        obj += devices/dbuscan/dbuscan_dbus
    endif

    cfg_obj += devices/dbuscan/$(platform)/dbuscan_drv_cfg
    ifdef app_variant
        cfg_obj += devices/dbuscan/dbuscan_dbus_cfg
    endif
endif

dep_component += MSP/mcal math

defines += DBUSCAN_INCLUDED

# end of protection guard preventing multiple inclusion
endif
