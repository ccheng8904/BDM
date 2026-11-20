###         make release example
---


The target builds an application variant in release version.
The output binaries are created within new sub-folder out/&lt;app_variant&gt;/bin/release. In those binaries, the FWU OTP memory area is excluded.
**NOTE:** Unlike in older Framework versions (up to v1.8.x), HWID compatibility check is no more performed after boot in FWU3. Instead, new Firmware Compatibility Check (FCC) is performed. Now, Programmer is no more entered when OTP area is empty.<br>For more details about FCC, please, refer to file [Firmware_Compatibility_Check](./../../prog/firmware_update/doc/Firmware_Compatibility_Check.md).

All built-in variants have to be built before usage of `release` target. Otherwise users will get error as shown in the example below.
Example of usage `make release` target (e.g. right after `make clean_all` target or after setup of a project).<br>
*NOTE: Output is shortened in the example.*

s:\Github\FW01>`make project=STM32G0_FWU3_LTS19 release`
```
Using external tooldrive M:
This is release build of MyVariant, parallel build (-j option) is suppressed
Working directory of MCU Framework NOT clean!!!
===== Based on MCU Framework (tags/v1.9.0) =====
=====  Project is not a GIT repository =====
+++++ building variant MyVariant by using compiler keilArm +++++
generating export file out/STM32G0_FWU3_LTS19/common_export.xml
generating export file out/STM32G0_FWU3_LTS19/MyVariant/variant_export.xml
keeping app/STM32G0_FWU3_LTS19/build/fwu_mem_map.mak
keeping app/STM32G0_FWU3_LTS19/prog/MyVariant.h
keeping app/STM32G0_FWU3_LTS19/prog/build_date.h
keeping app/STM32G0_FWU3_LTS19/prog/build_version.h
compiling app/STM32G0_FWU3_LTS19/prog/MSP/stm32g0/hadc01xs.c
compiling app/STM32G0_FWU3_LTS19/prog/MSP/stm32g0/hcct01xs.c
...
...
compiling common/prog/firmware_update/mem_drv/intflash_abs.c
compiling common/prog/firmware_update/mem_drv/com_mal.c
compiling common/prog/math/mathDiv.c
generating out/STM32G0_FWU3_LTS19/MyVariant/prog/ped_fw.lib
preprocessing file out/STM32G0_FWU3_LTS19/MyVariant/bin/LinkerScript.sct
preprocessing file out/STM32G0_FWU3_LTS19/MyVariant/bin/winflash_cfg.bp2
linking MyVariant
Program Size: Code=12450 RO-data=1570 RW-data=8 ZI-data=1488
--------Linking done-------------

creating metric file...
creating symbols and registry...
creating usage report...
To generate/rebuild  Boot-Loader type:
make variant=lromLoader project=STM32G0_FWU3_LTS19
To generate/rebuild Boot-Manager type:
make variant=BootManager project=STM32G0_FWU3_LTS19
To generate/rebuild programmer type:
make variant=programmer project=STM32G0_FWU3_LTS19
patching binaries with PatchBin...
Error: File out/STM32G0_FWU3_LTS19/BootManager/bin/BootManager.mot could not be found.
common/build/mixed/patchbin.mak:190: recipe for target 'out/STM32G0_FWU3_LTS19/MyVariant/bin/MyVariant.mot' failed
make: *** [out/STM32G0_FWU3_LTS19/MyVariant/bin/MyVariant.mot] Error 2
```
To speed-up the process, combined targets *all* and *release* can be used to get all built-in variants and perform release build.
See example:

