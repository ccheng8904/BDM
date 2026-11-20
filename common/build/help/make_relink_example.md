###         make relink example
---


The target removes binaries and runs link target again.

Note: A project name is set to STM32G4_FWU3_431 in the example. If you do not set a project name, please, ommit it in command line.

An example of usage target `make relink` after previous build:

```actionscript
make relink project=STM32G4_FWU3_431

Using external tooldrive M:
Working directory of MCU Framework NOT clean!!!
===== Based on MCU Framework (heads/New_help_concept_proposal) =====
=====  Project is not a GIT repository =====
+++++ building variant MyVariant by using compiler keilArm +++++
generating export file out/STM32G4_FWU3_431/MyVariant/make_export.xml
keeping app/STM32G4_FWU3_431/prog/MyVariant.h
--- Removing binaries ---
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


[Home page of help](../../help.md)
