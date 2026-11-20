# MADC - Analog to Digital Converter

ADC is used to convert analog signal to digital value. One hardware unit of ADC
can convert only one analog sample at the same time and next sample can start after
previous conversion is finished. AD conversion consists of number of steps.
Firstly analog multiplexer is set to corresponding analog input channel. Then
analog conversion is started either by software or hardware trigger. After
conversion is finished, result is read from result register and optionally
interrupt is generated and appropriate callback function is executed.

## 1 Data Types

### 1.1 Handler (`struct MADC_Handle`)

MADC requires some RAM memory for its function to store information about
state of peripheral unit, ongoing read/write operations and for callback
functionality. All this information is stored in handle structure. Each
handle instance must be allocated in static memory by the user, as the handle
is registered for interrupt handling during initialization. Pointer to handle
instance is used as first parameter for all high level C interface functions.

### 1.2 Peripheral Configuration (`struct MADC_Config`)

- `trigger`
  - MADC_TRIGGER_SOFTWARE - (default)
  - MADC_TRIGGER_HARDWARE
- `resolution` - Hardware of ADC is always configured for the highest resolution,
  then result is upscaled/downscaled to required format. The result is always
  right-aligned.
  - MADC_RESOLUTION_8_BITS
  - MADC_RESOLUTION_10_BITS
  - MADC_RESOLUTION_12_BITS
  - MADC_RESOLUTION_14_BITS
  - MADC_RESOLUTION_16_BITS
- `conversionTimeout` - The time limit(specified in microseconds), after which
  a single channel conversion result is marked as invalid if the conversion
  has not finished.

### 1.3 Supported Events (`enum MCAL_Event`)

MADC provides possibility to attach user callback function to `MCAL_EVENT_CHANNEL`
event or to `MCAL_EVENT_END_OF_SEQUENCE` event for selected channel/s.

For more information about event handling and callback function attachment,
please visit [MCAL Callback System](mcal_callback.md) page.

## 2 Interface Functions

For detailed description of interface functions visit [madc.h](../madc.h)

### 2.1 `MADC_init`

Main initialization function that will prepare and configure peripheral unit.
If some settings need to be reconfigured, it is possible to call init with
different configuration. If requested configuration is not achievable on given
AD converter unit/channel of microcontroller, initialization function will return
error value.

### 2.2 `MADC_enable` / `MADC_disable`

Functions for enabling/disabling of peripheral unit. After initialization the
peripheral unit is enabled.

### 2.3 `MADC_enablePeriphEvent` / `MADC_disablePeriphEvent`

Functions for enabling/disabling `MCAL_EVENT_END_OF_CONVERSION` for AD converter. When enabling
event for callback for the first time, the callback is also registered to list of
callbacks located in handle instance. Disabling of the event keeps callback
registered but internal flag is cleared to not execute callback if the event occurs.

### 2.4 `MADC_enableChannelEvent` / `MADC_disableChannelEvent`

Functions for enabling/disabling `MCAL_EVENT_CHANNEL` for specific channel of AD
converter. For callback registering, the same principle applies here as for
peripheral events. Please note that the same callback instance cannot be used
for both the peripheral and channel events, as they are registered to different
callback lists.

### 2.5 `MADC_enableScanEvent` / `MADC_disableScanEvent`

Functions for enabling/disabling interrupt generation on `MCAL_EVENT_END_OF_SEQUENCE` for
AD converter. When enabling event for callback for the first time, the callback is also registered
to list of callbacks located in handle instance. Disabling of the event keeps callback
registered but internal flag is cleared to not execute callback if the event occurs.

### 2.6 `MADC_enablePeriphScanEvent` / `MADC_disablePeriphScanEvent`

Functions for enabling/disabling `MCAL_EVENT_END_OF_SEQUENCE` for specific AD
converter. For callback registering, the same principle applies here as for
channel events. Please note that the same callback instance cannot be used
for both the peripheral and channel events, as they are registered to different
callback lists.

### 2.7 `MADC_readSample`

