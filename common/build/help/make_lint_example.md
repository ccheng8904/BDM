###         make lint example
---


This is the 2nd default target. It starts the lint inspection of source code for current variant.

Note: A project name is set to STM32G4_FWU3_431 in the example. If you do not set a project name, please, ommit it in command line.

The target can be executed in cases when `make link` was used.
The output of  example `make lint` was shortened.


```actionscript
make lint project=STM32G4_FWU3_431

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
...
...
Scanning dependency of common/prog/firmware_update/mem_drv/intflash_abs.c
--------Dependencies calculated-----------

Using external tooldrive M:
Working directory of MCU Framework NOT clean!!!
===== Based on MCU Framework (heads/New_help_concept_proposal) =====
=====  Project is not a GIT repository =====
+++++ building variant MyVariant by using compiler keilArm +++++
generating export file out/STM32G4_FWU3_431/MyVariant/make_export.xml
keeping app/STM32G4_FWU3_431/prog/MyVariant.h
-------- Starting lint --------
12:29:47,56
hadc01xs                    :-)  Errors: 0 MISRA man.: 0 MISRA req.: 0 Warnings: 0 MISRA adv.: 0 Infos:  0
hcct01xs                    :-)  Errors: 0 MISRA man.: 0 MISRA req.: 0 Warnings: 0 MISRA adv.: 0 Infos:  0
hdio01xs                    :-)  Errors: 0 MISRA man.: 0 MISRA req.: 0 Warnings: 0 MISRA adv.: 0 Infos: 12
hint01xs                    :-)  Errors: 0 MISRA man.: 0 MISRA req.: 0 Warnings: 0 MISRA adv.: 0 Infos:  0
huart01xs                   :-)  Errors: 0 MISRA man.: 0 MISRA req.: 0 Warnings: 0 MISRA adv.: 0 Infos:  0
hwdt01xs                    :-)  Errors: 0 MISRA man.: 0 MISRA req.: 0 Warnings: 0 MISRA adv.: 0 Infos:  1
...
...
BootManagerModule           :-)  Errors: 0 MISRA man.: 0 MISRA req.: 0 Warnings: 0 MISRA adv.: 0 Infos:  4
BootManagerSharedData       :-)  Errors: 0 MISRA man.: 0 MISRA req.: 0 Warnings: 0 MISRA adv.: 0 Infos:  0
BootManagerSharedDataUc     :-)  Errors: 0 MISRA man.: 0 MISRA req.: 0 Warnings: 0 MISRA adv.: 0 Infos:  0
intflash_abs                :-)  Errors: 0 MISRA man.: 0 MISRA req.: 0 Warnings: 0 MISRA adv.: 0 Infos:  1
12:30:58,68
-------- Linking lint --------
-------- Linting done --------
```

[Home page of help](../../help.md)
