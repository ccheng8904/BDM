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
#  Description      The 'check' make target definition includes:
#                   - obsolete make variables usage
#                   - TBD /other checks/
#*******************************************************************************
# defined list of obsolete make variables
# they are placed on separate lines for better reading
obsolete_vars = \
UART_CHANNEL \
NODE_ADDRESS \
DEFAULT_BAUDRATE \
DBUS_TX_PIN \
DBUS_RX_PIN \
WAKEUP_PIN \
DBUS_IDLE_PIN \
interface \
frequency \
connector \
include_depedency_files \
project_doxy_name \
project_doxy_number \
project_doxy_brief \
exclude_src \
linkerscript_defs \
toolpath

# obsolete make variables usage evaluating in current make variables
$(eval obsolete_vars_result =$(filter $(obsolete_vars),$(.VARIABLES)))
ifneq ($(obsolete_vars_result),)
    $(info !!! Obsolete make variables detected. Please run 'make check' to get more info !!!)
endif
# preparing of 'check' target command options
ifeq ($(OS),Windows_NT)
    check_find_command = findstr
    check_find_opt = /S /N /P /R /A:6
    check_find_string := $(foreach item, $(obsolete_vars), $(call prepost, /C:"\<, $(item),\>"))
    check_scope = $(app_path)\*.mak
else
    check_find_command = grep
    check_find_opt = -n -w -r -w --include=\*.mak
    check_find_string := $(foreach item, $(obsolete_vars), $(call prepost,-e ", $(item),"))
    check_scope = $(app_path)/
endif

.PHONY: check

check: check_obsolete

check_obsolete:
	@$(call prntGr,Checking obsolete variables in make files...)
ifeq (,$(obsolete_vars_result))
	@$(call prntGr,No obsolete variable found.)
else
	@$(call prntR,Following obsolete variables found:)
	@$(call prnt, $(obsolete_vars_result))
	@$(call prntR,in these files and line numbers:)
	@$(check_find_command) $(check_find_opt) $(check_find_string) $(check_scope)
endif

check_git_return_string = git version
# other checks: tools availability
check_tools:
	@echo Checking availability of local tools:
	@echo Checking availability of Git ....
ifneq (,$(filter $(check_git_return_string), $(git_version)))
	@$(call prntRd,Git does not exist: $(git_path))
endif
	@echo Checking availability of compiler bin ....
ifeq (,$(wildcard $(cc)))
	@$(call prntRd,Check if path exist: $(cc_bin_path))
endif
# TBD other tools
