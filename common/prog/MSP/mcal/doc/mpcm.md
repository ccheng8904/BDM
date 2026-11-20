# MPCM - Power and Clock Management

MPCM handles configuration of clock tree and manages transition between different power modes. This module is intended to replace former "basic" (`BASIC_vInitPlatform`) configuration. MPCM supports four power modes: normal run `MPCM_PMODE_RUN`, low-power run `MPCM_PMODE_LPRUN`, sleep `MPCM_PMODE_SLEEP` and stop mode `MPCM_PMODE_STOP`.

Although it is possible to run core with frequency below 1MHz, it is highly recommended to set core frequency
(and main peripheral bus clock frequency) to integer value of MHz and above 4MHz, otherwise functions of some modules
might not work properly (System Timer, TimerLib, DBus, ...). In low power modes when most systems are disabled, it 
is possible to run with lower speed than 1MHz e.g. from low speed oscillator, but with limitations mentioned above.

## 1 Data Types

### 1.1 Configuration (`struct MPCM_Config`)
Configuration structure is used to keep the information about configuration for single power mode. Usually, in platform dependent mpcm_mc files there are several configuration instances prepared for different system (core) and peripheral clock frequencies. If none of the prepared instances fits target application, it is possible to define custom configuration in application source code. Default configuration `MPCM_CFG_DEFAULT` takes core clock `CORE_CLOCK` and peripheral clock `PCLOCK` from make process as inputs and is intended to be used as a standard run configuration.

### 1.2 Power modes configuration (`struct MPCM_PowerModesConfig`)
This structure is used to configure MCU for all power modes. It should be defined as const as MPCM uses its information during every power mode change.
- `run` - configuration for main power run mode. It is activated after initialization.
- `lpRun` - configuration for low-power run mode
- `sleep` - configuration for sleep mode
- `stop` - configuration for stop mode

### 1.3 Power modes (`enum MPCM_PowerMode`)
This enumeration type defines all power mode states.

- `MPCM_PMODE_RESET` - state after MCU power-up and before initialization
- `MPCM_PMODE_RUN` - Standard state with all clocks running at nominal frequency. This mode is set after initialization.
- `MPCM_PMODE_LPRUN` - Low-power run mode, when clocks run at lowered frequencies to lower current consumption while maintaining code execution for both ISR routines and scheduler handle tasks. This mode can be used when DBus communication is needed but no other tasks with high CPU load are running.
- `MPCM_PMODE_SLEEP` - During sleep mode normal code execution is stopped (scheduler handle tasks), but usually all interrupts are running. In this mode also System Timer callback functions are executed when appropriate System Timer instance is processed by interrupt.
- `MPCM_PMODE_STOP` - In stop mode most peripherals are disabled and wake-up capability is limited. To wake up MCU from stop mode, peripherals with independent clock from CPU clock is needed. Typical wake-up sources from stop mode are external interrupts, watchdog timer or low-power timer. Some MCUs feature stop mode that requires MCU reset to wake up.

### 1.4 `enum MPCM_Clock`
Platform dependent enumeration type defining all available clock buses for given MCU.

### 1.5 Supported Events (`enum MCAL_Event`)
MCAL provides possibility to attach user callback function to different events:
- `MCAL_EVENT_PMODE_CHANGE_XXX` - events related to power mode change
- `MCAL_EVENT_CLOCK_CHANGE_XXX` - events related to clock tree reconfiguration

where `XXX` can be one of following:
- `REQ` - request to change power mode / clock configuration.
- `PRE` - before transition to new power mode / clock configuration
- `POST` - after transition to new power mode / clock configuration
- `ANY` - `REQ`, `PRE` and `POST` events combined

For more information about event handling and callback function attachment, please visit [MCAL Callback System](mcal_callback.md) page.

## 2 Interface Functions
For detailed description of interface functions visit [mpcm.h](../mcpcm.h)

### 2.1 `MPCM_init`
Main initialization function. It is used to set configuration for all power modes based on input parameter. Also, it configures MCU for `MPCM_PMODE_RUN` mode after boot.

