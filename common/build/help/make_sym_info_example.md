###         make sym_info example
---


The target runs link target and show application symbols.

Note: A project name is set to STM32G4_FWU3_431 in the example. If you do not set a project name, please, ommit it in command line.

An example of usage target `make sym_info` to show `ITBL_fixedVectors` or `BMDAT_sharedData`.

```actionscript
make sym_info project=STM32G4_FWU3_431

Using external tooldrive M:
Working directory of MCU Framework NOT clean!!!
===== Based on MCU Framework (heads/New_help_concept_proposal) =====
=====  Project is not a GIT repository =====
+++++ building variant MyVariant by using compiler keilArm +++++
generating export file out/STM32G4_FWU3_431/MyVariant/make_export.xml
keeping app/STM32G4_FWU3_431/prog/MyVariant.h

application symbols:
BMDAT_sharedData                  20000000    BS28
MDHD_ModuleHeader                 08007000    BS40
MDHD_ModuleHeader2                     ---    ---
MAL_partitionFlex                      ---    ---
MAL_partitionFlexHeader                ---    ---
MAL_DataAnchor                    08000010    BS12
MAL_hw_version                         ---    ---
ITBL_fixedVectors                 08000000    PTR2
ITBL_coreVectors                  08007200    PTR16
ITBL_peripheralVectors            08007240    PTR102
```

You can add more symbols by adding them to `watch_Symbols` variable in your `<MyVariant.mak>`.

An example of adding symbols. The lines were added to file `MyVariant.mak`:

```
watch_Symbols += MAL_FwuOptBytes
watch_Symbols += MAL_HwIdSupport_hw_id
```
and then `make sym_info` target is used again:

```actionscript
make sym_info project=STM32G4_FWU3_431

Using external tooldrive M:
Working directory of MCU Framework NOT clean!!!
===== Based on MCU Framework (heads/New_help_concept_proposal) =====
=====  Project is not a GIT repository =====
+++++ building variant MyVariant by using compiler keilArm +++++
generating export file out/STM32G4_FWU3_431/MyVariant/make_export.xml
keeping app/STM32G4_FWU3_431/prog/MyVariant.h

application symbols:
MAL_FwuOptBytes                   0800001C    BS4
MAL_HwIdSupport_hw_id             0800A3AC    UC8
BMDAT_sharedData                  20000000    BS28
MDHD_ModuleHeader                 08007000    BS40
MDHD_ModuleHeader2                     ---    ---
MAL_partitionFlex                      ---    ---
MAL_partitionFlexHeader                ---    ---
MAL_DataAnchor                    08000010    BS12
MAL_hw_version                         ---    ---
ITBL_fixedVectors                 08000000    PTR2
ITBL_coreVectors                  08007200    PTR16
ITBL_peripheralVectors            08007240    PTR102
```

[Home page of help](../../help.md)