s:\Github\FW01>`make project=STM32G0_FWU3_LTS19 all release`
```
Using external tooldrive M:
This is release build of MyVariant, parallel build (-j option) is suppressed
Working directory of MCU Framework NOT clean!!!
===== Based on MCU Framework (tags/v1.9.0) =====
=====  Project is not a GIT repository =====
+++++ building variant MyVariant by using compiler keilArm +++++
generating export file out/STM32G0_FWU3_LTS19/MyVariant/variant_export.xml
keeping app/STM32G0_FWU3_LTS19/build/fwu_mem_map.mak
s:/Github/FW01/common/tools/gmake/make -f common\build\makefile -B variant=BootManager -s
Using external tooldrive M:
load settings for BootManager
Working directory of MCU Framework NOT clean!!!
===== Based on MCU Framework (tags/v1.9.0) =====
=====  Project is not a GIT repository =====
+++++ building variant BootManager by using compiler keilArm +++++
keeping app/STM32G0_FWU3_LTS19/build/fwu_mem_map.mak
generating app/STM32G0_FWU3_LTS19/prog/BootManager.h
generating app/STM32G0_FWU3_LTS19/prog/build_date.h
generating app/STM32G0_FWU3_LTS19/prog/build_version.h
compiling app/STM32G0_FWU3_LTS19/prog/ped_fw/basic/stm32g0/basicMcXs.c
...
...
compiling common/prog/FuncSafe/armCmx/fs_registers_checks_arm_cmx.c
compiling common/prog/math/mathDiv.c
generating out/STM32G0_FWU3_LTS19/BootManager/prog/ped_fw.lib
preprocessing file out/STM32G0_FWU3_LTS19/BootManager/bin/LinkerScript.sct
preprocessing file out/STM32G0_FWU3_LTS19/BootManager/bin/winflash_cfg.bp2
linking BootManager
Program Size: Code=3404 RO-data=340 RW-data=0 ZI-data=1124
--------Linking done-------------

creating metric file...
creating symbols and registry...
creating usage report...
patching binaries with PatchBin...
s:/Github/FW01/common/tools/gmake/make -f common\build\makefile -B variant=programmer -s
Using external tooldrive M:
load settings for programmer
Working directory of MCU Framework NOT clean!!!
===== Based on MCU Framework (tags/v1.9.0) =====
=====  Project is not a GIT repository =====
+++++ building variant programmer by using compiler keilArm +++++
keeping app/STM32G0_FWU3_LTS19/build/fwu_mem_map.mak
generating app/STM32G0_FWU3_LTS19/prog/programmer.h
keeping app/STM32G0_FWU3_LTS19/prog/build_date.h
keeping app/STM32G0_FWU3_LTS19/prog/build_version.h
compiling common/prog/firmware_update/Programmer/ModuleHeader.c
compiling app/STM32G0_FWU3_LTS19/prog/MSP/stm32g0/hint01xs.c
compiling app/STM32G0_FWU3_LTS19/prog/MSP/stm32g0/huart01xs.c
...
...
compiling common/prog/firmware_update/mem_drv/com_mal.c
compiling common/prog/math/mathDiv.c
compiling common/prog/mem_utility/ring_buffer.c
generating out/STM32G0_FWU3_LTS19/programmer/prog/ped_fw.lib
preprocessing file out/STM32G0_FWU3_LTS19/programmer/bin/LinkerScript.sct
preprocessing file out/STM32G0_FWU3_LTS19/programmer/bin/winflash_cfg.bp2
linking programmer
Program Size: Code=12642 RO-data=1038 RW-data=12 ZI-data=1980
--------Linking done-------------

creating metric file...
creating symbols and registry...
creating usage report...
patching binaries with PatchBin...
s:/Github/FW01/common/tools/gmake/make -f common\build\makefile -B variant=lromLoader -s
Using external tooldrive M:
load settings for lromLoader
Working directory of MCU Framework NOT clean!!!
===== Based on MCU Framework (tags/v1.9.0) =====
=====  Project is not a GIT repository =====
+++++ building variant lromLoader by using compiler keilArm +++++
keeping app/STM32G0_FWU3_LTS19/build/fwu_mem_map.mak
generating app/STM32G0_FWU3_LTS19/prog/lromLoader.h
keeping app/STM32G0_FWU3_LTS19/prog/build_date.h
keeping app/STM32G0_FWU3_LTS19/prog/build_version.h
compiling common/prog/firmware_update/BootLoader/ModuleHeader.c
...
...
compiling common/prog/firmware_update/mem_drv/com_mal.c
compiling common/prog/math/mathDiv.c
generating out/STM32G0_FWU3_LTS19/lromLoader/prog/ped_fw.lib
preprocessing file out/STM32G0_FWU3_LTS19/lromLoader/bin/LinkerScript.sct
preprocessing file out/STM32G0_FWU3_LTS19/lromLoader/bin/winflash_cfg.bp2
linking lromLoader
Program Size: Code=1520 RO-data=164 RW-data=0 ZI-data=1072
--------Linking done-------------

creating metric file...
creating symbols and registry...
creating usage report...
patching binaries with PatchBin...
s:/Github/FW01/common/tools/gmake/make -f common\build\makefile -B variant=ramLoader -s
Using external tooldrive M:
load settings for ramLoader
Working directory of MCU Framework NOT clean!!!
===== Based on MCU Framework (tags/v1.9.0) =====
=====  Project is not a GIT repository =====
+++++ building variant ramLoader by using compiler keilArm +++++
generating export file out/STM32G0_FWU3_LTS19/common_export.xml
generating app/STM32G0_FWU3_LTS19/prog/ramLoader.h
keeping app/STM32G0_FWU3_LTS19/prog/build_date.h
keeping app/STM32G0_FWU3_LTS19/prog/build_version.h
compiling app/STM32G0_FWU3_LTS19/prog/ped_fw/timer/stm32g0/timerMcXs.c
compiling app/STM32G0_FWU3_LTS19/prog/MSP/stm32g0/huart01xs.c
compiling app/STM32G0_FWU3_LTS19/prog/MSP/stm32g0/hwdt01xs.c
...
...
compiling common/prog/firmware_update/mem_drv/com_mal.c
compiling common/prog/math/mathDiv.c
generating out/STM32G0_FWU3_LTS19/MyVariant/prog/ped_fw.lib
preprocessing file out/STM32G0_FWU3_LTS19/MyVariant/bin/LinkerScript.sct
preprocessing file out/STM32G0_FWU3_LTS19/MyVariant/bin/winflash_cfg.bp2
linking MyVariant
Program Size: Code=12450 RO-data=1570 RW-data=8 ZI-data=1488
--------Linking done-------------

creating metric file...
creating symbols and registry...
creating usage report...
To generate/rebuild  Boot-Loader type:
make variant=lromLoader project=STM32G0_FWU3_LTS19
To generate/rebuild Boot-Manager type:
make variant=BootManager project=STM32G0_FWU3_LTS19
To generate/rebuild programmer type:
make variant=programmer project=STM32G0_FWU3_LTS19
patching binaries with PatchBin...
-------- Starting lint --------
15:20:46,90
hadc01xs                       :-)  Errors:   0 MISRA man.:   0 MISRA req.:    0 Warnings:    0 MISRA adv.:    0 Infos:    0
hcct01xs                       :-)  Errors:   0 MISRA man.:   0 MISRA req.:    0 Warnings:    0 MISRA adv.:    0 Infos:    0
...
...
mathDiv                        :-)  Errors:   0 MISRA man.:   0 MISRA req.:    0 Warnings:    0 MISRA adv.:    0 Infos:    0
15:21:30,93
-------- Linking lint --------
-------- Linting done --------

keeping app/STM32G0_FWU3_LTS19/prog/MyVariant.h
keeping app/STM32G0_FWU3_LTS19/prog/build_date.h
keeping app/STM32G0_FWU3_LTS19/prog/build_version.h
compiling app/STM32G0_FWU3_LTS19/prog/MSP/stm32g0/hadc01xs.c
compiling app/STM32G0_FWU3_LTS19/prog/MSP/stm32g0/hcct01xs.c
...
...
compiling common/prog/firmware_update/mem_drv/com_mal.c
compiling common/prog/math/mathDiv.c
generating out/STM32G0_FWU3_LTS19/MyVariant/prog/ped_fw.lib
preprocessing file out/STM32G0_FWU3_LTS19/MyVariant/bin/LinkerScript.sct
preprocessing file out/STM32G0_FWU3_LTS19/MyVariant/bin/winflash_cfg.bp2
linking MyVariant
Program Size: Code=12450 RO-data=1570 RW-data=8 ZI-data=1488
--------Linking done-------------

creating metric file...
creating symbols and registry...
creating usage report...
To generate/rebuild  Boot-Loader type:
make variant=lromLoader project=STM32G0_FWU3_LTS19
To generate/rebuild Boot-Manager type:
make variant=BootManager project=STM32G0_FWU3_LTS19
To generate/rebuild programmer type:
make variant=programmer project=STM32G0_FWU3_LTS19
patching binaries with PatchBin...
generating binary out/STM32G0_FWU3_LTS19/MyVariant/bin/release/MyVariant.hex out/STM32G0_FWU3_LTS19/MyVariant/bin/release/MyVariant.mot
--- copy release files ---

```
*The output above is shortened.*

Example of `release` folder contents after `release` target is used:

s:\Github\FW01>`dir out\STM32G0_FWU3_LTS19\MyVariant\bin\release`
```
 Volume in drive S is Windows
 Volume Serial Number is D20A-EA69

 Directory of s:\Github\FW01\out\STM32G0_FWU3_LTS19\MyVariant\bin\release

04. 11. 2021  14:24    <DIR>          .
04. 11. 2021  14:24    <DIR>          ..
15. 11. 2021  15:22           106 496 Application
15. 11. 2021  15:22           363 137 MyVariant.hex
15. 11. 2021  15:22           387 310 MyVariant.mot
15. 11. 2021  15:22            18 775 MyVariant.syc
15. 11. 2021  15:22         1 096 751 MyVariant.sym
               7 File(s)      3 953 258 bytes
               2 Dir(s)  204 300 861 440 bytes free
```

[Home page of help](../../help.md)
