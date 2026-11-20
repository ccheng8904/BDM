# MDMA - Direct Memory Access

The DMA is used to perform programmable data transfers between memory-mapped
peripherals and/or memories, upon the control of an off-loaded CPU.

## 1 Data Types

###  1.1 Handler (`struct MDMA_Handle`)
MDMA requires some RAM memory for its function to store information about
state of peripheral unit, channel and for callback functionality. All this information is stored in handle structure. Each handle instance must be allocated in static memory by the user, as the handle is registered for interrupt handling during initialization. Pointer to handle instance is used as first parameter for all high level C interface functions.

### 1.2 Source/destination link (`MDMA_Link`)
This structure can represent the memory address or data register of MCU peripheral and this struct can be used as source or destination for DMA channel in MDMA_ChannelConfig. `MDMA_Link` consists of:

- `addr` - address of source/destination

- `dataWidth` - width of data at each transfer.
  - `MDMA_WIDTH_8_BIT`
  - `MDMA_WIDTH_16_BIT`
  - `MDMA_WIDTH_32_BIT`

- `increment` - `true`/`false` for enable/disable increment of address. If set to `true`, then after each transfer address will be incremented by 1, 2 or 4 bytes when `dataWidth` is `MDMA_WIDTH_8_BIT`, `MDMA_WIDTH_16_BIT` or `MDMA_WIDTH_32_BIT` respectively. If set to `false`, the address will not change after transfer.

### 1.3 MDMA channel configuration (`struct MDMA_ChannelConfig`)
This structure represent the main configuration settings of MDMA transfer, such as direction of transfer, transfer mode, number of transfers and both source and destination memory configurations.

```c
struct MDMA_ChannelConfig
{
    enum MDMA_Direction     direction;
    enum MDMA_Mode          mode;
    uint16_t                dataLen;
    
    struct MDMA_Link        src;
    struct MDMA_Link        dst;
};
```

- `direction` - direction of transfer.
  - `MDMA_DIRECTION_MEM_TO_MEM` - from memory to memory.
  - `MDMA_DIRECTION_MEM_TO_PERIPH` - from memory to peripheral.
  - `MDMA_DIRECTION_PERIPH_TO_MEM` - from peripheral to memory.
  - `MDMA_DIRECTION_PERIPH_TO_PERIPH` - from peripheral to peripheral.

- `mode` - transfer mode
  - `MDMA_MODE_SINGLE` - single shot DMA transfer.
  - `MDMA_MODE_CIRCULAR` - circular DMA transfer. After the last data transfer settings are automatically reloaded with the initially programmed value.

- `dataLen` - number of data of width `dataWidth` to be transfered.
- `src` - `MDMA_Link` structure, that describes source and consists of the address of the source buffer, the width of the data and whether the address will increment after each transfer or remain unchanged.
- `dst` - `MDMA_Link` structure, that describes destination and consists of the address of the destination buffer, the width of the data and whether the address will increment after each transfer or remain unchanged.

## 2 Interface Functions
For a detailed description of interface functions, visit [mdma.h](../mdma.h)

### 2.1 `MDMA_init`
Main initialization function, that will prepare and configure peripheral unit.

### 2.2 `MDMA_initChannel`
Initialization function, that will prepare and configure MDMA channel. If some settings needs to be reconfigured, it is possible to call init with different configuration. If requested configuration is not achievable, initialization function will return error value. For a detailed description of error values, visit [mcal_types.h](../mcal_types.h) and find `enum MCAL_Error`.

### 2.3 `MDMA_enable` / `MDMA_disable`
Functions for enabling/disabling of peripheral unit. After initialization the peripheral unit is enabled.

### 2.4 `MDMA_enableChannel` / `MDMA_disableChannel`
Functions for enabling/disabling channels.

### 2.5 `MDMA_enableChannelEvent` / `MDMA_disableChannelEvent`
Functions for enabling/disabling channel event.


