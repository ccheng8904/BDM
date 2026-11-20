###         make init example
---


The target copies configuration files from from templates to the app folder.

Note: A project name is set to STM32G4_FWU3_431 in the example. If you do not set a project name, please, ommit it in command line.

An example of usage target `make init` e.g. in case of missing configuration files. Only missing configuration files are copied. Existing files are kept.
The target can be used also after component was added to build process e.g. `FuncSafe`

```actionscript
make init project=STM32G4_FWU3_431

Using external tooldrive M:
Working directory of MCU Framework NOT clean!!!
===== Based on MCU Framework (heads/New_help_concept_proposal) =====
=====  Project is not a GIT repository =====
+++++ building variant MyVariant by using compiler keilArm +++++
generating export file out/STM32G4_FWU3_431/MyVariant/make_export.xml
keeping app/STM32G4_FWU3_431/prog/MSP/stm32g4/IntTbl.h
keeping app/STM32G4_FWU3_431/prog/MSP/stm32g4/hi2c01xs.h
keeping app/STM32G4_FWU3_431/prog/dbus/dbuspresentation_subsys0rxXS.h
keeping app/STM32G4_FWU3_431/prog/ped_fw/schedule/schedDef.h
keeping app/STM32G4_FWU3_431/prog/ped_fw/timer/timer_seg.h
keeping app/STM32G4_FWU3_431/prog/ped_fw/testSupport/TestSupportXS.c
keeping app/STM32G4_FWU3_431/prog/ped_fw/testSupport/TestSupportXS.h
keeping app/STM32G4_FWU3_431/prog/ped_fw/testengine/teng_msgsrv_XS.c
keeping app/STM32G4_FWU3_431/prog/ped_fw/testengine/teng_msgsrv_XS.h
keeping app/STM32G4_FWU3_431/prog/firmware_update/mal/module_administration_cfg.h
keeping app/STM32G4_FWU3_431/prog/firmware_update/hw_id_supported.h
keeping app/STM32G4_FWU3_431/prog/firmware_update/mem_drv_config.c
generating app/STM32G4_FWU3_431/prog/Funcsafe/fSafeXS.h
keeping app/STM32G4_FWU3_431/prog/MSP/stm32g4/option_bytes_dual_bank.eep
keeping app/STM32G4_FWU3_431/prog/MSP/stm32g4/option_bytes_single_bank.eep
keeping app/STM32G4_FWU3_431/prog/MSP/stm32g4/option_bytes.eep
generating app/STM32G4_FWU3_431/prog/FuncSafe/stm32g4/fsafe.mak
keeping app/STM32G4_FWU3_431/prog/MSP/stm32g4/hadc01xs.c
keeping app/STM32G4_FWU3_431/prog/MSP/stm32g4/hcct01xs.c
keeping app/STM32G4_FWU3_431/prog/MSP/stm32g4/hdio01xs.c
keeping app/STM32G4_FWU3_431/prog/MSP/stm32g4/hint01xs.c
keeping app/STM32G4_FWU3_431/prog/MSP/stm32g4/huart01xs.c
keeping app/STM32G4_FWU3_431/prog/MSP/stm32g4/hwdt01xs.c
keeping app/STM32G4_FWU3_431/prog/MSP/stm32g4/hi2c01xs.c
keeping app/STM32G4_FWU3_431/prog/MSP/stm32g4/IntTbl.c
keeping app/STM32G4_FWU3_431/prog/dbus/balXs.c
keeping app/STM32G4_FWU3_431/prog/dbus/dbusdllXs.c
keeping app/STM32G4_FWU3_431/prog/dbus/dbusmapping.c
keeping app/STM32G4_FWU3_431/prog/dbus/stm32g4/dbusmappingMc.c
keeping app/STM32G4_FWU3_431/prog/dbus/dbuspresentationXs.c
keeping app/STM32G4_FWU3_431/prog/dbus/dbuspresentation_updateXS.c
keeping app/STM32G4_FWU3_431/prog/dbus/dbuspresentation_productionXS.c
keeping app/STM32G4_FWU3_431/prog/ped_fw/basic/basicXs.c
keeping app/STM32G4_FWU3_431/prog/ped_fw/basic/stm32g4/basicMcXs.c
keeping app/STM32G4_FWU3_431/prog/ped_fw/schedule/scheduXS.c
keeping app/STM32G4_FWU3_431/prog/ped_fw/timer/timerXS.c
keeping app/STM32G4_FWU3_431/prog/ped_fw/timer/stm32g4/timerMcXs.c
keeping app/STM32G4_FWU3_431/prog/firmware_update/mal/fwu_opt_bytes_cfg.c
keeping app/STM32G4_FWU3_431/prog/firmware_update/mal/module_administration_cfg.c
keeping app/STM32G4_FWU3_431/prog/firmware_update/ModuleHeader.c
generating app/STM32G4_FWU3_431/prog/Funcsafe/fSafeXS.c
generating app/STM32G4_FWU3_431/prog/Funcsafe/stm32g4/fSafeMcXs.c
keeping app/STM32G4_FWU3_431/prog/ped_fw/schedule/schedDef.c
keeping app/STM32G4_FWU3_431/prog/firmware_update/Programmer/prog_user_task_config.c
keeping app/STM32G4_FWU3_431/prog/MSP/armCmx/int_prio_cfg.c
keeping app/STM32G4_FWU3_431/prog/MSP/armCmx/int_exc_cfg.c
keeping app/STM32G4_FWU3_431/prog/MSP/armCmx/systick_cfg.c
keeping app/STM32G4_FWU3_431/prog/MSP/stm32g4/LinkerScript.sct.h
keeping app/STM32G4_FWU3_431/prog/MSP/stm32g4/LinkerScript.ld.h
```

[Home page of help](../../help.md)
