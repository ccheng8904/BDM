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
#  Description      build include for MCAL component
#*******************************************************************************

#protection guard preventing multiple inclusion
ifndef mcal_build_mak
mcal_build_mak := 1


mcal_supported_modules_stm32g0   = mpcm mdio muart mexti msup mwdt mtim madc mi2c mspi mdma mdac
mcal_supported_modules_stm32g4   = mpcm mdio muart mexti msup mwdt mtim madc mi2c mspi mdma mdac
mcal_supported_modules_mkv5x     = mpcm mdio muart mexti msup mwdt mtim madc mi2c mspi
mcal_supported_modules_efr32xg21 = mdio muart mexti msup mi2c mspi
mcal_supported_modules_ra4       = mpcm mdio muart mexti msup mwdt mtim madc mi2c
mcal_supported_modules_ra2       = mpcm mdio muart mexti msup mwdt mtim madc mi2c mspi mdma
mcal_supported_modules_sh30f     = mpcm mdio muart mexti msup mwdt mtim madc mi2c mspi mdma
mcal_supported_modules_gd32f33   = mpcm mdio muart mexti msup mwdt mtim madc mi2c mspi mdma
mcal_supported_modules_psoc4     = mdio muart mexti msup mwdt mspi
mcal_supported_modules_gd32f30   = mpcm mdio muart mexti msup mwdt mtim madc mi2c mspi mdma
mcal_supported_modules_mspm0     = mpcm mdio muart mexti msup mwdt mtim madc mi2c mspi
mcal_supported_modules_gd32e50   = mpcm mdio muart mexti msup mwdt mtim madc mi2c mspi mdma

mcal_modules ?= $(mcal_supported_modules_$(platform))
ifneq (,$(filter-out $(mcal_supported_modules_$(platform)),$(mcal_modules)))
    $(error "ERROR. Supported modules are: $(mcal_supported_modules_$(platform)). Modules: $(filter-out $(mcal_supported_modules_$(platform)),$(mcal_modules)) are not valid")
endif

mcal_path = common/prog/MSP/mcal

search_path += $(mcal_path)/$(platform)
search_path += $(cfg_prog_path)/MSP/mcal/$(platform)

defines += MCAL_INCLUDED

ifneq ($(unit_test_variant), true)
    gen_src += MSP/mcal/$(platform)/mcal_channels.c
    gen_hdr += MSP/mcal/$(platform)/mcal_channels.h
    code_gen_cfg += $(sdk_prog_path)/MSP/mcal/$(platform)/mcal_channels.json
    ifeq ($(platform),ra2)
        code_gen_cfg += $(sdk_prog_path)/MSP/mcal/$(platform)/mcal_channels_RA2$(ra2_group_type).json
    endif

    ifeq ($(platform),mspm0)
        ifeq ($(mspm0_main_type),G)
            code_gen_cfg += $(sdk_prog_path)/MSP/mcal/$(platform)/mcal_channels_MSPM0$(mspm0_main_type)$(mspm0_sub_type).json
        else
            code_gen_cfg += $(sdk_prog_path)/MSP/mcal/$(platform)/mcal_channels_MSPM0$(mspm0_main_type).json
        endif
    endif

    nolib_src += MSP/mcal/mcal.c
    
    ifneq (,$(filter $(mcal_modules),mdio))
        nolib_src += $(addprefix MSP/mcal/,mdio.c $(platform)/mdio_mc.c)
        defines += MCAL_MDIO_INCLUDED
    endif
    
    ifneq (,$(filter $(mcal_modules),muart))
        nolib_src += $(addprefix MSP/mcal/,muart.c $(platform)/muart_mc.c)
        defines += MCAL_MUART_INCLUDED
    endif
    
    ifneq (,$(filter $(mcal_modules),mexti))
        nolib_src += $(addprefix MSP/mcal/,mexti.c $(platform)/mexti_mc.c)
        defines += MCAL_MEXTI_INCLUDED
    endif
    
    ifneq (,$(filter $(mcal_modules),mpcm))
        nolib_src += $(addprefix MSP/mcal/,mpcm.c $(platform)/mpcm_mc.c)
        defines += MCAL_MPCM_INCLUDED
    endif
    
    ifneq (,$(filter $(mcal_modules),madc))
        nolib_src += $(addprefix MSP/mcal/,madc.c $(platform)/madc_mc.c)
        defines += MCAL_MADC_INCLUDED
    endif
    
    ifneq (,$(filter $(mcal_modules),mtim))
        nolib_src += $(addprefix MSP/mcal/,mtim.c $(platform)/mtim_mc.c)
        defines += MCAL_MTIM_INCLUDED
    endif
    
    ifneq (,$(filter $(mcal_modules),mwdt))
        nolib_src += $(addprefix MSP/mcal/,mwdt.c $(platform)/mwdt_mc.c)
        defines += MCAL_MWDT_INCLUDED
    endif
    
    ifneq (,$(filter $(mcal_modules),mi2c))
        nolib_src += $(addprefix MSP/mcal/,mi2c.c $(platform)/mi2c_mc.c)
        defines += MCAL_MI2C_INCLUDED
    endif
    
    ifneq (,$(filter $(mcal_modules),mspi))
        nolib_src += $(addprefix MSP/mcal/,mspi.c $(platform)/mspi_mc.c)
        defines += MCAL_MSPI_INCLUDED
    endif
    
    ifneq (,$(filter $(mcal_modules),muart mspi mi2c))
        nolib_src += MSP/mcal/mcom.c
        defines += MCAL_MCOM_INCLUDED
    endif
    
    ifneq (,$(filter $(mcal_modules),mdma))
        nolib_src += $(addprefix MSP/mcal/,mdma.c $(platform)/mdma_mc.c)
        defines += MCAL_MDMA_INCLUDED
    endif
    
    ifneq (,$(filter $(mcal_modules),mdac))
        ifneq ($(dac_present),false)
            nolib_src += $(addprefix MSP/mcal/,mdac.c $(platform)/mdac_mc.c)
            defines += MCAL_MDAC_INCLUDED
        endif
    endif
    
    nolib_src += MSP/mcal/msup.c
    defines += MCAL_MSUP_INCLUDED
    
else
# unit_test_variant == true
    defines += MCAL_NDEBUG
    lib_src    += MSP/mcal/msup.c
    lib_src    += MSP/mcal/test/msup.mock.c
    lib_src    += MSP/mcal/test/madc.mock.cpp
    lib_src    += MSP/mcal/test/madc_priv.mock.cpp
    lib_src    += MSP/mcal/test/msup.mock.cpp
    lib_src    += MSP/mcal/test/mcal.mock.cpp
    lib_src    += MSP/mcal/test/mcom.mock.cpp
    ut_lib_src += MSP/mcal/madc.c
    ut_lib_src += MSP/mcal/mcom.c
    ut_lib_src += MSP/mcal/mcal.c
endif

dep_component += std_lib

#end of protection guard preventing multiple inclusion
endif
