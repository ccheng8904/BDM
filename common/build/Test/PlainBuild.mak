#*******************************************************************************
#  Copyright (c) 2017 BSH Hausgeraete GmbH,
#  Carl-Wery-Str. 34, 81739 Munich, Germany, www.bsh-group.de
#
#  All rights reserved. This program and the accompanying materials
#  are protected by international copyright laws.
#  Please contact copyright holder for licensing information.
#
#*******************************************************************************
#  PROJECT          PP PED FW
#*******************************************************************************
#  Description      make include for special variants: plainBuild_<comp>_<subcomp>
#*******************************************************************************

plainBuild      := $(call match,$(variant_base),^plainBuild(\w+)$$)

ifdef plainBuild
    std_variant = false
    common_components =
    FW_update_type = FWU0
    
    $(info --- Creating plainBuild for Comp: $(marked_comp), platform: $(platform) ---)
    
endif

