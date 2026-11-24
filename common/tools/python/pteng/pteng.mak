#*******************************************************************************
#  Copyright (c) 2017 BSH Hausgeraete GmbH,
#  Carl-Wery-Str. 34, 81739 Munich, Germany, www.bsh-group.de
#
#  All rights reserved. This program and the accompanying materials
#  are protected by international copyright laws.
#  Please contact copyright holder for licensing information.
#
#*******************************************************************************
#  Project          PTEng Implementation
#*******************************************************************************
#  Description      defines recipes for testbatch und unit-testinstrumentation
#*******************************************************************************


###############################################################################################
# ****** SW Unit Test Configuration ###########################################################
#
# ****** Configure Testframing: testframe all translation units but OS ***********************
#C-Files that you want to testframe (without test instrumentation)
CFILES_TO_TESTFRAME += $(obj_test) $(obj_tframe)  
#
#
# ****** Configure Stub-Files: either generate or provide (exception OS unit ti removed) *****
# specify for which units stub files shall be generated
GENERATE_STUBS_FOR_UNITS += $(obj_test) $(obj_stub) $(obj_tframe) 
#
#
#specify provided stub-files, i.e. for libs
#provided stub-call redirect-files
PROVIDED_REDIRECT_HEADER_FOR_LIBS += $(obj_custom_stub)
#provided stub.c-files to link
PROVIDED_STUB_CFILES_FOR_LIBS += $(obj_custom_stub)
#
#
#for non-Ansi-conform Signatures, Stub-files are provided,
#therefore, those units won't be stubbed
CFILES_WITH_PROVIDED_STUB_FILES += $(obj_nostub)
#
#
# ****** Configure which units shall be put under test and which units shall be stubbed ********
# specify which units shall be put under test (name of C-file)
#On HW you can basically test all c-files except dbus and OS
#On PC-Test you can at least test the application-files
UNITS_TO_PUT_UNDER_TEST += $(obj_test)
#
#
#specify which units (that are not under test) shall be stubbed 
#(if called by units that are under test, calls to functions of units, 
#that are under test are stubbed anyway)
FOR_UNITS_UNDER_TEST_STUB += COMMONSTUB $(obj_stub) $(obj_custom_stub)
#
#
# ****** Configure C-Files not to be included in testframes **************************************
#It can happen, that units occur, that are not under test but testframed, or that can not be 
#translated on PC, in this case dont include the units source in the testframe
#Also dont include c-files in testframes for units, that are not under test
NO_CFILE_INCLUDE_IN_TESTFRAME += $(obj_no_inc_in_tframe)
#
#
# ****** Create Stub for an undefined function ***************************************************
#Sometimes for definitions of variables, functions are used, that are not defined in the testframed 
#units, therefore the possibility is given to add additional stubs. 
ADDITIONAL_STUBFUNCTIONS += $(add_stubfunc)
#
# ****** Create Stub for an undefined function ***************************************************
#Sometimes for definitions of variables, functions are used, that are not defined in the testframed 
#units, therefore the possibility is given to add additional stubs. 
EXCLUDE_LOCAL_STUB += $(exclude_local_stub)
#
###################################################################################################


###################################################################################################
#*****Pathes                                                                                   #
###################################################################################################
pteng_modules                   = $(py_modules)/pteng
HWTarget_path                   = $(pteng_modules)/HWTarget
pteng_app                       = $(pteng_modules)/TEngApplPython.exe

utest_gen_stubs_from_axivion_py = $(py_modules)/mkStubs_from_Axivion.py
utest_gen_testframes_py         = $(py_modules)/mkTestFrames.py  

template_folder                 = $(local_tool_path)/Templates
template_defaultsetup_py        = $(template_folder)/Template_DefaultSetup.py
template_defaultsetup_ci_py     = $(template_folder)/Template_DefaultSetup_ci.py
template_testbatch_py           = $(template_folder)/Template_test.bat


###################################################################################################
#*****MACROS                                                                                      #
###################################################################################################

tiFolder = $(stub_path)

BUS_TYPE        ?= DBUS2
HOST_ADR        = 12
DBUSFLAGS       = VDBUS_FLAG_9600_BAUD,VDBUS_FLAG_NO_COLLISION

