# MDAC - Digital to analog converter

DAC is used to convert a digital value to an analog signal.

## 1 Data Types

### 1.1 Handler (`struct MDAC_Handle`)
MDAC requires some RAM to store information about the state of the peripheral unit. 
This information is stored in the handle structure. Each handle instance must be 
allocated in static memory by the user. The pointer to handle instance is the first parameter
for all high-level C interface functions.

### 1.2 Peripheral Configuration (`struct MDAC_Config`)

- `trigger`
  - MDAC_TRIGGER_SOFTWARE - (default)
  - MDAC_TRIGGER_HARDWARE

## 2 Interface Functions
For a detailed description of interface functions, visit [mdac.h](../mdac.h)

### 2.1 `MDAC_init`
`MDAC_init` is the main initialization function that will prepare and configure the peripheral unit.
If some settings need to be reconfigured, it is possible to call init with
a different configuration. If the requested configuration is not achievable on given
DA converter unit/channel of microcontroller, initialization function will return
an error value.

### 2.2 `MDAC_enable` / `MDAC_disable`
Both functions are used for enabling/disabling of DAC peripheral unit. After initialization the
peripheral unit is enabled.

### 2.3 `MDAC_initChannel`
Initialization function that will prepare and configure the channel. If initialization is not successful function will return an error value.

### 2.4 `MDAC_enableChannel` / `MDAC_disableChannel`
Both functions are used for enabling/disabling channels.

### 2.5 `MDAC_writeChannel`
The function sets the conversion data of the channel. Once data is set, the peripheral unit starts the conversion (the channel must be enabled). If the peripheral unit is not ready for DAC conversion, the function will return the appropriate error value.

### 2.6 `MDAC_getStatus`
The function returns the actual status of MDAC.
- MCAL_STATUS_RESET - Initial state, not initialized
- MCAL_STATUS_DISABLED - This status is set after calling MDAC_init() or MDAC_disable() functions. Peripheral is initialized, but is disabled.
- MCAL_STATUS_READY - Status is set after calling MDAC_enable() function. Peripheral is ready for operation.

## 3 Code Example in C

### 3.1 Includes:
```c
#include "mcal_channels.h"
```

### 3.2 Initialization of DA converter:
```c
static struct MDAC_Handle dacHandle;

const struct MDAC_Channel dacChannel =
{
    &MDAC1_OUT1_PA4_Descriptor,                 // Channel descriptor
    {
        MDAC_TRIGGER_SOURCE_SWTRIG,          // Trigger selection
        MDAC_DATAFORMAT_8_BITS,                 // Data format  selection
        MDAC_CHANNEL_MODE_NORMAL_BUFFER_EXT     // Chanel mode selection
    }
};

const struct MDAC_Config dacCfg =
{
    MDAC_TRIGGER_SOFTWARE
};

MDAC_init(&dacHandle, &MDAC1, &dacCfg);
MDAC_initChannel(&dacHandle);
```

### 3.3 Set and start AD conversion
```c
static uint16_t value = 0xFFU;
MDAC_writeChannel(&dacHandle, value);
MDAC_enableChannel(&dacHandle);
```

[Back to MCAL Homepage](../README.md)
