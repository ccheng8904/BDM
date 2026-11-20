# **FSF RAM CON - RAM consistency check for functional safety modules**

## **Introduction**

The RAM consistency check provides a service to test the consistency of a RAM data block that is used by a functional safety module.

## **Description**

### **Abbreviations**

* **consistent RAM function** The background task and the interrupt service routine, that require the RAM consistency check are called as 'consistent RAM function' in the further description.
* **consistent memory block** A 'consistent memory block' is a memory block, that is check for inconsistency. A 'consistent memory block' belongs to a 'subsystem'. A 'subsystem' contains 'consistent RAM functions'.
* **check method** A 'check method' is a method to detect a RAM inconsistency of a 'consistent memory block'. The implementation of FSF RAM CON contains different selectable methods.
* **check value** A 'check value' contains the information, against which the 'consistent memory block' is checked, to detect a RAM inconsistency. The layout of the 'check value' depends on the used 'check method'.
* **subsystem** A 'subsystem' is a set of modules containing background task and/or interrupt service routines. The subsystem is a enclosed entity.

### **Principle of Operation**

The FSF RAM CON provides different 'check methods' to check the consistency of a RAM block.
There exist 2 methods:

1. **RAM consistency check based on a 8 bit checkvalue**
2. **RAM consistency check based on a memory mirror**

In general the FSF RAM CON operates as follows.
1. The RAM consistency check is performed in 'consistent RAM functions' of a 'subsystem'
that is declared as functional safety relevant.
2. Before any read access in a 'consistent RAM function' to a variable in a 'consistent memory
block' is performed, the memory block is checked.
3. If a RAM inconsistency is discovered, the FSF_vCallApplicationErrorNotify() of the
channel FSF_RAM_CONSISTENCY_CHECK is called directly, using the given channel
identifier of a module that encountered an inconsistent RAM state.
4. At the time point, when a 'consistent RAM function' does not modify any variable in the
'consistent memory block' any more. The 'check value' is updated.
5. Exported interfaces of the 'subsystem', which modify variables in a 'consistent memory
block', update the 'check value' at the modification time point directly.

### **Definition of consistent memory blocks**

Variables of a 'subsystem', that shall be checked against illegal write accesses, are gathered in a 'consistent memory block'. The following has to be applied:

1. The definition of the variables of a 'subsystem' takes place in a structure.
2. The structure definition is in this context a 'consistent memory block'.
3. All elements of the structure have to be sorted by size, to minimize memory consumption.
4. For the definition of the structure, the macro **FSF_RAM_CON_DEFINE_VAR** has to be used.
5. Each 'consistent memory block' has an unique name.

The variables of a 'subsystem' shall be gathered in separate 'consistent memory blocks' using the
following rules:

1. variables, that are only used in a background task,
2. variables, that are only used by an interrupt service routine.
3. variables, that are used by an interrupt service routine and a background task.

The number of 'consistent memory blocks' shall also be defined in a manner,

1. that run time consumption is minimized,
2. that memory consumption is minimized,
3. that an inconsistent memory can be detected, at the beginning of a 'consistent RAM
function', before any write or read access of a variable in the 'consistent memory block' is
performed.  

It is important, that the 'consistent memory blocks' are not initialized at the definition. The
initialization of elements in the 'consistent memory block' shall be performed in the background
routine of the 'subsystem'.

**'check method' - RAM consistency check based on a 8 bit checksum**
The FSF RAM CON uses an 8 bit sum of the 'consistent memory block' to be checked. The 8 bit
sum is called 'check value' in the further description.

Each 'consistent memory block' has it's own 'check value'. If the 'check value' does not match the
current 8 bit sum of the 'consistent memory block', a RAM inconsistency is detected.

**'check method' - RAM consistency check based on a memory mirror**
Each 'consistent memory block' is mirrored in RAM. The content of the mirrored memory block is
equal to the 'consistent memory block', the 'subsystem operates' on.
If the 'consistent memory block' and the mirrored memory block have a different contents, a RAM
inconsistency is detected.

### **Usage**

The 'check method' is defined during compile time using a preprocessor directive. The
preprocessor directive determines the provided macros and functions of the subsystem FSF RAM
CON through the header file fsframcon.h. The preprocessor directive must be set before the
header file fsframcon::h is included in a module, that uses the RAM consistency service.

The following preprocessor directives are available:

1. ``` #define FSF_RAM_CON_BY_CHECK_VALUE``` - 'check method' - RAM consistency check
based on a 8 bit checksum
2. ``` #define FSF_RAM_CON_BY_MIRROR``` - 'check method' - RAM consistency check based on
a memory mirror
3. ``` #undef FSF_RAM_CON_BY_CHECK_VALUE``` and ``` #undef FSF_RAM_CON_BY_MIRROR``` - the RAM
consistency is disabled

The 'consistent RAM function'

1. has to check **each** memory block before any **read** access is performed to a variable of the
'consistent memory block' using the function **FSF_RAM_CON_vCheck.**

2. has to update the 'check value' of **each** memory block after **no write** access is performed to a variable of the 'consistent memory block'. The update is performed using the function **FSF_RAM_CON_vUpdateCheck** . The above is valid for 'consistent memory block',
which contain variables, that are read or written within the 'consistent RAM function'.

A 'consistent RAM function' uses the following code structure to add the RAM consistency check.
The example is made for a subsystem named 'Example', the 'consistent memory block' is named 'task'.
```c
#define FSF_RAM_CON_BY_MIRROR
#include "fsframcon.h"
// definition of the memory block
typedef struct _TExample_task {
uchar ucTest;
struct{
uchar bExample:1;
...
}ucMyBitField;
}TExample_task;
FSF_RAM_CON_DEFINE_VAR(Example, task);
// 'consistent RAM function' called on a regular base
void Example_MyConsistenRamFunc(void) {
FSF_RAM_CON_vCheck(Example, task, FSF_RAM_CON_MY_MY_FUNC_TASK);
Example_task.ucTest++;
Example_task.ucMyBitField.bExample = 0;
FSF_RAM_CON_vUpdateCheck(Example, task);
}
// Exported Interface
void Example_SetActive(void) {
FSF_RAM_CON_vSetBit(Example, task, ucMyBitField, bExample, 1);
}
```

It is recommended to have only one termination point within the 'consistent RAM function', since
each exit requires the update of the 'check value'.


The following circumstances require a direct update of the 'check value' at the assignment of a
value to a variable: 

1. all variables of a 'consistent memory block', that are modified in exported interfaces of the
subsystem, must update the check value in the interface function at the assignment of the value
2. all variables of a 'consistent memory block', that are written by a background task handler
and in addition by an interrupt service routine, must update the check value in the background task handler at the assignment of the value

The assignment of a value to a variable and the direct update of the check value is performed by
the following functions provided by FSF RAM CON:

* **FSF_RAM_CON_vSetUchar**
* **FSF_RAM_CON_vSetBoolean**
* **FSF_RAM_CON_vSetUint**
* **FSF_RAM_CON_vSetBit**
* **FSF_RAM_CON_vSetBitUchar**
* **FSF_RAM_CON_vResBitUchar**
* **FSF_RAM_CON_vSetBitUint**
* **FSF_RAM_CON_vResBitUint**

```c
void Example_SetActive(void) {
FSF_RAM_CON_vSetBit(Example, task, ucMyBitField, bExample, 1);
}
```

## **Implementation**

The implementation of the FSF RAM CON is found in the modules

1. fsramcon::c
2. fsramcon::h

## **Configuration**

none

## **Provided Services**

### **FSF_RAM_CON_DEFINE_VAR(module, blockName)**

This service performs the definition of a 'consistent memory block'.

* parameter *module* The mnemonic of the 'subsystem' e.g. ADOR.
* parameter *blockName* The name of the 'consistent memory block'.

### **FSF_RAM_CON_vCheck(module, blockName, ucId)**

This service performs the consistency check of a 'consistent memory block' against the related
'check value'. If an inconsistency is detected the function FSF_vCallApplicationErrorNotify is
called with the parameters

1. the functional safety channel identifier FSF_RAM_CONSISTENCY_CHECK defined in ```fsafe.h```,
2. the error code FSF_ERR_CHANNEL_ERROR
3. and the user identifier denoting the 'consistent memory block'.

* parameter *module* The mnemonic of the 'subsystem' e.g. ADOR.
* parameter *blockName* The name of the 'consistent memory block'.
* parameter *ucId* The identifier of the 'consistent memory block' passed to FSF_vCallApplicationErrorNotify.

### **FSF_RAM_CON_vUpdateCheck(module, blockName)**

This service updates the 'check value' of a 'consistent memory block'.

