# MI2C - Inter-Integrated Circuit

This component is microcontroller's abstraction layer for work with i2c bus communication.
The I2C bus was designed by Philips in the early ’80s to allow easy communication between components which reside on the same circuit board.
In praxis I2C is not only used on single boards but also to connect components which are linked via cable.
The name I2C translates into “Inter IC”. Sometimes the bus is called IIC or I²C bus pronounced I-squared-C.
Simplicity and flexibility are key characteristics that make this bus attractive to many applications.
Most significant features include:

- Only two bus lines are required named SDA for data and SCL for clock.
- No strict baud rate requirements like for instance with RS232, the master generates a bus clock
- Simple master/slave relationships exist between all components
- Each device connected to the bus is software-addressable by a unique address
- I2C is a true multi-master bus providing arbitration and collision detection

## 1. Data Types


### 1.1 Handler (`struct MI2C_Handle`)
Handle structure is used for storing runtime information needed for handling of interrupt callback attachment, buffered reading/writing and storing of some additional data.
Each handle instance must be allocated in static memory by the user, as the handle is registered for interrupt handling during initialization.
Pointer to handle instance is used as first parameter for all high level C interface functions.
Handle instance is initialized in component initialization function, so it is not necessary to fill its items at definition place.


### 1.2 Configuration(`MI2C_Config`)

MI2C can be configured using configuration structure that consists of:

- `baudrate` - communication speed in bits per second
    - `MI2C_BAUDRATE_STANDARD`  up to 100 kbit/s
    - `MI2C_BAUDRATE_FAST`      up to 400 kbit/s
    - `MI2C_BAUDRATE_FAST_PLUS` up to 1 Mbit/s (available only for HW I2C functions)
- `mode`
    - `MI2C_WORK_AS_MASTER`  master generates clock signal on SCL line, listens and answers
    - `MI2C_WORK_AS_SLAVE`   slave only listens and answers (this mode is not supported yet)
- `ownAddress`  
   In case of slave mode this defines own component address on I2C system, that can be addressed by another master.
   In case of master mode any value may be used.
- `commType`
    - `MI2C_TYPE_SW`
       Communication on bus is done by toggling of IO pins (ON, OFF, input, output).
       Available for all controllers, that support [MDIO component](mdio.md).
	- `MI2C_TYPE_HW`
       Communication on bus is done by standard I2C registers available in controller, works with interrupts.
       So far available for STM32G0, MKV5x and EFR32XG21 controllers.


### 1.3 Channel (`struct MI2C_Channel`)

Channel structure combines information about I2C unit, SDA and SCL pins and static microcontroller specific configuration.
Channel structure is used during initialization to map handle to configuration for given ECU hardware.
All available MI2C channels can be found in the auto-generated module mcal_channels.c/.h

Example of channel definition structure:
```c
struct MI2C_Channel mi2cChannel =
{
    .mi2c   = &MI2C1,
    .sdaPin = &MDIOB9_MI2C1_SDA,
    .sclPin = &MDIOB8_MI2C1_SCL,
    .scfg   = MI2C_SCFG_DEFAULT
};
```
- `mi2c`
This parameter describes i2c hardware unit, that is available in controller. Usually there are 2 units present,
that are already defined in module mi2c_mc with some of names MI2C0 MI2C1 MI2C2 etc.
    - In case `MI2C_TYPE_HW` is defined this parameter must be set properly.
    - In case `MI2C_TYPE_SW` this parameter doesn't matter and may have value `NULL`.
- `sdaPin`
This parameter describes serial data pin properties, further explanation you can find in [MDIO component](mdio.md).
- `sclPin`
This parameter describes serial clock pin properties, further explanation you can find in [MDIO component](mdio.md).
- `scfg`
This parameter is present only for some derivatives, its value is derivative dependent. In case of STM32G0 here can be defined type of i2c source clock.

### 1.4 Supported Events (`enum MCAL_Event`)

MI2C provides possibility to attach user callback function to different events:
- `MCAL_EVENT_READ_COMPLETE` - number of bytes for reading has been reached and stop condition occurred
- `MCAL_EVENT_WRITE_COMPLETE` - number of bytes for writing has been reached and stop condition occurred
- `MCAL_EVENT_ERROR` - error during communication has been detected

For more information about event handling and callback function attachment,
please visit [MCAL Callback System](mcal_callback.md) page.


