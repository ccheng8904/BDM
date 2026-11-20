###         make all example
---


The target builds all variants at once 
 * BootManager
 * lromLoader
 * lramLoader
 * programmer
 * and default application variant

Note: A project name is set to STM32G4_FWU3_431 in the example. If you do not set a project name, please, ommit it in command line.

An example of usage target `make all` e.g. after target `make clean_all`. The output from the target was shortened in the example.


```actionscript
make all project=STM32G4_FWU3_431

Using external tooldrive M:
Working directory of MCU Framework NOT clean!!!
===== Based on MCU Framework (heads/New_help_concept_proposal) =====
=====  Project is not a GIT repository =====
+++++ building variant MyVariant by using compiler keilArm +++++
s:/Github/FW01/common/tools/gmake/make -f common\build\makefile  variant=BootManager -s
Using external tooldrive M:
load settings for bootManager
Working directory of MCU Framework NOT clean!!!
===== Based on MCU Framework (heads/New_help_concept_proposal) =====
=====  Project is not a GIT repository =====
+++++ building variant BootManager by using compiler keilArm +++++
generating export file out/STM32G4_FWU3_431/BootManager/make_export.xml
keeping app/STM32G4_FWU3_431/prog/BootManager.h
compiling app/STM32G4_FWU3_431/prog/ped_fw/basic/stm32g4/basicMcXs.c
compiling app/STM32G4_FWU3_431/prog/MSP/stm32g4/hwdt01xs.c
compiling app/STM32G4_FWU3_431/prog/ped_fw/timer/timerXS.c
...
...
compiling common/prog/FuncSafe/fsCom.c
compiling common/prog/FuncSafe/fsramcon.c
compiling common/prog/FuncSafe/fs_psm.c
compiling common/prog/FuncSafe/armCmx/fs_registers_checks_arm_cmx.c
generating out/STM32G4_FWU3_431/BootManager/prog/ped_fw.lib
preprocessing file out/STM32G4_FWU3_431/BootManager/bin/LinkerScript.sct
preprocessing file out/STM32G4_FWU3_431/BootManager/bin/winflash_cfg.bp2
linking BootManager
Program Size: Code=5430 RO-data=482 RW-data=20 ZI-data=1100
--------Linking done-------------

creating metric file...
creating symbols and registry...
creating usage report...
patching binaries with PatchBin...
s:/Github/FW01/common/tools/gmake/make -f common\build\makefile  variant=programmer -s
Using external tooldrive M:
load settings for programmer
Working directory of MCU Framework NOT clean!!!
===== Based on MCU Framework (heads/New_help_concept_proposal) =====
=====  Project is not a GIT repository =====
+++++ building variant programmer by using compiler keilArm +++++
generating export file out/STM32G4_FWU3_431/programmer/make_export.xml
keeping app/STM32G4_FWU3_431/prog/programmer.h
compiling common/prog/firmware_update/Programmer/ModuleHeader.c
compiling app/STM32G4_FWU3_431/prog/MSP/stm32g4/hint01xs.c
...
...
compiling common/prog/firmware_update/mem_drv/mem_drv.c
compiling common/prog/firmware_update/hsi/hsi_basic.c
compiling common/prog/mem_utility/ring_buffer.c
generating out/STM32G4_FWU3_431/programmer/prog/ped_fw.lib
preprocessing file out/STM32G4_FWU3_431/programmer/bin/LinkerScript.sct
preprocessing file out/STM32G4_FWU3_431/programmer/bin/winflash_cfg.bp2
linking programmer
Program Size: Code=12216 RO-data=1088 RW-data=204 ZI-data=1800
--------Linking done-------------

creating metric file...
creating symbols and registry...
creating usage report...
patching binaries with PatchBin...
s:/Github/FW01/common/tools/gmake/make -f common\build\makefile  variant=lromLoader -s
Using external tooldrive M:
load settings for lromLoader
Working directory of MCU Framework NOT clean!!!
===== Based on MCU Framework (heads/New_help_concept_proposal) =====
=====  Project is not a GIT repository =====
+++++ building variant lromLoader by using compiler keilArm +++++
generating export file out/STM32G4_FWU3_431/lromLoader/make_export.xml
keeping app/STM32G4_FWU3_431/prog/lromLoader.h
compiling common/prog/firmware_update/BootLoader/ModuleHeader.c
...
...
compiling common/prog/firmware_update/BootLoader/BLservices.c
generating out/STM32G4_FWU3_431/lromLoader/prog/ped_fw.lib
preprocessing file out/STM32G4_FWU3_431/lromLoader/bin/LinkerScript.sct
preprocessing file out/STM32G4_FWU3_431/lromLoader/bin/winflash_cfg.bp2
linking lromLoader
Program Size: Code=1444 RO-data=160 RW-data=20 ZI-data=1052
--------Linking done-------------

creating metric file...
creating symbols and registry...
creating usage report...
patching binaries with PatchBin...
s:/Github/FW01/common/tools/gmake/make -f common\build\makefile  variant=ramLoader -s
Using external tooldrive M:
load settings for ramLoader
Working directory of MCU Framework NOT clean!!!
===== Based on MCU Framework (heads/New_help_concept_proposal) =====
=====  Project is not a GIT repository =====
+++++ building variant ramLoader by using compiler keilArm +++++
generating export file out/STM32G4_FWU3_431/ramLoader/make_export.xml
keeping app/STM32G4_FWU3_431/prog/ramLoader.h
compiling app/STM32G4_FWU3_431/prog/ped_fw/timer/stm32g4/timerMcXs.c
...
...
compiling common/prog/ped_fw/stdcrc/CrcCom.c
compiling common/prog/ped_fw/stdcrc/stm32g4/StdCrc.c
generating out/STM32G4_FWU3_431/ramLoader/prog/ped_fw.lib
preprocessing file out/STM32G4_FWU3_431/ramLoader/bin/LinkerScript.sct
preprocessing file out/STM32G4_FWU3_431/ramLoader/bin/winflash_cfg.bp2
linking ramLoader
Program Size: Code=2554 RO-data=242 RW-data=20 ZI-data=1320
--------Linking done-------------

creating metric file...
creating symbols and registry...
creating usage report...
patching binaries with PatchBin...
s:/Github/FW01/common/tools/gmake/make -f common\build\makefile  -s
Using external tooldrive M:
Working directory of MCU Framework NOT clean!!!
===== Based on MCU Framework (heads/New_help_concept_proposal) =====
=====  Project is not a GIT repository =====
common/build/mixed/depend.mak:33: out/STM32G4_FWU3_431/MyVariant/MyVariant.dep: No such file or directory
+++++ building variant MyVariant by using compiler keilArm +++++
generating export file out/STM32G4_FWU3_431/MyVariant/make_export.xml
keeping app/STM32G4_FWU3_431/prog/MyVariant.h
Scanning dependency of app/STM32G4_FWU3_431/prog/MSP/stm32g4/hadc01xs.c
...
...
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
...
...
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

-------- Starting lint --------
11:55:28,74
hadc01xs                 :-)  Errors:0  MISRA man.:0  MISRA req.:0  Warnings:0  MISRA adv.:0  Infos:0
hcct01xs                 :-)  Errors:0  MISRA man.:0  MISRA req.:0  Warnings:0  MISRA adv.:0  Infos:0
...
...
BootManagerSharedData    :-)  Errors:0  MISRA man.:0  MISRA req.:0  Warnings:0  MISRA adv.:0  Infos:0
BootManagerSharedDataUc  :-)  Errors:0  MISRA man.:0  MISRA req.:0  Warnings:0  MISRA adv.:0  Infos:0
intflash_abs             :-)  Errors:0  MISRA man.:0  MISRA req.:0  Warnings:0  MISRA adv.:0  Infos:1
11:56:40,57
-------- Linking lint --------
-------- Linting done --------
```
*The output above is shortened.*


[Home page of help](../../help.md)
