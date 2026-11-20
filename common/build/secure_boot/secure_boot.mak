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
#  Description      definitions for SiLabs commander signing topics
#*******************************************************************************

secure_commander_exe  ?= $(sl_commander_path)/commander.exe

secure_test_certificate     ?= common/build/secure_boot/VARIANT-C-test-key-certificate.secureboot.cert
secure_test_priv_key        ?= common/build/secure_boot/VARIANT-C-test-key-private.pem
secure_test_master_publ_key ?= common/build/secure_boot/VARIANT-C-master-key-public.pem

# signs a binary with silabs commander. Gaps are filled with patchBin in the beginning
# parameter 1: source file
# parameter 2: target file
define silabs_sign
@$(call prntGr,-------- Signing with development key ------------------)
@$(secure_commander_exe) convert $2 --secureboot  --certificate $(secure_test_certificate)  --keyfile $(secure_test_priv_key)  --outfile $2
@$(call prntGr,-------- Verify the signature --------------------------)
@$(secure_commander_exe)  util verifysign $1 --verify $(secure_test_master_publ_key)
@$(call prntGr,-------- Signing with development key done -------------)
endef

define silabs_create_gbl
@$(call prntGr,-------- Create gbl file -------------------------------)
@$(secure_commander_exe) gbl create $1 --app $2 --compress lzma
endef

define silabs_create_bl_gbl
@$(call prntGr,-------- Create gbl file including bootloader ----------)
@$(secure_commander_exe) gbl create $1 --app $2 --bootloader $3 --compress lzma
endef