Blocking function for performing one conversion over single channel. This function
is reentrant and can be called from interrupt service routine.

### 2.8 `MADC_isConversionResultValid`

Function for checking the latest single channel conversion result validity.

### 2.9 `MADC_readSampleWithValidation`

Blocking function for performing one conversion over a single channel. This function reads
the conversion result along with the result validity.

### 2.10 `MADC_startScanConversion`

Function for starting AD conversion.  Conversion result of :

- one channel can be returned by callback,
- multiple channels can be stored in predifined array ordered according to the predefined
  order of channels for scanning.
  If regular conversion is started during event conversion, event
  conversion is interrupted until regular conversion is finished.

## 3 Code Examples in C

### 3.1 Includes:
```c
#include "mcal_channels.h"
```

### 3.2 Initialization of AD converter:

```c
/* Initialization AD converter */
static struct MADC_Handle AnalogHandle;

const struct MADC_Config AnalogConfig = 
{
    MADC_TRIGGER_SOFTWARE,    // Software trigger selection
    MADC_RESOLUTION_12_BITS,  // Resolution width
    0U                        // Without a time limit for a single channel conversion
};

MADC_init(&AnalogHandle, &MADC1, &AnalogConfig);
```

### 3.3 Reading single analog value:

Precondition is to have AD converter initialized and appropriate pins configured
to `MDIO_MODE_ANALOG` mode.

```c
// Variable for storing of conversion result
uint16_t analogResult;

analogResult = MADC_readSample(&AnalogHandle, &MADC1_IN0_PA0);
```

### 3.4 Reading single analog value using callback:

Precondition is to have AD converter initialized and relevant pin set
to `MDIO_MODE_ANALOG` mode.

```c
// Channel used in ADC scan
const struct MADC_Channel *ADC_Scan_Channel[1u] = { &MADC1_IN0_PA0 };

void ConversionFinishedCallbackFunction(void *objPtr, uint32_t eventFlags, const struct MCAL_EventResponse *eventResponse)
{
    /* Get conversion result from event response */
    uint16_t analogResult = MCAL_getEventData_uint16(eventResponse);
}

/* Initialize callback */
static MCAL_Callback_t ConversionFinishedCallback;
MCAL_initCallback(&ConversionFinishedCallback, ConversionFinishedCallbackFunction, NULL);
MADC_enableChannelEvent(&AnalogHandle, ADC_Scan_Channel[0u], &ConversionFinishedCallback);

/* Start conversion of analog channel */
if(MADC_startScanConversion(&AnalogHandle, ADC_Scan_Channel, 1u, NULL) == MCAL_OK)
{
    /* conversion started */
}
```

### 3.5 Reading multiple analog values using callback:

Precondition is to have AD converter initialized and appropriate pins configured
to `MDIO_MODE_ANALOG` mode.

```c

// Channels used in ADC scan
const struct MADC_Channel *ADC_Scan_Channels[4u] = { &MADC1_IN0_PA0, &MADC1_IN1_PA1, &MADC1_IN2_PA2, &MADC1_IN3_PA3 };

// Variable for storing scan results
static uint16_t scan_Results[4u];

void ScanFinishedCallbackFunction(void *objPtr, uint32_t eventFlags, const struct MCAL_EventResponse *eventResponse)
{
    /* This callback is executed after all channels are converted. Results are stored in predefined array. */
}

/* Initialize callback */
static MCAL_Callback_t ScanFinishedCallback;
MCAL_initCallback(&ScanFinishedCallback, ScanFinishedCallbackFunction, NULL);
MADC_enablePeriphScanEvent(&AnalogHandle, &ScanFinishedCallback);

/* Start conversion of analog channel */
if(MADC_startScanConversion(&AnalogHandle, ADC_Scan_Channels, 4u, scan_Results) == MCAL_OK)
{
    /* conversion started */
}
```

### 3.6 How to use MDMA with MADC:

You can find an example of how to use MADC with MDMA in [mdma.md](mdma.md)

[Back to MCAL Homepage](../README.md)
