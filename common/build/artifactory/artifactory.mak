#*******************************************************************************
#  Copyright (c) 2017 BSH Hausgeraete GmbH,
#  Carl-Wery-Str. 34, 81739 Munich, Germany, www.bsh-group.de
#
#  All rights reserved. This program and the accompanying materials
#  are protected by international copyright laws.
#  Please contact copyright holder for licensing information.
#
#*******************************************************************************
#  Project          Generic SW
#*******************************************************************************
#  Description      artifactory SW update
#*******************************************************************************

ifneq ($(platform), Intel)
# artifactory main values 
-include common/build/artifactory/artifactory_credential.mak
no_dep_targets += artifactory_download artifactory_upload check_api_key curl_check datasheet refman errata

# Definitions for artifactory
af_doc_path     ?= app/doc
af_doc_repo     ?= https://artifactory-04.boschdevcloud.com/artifactory/ed-mcu-framework-generic-releases
af_doc_folder   ?= $(call upper, $(platform))

af_http_proxy           ?= http://rb-proxy-de.bsh.corp.bshg.com:8080/
af_bypass_http_proxy    ?= false
af_username             ?= user@bosch.com
af_api_key              ?= ABCDEFGHIJKMLNOPQRSTUVWXYZ

af_credentials     	 = --user $(af_username):$(af_api_key)

ifeq ($(af_bypass_http_proxy),false)
    af_proxy_conf      =  --proxy $(af_http_proxy) --ssl-no-revoke -k
endif

# Dummy (example) values for uploading to the artifactory.
af_target_uploade_path_to_file  ?= ./common/setup.bat 
af_target_upload_filename       ?= /$(notdir $(af_target_uploade_path_to_file))
af_target_upload_local_path     ?= /test_folder
af_target_upload_address        ?= $(af_doc_repo)$(af_target_upload_local_path)

# Dummy (example) values for downloading from the artifactory.
af_target_download_address      ?= $(af_doc_repo)/8001260387.zip
af_target_download_filename     ?= /$(notdir $(af_target_download_address))
af_target_download_local_path   ?= .

# Dummy names 
af_doc_ds_name ?= no_platform_ds
af_doc_rm_name ?= no_platform_rm
af_doc_er_name ?= no_platform_er

# Datasheet and reference manual paths in doc folder
af_doc_ds = $(af_doc_path)/$(af_doc_ds_name)
af_doc_rm = $(af_doc_path)/$(af_doc_rm_name)
af_doc_er = $(af_doc_path)/$(af_doc_er_name)

# Datasheet and reference manual paths in artifactory
$(af_doc_ds) = $(af_doc_repo)/$(af_doc_folder)/$(af_doc_ds_name)
$(af_doc_rm) = $(af_doc_repo)/$(af_doc_folder)/$(af_doc_rm_name)
$(af_doc_er) = $(af_doc_repo)/$(af_doc_folder)/$(af_doc_er_name)

curl_check:
ifeq ($(wildcard $(curl)),)
	$(call tool_error,$(curl))
endif

artifactory_download: curl_check
	@echo Downloading $(af_target_download_address) to $(af_target_download_local_path)
	@$(curl) $(af_credentials) $(af_proxy_conf) $(af_target_download_address) -o $(af_target_download_local_path)$(af_target_download_filename)

artifactory_upload: curl_check
	@echo Uploading $(af_target_uploade_path_to_file) to $(af_target_upload_address)
	@$(curl) $(af_credentials) $(af_proxy_conf) -T $(af_target_uploade_path_to_file) $(af_target_upload_address)$(af_target_upload_filename)

# Target for downloading and opening datasheet/reference manual 
define same_size
@echo Local file is up-to-date.
endef

define different_size
@echo New version found in Artifactory.
@echo Downloading documentation $@ for $(platform)
@$(curl) $(af_credentials) $(af_proxy_conf) $($@) -o $@
endef

$(af_doc_ds) $(af_doc_rm) $(af_doc_er): curl_check $(af_doc_path)/
	@$(eval NEWSIZE=$(shell curl $(af_credentials) $(af_proxy_conf) -s -I $($@) | findstr 'Content-Length'))
	@$(eval NEWSIZE=$(subst Content-Length: ,,$(NEWSIZE)))
	@$(eval OLDSIZE=$(shell $(xget_file_size) $@))
	@$(eval SAME_FILE_SIZE=$(call match,$(NEWSIZE),$(OLDSIZE)))
	$(warning Porovnanie $(NEWSIZE),$(OLDSIZE) )
	@$(if $(SAME_FILE_SIZE),$(call same_size),$(call different_size))

datasheet: | $(af_doc_ds)
	@CMD /C START "" $|

refman: | $(af_doc_rm)
	@CMD /C START "" $|

errata: | $(af_doc_er)
	@CMD /C START "" $|

endif