#UDA for test environment
ifdef UDA
    UDA_TENG     ?= $(UDA)
else
    UDA_TENG     ?= UDA1
endif

#baud rate for test environment
BAUD_TENG    ?= VDBUS_FLAG_9600_BAUD    

TESTMSGSRV_SUBNODE_ADDRESS ?= 10

ifdef unit_test

    DEFAULT_TARGETS != link

    defines_cl += WORK_WITH_WRAPPER
    defines += TENG_INCLUDED
endif

stub_locals ?= true
place_covr ?= true


#Options for the DefaultSetup.py and TestCoverage.py
CONFIG_PY_OPT += COM_CFG_filepath=$(CURDIR)/$(PATH_COMCONFIG)
CONFIG_PY_OPT += PSTD_FILEPATH=$(CURDIR)/$(axivion_pstd)
CONFIG_PY_OPT += PSYD_FILEPATH=$(CURDIR)/$(maksym_psyd)
CONFIG_PY_OPT += CVRLI_FILEPATH=$(CURDIR)/$(tiFolder)/COVR_perLineInfo.json


#Options for the COMConfig.xml
ifneq ($(UDA), $(NULL))
COM_CFG_filecontent += <UDA_TENG>$(UDA_TENG)</UDA_TENG>
endif

ifneq ($(NODE_ADDRESS), $(NULL))
COM_CFG_filecontent += <NODE_ADDRESS>$(NODE_ADDRESS)</NODE_ADDRESS>
endif

ifneq ($(DBUSFLAGS), $(NULL))
COM_CFG_filecontent += <DBUS_FLAGS>$(DBUSFLAGS)</DBUS_FLAGS>
endif

ifneq ($(HOST_ADR), $(NULL))
COM_CFG_filecontent += <HOST>$(HOST_ADR)</HOST>
endif

COM_CFG_filecontent += <TESTMSGSRV_SUBNODE_ADDRESS>$(TESTMSGSRV_SUBNODE_ADDRESS)</TESTMSGSRV_SUBNODE_ADDRESS>
COM_CFG_filecontent += <FOLDER>$(variant)</FOLDER>
COM_CFG_filecontent += <variant>$(variant_base)</variant>


#Options for creating the test.bat
TESTBATCH_OPT += PYTHONHOME=$(python_path)
TESTBATCH_OPT += PYTHONPATH=$(python_path);$(python_path)/DLLs;$(python_path)/Lib;$(CURDIR)/$(pteng_modules);$(CURDIR)/$(local_tool_path)/python;$(axivion_python_module_path);
TESTBATCH_OPT += PATH=$(python_path);$(CURDIR)/$(pteng_modules);$(gcc_intel_path)/bin;$(axivion_python_module_path);$(axivion_python_module_path_bauhaus);%Path%
TESTBATCH_OPT += PATH_DEFAULTSETUP_PY=$(variant_base)_DefaultSetup.py
#TESTBATCH_OPT += PATH_TESTCOVERAGE_PY=$(PATH_TESTCOVERAGE)
TESTBATCH_OPT += PYTHON=@$(python) 

#we need macro to destine HW or PC-Test
ifneq ($(platform),Intel)
TESTBATCH_OPT += PTENG_BINARY=$(CURDIR)/$(pteng_app)
else
TESTBATCH_OPT += PTENG_BINARY=$(CURDIR)/$(out_path)/$(variant)/bin/$(variant_base).exe
endif


#Configure the Stub-Algorithm
STUB_GEN_OPT_cfg_file += <project>$(variant_base)</project>
STUB_GEN_OPT_cfg_file += <type_system_key>$(UTEST_TYPE_SYSTEM)</type_system_key>
STUB_GEN_OPT_cfg_file += <Axivion_IR_filepath>$(axivion_uti_ir)</Axivion_IR_filepath>
STUB_GEN_OPT_cfg_file += <template_folder>$(template_folder)</template_folder>
STUB_GEN_OPT_cfg_file += <stub_rootfolder>$(tiFolder)</stub_rootfolder>
STUB_GEN_OPT_cfg_file += <axivion_py_modules>$(axivion_python_module_path)</axivion_py_modules>
STUB_GEN_OPT_cfg_file += <stub_locals>$(stub_locals)</stub_locals>
STUB_GEN_OPT_cfg_file += <place_covr>$(place_covr)</place_covr>


