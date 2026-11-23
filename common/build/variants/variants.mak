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
#  Description      handling of default variants / variant naming
#*******************************************************************************

# tool for instrumenting code, for having
# more power over taken implementation and results
include common/build/Test/Instrument.mak

# TODO: Remove lines 20-47 after test scripts are updated
# usual variant naming: <variant> --> this file does nothing special
# variant naming handled here:
# <platform>_[toolchain]/<variant>
# if toolchain may be omitted, in this case default compiler will be used

# remove possible platform folder form variant
variant_base  = $(notdir $(variant))
variant_dir   = $(patsubst %/,%,$(dir $(variant)))
std_variant = true
#$(error $(variant_base))
sup_platforms_pat  := mkv5x|mke14z|stm32g0|stm32g4|stm32f0|stm32f1|stm32f3|stm32f4|stm32l4|Intel|efr32xg21
sup_toolchains_pat := keilArm|gccArm|armclang|gccIntel

ifneq (,$(filter $(cc_build_path),keilArm gccArm armclang))
    cortexm_core_present = true
endif

ifneq ($(variant_base), $(variant))
# variant name contains folder --> special naming!
    platform    := $(call match,$(variant_dir),^($(sup_platforms_pat))(?:_\w+)?$$)
    cc_build_path   := $(call match,$(variant_dir),^(?:$(sup_platforms_pat))_($(sup_toolchains_pat))$$)
    ifdef platform
        ifndef cc_build_path
            $(info Hint: toolchain not provided, default compiler used)
        endif

        # selects the required micro controller support package
        msp = MSP/$(platform)
    endif

    #set default implementation only for custom/test variant builds
    #otherwise it can affect built-in-variants
    implementation ?= $(platform)

    include common/build/variants/testvariant.mak
endif

include common/build/variants/testinclude.mak

ifeq ($(variant_base), BootManager)
    include common/build/variants/bootManager.mak
    fwu_production_data = true
endif

ifeq ($(variant_base), testApp)
    include common/build/variants/testApp.mak
endif

ifeq ($(variant_base), romLoader)
    include common/build/variants/romLoader.mak
endif

ifeq ($(variant_base), lromLoader)
    include common/build/variants/lromLoader.mak
endif

ifeq ($(variant_base), ramLoader)
    include common/build/variants/ramLoader.mak
endif

ifeq ($(variant_base), programmer)
    include common/build/variants/programmer.mak
    fwu_production_data = true
endif

ifeq ($(variant_base), gecko_bl)
    include common/build/variants/gecko_bl.mak
    fwu_production_data = true
endif

#------- Special BTM FWU4 variants --------
# variant includes are EFR21 specific
ifeq ($(variant_base), gecko_bl)
    include common/build/variants/gecko_bl.mak
endif

#---- END: Special BTM FWU4 variants ------

ifeq ($(variant_base), fsTestHarness)
    $(info load settings for fsTestHarness)
    include common/build/variants/fsTestHarness.mak
endif

ifeq ($(variant_base), HALTest)
    $(info load settings for HAL-Test)
    include common/build/variants/test_mcal.mak
endif

ifneq (,$(filter $(variant_base),romLoader lromLoader ramLoader programmer))
    ifeq ($(dbus_mapping),dbuscan)
        common_components += devices/dbuscan MSP/mcal
        mcal_modules ?= mdio mexti mspi msup mwdt
        ifneq (,$(filter $(platform),ra2))
            mcal_modules += mpcm
        endif
    endif
endif


# variant for doctest
ifeq ($(variant_base), dtest)
    include common/build/variants/dtest.mak
endif
ifeq ($(ut_do_app_dtest), true)
    ut_do_app_test = true
    include common/build/variants/dtest.mak
endif

# variant for Tautest
ifeq ($(variant_base), ttest)
    include common/build/variants/ttest.mak
endif
ifeq ($(ut_do_app_ttest), true)
    ut_do_app_test = true
    include common/build/variants/ttest.mak
endif

