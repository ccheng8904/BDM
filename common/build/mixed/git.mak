#*******************************************************************************
#  Copyright (c) 2017 BSH Hausgeraete GmbH,
#  Carl-Wery-Str. 34, 81739 Munich, Germany, www.bsh-group.de
#
#  All rights reserved. This program and the accompanying materials
#  are protected by international copyright laws.
#  Please contact copyright holder for licensing information.
#
#*******************************************************************************
#  Project          MCU Framework
#*******************************************************************************
#  Description      Git related target and definitions
#*******************************************************************************

git_version := $(shell $(git) --version 2>$(null))

project_rev_hash  ?= 0
version_build     ?= 0x$(project_rev_hash)
defines_version += PROJECT_REV_HASH=UINT32_C(0x$(project_rev_hash))
defines_version += VERSION_BUILD=UINT32_C($(call calc,$(version_build)))

ifneq (,$(git_version))
    # check if MCU FW is GIT repository or submodule
    ifneq (,$(wildcard $(sdk_path)/.git))
        # get FW version, changes in w.d., etc.
        git_local_fw_change := $(shell $(git) -C $(sdk_path) status --porcelain)
        framework_version := $(shell $(git) -C $(sdk_path) describe --tags --abbrev=0)
        
        ifeq (,$(git_local_fw_change))
            mcu_fw_git_rev_hash := $(shell $(git) -C $(sdk_path) rev-parse --short HEAD)
            defines_version += MCU_FW_GIT_REV_HASH=UINT32_C(0x$(mcu_fw_git_rev_hash))
        else
            $(info Working directory of MCU Framework NOT clean!!!)
        endif
        
        $(info ===== Based on MCU Framework ($(framework_version)) =====)
    
    else
        $(info =====  MCU Framework (common) is not GIT repository =====)
    endif

    # check if present project is a GIT repository
    ifneq (,$(wildcard .git))
        # get project version, changes in w.d., etc.
        git_local_change := $(shell $(git) status --porcelain)
        project_version := $(shell $(git) describe --all)
        
        ifeq (,$(git_local_change))
            project_rev_hash := $(shell $(git) rev-parse --short HEAD)
        else
            $(info Working directory NOT clean!!!)
        endif
        # set default build revision if nothing is set

        $(info ===== Based on project revision ($(project_version)) =====)

    else
        $(info =====  Project is not a GIT repository =====)
    endif

else
    $(info GIT not found)
endif