ifneq ($(GENERATE_STUBS_FOR_UNITS), $(NULL))
STUB_GEN_OPT_cfg_file += $(call xml_embrace,$(GENERATE_STUBS_FOR_UNITS),GENERATE_STUBS_FOR_UNITS)
endif

ifneq ($(UNITS_TO_PUT_UNDER_TEST), $(NULL))
STUB_GEN_OPT_cfg_file += $(call xml_embrace,$(UNITS_TO_PUT_UNDER_TEST),UNITS_TO_PUT_UNDER_TEST)
endif

ifneq ($(CFILES_TO_TESTFRAME), $(NULL))
STUB_GEN_OPT_cfg_file += $(call xml_embrace,$(CFILES_TO_TESTFRAME),Units_to_TF)
endif

ifneq ($(CFILES_WITH_PROVIDED_STUB_FILES), $(NULL))
STUB_GEN_OPT_cfg_file += $(call xml_embrace,$(CFILES_WITH_PROVIDED_STUB_FILES),CFILES_WITH_PROVIDED_STUB_FILES)
endif

ifneq ($(PROVIDED_REDIRECT_HEADER_FOR_LIBS), $(NULL))
STUB_GEN_OPT_cfg_file += $(call xml_embrace,$(PROVIDED_REDIRECT_HEADER_FOR_LIBS),provided_rd_Header)
endif

ifneq ($(ADDITIONAL_STUBFUNCTIONS), $(NULL))
STUB_GEN_OPT_cfg_file += $(call xml_embrace_no_uc,$(ADDITIONAL_STUBFUNCTIONS),ADDITIONAL_STUBFUNCTIONS)
endif

ifneq ($(EXCLUDE_LOCAL_STUB), $(NULL))
STUB_GEN_OPT_cfg_file += $(call xml_embrace_no_uc,$(EXCLUDE_LOCAL_STUB),EXCLUDE_LOCAL_STUB)
endif


#Configure the Testframe-Algorithm
TESTFRAME_GEN_OPT_cfg_file += <Axivion_IR_filepath>$(axivion_uti_ir)</Axivion_IR_filepath>
TESTFRAME_GEN_OPT_cfg_file += <project>$(variant_base)</project>
TESTFRAME_GEN_OPT_cfg_file += <type_system_key>$(UTEST_TYPE_SYSTEM)</type_system_key>
TESTFRAME_GEN_OPT_cfg_file += <stub_rootfolder>$(tiFolder)</stub_rootfolder>
TESTFRAME_GEN_OPT_cfg_file += <template_folder>$(template_folder)</template_folder>

ifneq ($(CFILES_TO_TESTFRAME), $(NULL))
TESTFRAME_GEN_OPT_cfg_file += $(call xml_embrace,$(CFILES_TO_TESTFRAME),Units_to_TF)
endif

#Defines becoming active in Testframes
ifdef unit_test
    ifneq ($(FOR_UNITS_UNDER_TEST_STUB), $(NULL)) 
    defines += $(foreach i,$(FOR_UNITS_UNDER_TEST_STUB),$(call uppercase,FOR_UNIT_UNDER_TEST_STUB_$(basename $(i))))
    endif

    ifneq ($(UNITS_TO_PUT_UNDER_TEST), $(NULL)) 
    defines += $(foreach i,$(UNITS_TO_PUT_UNDER_TEST),$(call uppercase,$(basename $(i))_UNDER_TEST))
    endif

    ifneq ($(NO_CFILE_INCLUDE_IN_TESTFRAME), $(NULL))
    defines += $(foreach i,$(NO_CFILE_INCLUDE_IN_TESTFRAME))),$(call uppercase,NO_CFILE_INCLUDE_IN_TESTFRAME_FOR_$(basename $(i))))
    endif
endif

MkTFrameGenCfgFile = <config>\n$(foreach i,$(TESTFRAME_GEN_OPT_cfg_file),$(i)\n)</config>
MkStubGenCfgFile = <config>\n$(foreach i,$(STUB_GEN_OPT_cfg_file),$(i)\n)</config>

