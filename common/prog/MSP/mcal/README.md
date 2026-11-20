# MCAL - Microcontroller Abstraction Layer
MCAL is successor of HAL (Hardware Abstraction Layer) that was designed for
8-bit microcontrollers with few peripheral units and tight memory restriction.
MCAL benefits from current trend in MCU architecture, where microcontrollers
have multiple instances (units) of the same peripheral, which are controlled
by similar (mostly the same) structure of registers. This approach allows
creation of common set of functions for controlling all of the peripheral
instances, reducing development time and allow easier testing of MCAL
libraries. Some of MCAL features are non-blocking buffered communication via
UART, SPI and I2C, runtime callback function attachment and automatic baudrate
reconfiguration when switching between different power modes.

MCAL component consists of several modules (subcomponents):
- [MPCM - Power and Clock Management](doc/mpcm.md)
- [MDIO - Digital Input/Output](doc/mdio.md)
- [MEXTI - External Interrupts](doc/mexti.md)
- [MUART - Universal Asynchronous Receiver-Transmitter](doc/muart.md)
- [MSPI - Serial Peripheral Interface](doc/mspi.md)
- [MI2C - Inter-Integrated Circuit](doc/mi2c.md)
- [MTIM - Hardware Timer](doc/mtim.md)
- [MADC - Analog-Digital Converter](doc/madc.md)
- [MWDT - Watchdog Timer](doc/mwdt.md)
- [MDMA - Direct Memory Access](doc/mdma.md)
- [MDAC - Digital-Analog Converter](doc/mdac.md)


## Generic MCAL Module Structure
Each MCAL module consists of two parts, namely low level platform specific
layer and generic platform independent layer. Low level layer is mostly used
for implementation of configuration functions and functions for peripheral
register access. Platform independent layer covers handling of buffered
communication, and handling of callback functionality.

- mxxx.h - platform independent interface declaration (main API)
- mxxx.c - platform independent implementation
- mxxx_types.h - platform independent type definitions
- mxxx_priv.h - platform independent inter-layer interface declaration
- [platform]/mxxx_mc.h - platform dependent interface and static configuration declaration.
- [platform]/mxxx_mc.c - platform dependent implementation


## Peripheral Unit and Channel Naming
To differentiate between different peripheral units, there is following
convention `[mxxx]<suffix>[num]`, where:
- mxxx - mcal library abbreviation, e.g. MDIO, MUART, MTIM, ...
- suffix - optional suffix in case microcontroller has multiple types of same peripheral, e.g. normal and low power
- num - unit number identical with vendor numbering in datasheet and reference manual.

This convention is different than in previous HAL, where numbering of units
for all peripherals started from number 0, which sometimes led to confusion
between MCU vendor naming and HAL naming (e.g. `USART1` for any STM32 was
named `HUART0`)


## Return Values
MCAL uses unified return type `enum MCAL_Error` for most of high level interface functions. To simplify checking of return
value from those functions, macros `MCAL_RETURN_ON_ERROR` and `MCAL_ASSERT_ON_ERROR` from [mcal_assert](mcal_assert.h)
can be used. Both macros evaluate return value from called function and if the value is not MCAL_OK then either 
return from function is executed with given return value (`MCAL_RETURN_ON_ERROR`) or assert function is called 
(`MCAL_ASSERT_ON_ERROR`), reporting file name, line number and expression (function) that returned error value.


## Callback system
MCAL uses callback system based on [std_lib/std_callback](../../std_lib/std_callback.h) with some modifications to
return value and parameters of callback functions. For more detail see [MCAL Callback System](doc/mcal_callback.md)


## C++ Compatibility
Most of MCAL module features something called C++ wrapper, which is basically
C++ class with methods mapped to C functions to allow easier instantiation
and use of the modules from C++ source code.


## Using of MCAL in MCU FW
MCAL is handled as any other component in MCU FW. To add MCAL for compilation,
add "MSP/mcal" component to common_components make variable in variant build file:
```make
common_components += MSP/mcal
```

It is also possible to explicitly set which modules will be added to build
process, by setting "mcal_modules" make variable in variant build file. By
default all available modules for given platform are used.
```c
mcal_modules = mdio mpcm muart
```

For each added module, there is definition generated in variant header file as
well as definition that MCAL as a whole is used.
```c
#define MCAL_INCLUDED
#define MCAL_MDIO_INCLUDED
#define MCAL_MUART_INCLUDED
#define MCAL_MPCM_INCLUDED
```

[Home page of help](../../../README.md)
