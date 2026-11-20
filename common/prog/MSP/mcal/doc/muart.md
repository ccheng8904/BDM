# MUART - Universal Asynchronous Receiver-Transmitter

UART is the most common type communication peripheral in microcontroller
applications. Usually it is used as communication interface between two
microcontrollers, between microcontroller and PC or in specific hardware
communication it can be used as communication interface between multiple
nodes (e.g. in case of DBus). UART can be used for transferring binary data,
or ASCII text data for the purpose of debugging or logging. Text data can be
easily viewed on PC by using serial terminal like e.g. PuTTY, so it can be used
as quick debugging interface.


## 1 Data Types

###  1.1 Handler (`struct MUART_Handle`)
MUART requires some RAM memory for its function to store information about
state of peripheral unit, ongoing read/write operations and for callback
functionality. All this information is stored in handle structure. Each
handle instance must be allocated in static memory by the user, as the handle
is registered for interrupt handling during initialization. Pointer to handle
instance is used as first parameter for all high level C interface functions.

### 1.2 Configuration (`struct MUART_Config`)
MUART can be configured using configuration structure that consists of:
- `dataBits` - number of data bits, not including parity and stop bit(s)
  - `MUART_DATA_BITS_7`
  - `MUART_DATA_BITS_8` (default)
  - `MUART_DATA_BITS_9`
- `parity` - parity selection
  - `MUART_PARITY_NONE` (default)
  - `MUART_PARITY_ODD`
  - `MUART_PARITY_EVEN`
- `stopBits` - number of stop bits (_5 at the end means half of bit)
  - `MUART_STOP_BITS_0_5`
  - `MUART_STOP_BITS_1` (default)
  - `MUART_STOP_BITS_1_5`
  - `MUART_STOP_BITS_2`
- `baudrate` - communication speed in bits per second

### 1.3 Channel (`struct MUART_Channel`)
Channel structure combines information about UART unit, RX and TX pins and
static microcontroller specific configuration. Channel structure is used
during initialization to map handle to configuration given ECU hardware.

Usually each UART unit can have several different combinations of RX and TX
pins. For usual combinations of pin mapping together with default static
configuration there are prepared channels in mcal_channels.c/h files, that can be
used directly from application source code. To select right channel and pin
mapping, please use following convention: `MUART[unit]_RX_[rxPin]_TX_[txPin]`,
where:
- `unit` - number of peripheral unit, e.g. 1 for U(S)ART1
- `rxPin` - port and pin number, e.g. PA10
- `txPin` - port and pin number, e.g. PA9

If selected microcontroller has multiple types of UART peripheral, e.g.
standard and low power, low power units will have "LP" before unit number
e.g. `MUARTLP1_RX_PA3_TX_A2`. In case channel for specific configuration does
not exists but is supported by microcontroller hardware, it is possible to
create custom MUART channel inside application source file.

### 1.4 Supported Events (`enum MCAL_Event`)
MUART provides possibility to attach user callback function to different events:
- `MCAL_EVENT_READ_BYTE` - single byte was received
- `MCAL_EVENT_WRITE_BYTE` - single byte was moved to be transmitted
- `MCAL_EVENT_READ_COMPLETE` - number of bytes for reading has been reached, or reading has been terminated by user intervention
- `MCAL_EVENT_WRITE_COMPLETE` - number of bytes for writing has been reached
- `MCAL_EVENT_ERROR` - error during reading has been detected (parity error, framing error, ...)

For more information about event handling and callback function attachment,
please visit [MCAL Callback System](mcal_callback.md) page.


## 2 Interface Functions
For detailed description of interface functions visit [muart.h](../muart.h)

### 2.1 `MAURT_Init`
Main initialization function, that will prepare and configure peripheral unit.
If some settings needs to be reconfigured, it is possible to call init with
different configuration. If requested configuration is not achievable on given
microcontroller, initialization function will return error value.

### 2.2 `MUART_enable` / `MUART_disable`
Functions for enabling/disabling of peripheral unit. After initialization the
peripheral unit is by default enabled.

### 2.3 `MUART_enableEvent` / `MUART_disableEvent`
Functions for enabling/disabling specific events. When enabling event for
callback for the first time, the callback is also registered to list of
callbacks located in handle instance. Disabling of the event callback
keeps registered, but internal flag is cleared to not call callback in
case event occurs.

### 2.4 `MUART_writeByte` / `MUART_readByte`
Functions for reading/writing of single byte.

