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
#  Description      Binary generation makefile
#*******************************************************************************

# Clear ccache options
ccache_opt =

# Exclude ccache targets from dependency generation
no_dep_targets += ccache%

# Check if ccache is enabled and supported - by default ccache is disabled
ccache_enabled ?= false
ifeq ($(ccache_enabled), true)
    ifneq (,$(filter keilArm gccArm armclang iarRx, $(cc_build_path)))
        $(info CCACHE is enabled. Please note, that CCACHE is currently available only for experimental evaluation)
        ccache_opt = $(ccache)
    else
        $(info CCACHE is not supported for selected compiler)
    endif
endif


.PHONY: ccache_stat ccache_clean

# Target for showing ccache statistics
ccache_stat:
	@$(ccache) --show-stats

# Clean whole ccache database
ccache_clean:
	@$(ccache) -C