# Code generator entry for generating tautest variant build file
code_gen += var_tautest_mak
var_tautest_mak_template     = $(template_folder)/var_tautest_build.tpl.mak
var_tautest_mak_output       = $(app_path)/build/tautest.mak
var_tautest_mak_overwrite    = false

# Code generator entry for generating tautest variant build file
code_gen += var_doctest_mak
var_doctest_mak_template     = $(template_folder)/var_doctest_build.tpl.mak
var_doctest_mak_output       = $(app_path)/build/doctest.mak
var_doctest_mak_overwrite    = false

RAMLOADER  ?= $(out_path)/ramLoader/bin/ramLoader.mot
ifeq ($(FW_update_type), FWU3)
    BOOTLOADER ?= $(out_path)/lromLoader/bin/lromLoader.mot
    btl_variant = lromLoader
    BOOTMANAGER = $(out_path)/BootManager/bin/BootManager.mot
    PROGRAMMER  = $(out_path)/programmer/bin/programmer.mot
    fwu_variants = BootManager programmer lromLoader ramLoader
else ifeq ($(FW_update_type), FWU1)
    BOOTLOADER ?= $(out_path)/romLoader/bin/romLoader.mot
    btl_variant = romLoader
    fwu_variants = romLoader ramLoader
else ifeq ($(FW_update_type), FWU4)
    ifneq ($(platform),efr32xg21)
        $(error FWU4 not provided by platform $(platform)!)
    endif
    GECKO_BL   ?= $(out_path)/gecko_bl/bin/gecko_bl.mot
    fwu_variants = gecko_bl
else ifeq ($(FW_update_type), FWU0)
    bootloader_merge = false
    # for intel build, avoid output "No firmware update variant FWU defined"
else
    $(info !!No firmware update variant FWU defined!!)
endif


ifneq (,$(filter $(variant_base),$(built_in_variants)))
    $(info load settings for $(variant_base))
    defines       += BUILT_IN_VARIANT
    build_in_variant = true
else
    defines       += APP_VARIANT
    app_variant = true
    # by default production messages and related defines are available
    # in FWU3 || FWU4 application type only
    ifneq (,$(filter $(FW_update_type),FWU3 FWU4))
        fwu_production_data = true
    endif
endif

ifneq (,$(filter $(variant_base),$(fwu_variants)))
    defines       += FWU_VARIANT
    ifndef fwu_conditional_make
        build_type     = RELEASE
    endif
    optim_level    = $(fwu_optim_level)
    cc_rtti        = DISABLED
    cc_link_time_opt = DISABLED
    auto_lint      = false
endif

ifneq (,$(filter $(variant_base),romLoader lromLoader gecko_bl ramLoader))
    defines       += BOOTLOADER_VARIANT
endif


ifdef project
    project_make_str = project=$(project)
endif

ifeq (,$(filter $(variant_base),romLoader lromLoader))
    $(BOOTLOADER):
		@$(call prntRd,To generate/rebuild  Boot-Loader type:)
		@$(call prntGr,make variant=$(btl_variant) $(project_make_str))
endif

ifneq ($(variant_base), ramLoader)
$(RAMLOADER):
	@$(call prntRd,To generate/rebuild RAM-Loader type:)
	@$(call prntGr,make variant=ramLoader $(project_make_str))
endif

ifneq ($(variant_base), BootManager)
$(BOOTMANAGER):
	@$(call prntRd,To generate/rebuild Boot-Manager type:)
	@$(call prntGr,make variant=BootManager $(project_make_str))
endif

ifneq ($(variant_base), programmer)
$(PROGRAMMER):
	@$(call prntRd,To generate/rebuild programmer type:)
	@$(call prntGr,make variant=programmer $(project_make_str))
endif


$(fwu_variants):
	$(MAKE) -f $(firstword $(MAKEFILE_LIST)) $(MFLAGS) variant=$@ -s

# Keep the generic single-variant project behavior aligned with the default
# build target: "make all" should produce the same application artifacts as
# "make" without recursing into all FWU variants.
all: DEF_GOAL
