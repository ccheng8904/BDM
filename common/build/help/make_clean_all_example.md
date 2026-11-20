###         make clean_all example
---


The target removes all content in the `out` folder for all variants.
An example of usage target `make clean_all`:

Note: A project name is set to STM32G4_FWU3_431 in the example. If you do not set a project name, please, ommit it in command line.


Status of `out` folder before applying `make clean_all` target:

```actionscript
dir out\STM32G4_FWU3_431

 Volume in drive S is Windows
 Volume Serial Number is E889-5163

 Directory of s:\Github\FW01\out\STM32G4_FWU3_431

27. 10. 2020  11:46    <DIR>          .
27. 10. 2020  11:46    <DIR>          ..
27. 10. 2020  11:46    <DIR>          BootManager
27. 10. 2020  11:45    <DIR>          lromLoader
27. 10. 2020  11:48    <DIR>          MyVariant
27. 10. 2020  11:46    <DIR>          programmer
27. 10. 2020  11:45    <DIR>          ramLoader
               0 File(s)              0 bytes
               7 Dir(s)  23 655 702 528 bytes free
```

Executing target with specified BootManager variant:

```actionscript
make clean_all project=STM32G4_FWU3_431

Using external tooldrive M:
Working directory of MCU Framework NOT clean!!!
===== Based on MCU Framework (heads/New_help_concept_proposal) =====
=====  Project is not a GIT repository =====
+++++ building variant MyVariant by using compiler keilArm +++++
--- Cleaning all variants ---
--- Cleaning all variants done ---
```

Status of `out` folder after applying `make clean_all` target:
```actionscript
dir out\STM32G4_FWU3_431

 Volume in drive S is Windows
 Volume Serial Number is E889-5163

 Directory of s:\Github\FW01\out

File Not Found
```

Note: the folder `STM32G4_FWU3_431` (project name) was removed from `out` folder, not only its content.

[Home page of help](../../help.md)
