#*******************************************************************************
#  Copyright (c) 2022 BSH Hausgeraete GmbH,
#  Carl-Wery-Str. 34, 81739 Munich, Germany, www.bsh-group.de
#
#  All rights reserved. This program and the accompanying materials
#  are protected by international copyright laws.
#  Please contact copyright holder for licensing information.
#
#*******************************************************************************
#  VARIANT          <$macro("$1_$2 ",$(dmodule),$(dvariant))>
#*******************************************************************************
#  Description      Application data variant build file
#*******************************************************************************

is_data_variant = true

# application specific components
app_components = <$macro("$1_$2 ",$(dmodule),$(dvariant))>

# application specific definitions
defines += 

# additional search paths
search_path +=

FWU_DATA_VARIANT_START_ADDRESS = $(FWU_<$toupper($(dmodule))>_START_ADDRESS)
FWU_DATA_VARIANT_END_ADDRESS = $(FWU_<$toupper($(dmodule))>_END_ADDRESS)

