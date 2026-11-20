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
#  Description      Set paths and variables for axivion targets
#*******************************************************************************

# !! check if x64 path is correct
axivion_bin_path           = $(axivion_path)/bin
axivion_python_module_path = $(axivion_path)/lib/scripts
axivion_python_module_path_bauhaus = $(axivion_path)/lib/scripts/bauhaus

axivion_path_out    = $(CURDIR)/$(out_path)/$(variant)/Axivion
axivion_db_path_out ?= $(CURDIR)/$(out_path)/$(variant)/Axivion/Axivion_DB
axivion_config_path = $(CURDIR)/common/build/axivion/config

# Axivion Tools
ax_ir_to_rfg    = $(axivion_bin_path)/ir2rfg.exe
ax_gravis       = $(axivion_bin_path)/gravis.exe
ax_configurator = $(axivion_bin_path)/bauhaus_config.exe
ax_irviewer     = $(axivion_bin_path)/irviewer.exe
ax_python       = $(axivion_bin_path)/rfgscript.exe
ax_pstd_gen     = $(py_modules)/mkPSTD.py
ax_cafeCC       = $(axivion_bin_path)/cafeCC.exe

#export environment variables for axivion
export BAUHAUS_CONFIG=$(axivion_config_path)
export PATH:=$(axivion_bin_path);$(PATH)
export BAUHAUS_PYTHON=$(python2)
export BAUHAUS_BIN_PATH=$(axivion_bin_path)
export AXIVION_PROJECT_NAME=$(project)
export AXIVION_VARIANT_NAME=$(variant_base)
export AXIVION_PROJECT_PATH=$(CURDIR)
#export AXIVION_LOCAL_BUILD=True
export AXIVION_SYSTEM_STYLECHECK=False
export AXIVION_TRANSLUNIT_STYLECHECK=False
export AXIVION_NAMINGCONV_STYLECHECK=False
export PYTHONPATH=$(CURDIR)\$(local_tool_path)\PythonTEng

export AXIVION_PATH_OUT=$(axivion_path_out)
export AXIVION_DB_PATH_OUT=$(axivion_db_path_out)
export AXIVION_EXCLUDE_PATH=*/common/prog/MSP/compiler/*
export TOOL_PATH = $(local_tool_path)

export axivion_ir       = $(axivion_path_out)/$(project)_$(variant_base).ir
export axivion_uti_ir   = $(axivion_path_out)/$(project)_$(variant_base)_uti.ir
axivion_rfg_simple      = $(axivion_path_out)/$(project)_$(variant_base)_lite.rfg
axivion_rfg_enriched    = $(axivion_path_out)/$(project)_$(variant_base).rfg
axivion_rfg             = $(axivion_rfg_simple)
axivion_rfg_ci          = $(axivion_rfg_enriched)

axivion_outpaths   = $(axivion_path_out)/$(variant_base).ax
axivion_metric_outpaths   = $(axivion_path_out)/$(variant_base).axm
axivion_sty        = $(axivion_path_out)/$(variant_base).stc
axivion_sty_system = $(axivion_path_out)/$(variant_base).sstc
axivion_sty_naming = $(axivion_path_out)/$(variant_base)_NAMINGCONV.nstc
axivion_sty_unit   = $(axivion_path_out)/$(variant_base)_UNIT.ustc
axivion_sty_help   = $(axivion_path_out)/Axivion_$(axivion_version)_MisraOptions.stc
axivion_pstd       = $(bin_path)/$(variant_base)_pstd.xml
axivion_sty_mleak  = $(axivion_path_out)/$(variant_base).mlstc

#No_code_analysis Variable
axivion_src   ?=$(filter %.c %.cpp, $(call filter-out,$(addsuffix /%,$(final_no_code_filter)),$(sources)))

axivion_c_src   ?=$(filter %.c, $(call filter-out,$(addsuffix /%,$(no_code_analysis)),$(sources)))
axivion_cpp_src   ?=$(filter %.cpp, $(call filter-out,$(addsuffix /%,$(no_code_analysis)),$(sources)))
ax_ustc    = $(addprefix $(out_prog_path)/,$(axivion_c_src:.c=.ustc))
ax_ustc    += $(addprefix $(out_prog_path)/,$(axivion_cpp_src:.cpp=.ustc))
ax_nstc    = $(addprefix $(out_prog_path)/,$(axivion_c_src:.c=.nstc))
ax_nstc    += $(addprefix $(out_prog_path)/,$(axivion_cpp_src:.cpp=.nstc))
ax_mstc    = $(addprefix $(out_prog_path)/,$(axivion_c_src:.c=.mstc))
ax_mstc    += $(addprefix $(out_prog_path)/,$(axivion_cpp_src:.cpp=.mstc))
ax_rfg    =  $(addprefix $(out_prog_path)/,$(axivion_c_src:.c=.rfg))
ax_rfg    += $(addprefix $(out_prog_path)/,$(axivion_cpp_src:.cpp=.rfg))

ax_obj     = $(call prepost,$(out_prog_path)/,$(basename $(filter %.c %.cpp, $(axivion_src))),.axob)

axivion_uti_src   ?=$(filter %.c %.cpp, $(call source_lattice, $(axivion_src), $(obj_test)))
ax_obj_test = $(addprefix $(out_prog_path)/,$(axivion_uti_src:.c=.axob))