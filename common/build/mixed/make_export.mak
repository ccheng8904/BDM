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
#  Description      Code generation makefile
#*******************************************************************************

make_export += variant_export common_export

# Essential make variables to export
variant_export_output_file = $(variant_export_xml)
variant_export_variable += variant_base defines

common_export_output_file = $(common_export_xml)
common_export_variable += defines_date defines_version

# Function for appending make variable and its elements to XML file
define make_export_append_variable =
$(file >>$(2),<$(1)>)
$(foreach element,$($(1)),$(file >>$(2),<element>$(element)</element>))
$(file >>$(2),</$(1)>)
endef


define make_export_append_dictionary_entry =
$(eval _entry=$(subst =, ,$(1)))
$(file >>$(2),<$(firstword $(_entry))>$(lastword $(_entry))</$(firstword $(_entry))>)
endef


define make_export_append_dictionary_element =
$(file >>$(2),<element>)
$(foreach element,$($(1)), $(call make_export_append_dictionary_entry,$(element),$(2)))
$(file >>$(2),</element>)
endef


define make_export_append_dictionary =
$(file >>$(2),<$(1)>)
$(foreach item,$($(1)),$(call make_export_append_dictionary_element,$(item),$(2)))
$(file >>$(2),</$(1)>)
endef


define make_export_create_xml =
$(file >$($(1)_output_file).tmp,<?xml version="1.0" encoding="UTF-8"?>)
$(file >>$($(1)_output_file).tmp,<make_export>)
$(foreach var,$($(1)_variable),$(call make_export_append_variable,$(var),$($(1)_output_file).tmp))
$(foreach var,$($(1)_dictionary),$(call make_export_append_dictionary,$(var),$($(1)_output_file).tmp))
$(file >>$($(1)_output_file).tmp,</make_export>)
endef


define make_export_template
.PHONY: $(1)
$(1): $$($(1)_output_file)
$$($(1)_output_file): FORCE | $(dir $($(1)_output_file))
	$$(call make_export_create_xml,$(1))
	$$(eval _file_diff = $$(call compare_files, $$@.tmp, $$@))
	$$(if $$(filter $$(_file_diff), False), $(call prnt,generating export file $$@) && $$(call copy, $$@.tmp, $$@))
	@$$(call del, $$@.tmp)
endef


$(foreach export,$(make_export),$(eval $(call make_export_template,$(export))))