## 3 Code Examples in C

### 3.1 Includes
```c
#include "mcal_channels.h"
```

### 3.2 Initialization
Example with circular memory to memory configuration:
```c
static uint16_t srcBuf[] = {0, 1, 2, 3, 4};
static uint16_t dstBuf[] = {5, 6, 7, 8, 9};

struct MDMA_ChannelConfig dmaCfg =
{
    MDMA_DIRECTION_MEM_TO_MEM,
    MDMA_MODE_CIRCULAR,
    5,
    /* source Link */
    {
        (uint32_t)srcBuf,
        MDMA_WIDTH_16_BIT,
        true
    },
    /* destination Link */
    {
        (uint32_t)dstBuf,
        MDMA_WIDTH_16_BIT,
        true
    }
};

struct MDMA_Periph dmaPeriph = {&MDMA1_Descriptor};

struct MDMA_Channel dmaChannel = 
{
    &MDMA1_CH1_Descriptor,
    {
        MDMA_PRIORITY_LOW,
        MDMA_EVENT_SOURCE_TIM1_CH1
    }
};

static struct MDMA_Handle dmaHandle;

MDMA_init(&dmaHandle, &dmaPeriph, &dmaChannel);
MDMA_initChannel(&dmaHandle, &dmaCfg);
```

### 3.3 Enabling of MDMA channel and starting transfer
```c
MDMA_enableChannel(&dmaHandle);
```

### 3.4 How to use MDMA with MADC
Initialization and enabling of MDMA:
```c
const struct MDMA_ChannelConfig dmaCfg =
{
    MDMA_DIRECTION_PERIPH_TO_MEM,
    MDMA_MODE_CIRCULAR,
    2,
    {
        (uint32_t)ADC_DR__ADDR,
        MDMA_WIDTH_16_BIT,
        false
    },
    {
        (uint32_t)dstBuf,
        MDMA_WIDTH_16_BIT,
        true
    }
};

struct MDMA_Periph dmaPeriph = {&MDMA1_Descriptor};

struct MDMA_Channel dmaChannel = 
{
    &MDMA1_CH1_Descriptor,
    {
        MDMA_PRIORITY_LOW,
        MDMA_EVENT_SOURCE_ADC
    }
};

static struct MDMA_Handle dmaHandle;

MDMA_init(&dmaHandle, &dmaPeriph, &dmaChannel);
MDMA_initChannel(&dmaHandle, &dmaCfg);

MDMA_enableChannel(&dmaHandle);
```

Initialization and enabling of Timer:
```c
const struct MTIM_TimerConfig timConfig = 
{
    640,
    1000,
    MTIM_TIMER_DIRECTION_UP,
    MTIM_TIMER_MODE_TIMER
};

const struct MTIM_ChannelConfig timChConfig = 
{
    MTIM_CHANNEL_MODE_COMPARE,
    MTIM_CHANNEL_CAPTURE_TRIGGER_DISABLED,
    MTIM_CHANNEL_COMPARE_ACTION_TOGGLE
};

struct MTIM_Handle timHandle;

MTIM_initTimer(&timHandle, &MTIM1, &timConfig);   
MTIM_initChannel(&timHandle, &MTIM1_CH4, &timChConfig);

MTIM_enable(&timHandle);
```

Initialization of MADC(with Timer 1 Channel 4 trigger):
```c
const struct MADC_Periph adcPeriph = 
{
    &MADC1_Descriptor,
    {
        MADC_SLOW_SAMPLE_TIME_DEFAULT,
        MADC_FAST_SAMPLE_TIME_DEFAULT,
        MADC_TRIGGER_SOURCE_TIM1_CC4,
        MADC_TRIGGER_POLARITY_RISING_FALLING
    }
};

const struct MADC_Config adcConfig = 
{
    MADC_TRIGGER_HARDWARE,    
    MADC_RESOLUTION_12_BITS 
};

struct MADC_Handle adcHandle;

const struct MADC_Channel *adcChannels[] =
{
    &MADC1_IN0_PA0,
    &MADC1_IN1_PA1
};

MADC_init(&adcHandle, &adcPeriph, &adcConfig);

MADC_enableDMA(&adcPeriph);
```