### 2.2 `MPCM_changePowerMode`
Function for switching between power modes. When changing power mode to `MPCM_PMODE_SLEEP` or `MPCM_PMODE_STOP` mode, this function behaves as blocking code execution until wake-up request is generated. After returning from sleep/stop mode, previous power mode is restored. Any power mode change should be done within handle task especially when changing to sleep or stop mode to prevent possible problems with IRQ blocking.

### 2.3 `MPCM_wakeUp`
This function should be called from IRQ routine in order to wake up MCU from sleep or stop mode. After waking up from `MPCM_PMODE_SLEEP` or `MPCM_PMODE_STOP` mode by calling `MPCM_wakeUp` function, MPCM automatically returns MCU to previous power mode, so it is not possible to change directly from sleep to stop mode and vice versa.

### 2.4 `MPCM_abortPowerModeChange`
Function used to abort change of power mode in the phase of executing callback to "change request" event (`MCAL_EVENT_PMODE_CHANGE_REQ` and `MCAL_EVENT_CLOCK_CHANGE_REQ` events). This function can be used in the case when change of power mode may cause issues like corruption of internal/external memory, damage to ECU/appliance or even safety issues (e.g. motor is still running). Ideally, this should never happen as the request to change power mode should be called only when all systems are ready for it and it should be handled by application layer.

### 2.5 `MPCM_getClockFreq`
Function for reading actual clock frequency of clock bus defined by input parameter.

## 3 Code Examples in C

### 3.1 Includes:
```c
#include "mcal_channels.h"
```

### 3.2 Initialization:
```C
const struct MPCM_PowerModesConfig customPowerModes =
{
    /* Check available configurations for your platform in mcpm_mc.h */
    &MPCM_CFG_DEFAULT,              // Take configuration from make process
    &MPCM_CFG_CORE_8_PCLK_8,        // Lowered clock speeds for low-power run
    NULL,                           // No special configuration for sleep mode
    &MPCM_CFG_STOP
};

void AppHandleTask(void)
{
    static bool isInitialized = false;

    if(!isInitialized)
    {
        isInitialized = true;

        MPCM_init(&customPowerModes);
    }
}
```


### 3.3 Attachment of callback to clock change events:
```C
void ClockChangeCallbackFunction(void *obj, uint32_t flags, const struct MCAL_EventResponse *eventResponse)
{
    if(MCAL_isEvent(eventResponse, MCAL_EVENT_CLOCK_CHANGE_REQ))
    {
        /* Check if module is prepared for transition */
        if(isModuleBusy)
        {
            // There has to be serious reason for this
            MPCM_abortPowerModeChange();
        }
    }
    else if(MCAL_isEvent(eventResponse, MCAL_EVENT_CLOCK_CHANGE_PRE))
    {
        /* Clock tree is about to change */
        ...
    }
    else if(MCAL_isEvent(eventResponse, MCAL_EVENT_CLOCK_CHANGE_POST))
    {
        /* Clock tree has been changed, do adaptations if necessary */
        ...
    }
    else
    {
        /* Satisfy Lint */
    }
}

/* Callback instance definition */
static MCAL_Callback_t ClockChangeCallback;

void AppHandleTask(void)
{
    static bool isInitialized = false;

    if(!isInitialized)
    {
        isInitialized = true;

        MCAL_initCallback(&ClockChangeCallback, ClockChangeCallbackFunction, NULL);
        MPCM_enableEvent(&ClockChangeCallback, MCAL_EVENT_CLOCK_CHANGE_ANY);
    }
}
```

### 3.4 Waking up from sleep/stop mode using external interrupt:
For initialization of external interrupt and callback function attachment see [mexti](mexti.md) documentation.
```C
void ExternalInterruptFunction(void *obj, uint32_t flags, const struct MCAL_EventResponse *eventResponse)
{
    MPCM_wakeUp();
}

void AppHandleTask(void)
{
    /* Change to sleep power mode */
    MPCM_changePowerMode(MPCM_PMODE_SLEEP);

    /* This line will be reached after MPCM_wakeUp function is called from IRQ routine */
}
```

[Back to MCAL Homepage](../README.md)
