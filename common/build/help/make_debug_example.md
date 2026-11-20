###         make debug example
---


The target shows content of specified [variable](./make_variables.md) written after `var=`.

An example of usage target `make debug var=fwu_variants` to show all FWU3 built-in variants in make process:

Note: A project name is set to STM32G4_FWU3_431 in the example. If you do not set a project name, please, ommit it in command line.
The `<variant>.h` file is create based on these defines in the `app/<MyVariant>/prog` folder

```actionscript
make project=STM32G4_FWU3_431 debug var=fwu_variants

Using external tooldrive M:
Working directory of MCU Framework NOT clean!!!
===== Based on MCU Framework (heads/New_help_concept_proposal) =====
=====  Project is not a GIT repository =====
+++++ building variant MyVariant by using compiler keilArm +++++
BootManager
programmer
lromLoader
ramLoader
```


---


This example of target `make debug var=defines_cl` shows command line defines for current variant:

```actionscript
make debug var=defines_cl project=STM32G4_FWU3_431

Using external tooldrive M:
Working directory of MCU Framework NOT clean!!!
===== Based on MCU Framework (heads/New_help_concept_proposal) =====
=====  Project is not a GIT repository =====
+++++ building variant MyVariant by using compiler keilArm +++++
VARIANT=MyVariant
VARIANT_MYVARIANT
KEILARM
```
---

This example of target `make variant=romLoader debug var=defines` shows defines for specified variant (romLoader variant used in here):

```actionscript
make debug var=defines project=STM32G4_FWU3_431 variant=romLoader

Using external tooldrive M:
load settings for romLoader
Working directory of MCU Framework NOT clean!!!
===== Based on MCU Framework (heads/New_help_concept_proposal) =====
=====  Project is not a GIT repository =====
+++++ building variant romLoader by using compiler keilArm +++++
CRC32_SIZE=UINT32_C(0x4)
STACK_MAGIC_SIZE=UINT32_C(0x4)
NO_RAM_INIT
NO_ISR_SUPPORT
need_fixed_vectors
BUILT_IN_VARIANT
FWU_VARIANT
BOOTLOADER_VARIANT
UC_VENDOR=ST
PROC_FAMILY=STM32G431
STM32_ARM
STM32G4
STM32G4XX
STM32G431
__DBM_INITIAL_MANUAL_TX_INT_CALL_NEEDED
DEF_FLASH_SIZE=128u
RESET_VECTOR=0x8000004
BL_APPLICATION_START=0x8001004
```
*The output above is shortened.*

---


This example of target `make debug var=copt` shows generic compiler options defined for current variant:

```actionscript
make debug var=copt project=STM32G4_FWU3_431

Using external tooldrive M:
Working directory of MCU Framework NOT clean!!!
===== Based on MCU Framework (heads/New_help_concept_proposal) =====
=====  Project is not a GIT repository =====
+++++ building variant MyVariant by using compiler keilArm +++++
-DVARIANT=MyVariant
-DVARIANT_MYVARIANT
-DKEILARM
-Icommon/prog
-Iapp/STM32G4_FWU3_431/prog
-Istm32g4
-I..
-Istm32g4
-I..
-Icommon/prog/firmware_update/hsi
-Icommon/prog/dbus
-Icommon/prog/MSP
-Icommon/prog/MSP/compiler/keilArm
-Icommon/prog/MSP/stm32g4
-Icommon/prog/ped_fw
-Iapp/STM32G4_FWU3_431/prog/ped_fw
-Iapp/STM32G4_FWU3_431/prog/MSP/stm32g4
-Icommon/prog/ped_fw/utility
-Icommon/prog/ped_fw/timer
-Icommon/prog/std_lib
-Icommon/prog/FuncSafe
-Icommon/prog/MSP/CMSIS
-Icommon/prog/MSP/armCmx
-IM:/Keil/MDK_ARM_5.29/ARM/ARMCC/lib/armlib\\
-IM:/Keil/MDK_ARM_5.29/ARM/ARMCC/lib/cpplib\\
-IM:/Keil/MDK_ARM_5.29/ARM/ARMCC/include/
-O3
--thumb
--split_sections
--cpu=Cortex-M4.fp
--fpu=FPv4-SP
--apcs=/hardfp
-g
-c
--dwarf2
--no_debug_macros
--asm
--interleave
--apcs=interwork
--diag_suppress
951
--diag_suppress
1296
```
---

This example of target `make debug var=ccopt` shows C language compiler options defined for current variant:

```actionscript
make debug var=ccopt project=STM32G4_FWU3_431

Using external tooldrive M:
Working directory of MCU Framework NOT clean!!!
===== Based on MCU Framework (heads/New_help_concept_proposal) =====
=====  Project is not a GIT repository =====
+++++ building variant MyVariant by using compiler keilArm +++++
--c99
```
---

This example of target `make debug var=cppopt` shows C++ language compiler options defined for current variant:

```actionscript
make debug var=cppopt project=STM32G4_FWU3_431

Using external tooldrive M:
Working directory of MCU Framework NOT clean!!!
===== Based on MCU Framework (heads/New_help_concept_proposal) =====
=====  Project is not a GIT repository =====
+++++ building variant MyVariant by using compiler keilArm +++++
```
As you see, there is no C++ option.

[Home page of help](../../help.md)
