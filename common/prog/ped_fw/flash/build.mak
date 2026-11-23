#  All rights reserved. This program and the accompanying materials
#  are protected by international copyright laws.
#  Please contact copyright holder for licensing information.
#
#*******************************************************************************
#  PROJECT          PP PED FW
#*******************************************************************************
#  Description      build include PED Framework, subcomponent: flash
#*******************************************************************************

#protection guard preventing multiple inclusion
ifndef flash_build_mak
flash_build_mak := 1

# default variant of flash component
# possible variants are flash and flashinram
flash_variant ?= flash


# is_flash_supported_platform contains platform name if platform is supported
flash_supported_platforms := stm32f0 stm32f1 stm32f3 stm32f4 stm32g0 mke14z stm32l4 stm32g4 mkv5x gd32e23 ra4 ra2 stm32h7 gd32f33 gd32e50 gd32f30 mspm0 cms32m67
is_flash_supported_platform = $(filter %$(platform),$(flash_supported_platforms))


ifeq ($(marked_comp),ped_fw/flash)
    obj_test = flash flashXs $(platform)/flashMcXs
    add_stubfunc +=
endif


ifeq ($(std_variant),true)

# Test if is_flash_supported_platform is not empty --> platform is supported by flash
ifneq ($(strip $(is_flash_supported_platform)),)
    obj     += $(addprefix ped_fw/flash/,flash $(platform)/flashMc)
    cfg_obj += $(addprefix ped_fw/flash/,flashXs $(platform)/flashMcXs)
    cfg_hdr += $(addprefix ped_fw/flash/,flashXs.h $(platform)/flashMcXs.h)

    search_path += $(app_path)/prog/ped_fw/flash/$(platform)
    doxy_image_path_ += ped_fw/flash/doc

    ifeq ($(flash_variant), flash)
        # standard flash - no options needed
    else ifeq ($(flash_variant), flashinram)
        defines += WORK_FROM_RAM
    else
        $(error Invalid flash_variant=$(flash_variant))
    endif

    # to enable debug of flash, define flash_debug=true
    ifeq ($(flash_debug),true)
        debug    = true
        # assert is included
        defines += ASSERT_INCLUDED
        cfg_obj += $(addprefix ped_fw/flash/,assertXs)
        cfg_hdr += $(addprefix ped_fw/flash/,assertXs.h)
        obj_test += AssertXs
    endif

else
    $(error FLASH component is not supported for platform $(platform))
endif

endif

#end of protection guard preventing multiple inclusion
endif
