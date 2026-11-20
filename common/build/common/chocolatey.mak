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
#  Description      Chocolatey recipes for installing packages
#*******************************************************************************

choco_make = $(MAKE) -f common/build/makefile -s

choco_python_path = C:/Programs/Python/3.9.1

choco_python = $(choco_python_path)/python.exe

#chocolatey python-scripts
py_choco_gen_config = common\Tools\python\choco\gen_config.py

# Essential make variables to export choco config
make_export += choco_export
choco_export_output_file = out/choco/choco_export.xml
choco_export_variable = choco_packages

choco_pkg_config = out/choco/choco_pkg.config
pip_pkg_txt = out/choco/pip_pkg.txt

no_dep_targets += choco_config pip_update choco_python choco_install

define pip_create_pkg_txt =
$(file >$(pip_pkg_txt), )
$(foreach pkg,$(pip_packages),$(file >>$(pip_pkg_txt),$(pkg)))
endef


choco_config: choco_export
	@echo generating $(choco_pkg_config)
	@$(choco_python) $(py_choco_gen_config) -tools $(choco_export_output_file) -config $(choco_pkg_config) $(and $(platform), -platform $(platform) -cc $(cc_build_path))
	@echo generating $(pip_pkg_txt)

pip_update:
	@(md out >NUL 2>&1) | (exit \b 0)
	@(md out\choco >NUL 2>&1) | (exit \b 0)
	$(call pip_create_pkg_txt)
	@$(python) -m pip install -r $(pip_pkg_txt)

choco_python:
	@choco feature disable --name=showNonElevatedWarnings
ifeq (,$(wildcard $(choco_python)))
	@(choco uninstall Python -y)  | (exit \b 0)
	@choco install Python --version=3.9.1 -y --allow-downgrade --allowmultipleversions --no-progress --force
endif

choco_pkg: choco_config
	@echo ---------- Updating with $(choco_pkg_config) ----------
	@choco feature disable --name=showNonElevatedWarnings
	@choco install $(CURDIR)/$(choco_pkg_config) -y --allow-downgrade --allowmultipleversions --no-progress $(choco_options)

choco_install:
	@$(choco_make) choco_python
	@$(choco_make) python_path=$(choco_python_path) $(and $(variant), variant=$(variant)) $(and $(project), project=$(project)) choco_pkg
