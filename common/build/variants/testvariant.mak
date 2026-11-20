#*******************************************************************************
#  Copyright (c) 2017 BSH Hausgeraete GmbH,
#  Carl-Wery-Str. 34, 81739 Munich, Germany, www.bsh-group.de
#
#  All rights reserved. This program and the accompanying materials
#  are protected by international copyright laws.
#  Please contact copyright holder for licensing information.
#
#*******************************************************************************
#  Project          Generic SW
#*******************************************************************************
#  Description      handling of testvariant
#*******************************************************************************


common_components = $(msp) dbus ped_fw firmware_update


# include definitions for test server application
include common/build/Test/TestServer.mak
# include definitions for test application
include common/build/Test/TestClient.mak
# include definitions for test application on HW
include common/build/Test/TestOnHW.mak
# also plain gcc/... build is possbile
include common/build/Test/PlainBuild.mak



