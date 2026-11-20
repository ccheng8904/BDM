###         make link example
---


The target creates binary by running linkage process for current variant.

Note: A project name is set to STM32G4_FWU3_431 in the example. If you do not set a project name, please, omit it in command line.

An example of usage target `make link` after previous build:

```actionscript
make link project=STM32G4_FWU3_431

Using external tooldrive M:
Working directory of MCU Framework NOT clean!!!
===== Based on MCU Framework (heads/New_help_concept_proposal) =====
=====  Project is not a GIT repository =====
+++++ building variant MyVariant by using compiler keilArm +++++
generating export file out/STM32G4_FWU3_431/MyVariant/make_export.xml
preprocessing file out/STM32G4_FWU3_431/MyVariant/bin/LinkerScript.sct
preprocessing file out/STM32G4_FWU3_431/MyVariant/bin/winflash_cfg.bp2
linking MyVariant
Program Size: Code=12540 RO-data=1836 RW-data=268 ZI-data=1224
--------Linking done-------------

creating metric file...
creating symbols and registry...
creating usage report...
patching binaries with PatchBin...
```

An example of usage target `make link` when no build has been made before this command, or `make clean` target has been used before:

```actionscript
make link project=STM32G4_FWU3_431

Using external tooldrive M:
Working directory of MCU Framework NOT clean!!!
===== Based on MCU Framework (heads/New_help_concept_proposal) =====
=====  Project is not a GIT repository =====
common/build/mixed/depend.mak:33: out/STM32G4_FWU3_431/MyVariant/MyVariant.dep: No such file or directory
+++++ building variant MyVariant by using compiler keilArm +++++
generating export file out/STM32G4_FWU3_431/MyVariant/make_export.xml
keeping app/STM32G4_FWU3_431/prog/MyVariant.h
Scanning dependency of app/STM32G4_FWU3_431/prog/MSP/stm32g4/hadc01xs.c
Scanning dependency of app/STM32G4_FWU3_431/prog/MSP/stm32g4/hcct01xs.c
Scanning dependency of app/STM32G4_FWU3_431/prog/MSP/stm32g4/hdio01xs.c
Scanning dependency of app/STM32G4_FWU3_431/prog/MSP/stm32g4/hint01xs.c
Scanning dependency of app/STM32G4_FWU3_431/prog/MSP/stm32g4/huart01xs.c
Scanning dependency of app/STM32G4_FWU3_431/prog/MSP/stm32g4/hwdt01xs.c
Scanning dependency of app/STM32G4_FWU3_431/prog/MSP/stm32g4/hi2c01xs.c
Scanning dependency of app/STM32G4_FWU3_431/prog/MSP/stm32g4/IntTbl.c
Scanning dependency of app/STM32G4_FWU3_431/prog/dbus/balXs.c
Scanning dependency of app/STM32G4_FWU3_431/prog/dbus/dbusdllXs.c
Scanning dependency of app/STM32G4_FWU3_431/prog/dbus/dbusmapping.c
Scanning dependency of app/STM32G4_FWU3_431/prog/dbus/stm32g4/dbusmappingMc.c
Scanning dependency of app/STM32G4_FWU3_431/prog/dbus/dbuspresentationXs.c
Scanning dependency of app/STM32G4_FWU3_431/prog/dbus/dbuspresentation_updateXS.c
Scanning dependency of app/STM32G4_FWU3_431/prog/dbus/dbuspresentation_productionXS.c
Scanning dependency of app/STM32G4_FWU3_431/prog/ped_fw/basic/basicXs.c
Scanning dependency of app/STM32G4_FWU3_431/prog/ped_fw/basic/stm32g4/basicMcXs.c
Scanning dependency of app/STM32G4_FWU3_431/prog/ped_fw/schedule/scheduXS.c
Scanning dependency of app/STM32G4_FWU3_431/prog/ped_fw/timer/timerXS.c
Scanning dependency of app/STM32G4_FWU3_431/prog/ped_fw/timer/stm32g4/timerMcXs.c
Scanning dependency of app/STM32G4_FWU3_431/prog/firmware_update/mal/fwu_opt_bytes_cfg.c
Scanning dependency of app/STM32G4_FWU3_431/prog/firmware_update/mal/module_administration_cfg.c
Scanning dependency of app/STM32G4_FWU3_431/prog/firmware_update/ModuleHeader.c
Scanning dependency of common/prog/MSP/armCmx/IntTblArmCM.c
Scanning dependency of app/STM32G4_FWU3_431/prog/ped_fw/schedule/schedDef.c
Scanning dependency of app/STM32G4_FWU3_431/prog/firmware_update/Programmer/prog_user_task_config.c
Scanning dependency of app/STM32G4_FWU3_431/prog/MSP/armCmx/int_prio_cfg.c
Scanning dependency of app/STM32G4_FWU3_431/prog/MSP/armCmx/int_exc_cfg.c
Scanning dependency of app/STM32G4_FWU3_431/prog/MSP/armCmx/systick_cfg.c
Scanning dependency of common/prog/ped_fw/timer/system_timer.c
Scanning dependency of common/prog/MSP/armCmx/hsupArmCM.c
Scanning dependency of common/prog/MSP/armCmx/systick.c
Scanning dependency of common/prog/MSP/armCmx/int_prio.c
Scanning dependency of common/prog/std_lib/std_version.c
Scanning dependency of common/prog/std_lib/std_callback.c
Scanning dependency of common/prog/MSP/stm32g4/hadc01.c
Scanning dependency of common/prog/MSP/stm32g4/hcct01.c
Scanning dependency of common/prog/MSP/stm32g4/hcct01i.c
Scanning dependency of common/prog/MSP/stm32g4/hdio01.c
Scanning dependency of common/prog/MSP/stm32g4/hint01.c
Scanning dependency of common/prog/MSP/stm32g4/hint01i.c
Scanning dependency of common/prog/MSP/stm32g4/huart01.c
Scanning dependency of common/prog/MSP/stm32g4/huart01i.c
Scanning dependency of common/prog/MSP/stm32g4/hwdt01.c
Scanning dependency of common/prog/MSP/stm32g4/weakvectbl.c
Scanning dependency of common/prog/MSP/stm32g4/mem_drv/intflash_drv.c
Scanning dependency of common/prog/MSP/hi2c01.c
Scanning dependency of common/prog/dbus/bal.c
Scanning dependency of common/prog/dbus/dbusdll.c
Scanning dependency of common/prog/dbus/dbuspresentation.c
Scanning dependency of common/prog/dbus/dbus_lock.c
Scanning dependency of common/prog/dbus/bal_breaks.c
Scanning dependency of common/prog/ped_fw/basic/main.c
Scanning dependency of common/prog/ped_fw/schedule/schedule.c
Scanning dependency of common/prog/ped_fw/stdcrc/CrcCom.c
Scanning dependency of common/prog/ped_fw/stdcrc/stm32g4/StdCrc.c
Scanning dependency of common/prog/ped_fw/timer/timerlib.c
Scanning dependency of common/prog/ped_fw/utility/utility.c
Scanning dependency of common/prog/ped_fw/watchdogtimer/watchdogtimer.c
Scanning dependency of common/prog/firmware_update/mal/moduleAdministration.c
Scanning dependency of common/prog/firmware_update/mal/dataAnchor.c
Scanning dependency of common/prog/firmware_update/BootManager/BootManagerModule.c
Scanning dependency of common/prog/firmware_update/BootManager/BootManagerSharedData.c
Scanning dependency of common/prog/firmware_update/BootManager/stm32g4/BootManagerSharedDataUc.c
Scanning dependency of common/prog/firmware_update/mem_drv/intflash_abs.c
--------Dependencies calculated-----------

Using external tooldrive M:
Working directory of MCU Framework NOT clean!!!
===== Based on MCU Framework (heads/New_help_concept_proposal) =====
=====  Project is not a GIT repository =====
+++++ building variant MyVariant by using compiler keilArm +++++
generating export file out/STM32G4_FWU3_431/MyVariant/make_export.xml
keeping app/STM32G4_FWU3_431/prog/MyVariant.h
compiling app/STM32G4_FWU3_431/prog/MSP/stm32g4/hadc01xs.c
compiling app/STM32G4_FWU3_431/prog/MSP/stm32g4/hcct01xs.c
compiling app/STM32G4_FWU3_431/prog/MSP/stm32g4/hdio01xs.c
compiling app/STM32G4_FWU3_431/prog/MSP/stm32g4/hint01xs.c
compiling app/STM32G4_FWU3_431/prog/MSP/stm32g4/huart01xs.c
compiling app/STM32G4_FWU3_431/prog/MSP/stm32g4/hwdt01xs.c
compiling app/STM32G4_FWU3_431/prog/MSP/stm32g4/hi2c01xs.c
compiling app/STM32G4_FWU3_431/prog/MSP/stm32g4/IntTbl.c
compiling app/STM32G4_FWU3_431/prog/dbus/balXs.c
compiling app/STM32G4_FWU3_431/prog/dbus/dbusdllXs.c
compiling app/STM32G4_FWU3_431/prog/dbus/dbusmapping.c
compiling app/STM32G4_FWU3_431/prog/dbus/stm32g4/dbusmappingMc.c
compiling app/STM32G4_FWU3_431/prog/dbus/dbuspresentationXs.c
compiling app/STM32G4_FWU3_431/prog/dbus/dbuspresentation_updateXS.c
compiling app/STM32G4_FWU3_431/prog/dbus/dbuspresentation_productionXS.c
compiling app/STM32G4_FWU3_431/prog/ped_fw/basic/basicXs.c
compiling app/STM32G4_FWU3_431/prog/ped_fw/basic/stm32g4/basicMcXs.c
compiling app/STM32G4_FWU3_431/prog/ped_fw/schedule/scheduXS.c
compiling app/STM32G4_FWU3_431/prog/ped_fw/timer/timerXS.c
compiling app/STM32G4_FWU3_431/prog/ped_fw/timer/stm32g4/timerMcXs.c
compiling app/STM32G4_FWU3_431/prog/firmware_update/mal/fwu_opt_bytes_cfg.c
compiling app/STM32G4_FWU3_431/prog/firmware_update/mal/module_administration_cfg.c
compiling app/STM32G4_FWU3_431/prog/firmware_update/ModuleHeader.c
compiling app/STM32G4_FWU3_431/prog/ped_fw/schedule/schedDef.c
compiling app/STM32G4_FWU3_431/prog/firmware_update/Programmer/prog_user_task_config.c
compiling app/STM32G4_FWU3_431/prog/MSP/armCmx/int_prio_cfg.c
compiling app/STM32G4_FWU3_431/prog/MSP/armCmx/int_exc_cfg.c
compiling app/STM32G4_FWU3_431/prog/MSP/armCmx/systick_cfg.c
compiling common/prog/MSP/armCmx/IntTblArmCM.c
compiling common/prog/ped_fw/timer/system_timer.c
compiling common/prog/MSP/armCmx/hsupArmCM.c
compiling common/prog/MSP/armCmx/systick.c
compiling common/prog/MSP/armCmx/int_prio.c
compiling common/prog/std_lib/std_version.c
compiling common/prog/std_lib/std_callback.c
compiling common/prog/MSP/stm32g4/hadc01.c
compiling common/prog/MSP/stm32g4/hcct01.c
compiling common/prog/MSP/stm32g4/hcct01i.c
compiling common/prog/MSP/stm32g4/hdio01.c
compiling common/prog/MSP/stm32g4/hint01.c
compiling common/prog/MSP/stm32g4/hint01i.c
compiling common/prog/MSP/stm32g4/huart01.c
compiling common/prog/MSP/stm32g4/huart01i.c
compiling common/prog/MSP/stm32g4/hwdt01.c
compiling common/prog/MSP/stm32g4/weakvectbl.c
compiling common/prog/MSP/stm32g4/mem_drv/intflash_drv.c
compiling common/prog/MSP/hi2c01.c
compiling common/prog/dbus/bal.c
compiling common/prog/dbus/dbusdll.c
compiling common/prog/dbus/dbuspresentation.c
compiling common/prog/dbus/dbus_lock.c
compiling common/prog/dbus/bal_breaks.c
compiling common/prog/ped_fw/basic/main.c
compiling common/prog/ped_fw/schedule/schedule.c
compiling common/prog/ped_fw/stdcrc/CrcCom.c
compiling common/prog/ped_fw/stdcrc/stm32g4/StdCrc.c
compiling common/prog/ped_fw/timer/timerlib.c
compiling common/prog/ped_fw/utility/utility.c
compiling common/prog/ped_fw/watchdogtimer/watchdogtimer.c
compiling common/prog/firmware_update/mal/moduleAdministration.c
compiling common/prog/firmware_update/mal/dataAnchor.c
compiling common/prog/firmware_update/BootManager/BootManagerModule.c
compiling common/prog/firmware_update/BootManager/BootManagerSharedData.c
compiling common/prog/firmware_update/BootManager/stm32g4/BootManagerSharedDataUc.c
compiling common/prog/firmware_update/mem_drv/intflash_abs.c
generating out/STM32G4_FWU3_431/MyVariant/prog/ped_fw.lib
preprocessing file out/STM32G4_FWU3_431/MyVariant/bin/LinkerScript.sct
preprocessing file out/STM32G4_FWU3_431/MyVariant/bin/winflash_cfg.bp2
linking MyVariant
Program Size: Code=12540 RO-data=1836 RW-data=268 ZI-data=1224
--------Linking done-------------

creating metric file...
creating symbols and registry...
creating usage report...
patching binaries with PatchBin...
SoftwareSigner v1.0
signature[64] -> 94ab06d16df1ad8fe327454478396ad7583d9db0b72bfd775581cd7009ff53720d1dc9ed83bb10d9f156a39b33635f4fd7c916637c7841049b8db26e31072c02
Verification -> %!s(bool=true)
exit
```

[Home page of help](../../help.md)
