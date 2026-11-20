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
#  Description      make include for special variant: testServer
#*******************************************************************************

# Naming for testServer variants:
# e.g. mke14z/testServer
testServer      := $(call match,$(variant_base),^testServer$$)

ifdef testServer
    # special variant: testServer
    testServer  = 1
    
    variant_comp    = 
    variant_subcomp = 
    
    $(info --- Creating Test-Server application for platform: '$(platform)' ---)
endif