#######################################################################################################
#*****RECIPES                                                                                         #
#######################################################################################################

no_dep_targets += utbat

MkStubGenCfgFile.xml:
	@$(file >$(out_path)\$(variant)\MkStubGenCfgFile.xml,$(MkStubGenCfgFile))


MkTFrameGenCfgFile.xml:
	@$(file >$(out_path)\$(variant)\MkTFrameGenCfgFile.xml,$(MkTFrameGenCfgFile))



.PHONY: $(out_path)/$(variant)/$(variant_base)_ComConfig.xml $(out_path)/$(variant)/$(variant_base)_DefaultSetup.py     
$(out_path)/$(variant)/$(variant_base)_ComConfig.xml:
	@echo Generating $@ ...
	@$(file > $@,<config>) $(foreach i, $(COM_CFG_filecontent),$(file >> $@,$(i)))
	@$(file >> $@,</config>)

$(out_path)/$(variant)/$(variant_base)_DefaultSetup.py:
	@echo Generating $@ from template: $(template_defaultsetup_py)...
	@set Path=$(python_path);%Path%
	@set PYTHONPATH=$(python_path)\Lib
	@set PYTHONHOME=$(python_path)
	@$(python) $(replace) --outfile $@ --template $(template_defaultsetup_py) $(foreach i,$(CONFIG_PY_OPT), -D $(i))

$(out_path)/$(variant)/$(variant_base)_DefaultSetup_ci.py:
	@echo Generating $@ from template: $(template_defaultsetup_ci_py)...
	@set Path=$(python_path);%Path%
	@set PYTHONPATH=$(python_path)\Lib
	@set PYTHONHOME=$(python_path)
	@$(python) $(replace) --outfile $@ --template $(template_defaultsetup_ci_py) $(foreach i,$(CONFIG_PY_OPT), -D $(i))

genStubBatch = common/tools/python/mkStubs.bat

uti: ax_uti MkStubGenCfgFile.xml
	@call $(genStubBatch) $(axivion_bin_path) $(python2_path) $(axivion_python_module_path) $(utest_gen_stubs_from_axivion_py) $(out_path)\$(variant)\MkStubGenCfgFile.xml $(python2) $(CURDIR)/$(pteng_modules)
	@$(call prnt,-------------- Unit-Test-Instrumentation generated ---------------)
	@$(call prntnl)
	@$(call prntnl)


utframes: ax_uti MkTFrameGenCfgFile.xml
	@set Path=$(axivion_bin_path);%Path%
	@set BAUHAUS_PYTHON=$(python)
	@set PYTHONHOME=$(axivion_python_module_path);$(python_path)
	@set PYTHONPATH=$(python_path)\Lib
	@echo Generating testframes...
	@$(python) $(utest_gen_testframes_py) -cfg_file $(out_path)\$(variant)\MkTFrameGenCfgFile.xml


utbat: mtestcfg $(out_path)\$(variant)\$(variant_base)_test.bat

PATH_DEFAULTSETUP = $(out_path)/$(variant)/$(variant_base)_DefaultSetup.py
PATH_DEFAULTSETUP_CI = $(out_path)/$(variant)/$(variant_base)_DefaultSetup_ci.py
PATH_COMCONFIG = $(out_path)/$(variant)/$(variant_base)_ComConfig.xml

.PHONY: mtestcfg
mtestcfg: $(PATH_COMCONFIG) $(PATH_DEFAULTSETUP) $(PATH_DEFAULTSETUP_CI)

percent := %
defines_testbatch_opt = $(foreach i,$(TESTBATCH_OPT), -D $(i))
defines_testbatch_opt_subst = $(subst %,$(percent),$(defines_testbatch_opt))

.PHONY : $(out_path)\$(variant)\$(variant_base)_test.bat
$(out_path)\$(variant)\$(variant_base)_test.bat:
	@echo Generating testbatch with $(platform) rule $@ ...
	@set Path=$(python_path);%Path%
	@set PYTHONPATH=$(python_path)\Lib
	@set PYTHONHOME=$(python_path)
	@$(python) $(replace) --outfile $(out_path)\$(variant)\$(variant_base)_test.bat --template $(template_testbatch_py) $(defines_testbatch_opt_subst)


