.DEFAULT_GOAL := setup_build_gui

include common/build/common/common.mak

setup_build_gui:
	@cd $(sdk_path) && $(python) tools/python/setup_buildgui.py
