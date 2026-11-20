#*******************************************************************************
#  Copyright (c) 2017 BSH Hausgeraete GmbH,
#  Carl-Wery-Str. 34, 81739 Munich, Germany, www.bsh-group.de
#
#  All rights reserved. This program and the accompanying materials
#  are protected by international copyright laws.
#  Please contact copyright holder for licensing information.
#
#*******************************************************************************
#  Project          MCU-Framework
#*******************************************************************************
#  Description      Axivion targets
#*******************************************************************************
include common/build/axivion/axivion_paths.mak
include common/build/axivion/axivion_configurations.mak

ifeq ($(final_no_code_filter),$(empty))
    final_no_code_filter = NONE
endif

ifeq ($(no_code_analysis),$(empty))
    no_code_analysis = NONE
endif

.PHONY: axivion axgravis axconfig
axivion:           $(axivion_ir)
ax_uti:            $(axivion_uti_ir)
axstyle_complete:  $(axivion_sty)
axstyle:           axivion_out axstyle_units axstyle_system
axstyle_quick:     axstyle_units_quick axstyle_system
axstyle_units:     $(axivion_config_path)/Stylecheck_config.py $(ax_ustc)
axstyle_mleak:	   $(axivion_sty_mleak) 
axmetric:          axivion_out $(ax_rfg) $(axivion_config_path)/metric_config.py $(ax_mstc)
axstyle_system:      $(axivion_sty_system)
axstyle_units_quick: $(axivion_sty_unit)
ncov_quick:          $(axivion_sty_naming)
ncov:                axivion_out $(axivion_config_path)/Stylecheck_config.py $(ax_nstc)
axstyle_help:        $(axivion_sty_help)
axanalysis:          lint $(axivion_rfg_ci) 
axgravis:            $(axivion_rfg)
axpstd: $(axivion_pstd)


axivion_out :
	@$(call mkdir, $(axivion_path_out))

$(axivion_ir): axivion_out $(ax_obj)
	@$(file >$@.in, $(ax_obj))
	@$(file >>$@.in, -o $@)
	@$(ax_cafeCC) @$@.in >$(axivion_path_out)/ir.log 2>&1

# display the IR-file in IR-viewer
axirviewer: $(axivion_ir)
	@echo Opening axirviewer...
	@$(ax_irviewer) $(axivion_ir)
    
$(axivion_uti_ir): $(ax_obj_test)
	@$(python) $(mkfolders) -folders $(axivion_path_out)
	@$(axivion_bin_path)/irlink.exe $+ $@ >$(axivion_path_out)/ir_uti.log 2>&1

$(axivion_rfg_simple): $(axivion_ir) $(axivion_config_path)\Ir2rfg.config
	@echo generating simple rfg file
	@$(ax_ir_to_rfg) --lib common/prog/MSP/compiler* --lib M:* --cfg $(axivion_ir) $@

$(axivion_rfg_enriched): $(axivion_ir) $(addprefix $(axivion_config_path)/,build.conf Ir2rfg.config)
	@echo Generating Axivion database...
	@$(call mkdir,$(axivion_db_path_out))
	@$(python) $(axivion_config_path)/cibuild_excludes.py --no_code_analysis $(no_code_analysis)
	@$(python) $(local_tool_path)\python\lint_integration.py $(axivion_path_out) $(AXIVION_VARIANT_NAME)
	@$(ax_python) $(axivion_bin_path)/cibuild.py --config=$(axivion_config_path)/build.conf --project=AxivionBuild --driver=nightly > $(axivion_path_out)\$(variant_base)_cibuild.log 2>&1
	@echo Cibuild - build finished.

ax_cibuild_start:
	@$(call mkdir,$(axivion_db_path_out)\config)
	@$(axivion_bin_path)\dashserver2 start --confdir="$(axivion_db_path_out)\config"  --install_file="$(axivion_db_path_out)\$(AXIVION_PROJECT_NAME)_$(AXIVION_VARIANT_NAME).db"
	
ax_cibuild_stop:
	@$(axivion_bin_path)\dashserver2 stop --confdir="$(axivion_db_path_out)\config" 

$(axivion_sty): AXIVION_SYSTEM_STYLECHECK=True
$(axivion_sty): $(axivion_ir) $(axivion_config_path)/Stylecheck_config.py
	@$(ax_python) $(axivion_bin_path)/stylecheck.py -ir $(axivion_ir)  -out $@  -brief -omit_disabled -j $(ax_exclude_opt) > $(axivion_path_out)/$(variant_base)_stc.log 2>&1

$(axivion_sty_help): $(axivion_config_path)/Stylecheck_config.py
	@$(ax_python) $(axivion_bin_path)/stylecheck.py -list -verbose

$(axivion_sty_system): AXIVION_SYSTEM_STYLECHECK=True
$(axivion_sty_system): $(axivion_ir)  $(axivion_config_path)/Stylecheck_config.py
	@$(ax_python) $(axivion_bin_path)/stylecheck.py -ir $(axivion_ir)  -out $@  -brief -omit_disabled $(ax_exclude_opt) 2>nul
	@$(perl) $(local_tool_path)/mixed/ParseAxivion.pl $@ $(axivion_outpaths)

$(axivion_sty_mleak) : AXIVION_SYSTEM_STYLECHECK=True
$(axivion_sty_mleak) : $(axivion_ir)  $(axivion_config_path)/Stylecheck_config.py
	@$(ax_python) $(axivion_bin_path)/stylecheck.py -ir $(axivion_ir) -out $@ -brief -config_key Config_app -omit_disabled $(ax_exclude_opt) -system 2>nul
	@$(perl) $(local_tool_path)/mixed/ParseAxivion.pl $@ $(axivion_outpaths)
	
