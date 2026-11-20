# MEXTI - External Interrupt

This component is microcontroller's abstraction layer for work with external interrupts.


## 1. Data Types

### 1.1 Handler (`struct MEXTI_Handle`)
Handling structure is used for storing run-time data used for callback and event handling.
All instances of this structure must be allocated in static memory.
It is initialized in component initialization function, so it is not necessary to fill its items at definition place.

### 1.2 Configuration (`struct MEXTI_Config`)
MEXTI can be configured by using configuration structure that consists of:
- `trigger` - active event for triggering interrupt
    - `MEXTI_TRIGGER_DISABLE`
    - `MEXTI_TRIGGER_RISING`
    - `MEXTI_TRIGGER_FALLING`
    - `MEXTI_TRIGGER_RISING_FALLING`
    - `MEXTI_TRIGGER_LOW_LEVEL`
    - `MEXTI_TRIGGER_HIGH_LEVEL`

### 1.3 Channel (`struct MEXTI_Channel`)

MEXTI channel contains information about input pin and microcontroller specific configuration if any.
- `mextiPin` - port and pin number

List of all existing MEXTI channels can be found in mcal_channels.c file.

### 1.4 Supported Events (`enum MCAL_Event`)
MEXTI provides possibility to attach user callback function to one event:
- `MCAL_EVENT_TRIGGER` - status change on pin occurred

For more information about event handling and callback function attachment,
please visit [MCAL Callback System](mcal_callback.md) page.


## 2. Interface functions
For detailed description of interface functions visit [mexti.h](../mexti.h)

### 2.1 `MEXTI_init`
This is the first function that must be called in order to work with MEXTI component. It initializes MEXTI handle structure and peripheral HW. To its input parameters belong above metioned handle, channel and configuration structures.

### 2.2 `MEXTI_enableEvent` / `MEXTI_disableEvent`
This function enables/disables event for callback functionality. Callback instance must be already initialized, see [MCAL Callback System](mcal_callback.md).

### 2.3 `MEXTI_isEvent`
This function checks if event is pending.


## 3. Code Examples in C

### 3.1 Includes
```c
#include "mcal_channels.h"
```

### 3.2 Initialization
```c
struct MEXTI_Config mextiCfg =
{
    .trigger = MEXTI_TRIGGER_DISABLE
};

const struct MEXTI_Channel *MEXTI_IN1 =
{
    &MEXTIA0
};

static struct MEXTI_Handle MextiHandle1;

struct MDIO_Config mextiGpioCfg = MDIO_CFG_INPUT_PULL_UP;

MDIO_configure(MEXTI_IN1->mextiPin, &mextiGpioCfg);
mextiCfg.trigger = MEXTI_TRIGGER_RISING;
MEXTI_init(&MextiHandle1, MEXTI_IN1, &mextiCfg);
```

### 3.3 Callback function attachment
```c
static MCAL_Callback_t MextiCallback1;

void MEXTI_eventCallbackFunction(void *obj, uint32_t flags, const struct MCAL_EventResponse *eventResponse)
{
    /* add your code to do action after rising edge on pin A0 */
}

/* Callback initialization */
MCAL_initCallback(&MextiCallback1, MEXTI_eventCallbackFunction, (void*)&MextiHandle1);

/* Enabling of event and attaching it to callback */
MEXTI_enableEvent(&MextiHandle1, &MextiCallback1);
```
[Back to MCAL Homepage](../README.md)
