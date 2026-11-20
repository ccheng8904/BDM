# MDIO - Digital Input/Output
A general-purpose input/output is an uncommitted digital signal pin on microcontroller
which may be used as an input or output, or both, and is controllable by the user at runtime.
Each port bit can be individually configured by software in several modes.


## 1 Data Types

### 1.1 Configuration (`struct MDIO_Config`)
- `mode` - operation mode
  - `MDIO_MODE_DIGITAL_INPUT`
  - `MDIO_MODE_DIGITAL_OUTPUT`
  - `MDIO_MODE_ALTERNATE_FUNCTION`
  - `MDIO_MODE_ANALOG`
- `pullType` - pull type
  - `MDIO_PULL_DISABLED`
  - `MDIO_PULL_UP`
  - `MDIO_PULL_DOWN`
- `outputType` - output type
  - `MDIO_OUTPUT_TYPE_PUSH_PULL`
  - `MDIO_OUTPUT_TYPE_OPEN_DRAIN`
  - `MDIO_OUTPUT_TYPE_OPEN_SOURCE`
- `outputState` - initial state of output after configuration
  - `MDIO_OUTPUT_STATE_NO_CHANGE`
  - `MDIO_OUTPUT_STATE_LOW`
  - `MDIO_OUTPUT_STATE_HIGH`

### 1.2 Channel (`struct MDIO_Channel`)
Channel structure combines information about GPIO unit, pin number and static microcontroller specific configuration.
List of existing values you can find in mcal_channels.c file.

## 2 Interface Functions
For detailed description of interface functions visit [mdio.h](../mdio.h)

### 2.1 `MDIO_init`
This function initializes all properties on peripheral HW for certain I/O pin.
To be able to work with certain I/O pin it must be called as the first function.

### 2.2 `MDIO_read` / `MDIO_write`
Function for reading/writing of MDIO pin level.

### 2.3 `MDIO_set` / `MDIO_reset`
Function for setting MDIO pin to HIGH/LOW level.

### 2.4 `MDIO_toggle`
Function for toggling of MDIO pin level.

## 3 Code Examples in C

### 3.1 Includes:
```c
#include "mcal_channels.h"
```

### 3.2 Using MDIO as digital input:
```c
const struct MDIO_Config myCfg = MDIO_CFG_INPUT;
const struct MDIO_Channel *myChannel  = &MDIOB0;
bool pinStatus;

MDIO_init(myChannel, &myCfg);
pinStatus = MDIO_read(myChannel);
```

### 3.3 Using MDIO as digital output:
```c
const struct MDIO_Config myCfg = MDIO_CFG_OUTPUT_PUSH_PULL;
const struct MDIO_Channel *myChannel = {&MDIOB1};

MDIO_init(myChannel, &myCfg);
MDIO_set(myChannel);
...
MDIO_toggle(myChannel);
```

### 3.4 Using MDIO in alternative function mode:
```c
/* pins configuration for UART */
struct MDIO_Config cfg = MDIO_CFG_ALTERNATE_FUNCTION;
const struct MDIO_Channel *rxPin = &MDIOA10_MUART1_RX;
const struct MDIO_Channel *txPin = &MDIOA9_MUART1_TX;

MDIO_init(txPin, &cfg);
cfg.pullType = MDIO_PULL_UP;
MDIO_init(rxPin, &cfg);
```

## 4 Code Examples in C++

### 4.1 Includes:
```c
#include "mcal_channels.h"
```

### 4.2 Using MDIO as digital input:
```c
const struct MDIO_Config myCfg = MDIO_CFG_INPUT;
bool pinStatus;
static MDIO::Channel testInput(&MDIOB0);

testInput.init(&myCfg);
pinStatus = testInput.read();
```

### 4.3 Using MDIO as digital output:
```c
const struct MDIO_Config myCfg = MDIO_CFG_OUTPUT_PUSH_PULL;
const struct MDIO_Channel *myChannel = {&MDIOB1};
static MDIO::Channel testOutput(myChannel);

testOutput.init(&myCfg);
testOutput.set();
...
testOutput.toggle();
```

### 4.4 Using MDIO in alternative function mode:
```c
/* pins configuration for UART */
struct MDIO_Config cfg = MDIO_CFG_ALTERNATE_FUNCTION;
const struct MDIO_Channel *rxPin = {&MDIOA10_MUART1_RX};
const struct MDIO_Channel *txPin = {&MDIOA9_MUART1_TX};
static MDIO::Channel Rx(rxPin);
static MDIO::Channel Tx(txPin);

Rx.init(&cfg);
cfg.pullType = MDIO_PULL_UP;
Tx.init(&cfg);
```
[Back to MCAL Homepage](../README.md)
