#*******************************************************************************
#  Copyright (c) 2024 BSH Hausgeraete GmbH,
#  Carl-Wery-Str. 34, 81739 Munich, Germany, www.bsh-group.de
#
#  All rights reserved. This program and the accompanying materials
#  are protected by international copyright laws.
#  Please contact copyright holder for licensing information.
#
#*******************************************************************************
#  Variant          doctest
#*******************************************************************************
#  Description      definition for doctest variant
#*******************************************************************************
ut_do_app_dtest = true

# Configure debug_extended component for doctest
defines += DBGX_DOCTEST_CONFIG_ENABLED
defines += DBGX_INPUT_FUNCTIONS_ENABLED

# external components
ext_components = 

# add your application specific components here
app_components =
