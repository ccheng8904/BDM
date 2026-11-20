# MWDT - WatchDog Timer
This component is a microcontroller's abstraction layer for work with watchdog timers.

Watchdog timer is a hardware timer that generates an MCU system reset on expiry of a programmed timeout period unless the application program periodically refreshes the content of the watchdog counter. It is used to reset an MCU in case the program gets stuck because of some unpredicted software or hardware failure. Possible failures include runaway of code, code stuck in never-ending loop or faults of the core (Hard Fault, Bus Fault, ...) that can lead to serious consequences. In such cases, the watchdog brings the MCU into a safe state of operation.

Based on watchdog hardware unit of the MCU, the watchdog can operate in two different modes- normal or window mode.
In a normal mode, the content of the watchdog counter must be refreshed sometime before given timeout period expires, otherwise a reset is generated.
In a window mode, the content of the watchdog counter must be refreshed only in given window period, otherwise a reset is generated.


## 1 Data Types
### 1.1 Configuration (`struct MWDT_Config`)
MWDT can be configured using configuration structure that consists of:
- `mode` - watchdog operating mode
  - `MWDT_NORMAL_MODE`
  - `MWDT_WINDOW_MODE`
- `upperTimeLimit` - timeout value in milliseconds of range 1-65535
- `lowerTimeLimit` - window value in milliseconds of range 1-65535, is ignored in normal mode

  Some timeout and window values are predefined in [mwdt.h](../mwdt.h)

- `debugConfig` - watchdog state during debug session
  - `MWDT_DEBUG_RUNNING`- watchdog runs during debug session
  - `MWDT_DEBUG_STOPPED`- watchdog is stopped during debug session


## 2 Interface Functions
For detailed description of interface functions visit [mwdt.h](../mwdt.h)
### 2.1 `MWDT_init`
Initialization function that prepares, configures and enables peripheral watchdog unit according to defined configuration structure. If some settings needs to be reconfigured, it is possible to call `MWDT_init` with different configuration. If requested configuration is not achievable on given microcontroller, initialization function will return error value.

### 2.1 `MWDT_trigger`
Function for so called "watchdog triggering" (refreshing the content of the watchdog counter).

### 2.1 `MWDT_getPrecisionInPercent`
Function for calculating the deviation (in percent) between required and achievable watchdog timeout. It is used internally by `MWDT_init` function- if the deviation is above the required limit, initialization function will return error value.


## 3 Code Examples in C

### 3.1 Includes:
```c
#include "mcal_channels.h"
```

### 3.2 Initialization and refreshing watchdog:
```c
// Configuration structure definition
struct MWDT_Config mwdtCfg =
{
    MWDT_WINDOW_MODE,
    MWDT_100MS_UPPER_TIMELIMIT,
    MWDT_10MS_LOWER_TIMELIMIT,
    MWDT_DEBUG_STOPPED
};

// To initialize watchdog, call function:
MWDT_init(&mwdtCfg);

// To refresh watchdog, call function:
MWDT_trigger();
```

[Back to MCAL Homepage](../README.md)
