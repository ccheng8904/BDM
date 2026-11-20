# MSPI - Serial Peripheral Interface 
The SPI interface can be used to communicate with external devices using
the SPI protocol. The serial peripheral interface (SPI) protocol supports
full-duplex synchronous serial communication with external devices.
The interface is configured as master and in this case it provides the
communication clock (SCLK) to the external slave device.

Chip select pin can operate in two modes. One is manual mode, where application
is responsible for activating appropriate chip select pin of slave device.
Second mode is automatic activation of chip select during transfer of data.
Automatic chip select activation can be used only when there is single
slave device.

## 1. Data Types

###  1.1 Handler (`struct MSPI_Handle`)
MSPI requires some RAM memory for its function to store information about
state of peripheral unit, ongoing read/write operations and for callback
functionality. All this information is stored in handle structure. Each
handle instance must be allocated in static memory by the user, as the handle
is registered for interrupt handling during initialization. Pointer to handle
instance is used as first parameter for all high level C interface functions. 

### 1.2 Configuration (`struct MSPI_Config`)
MSPI can be configured using configuration structure that consists of:
- `dataBits` - number of data bits
  - `MSPI_DATA_BITS_8` (default)
  - `MSPI_DATA_BITS_16`
- `clockPolarity` - level on SCLK pin if nothing transmit
  - `MSPI_CLOCK_POLARITY_0`
  - `MSPI_CLOCK_POLARITY_1` (default)
- `clockPhase` - determines the polarity of SCLK
  - `MSPI_CLOCK_PHASE_0`
  - `MSPI_CLOCK_PHASE_1` (default)
- `dataInvert` - data inversion
  - `MSPI_DATA_INVERT_DISABLED` (default)
  - `MSPI_DATA_INVERT_ENABLED`
- `frameFormat` - format for transmit and receive with MSB or LSB first (Most/Least Significant Bit)
  - `MSPI_FRAME_FORMAT_MSB_FIRST` (default)
  - `MSPI_FRAME_FORMAT_LSB_FIRST`
- `chipSelMode` - chip select setting
  - `MSPI_CHIP_SELECT_MANUAL`
  - `MSPI_CHIP_SELECT_AUTO_LOW` (default)
  - `MSPI_CHIP_SELECT_AUTO_HIGH`
- `clockFreq` - communication speed - generated frequency on SCLK pin
- `misoPullResistor` - enable/disable internal pull up/down resistor connected to MISO (master input) pin
  - `MDIO_PULL_DISABLED`
  - `MDIO_PULL_UP` (default)
  - `MDIO_PULL_DOWN`

### 1.3 Channel (`struct MSPI_Channel`)
Channel structure combines information about SPI unit, SPI pins (SCLK, MISO, MOSI and CS) and
static microcontroller specific configuration. Channel structure is used
during initialization to map handle to configuration given ECU hardware.

Usually each SPI unit can have several different combinations of pins.
For usual combinations of pin mapping together with default static
configuration there are prepared channels in mspi_mc.h file, that can be
used directly from application source code.

### 1.4 Supported Events `enum MCAL_Event`
MSPI provides possibility to attach user callback function to different events:
- `MCAL_EVENT_READ_COMPLETE` - number of bytes for reading has been reached, or reading has been terminated by user intervention
- `MCAL_EVENT_WRITE_COMPLETE` - number of bytes for writing has been reached
- `MCAL_EVENT_TRANSFER_COMPLETE` - transfer has been completed - number of bytes for reading and writing has been reached
- `MCAL_EVENT_ERROR` - error during reading has been detected

For more information about event handling and callback function attachment,
please visit [MCAL Callback System](mcal_callback.md) page.


## 2. Interface Functions
For detailed description of interface functions visit [mspi.h](../mspi.h)

### 2.1 MSPI_init
Main initialization function, that will prepare and configure peripheral unit.
If some settings needs to be reconfigured, it is possible to call init with
different configuration. If requested configuration is not achievable on given
microcontroller, initialization function will return error value.

### 2.2 MSPI_enable / MSPI_disable
Functions for enabling/disabling of peripheral unit. After initialization the
peripheral unit is by default enabled.

### 2.3 MSPI_enableEvent / MSPI_disableEvent
Functions for enabling/disabling specific events. When enabling event for
callback for the first time, the callback is also registered to list of
callbacks located in handle instance. Disabling of the event callback
keeps registered, but internal flag is cleared to not call callback in
case event occurs.

### 2.4 MSPI_read / MSPI_write / MSPI_transfer
Functions for buffered non-blocking read/write operations. Allocation of memory
for read and write buffers must be done in application and data buffers must be
allocated either in static memory in case of reading and writing or in ROM
memory for writing only. In case new read/write requested comes before previous
is finished, function will return `MCAL_ERROR_BUSY` and request is denied.

There are three types of communication:
- `MSPI_transfer` - This function starts simultaneous read and write operation.
Data from write buffer are sent over MOSI pin and received data from MISO pin
are stored in read buffer. If the length of read buffer is greater than length
of write buffer, dummy data are sent out in order to receive given number of
bytes defined by length of read buffer.
- `MSPI_write` - Same as `MSPI_transfer`, but with write only functionality.
- `MSPI_read` - Same as `MSPI_transfer`, but with read only functionality.

### 2.5 MSPI_readBlocking / MSPI_writeBlocking / MSPI_transferBlocking
Similar fuctions to MSPI_read / MSPI_write / MSPI_transfer, but handling of transfer
is handled in loop inside function, thus blocking other operations until transfer
is complete. If the transfer is not finished in given time period, error value
`MCAL_ERROR_TIMEOUT` is returned.