* parameter *module* The mnemonic of the 'subsystem' e.g. ADOR.
* parameter *blockName* The name of the 'consistent memory block' to update the 'check value'.

### **FSF_RAM_CON_vSetUchar(module, blockName, elementName, ucNewVal)**

This function sets the value of a variable of type uchar and updates the check value related to a
'consistent memory block.

* parameter *module* The mnemonic of the 'subsystem' e.g. ADOR.
* parameter *blockName* The name of the 'consistent memory block' to update the 'check value'.
* parameter *elementName* The name of the variable (structure element).
* parameter *ucNewVal* The value to set.

### **FSF_RAM_CON_vSetBoolean(module, blockName, elementName, bNewVal)**

This function sets the value of a variable of type boolean and updates the check value related to a 'consistent memory block.

* parameter *module* The mnemonic of the 'subsystem' e.g. ADOR.
* parameter *blockName* The name of the 'consistent memory block' to update the 'check value'.
* parameter *elementName* The name of the variable (structure element).
* parameter *bNewVal* The value to set.

### **FSF_RAM_CON_vSetUint(module, blockName, elementName, uiNewVal)**

This function sets the value of a variable of type uint and updates the check value related to a
'consistent memory block.

* parameter *module* The mnemonic of the 'subsystem' e.g. ADOR.
* parameter *blockName* The name of the 'consistent memory block' to update the 'check value'.
* parameter *elementName* The name of the variable (structure element).
* parameter *uiNewVal* The value to set.

### **FSF_RAM_CON_vSetBit(module, blockName, bitFieldName, bitName, ucNewVal)**

This function sets a bit in a bitfield of size uchar and updates the check value related to a
'consistent memory block.

> Warning:
>  
> The size of the bitfield must not be greater than 8 bit.

* parameter *module* The mnemonic of the 'subsystem' e.g. ADOR.
* parameter *blockName* The name of the 'consistent memory block' to update the 'check value'.
* parameter *bitFieldName* The name of the bitfield (structure element).
* parameter *bitName* The name of the bit in the bitfield (structure element).
* parameter *ucNewVal* The value to set.

### **FSF_RAM_CON_vSetBitUchar(module, blockName, bitFieldVar, bitMask)**

This function sets bits in a variable of type uchar using a bitmask and updates the check value related to a 'consistent memory block.

* parameter *module* The mnemonic of the 'subsystem' e.g. ADOR.
* parameter *blockName* The name of the 'consistent memory block' to update the 'check value'.
* parameter *bitFieldVar* The name of the bitfield variable (structure element).
* parameter *bitMask* The mask denoting the bits to set of type uchar.

### **FSF_RAM_CON_vResBitUchar(module, blockName, bitFieldVar, bitMask)**

This function resets bits in a variable of type uchar using a bitmask and updates the check value related to a 'consistent memory block.

* parameter *module* The mnemonic of the 'subsystem' e.g. ADOR.
* parameter *blockName* The name of the 'consistent memory block' to update the 'check value'.
* parameter *bitFieldVar* The name of the bitfield variable (structure element).
* parameter *bitMask* The mask denoting the bits to reset of type uchar.

### **FSF_RAM_CON_vSetBitUint(module, blockName, bitFieldVar, bitMask)**

This function sets bits in a variable of size uint using a bitmask and updates the check value related to a 'consistent memory block.

* parameter *module* The mnemonic of the 'subsystem' e.g. ADOR.
* parameter *blockName* The name of the 'consistent memory block' to update the 'check value'.
* parameter *bitFieldVar* The name of the bitfield variable (structure element).
* parameter *bitMask* The mask denoting the bits to set of type uint.

### **FSF_RAM_CON_vResBitUint(module, blockName, bitFieldVar, bitMask)**

This function resets bits in a variable of size uint using a bitmask and updates the check value related to a 'consistent memory block.

* parameter *module* The mnemonic of the 'subsystem' e.g. ADOR.
* parameter *blockName* The name of the 'consistent memory block' to update the 'check value'.
* parameter *bitFieldVar* The name of the bitfield variable (structure element).
* parameter *bitMask* The mask denoting the bits to reset of type uint.

## **Simple Example**

Each call of the task handle can check the result. It is not necessary to call function several times to distinguish error or success.