### 2.5 `MUART_read` / `MUART_write` / `MUART_writeStr`
Functions for buffered non-blocking read/write operations. Allocation of memory
for read and write buffers must be done in application and data buffers must be
allocated either in static memory in case of reading and writing or in ROM
memory for writing only. In case new read/write requested comes before previous
is finished, function will return `MCAL_ERROR_BUSY` and request is denied.

### 2.6 `MUART_getStatus` / `MUART_getReadStatus` / `MUART_getWriteStatus`
Functions for querying of actual status of peripheral. As UART uses asynchronous
communication, read and write operations are independent of each other, thus
busy status of communication in one direction does not block communication in
other direction. Because of this, MUART has separate interface to get status
of read operation (MUART_getReadStatus), write operation (MUART_getWriteStatus)
and overall status of both read and write operations (MUART_getStatus).


## 3 Code Examples in C

### 3.1 Includes:
```c
#include "mcal_channels.h"
```

### 3.2 Initialization:
```c
/* Initialization and sending of string in C */
static struct MUART_Handle MuartHandle;

const struct MUART_Config MuartHandleConfig = 
{
    MUART_DATA_BITS_8,
    MUART_PARITY_NONE,
    MUART_STOP_BITS_1,
    9600u
};

MUART_init(&MuartHandle, &MUART1_RX_PA10_TX_PA9, &MuartHandleConfig);
MUART_enable(&MuartHandle);
MUART_writeStr(&MuartHandle, "Starting of application");
```


### 3.3 Callback function attachment:
```c
/* Callback function for both, read and write complete events */
void MuartHandleCallbackFunction(void *objPtr, uint32_t eventFlags, const struct MCAL_EventResponse *eventResponse)
{
    /* User code */
}

/* Callback instance definition */
static MCAL_Callback_t MuartHandleCallback;

/* Callback initialization */
MCAL_initCallback(&MuartHandleCallback, MuartHandleCallbackFunction, NULL);

/* Enabling of events and attaching them to callback */
MUART_enableEvent(&MuartHandle, &MuartHandleCallback, MCAL_EVENT_READ_COMPLETE);
MUART_enableEvent(&MuartHandle, &MuartHandleCallback, MCAL_EVENT_WRITE_COMPLETE);
```


### 3.4 Non-blocking read with checking of termination character using separate callbacks:
```c
/* This function will be called when any data is received */
void MuartHandleReadByteCallbackFunction(void *objPtr, uint32_t eventFlags, const struct MCAL_EventResponse *eventResponse)
{
    uint8_t readData = MCAL_getEventData_uint8(eventResponse);
    
    if(readData == '\n')
    {
        MUART_finishRead(&MuartHandle);
    }
}

/* This function will be called when termination character is found or read count is reached  */
void MuartHandleReadCompleteCallbackFunction(void *objPtr, uint32_t eventFlags, const struct MCAL_EventResponse *eventResponse)
{
    /* User code */
}

/* Callback instance definitions */
static MCAL_Callback_t MuartHandleReadByteCallback;
static MCAL_Callback_t MuartHandleReadCompleteCallback;

/* Callback initialization */
MCAL_initCallback(&MuartHandleReadByteCallback, MuartHandleReadByteCallbackFunction, NULL);
MCAL_initCallback(&MuartHandleReadCompleteCallback, MuartHandleReadCompleteCallbackFunction, NULL);

/* Enabling of events and attaching them to callbacks */
MUART_enableEvent(&MuartHandle, &MuartHandleReadByteCallback, MCAL_EVENT_READ_BYTE);
MUART_enableEvent(&MuartHandle, &MuartHandleReadCompleteCallback, MCAL_EVENT_READ_COMPLETE);

/* Start reading data to readBuffer with maximum count of 256 bytes */
static uint8_t readBuffer[256u];
MUART_read(&MuartHandle, readBuffer, 256u);
```


## 4. Code Examples in C++

### 4.1 Includes:
```c++
#include "mcal_channels.h"
```

### 4.2 Initialization:
```c++
/* Initialization and sending of string in C++ */
static MUART::Channel MuartHandle(&MUART1_RX_PA10_TX_PA9);

const struct MUART_Config MuartHandleConfig = 
{
    MUART_DATA_BITS_8,
    MUART_PARITY_NONE,
    MUART_STOP_BITS_1,
    19200u
};

MuartHandle.init(&MuartHandleConfig);
MuartHandle.enable();
MuartHandle.write("Starting of application\r\n");
```

[Back to MCAL Homepage](../README.md)