### 2.6 `MSPI_getStatus`
Function for querying actual status of peripheral.


## 3. Code Examples in C

### 3.1 Includes:
```c
#include "mcal_channels.h"
```

### 3.2 Initialization:
```c
static struct MSPI_Handle mspiHandle;

/* Setting the SPI unit according to the requirements of the application:
The communication frequency will be set to equal or less than it is possible to generate by hardware.*/
struct MSPI_Config mspiCfg =
{
    .dataBits      = MSPI_DATA_BITS_8,
    .clockPolarity = MSPI_CLOCK_POLARITY_1,
    .clockPhase    = MSPI_CLOCK_PHASE_1,
    .dataInvert    = MSPI_DATA_INVERT_DISABLED,
    .frameFormat   = MSPI_FRAME_FORMAT_MSB_FIRST,
    .chipSelMode   = MSPI_CHIP_SELECT_AUTO_LOW,
    .clockFreq     = 1000000u, // desired frequency in Hz
    .misoPullResistor = MDIO_PULL_UP
};

/* SPI unit and pin mapping */
static const struct MSPI_Channel mspiChannel =
{
    .mspi = &MSPI2,
    .sclk = &MDIOB10_MSPI2_SCK,
    .miso = &MDIOA3_MSPI2_MISO,
    .mosi = &MDIOB11_MSPI2_MOSI,
    .cs   = &MDIOB9
};

MSPI_init(&mspiHandle, &mspiChannel, &mspiCfg);
```


### 3.3 How to use `MSPI_transfer`
The function `MSPI_transfer` needs write and read buffers. These buffers must be static.
```c
static const uint8_t writeBuffer[] = {0, 1, 2, 3};
static       uint8_t readBuffer[4];
if(MCAL_OK != MSPI_transfer(&mspiHandle, writeBuffer, sizeof(writeBuffer), readBuffer, sizeof(readBuffer)))
{
    // ERROR - Transfer has not started
}
```

### 3.4 How to use `MSPI_read`
The function `MSPI_read` needs read buffer only. The buffer must be static.
```c
static uint8_t readBuffer[4];
if(MCAL_OK != MSPI_read(&mspiHandle, readBuffer, sizeof(readBuffer)))
{
    // ERROR - Transfer has not started
}
```

### 3.5 How to use `MSPI_write`
The function `MSPI_write` needs write buffer only. The buffer must be static or static const.
```c
static const uint8_t writeBuffer[] = {0, 1, 2, 3};
if(MCAL_OK != MSPI_write(&mspiHandle, writeBuffer, sizeof(writeBuffer)))
{
    // ERROR - Transfer has not started
}
```

### 3.6 Callback function attachment:
More details regarding [Callback System](./mcal_callback.md).
```c
/* Callback function for both, read and write complete events */
void myMspiCallbackFunction(void *obj, uint32_t flags, const struct MCAL_EventResponse *eventResponse)
{
    if((flags & (uint32_t)MCAL_EVENT_WRITE_COMPLETE) != 0U)
    {
        /* User code - This code will executed after write complete */
        if(true == startNewTransferFromCallback) {
            MSPI_endCommunication(&mspiHandle);   // must be called before new transfer
            if(MCAL_OK != MSPI_transfer(&mspiHandle, writeBuffer, sizeof(writeBuffer), readBuffer, sizeof(readBuffer)))
            {
                // ERROR - Transfer has not started
            }
        }
    }
    if((flags & (uint32_t)MCAL_EVENT_READ_COMPLETE) != 0U)
    {
        /* User code - This code will executed after read complete */
        if(true == startNewTransferFromCallback) {
            MSPI_endCommunication(&mspiHandle);   // must be called before new transfer
            if(MCAL_OK != MSPI_transfer(&mspiHandle, writeBuffer, sizeof(writeBuffer), readBuffer, sizeof(readBuffer)))
            {
                // ERROR - Transfer has not started
            }
        }
    }
    if((flags & (uint32_t)MCAL_EVENT_TRANSFER_COMPLETE) != 0U)
    {
        /* User code - This code will executed after transfer complete */
        if(true == startNewTransferFromCallback) {
            // MSPI_endCommunication() must not be called before new transfer in case of MCAL_EVENT_TRANSFER_COMPLETE event
            if(MCAL_OK != MSPI_transfer(&mspiHandle, writeBuffer, sizeof(writeBuffer), readBuffer, sizeof(readBuffer)))
            {
                // ERROR - Transfer has not started
            }
        }
    }
}

/* Callback instance definition */
static MCAL_Callback_t myMspiCallback;

/* Callback initialization */
MCAL_initCallback(&myMspiCallback, myMspiCallbackFunction, (void*)&mspiHandle);

/* Enabling of events and attaching them to callback */
MSPI_enableEvent(&mspiHandle, &myMspiCallback, MCAL_EVENT_READ_COMPLETE);
MSPI_enableEvent(&mspiHandle, &myMspiCallback, MCAL_EVENT_WRITE_COMPLETE);
MSPI_enableEvent(&mspiHandle, &myMspiCallback, MCAL_EVENT_TRANSFER_COMPLETE);
```


### 3.7 How to use MDMA with MSPI:
You can find an example of how to use MADC with MSPI in [mdma.md](mdma.md)

[Back to MCAL Homepage](../README.md)