```c
/******************************************************************************
 *  Copyright (c) 2017 BSH Hausgeraete GmbH,
 *  Carl-Wery-Str. 34, 81739 Munich, Germany, www.bsh-group.de
 *
 *  All rights reserved. This program and the accompanying materials
 *  are protected by international copyright laws.
 *  Please contact copyright holder for licensing information.
 *
 ******************************************************************************
 *  PROJECT          SW_LIB_FRAMEWORK_NG_Git
 *  COMP_ABBREV      FS_SIMP
 ******************************************************************************/

/******************************************************************************/
/* DOCUMENTATION                                                              */
/******************************************************************************/
/** \file fs_app_channel_0.c
 *
 *  \ingroup  <DOXY_GROUP>
 *
 *  \brief    Implementation of ledcontroll.c
 *
 *  \details  A detailed file description that may span several lines and
 *            that has to be terminated by an empty line.
 */

/*****************************************************************************/
/* INCLUDES */
/*****************************************************************************/
#include "bsh_stdinc.h"
#include <stdint.h>

#include "fs_app_simple.h"

#include "fSafe.h"
#include "fSafeXS.h"
#include "fspriv.h"
#include "fsramcon.h"

/*****************************************************************************/
/* LOCAL DEFINITIONS                                                         */
/*****************************************************************************/

/*****************************************************************************/
/* LOCAL TYPE DEFINITIONS */
/*****************************************************************************/
/** \brief    Task handler states.
 *
 *  \details  All Task states have to be defined hier.
 *
 */
typedef enum FsSimpTaskState_e
{
    FS_SIMP_INIT = 0, ///< Initial BTN
    FS_SIMP_WORKING   ///< Initial Variable
} FsSimpTaskState_t;
/*****************************************************************************/
/* LOCAL FUNCTION DECLARATIONS */
/*****************************************************************************/

/*****************************************************************************/
/* LOCAL (CONST) VARIABLE DEFINITIONS */
/*****************************************************************************/
SDEF_SetSegmentRW(TIMER8_100MS) 
static Ttimer8 FS_SIMP_u8Timer;
SDEF_SetSegmentRW(default)

static FsSimpTaskState_t FS_SIMP_eTaskState = FS_SIMP_INIT;

typedef struct _FS_SIMP_val
{
    uint8_t u8Test;
    uint16_t u16Test;
    struct
    {
        uint8_t b2ndExample : 1;
        uint8_t b2ndExample1 : 3;
        uint8_t reserved : 4;
    } ucMy2ndBitField;
    BOOL bool2ndExample;
} TFS_SIMP_val;

#ifdef FUNCTIONAL_SAFETY_RAMCON_CHECK_ACTIVE
FSF_RAM_CON_DEFINE_VAR(FS_SIMP, val);
#else
TFS_APP_val FS_SIMP_val;
#endif

/*****************************************************************************/
/* LOCAL FUNCTION DEFINITIONS */
/*****************************************************************************/

/*****************************************************************************/
/* GLOBAL FUNCTION DEFINITIONS */
/*****************************************************************************/
void FS_SIMP_vSetExampleFlag(void)
{
    FSF_RAM_CON_vSetBit(FS_SIMP, val, ucMy2ndBitField, b2ndExample, 1u);
}

void FS_SIMP_vClearExampleFlag(void)
{
    FSF_RAM_CON_vSetBit(FS_SIMP, val, ucMy2ndBitField, b2ndExample, 0u);
}

uint8_t FS_SIMP_u8SimpleHandleTask(void)
{
    uint8_t retVal = TASK_NOT_INITIALISED;

    if (FS_SIMP_eTaskState != FS_SIMP_INIT)
        FSF_RAM_CON_vCheck(FS_SIMP, val, FSF_SIMPLE_CHECK);

    switch (FS_SIMP_eTaskState)
    {
    case FS_SIMP_INIT:
        /* code */
        FS_SIMP_eTaskState = FS_SIMP_WORKING;
        FS_SIMP_val.ucMy2ndBitField.b2ndExample = 0u;
        FS_SIMP_val.ucMy2ndBitField.b2ndExample1 = 1u;
        break;
    case FS_SIMP_WORKING:
        FS_SIMP_val.u8Test++;
        FSF_vReportSuccess(FSF_SIMPLE_CHECK);
        break;

    default:
        break;
    }

    if (FS_SIMP_eTaskState != FS_SIMP_INIT)
        retVal = TASK_INITIALISED;

    FSF_RAM_CON_vUpdateCheck(FS_SIMP, val);

    return retVal;
}

```
