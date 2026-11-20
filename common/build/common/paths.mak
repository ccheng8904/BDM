#*******************************************************************************
#  Copyright (c) 2017 BSH Hausgeraete GmbH,
#  Carl-Wery-Str. 34, 81739 Munich, Germany, www.bsh-group.de
#
#  All rights reserved. This program and the accompanying materials
#  are protected by international copyright laws.
#  Please contact copyright holder for licensing information.
#
#*******************************************************************************
#  Project          PP PED FW
#*******************************************************************************
#  Description      defines the tool paths
#*******************************************************************************
# Path to null folder (OS dependent)
ifeq ($(OS),Windows_NT)
    null = NUL
else
    null = /dev/null
endif

# Paths to batch scripts
xcopy           = $(local_tool_path)\mixed\copy.bat
xmkdir          = $(local_tool_path)\mixed\mkdir.bat
xrmdir          = $(local_tool_path)\mixed\rmdir.bat
xtouch          = $(local_tool_path)\mixed\touch.bat
xfilecmp        = $(local_tool_path)\mixed\filecmp.bat
xget_file_size  = $(local_tool_path)\mixed\get_file_size.bat

# TODO: check if these two definition could be moved to another make file
python          = $(python_path)/Python.exe
python2         = $(python2_path)/Python.exe

# PSFlasher test 
psflash_py      = $(local_tool_path)/psflasher/psflash.py

py_modules     ?= $(local_tool_path)\python
copyCfg         = $(py_modules)\copyCfg.py
replace         = $(py_modules)\replace.py
mkdefines       = $(py_modules)\mk_defines.py
mkfolders       = $(py_modules)\mk_folders.py
py_fprintf      = $(py_modules)\fprintf.py


# Python scripts for generating files
code_gen_files_py = $(local_tool_path)\python\code_generator\generate_files.py
code_gen_config_py = $(local_tool_path)\python\code_generator\generate_config.py