$(axivion_sty_unit): AXIVION_TRANSLUNIT_STYLECHECK=True
$(axivion_sty_unit): $(axivion_ir) $(axivion_config_path)/Stylecheck_config.py
	@$(ax_cafeCC) $+ -o$@ >$(axivion_path_out)/style_split.log 2>&1
	@$(ax_python) $(axivion_bin_path)/stylecheck.py -ir $(axivion_ir)  -out $@  -brief -omit_disabled -j $(ax_exclude_opt) > $(axivion_path_out)/$(variant_base)_ustc.log 2>&1
	@$(ax_python) $(local_tool_path)/python/ReportSplitter.py -r ustc -p $(axivion_sty_unit) -i $(axivion_ir)

$(axivion_sty_naming): AXIVION_NAMINGCONV_STYLECHECK=True
$(axivion_sty_naming): $(axivion_ir) $(axivion_config_path)/Stylecheck_config.py
	@$(ax_cafeCC) $+ -o$@ >$(axivion_path_out)/style_naming.log 2>&1
	@$(ax_python) $(axivion_bin_path)/stylecheck.py -ir $(axivion_ir)  -out $@  -brief -omit_disabled -j $(ax_exclude_opt) > $(axivion_path_out)/$(variant_base)_nstc.log 2>&1
	@$(ax_python) $(local_tool_path)/python/ReportSplitter.py -r nstc -p $(axivion_sty_naming) -i $(axivion_ir)

$(axivion_pstd): $(axivion_ir)
	@echo Generating $(notdir $@) ...
	$(call mkdir, $(dir $@))
	@$(ax_python) $(ax_pstd_gen) -g $(axivion_ir) -o $@ -p $(variant_base) -f $(platform) $(ax_pstd_gen_opt)

.PHONY: axstyle2console
axstyle2console: $(ax_ustc) $(axivion_sty_system)
	@$(python) $(local_tool_path)/python/file_contents2console.py $(axivion_path_out) "axstyle2console"

.PHONY: ncov2con
ncov2con: $(ax_nstc) $(axivion_sty_system)
	@$(python) $(local_tool_path)/python/file_contents2console.py $(axivion_path_out) "ncov2con"

axgravis: $(axivion_rfg)
	@$(ax_gravis) $(axivion_rfg)

axconfig: $(addprefix $(axivion_config_path)/,Build.conf Ir2rfg.config)
	@$(ax_configurator) $(analysis_path_out)

# calculate the metric
#axbmi: $(axmetric)
#	@$(python) $(local_tool_path)/python/BMI_calculation.py
	
# ***************************************************************************************************************
# Implicit rules for axivion
# ***************************************************************************************************************
$(out_prog_path)/%.ustc : AXIVION_TRANSLUNIT_STYLECHECK=True
$(out_prog_path)/%.ustc : $(out_prog_path)/%.axob
	@$(ax_python) $(axivion_bin_path)/stylecheck.py -ir $<  -out $@  -brief -omit_disabled -config_key Config $(ax_exclude_opt) 2>nul
	@$(perl) $(local_tool_path)/mixed/ParseAxivion.pl $@ $(axivion_outpaths)


$(out_prog_path)/%.nstc : AXIVION_NAMINGCONV_STYLECHECK=True
$(out_prog_path)/%.nstc : $(out_prog_path)/%.axob
	@$(ax_python) $(axivion_bin_path)/stylecheck.py -ir $<  -out $@  -brief -omit_disabled  $(ax_exclude_opt) 2>nul 
	@$(perl) $(local_tool_path)/mixed/ParseAxivion.pl $@ $(axivion_outpaths)

$(out_prog_path)/%.mstc : $(out_prog_path)/%.rfg
	@$(ax_python) $(axivion_bin_path)/stylecheck.py -rfg $< -out $@ $(ax_exclude_opt) -config_key Metric 2>nul 
	@$(perl) $(local_tool_path)/mixed/ParseMetric.pl $@ $(axivion_metric_outpaths)

$(out_prog_path)/%.rfg : $(out_prog_path)/%.axob
	@$(ax_ir_to_rfg) --lib common/prog/MSP/compiler* --lib M:* --cfg $< $@  2>nul

$(out_prog_path)/%.axob : %.c $(varHeader) | $(code_gen_log)
	@$(call mkdir, $(dir $@))
	@$(ax_cafeCC) -c -w $(axcopt) $(axivionopt) $< -o$@

$(out_prog_path)/%.axob : %.cpp $(varHeader) | $(code_gen_log)
	@$(call mkdir, $(dir $@))
	@$(ax_cafeCC) -c $(axcppopt) $(axivionopt) --std=c++14 $< -o$@ 2>nul

# ***************************************************************************************************************
# Implicit rules for single file and folder analysis in Eclipse
# ***************************************************************************************************************	
#
.PHONY: FORCE

FORCE:

%.o: AXIVION_TRANSLUNIT_STYLECHECK=True
%.o: %.c FORCE
	@$(ax_cafeCC) -c $(axcopt) $(axivionopt) $< -o "$@"
	@$(ax_python) $(axivion_bin_path)\stylecheck.py -j -quiet -brief -omit_disabled -ir $@ $(ax_exclude_opt) 2>nul
	@del $@

%.o: AXIVION_TRANSLUNIT_STYLECHECK=True
%.o: % FORCE
	@$(ax_cafeCC) -c $(axcopt) $(axivionopt) -shared -o "$@" $(wildcard $</*.c)
	@$(ax_python) $(axivion_path)\stylecheck.py -quiet -brief -omit_disabled -ir "$@" -include "$</*" $(ax_exclude_opt) 2>null
	@del $@
