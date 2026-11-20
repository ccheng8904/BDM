###         make clean example
---


The target removes all content in the `out` folder for current variant.
An example of usage target `make clean`:

Note: A project name is set to STM32G4_FWU3_431 in the example. If you do not set a project name, please, ommit it in command line.


Status of `out` folder before applying `make clean` target:

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
make clean project=STM32G4_FWU3_431 variant=BootManager

Using external tooldrive M:
load settings for bootManager
Working directory of MCU Framework NOT clean!!!
===== Based on MCU Framework (heads/New_help_concept_proposal) =====
=====  Project is not a GIT repository =====
+++++ building variant BootManager by using compiler keilArm +++++
--- Cleaning all ---
--- Cleaning done ---
```

Status of `out` folder after applying `make clean` target:

```actionscript
dir out\STM32G4_FWU3_431

 Volume in drive S is Windows
 Volume Serial Number is E889-5163

 Directory of s:\Github\FW01\out\STM32G4_FWU3_431

25. 01. 2021  08:29    <DIR>          .
25. 01. 2021  08:29    <DIR>          ..
27. 10. 2020  11:45    <DIR>          lromLoader
27. 10. 2020  11:48    <DIR>          MyVariant
27. 10. 2020  11:46    <DIR>          programmer
27. 10. 2020  11:45    <DIR>          ramLoader
               0 File(s)              0 bytes
               6 Dir(s)  23 678 332 928 bytes free
```
Only `BootManager` variant was removed from `out` folder.

To remove content of default application variant in the `out` folder all you need to do is simply do not specify variant in the make target.
```actionscript
make clean project=STM32G4_FWU3_431

Using external tooldrive M:
Working directory of MCU Framework NOT clean!!!
===== Based on MCU Framework (heads/New_help_concept_proposal) =====
=====  Project is not a GIT repository =====
+++++ building variant MyVariant by using compiler keilArm +++++
--- Cleaning all ---
--- Cleaning done ---
```
Status of `out` folder after applying `make clean` target:
```actionscript
dir out\STM32G4_FWU3_431

 Volume in drive S is Windows
 Volume Serial Number is E889-5163

 Directory of s:\Github\FW01\out\STM32G4_FWU3_431

25. 01. 2021  08:36    <DIR>          .
25. 01. 2021  08:36    <DIR>          ..
27. 10. 2020  11:45    <DIR>          lromLoader
27. 10. 2020  11:46    <DIR>          programmer
27. 10. 2020  11:45    <DIR>          ramLoader
               0 File(s)              0 bytes
               5 Dir(s)  23 780 130 816 bytes free
```
`MyVariant` variant was removed from `out` folder.

[Home page of help](../../help.md)