## 2. Interface functions

For detailed description of interface functions visit [mi2c.h](../mi2c.h)

### 2.1 `MI2C_init`

Main initialization function, that will prepare and configure peripheral unit. If some settings needs to be reconfigured, it is possible to call init with different configuration.
If requested configuration is not achievable on given microcontroller, initialization function will return error value.

### 2.2 `MI2C_enable` / `MI2C_disable`
Functions for enabling/disabling of peripheral unit. After initialization the peripheral unit is by default disabled, before any read/write operation it needs to be enabled.

### 2.3 `MI2C_forceBusRelease`
This function generates clock pulses and a stop condition to force a slave to release the bus.
It has meaning in situations when i2c communication runs and unexpected reset occurs.
I such cases may slave partner get stuck and hold SDA line low.

> This function can be used only in `MI2C_WORK_AS_MASTER` mode.

### 2.4 `MI2C_read` / `MI2C_write`
Functions for buffered read/write operations. Allocation of memory for read and write buffers must be done in application and data buffers must be allocated either in static memory
in case of reading and writing or in ROM memory for writing only.

> These functions can be used only in `MI2C_WORK_AS_MASTER` mode.
>
>> - In configuration case 'MI2C_TYPE_HW' they work in non-blocking mode and use interrupts.
>> - In configuration case 'MI2C_TYPE_SW' they work in blocking mode, all work is done during the function call.

### 2.5 `MI2C_transfer`
Function at first writes bytes from TX buffer to another communication partner and then reads bytes into RX buffer from the same communication partner.
> This function can be used only in `MI2C_WORK_AS_MASTER` mode.
>
>> - In configuration case 'MI2C_TYPE_HW' it works in non-blocking mode and uses interrupts.
>> - In configuration case 'MI2C_TYPE_SW' it works in blocking mode, all work is done during the function call.

### 2.6 `MI2C_configureBaudrate`
Function configures communication baudrate to one of 3 predefined values.
> This function can be used only in `MI2C_WORK_AS_MASTER` mode.
> In configuration case 'MI2C_TYPE_SW' the value 'MI2C_BAUDRATE_FAST_PLUS' is out of reach, don't use it.

## 3. Code Example in C

### 3.1 Includes:
```c
#include "mcal_channels.h"
```

### 3.2 Basic work:
```c
    struct MI2C_Handle mi2cHandle;

    struct MI2C_Config mi2cCfg = 
    {
        .baudrate   = MI2C_BAUDRATE_FAST,
        .mode       = MI2C_WORK_AS_MASTER,
        .ownAddress = 0u,
        .commType   = MI2C_TYPE_SW
    };

    struct MI2C_Channel mi2cChannel = MI2C1_SDA_PB9_SCL_PB8;

    void Test(void)
    {
        if(MCAL_OK == MI2C_init(&mi2cHandle, &mi2cChannel, &mi2cCfg))
        {
            MI2C_enable(&mi2cHandle);
            /* release SDA line in case of possible previous fault */
            if(MCAL_OK == MI2C_forceBusRelease(&mi2cHandle))
            {
                MI2C_retVal = MI2C_transfer(&mi2cHandle, transferData_write, (uint16_t)sizeof(transferData_write), transferData_read, (uint16_t)sizeof(transferData_read), CHIP1_BASE_ADDRESS);
                while(mi2cHandle.taskState != MI2C_TS_READY)
                {
                    /* wait until ready */
                }
            }
            MI2C_disable(&mi2cHandle);
        }
    }
```

### 3.3 Callback function attachment:
```c
/* Callback function for both, read and write complete events */
void Mi2cHandleCallbackFunction(void *objPtr, uint32_t eventFlags, const struct MCAL_EventResponse *eventResponse)
{
    /* User code */
}

/* Callback instance definition */
static struct STDCB_Callback Mi2cHandleCallback;

/* Callback initialization */
MCAL_initCallback(&Mi2cHandleCallback, Mi2cHandleCallbackFunction, NULL);

/* Enabling of events and attaching them to callback */
MI2C_enableEvent(&mi2cHandle, &Mi2cHandleCallback, MCAL_EVENT_WRITE_COMPLETE);
MI2C_enableEvent(&mi2cHandle, &Mi2cHandleCallback, MCAL_EVENT_READ_COMPLETE);
```

[Back to MCAL Homepage](../README.md)
