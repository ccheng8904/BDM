###         make rmbin example
---


The target removes binaries only in the `out` folder for current variant.
An example of usage target `make rmbin`:

Note: A project name is set to STM32G4_FWU3_431 in the example. If you do not set a project name, please, ommit it in command line.


Status of `out` folder before applying `make rmbin` target:

```actionscript
dir out\STM32G4_FWU3_431\MyVariant\

 Volume in drive S is Windows
 Volume Serial Number is E889-5163

 Directory of s:\Github\FW01\out\STM32G4_FWU3_431\MyVariant

25. 01. 2021  09:04    <DIR>          .
25. 01. 2021  09:04    <DIR>          ..
25. 01. 2021  09:04    <DIR>          bin
25. 01. 2021  09:02             7 477 code_gen.pkl
25. 01. 2021  09:04    <DIR>          lint
25. 01. 2021  09:03            10 354 make_export.xml
25. 01. 2021  09:03            88 931 MyVariant.dep
25. 01. 2021  09:04    <DIR>          prog
               3 File(s)        106 762 bytes
               5 Dir(s)  23 645 741 056 bytes free
```

```actionscript
make rmbin project=STM32G4_FWU3_431

Using external tooldrive M:
Working directory of MCU Framework NOT clean!!!
===== Based on MCU Framework (heads/New_help_concept_proposal) =====
=====  Project is not a GIT repository =====
+++++ building variant MyVariant by using compiler keilArm +++++
--- Removing binaries ---
```

Status of `out` folder after applying `make rmbin` target:
```actionscript
dir out\STM32G4_FWU3_431\MyVariant\

 Volume in drive S is Windows
 Volume Serial Number is E889-5163

 Directory of s:\Github\FW01\out\STM32G4_FWU3_431\MyVariant

25. 01. 2021  09:05    <DIR>          .
25. 01. 2021  09:05    <DIR>          ..
25. 01. 2021  09:02             7 477 code_gen.pkl
25. 01. 2021  09:04    <DIR>          lint
25. 01. 2021  09:03            10 354 make_export.xml
25. 01. 2021  09:03            88 931 MyVariant.dep
25. 01. 2021  09:04    <DIR>          prog
               3 File(s)        106 762 bytes
               4 Dir(s)  23 658 926 080 bytes free
```

Note: the folder `bin` has been removed from `out` folder, not only its content.

[Home page of help](../../help.md)