Start conversion:
```c
if (MADC_startScanConversion(&adcHandle, adcChannels, 2u, results) == MCAL_OK)
{
  /* conversion started */
}
```

### 3.5 How to use MDMA with MSPI
Initialization of MDMA:
```c
struct MDMA_Periph dmaPeriph = {&MDMA1_Descriptor};

struct MDMA_Channel dmaTxChannel = 
{
    &MDMA1_CH1_Descriptor,
    {
        MDMA_PRIORITY_LOW,
        MDMA_EVENT_SOURCE_SPI2_TX
    }
};

struct MDMA_Channel dmaRxChannel = 
{
    &MDMA1_CH2_Descriptor,
    {
        MDMA_PRIORITY_LOW,
        MDMA_EVENT_SOURCE_SPI2_RX
    }
};

static struct MDMA_Handle dmaTxHandle;
static struct MDMA_Handle dmaRxHandle;

MDMA_init(&dmaTxHandle, &dmaPeriph, &dmaTxChannel);
MDMA_init(&dmaRxHandle, &dmaPeriph, &dmaRxChannel);
```

Initialization of MSPI:
```c
static struct MSPI_Handle mspiHandle;
struct MSPI_Config mspiCfg = {
            MSPI_DATA_BITS_8,
            MSPI_CLOCK_POLARITY_1,
            MSPI_CLOCK_PHASE_1,
            MSPI_DATA_INVERT_DISABLED,
            MSPI_FRAME_FORMAT_MSB_FIRST,
            MSPI_CHIP_SELECT_AUTO_LOW,
            100000U, // -- this is required frequency in Hz which will to generate on SCLK pin
            MDIO_PULL_UP
};

const struct MDIO_Channel SPI_PIN_SCLK = MDIOB10_MSPI2_SCK;
const struct MDIO_Channel SPI_PIN_MISO = MDIOA3_MSPI2_MISO;
const struct MDIO_Channel SPI_PIN_MOSI = MDIOB11_MSPI2_MOSI;
const struct MDIO_Channel SPI_PIN_CS   = MDIOB9_MSPI2_NSS;

static const struct MSPI_Channel mspiChannel =
    { &MSPI2, &SPI_PIN_SCLK, &SPI_PIN_MISO, &SPI_PIN_MOSI, &SPI_PIN_CS };

MSPI_init(&mspiHandle, &mspiChannel, &mspiCfg);
MSPI_initDma(&mspiHandle, &dmaTxHandle, &dmaRxHandle);
```

Transfer:
```c
static const uint8_t writeBuffer[] = {0, 1};
if (MCAL_OK != MSPI_transferDma(&mspiHandle, writeBuffer, 2u, NULL, 0u))
{
    /* ERROR - Transfer has not started */
}
```
In case call of function MSPI_transferDma was successful and parameters for next transfer do not change it is possible to use
transfer continue function:
```c
if (MCAL_OK != MSPI_transferDmaContinue(&mspiHandle))
{
    /* ERROR - Transfer continue has not started */
}
```

### 3.6 Callback function attachment
```c
/* Callback function */
void mdmaCallbackFunction(void *obj, uint32_t flags, const struct MCAL_EventResponse *eventResponse)
{
    /* user code */
}

/* Callback instance definition */
static MCAL_Callback_t dmaCallback;

/* Callback initialization */
MCAL_initCallback(&dmaCallback, mdmaCallbackFunction, (void*)&dmaHandle);

/* Enabling of events and attaching them to callback */
MDMA_enableChannelEvent(&dmaHandle, &dmaCallback);
```