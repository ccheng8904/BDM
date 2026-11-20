# MTIM - Timer

MCAL Timer library is used to manage hardware timers of microcontroller.
Microcontrollers have multiple units of hardware timers that are often of
different type (standard timers, advanced timers, low power timers, ...).
MTIM covers features that are common for most MCU timers. 

Timer can have multiple channels that can operate in different modes, like
capture, compare or PWM. In capture mode, the actual count of timer is copied
to channel value register in case trigger occurs, usually in form of external
event (rising/falling edge on MCU pin). Compare mode is used to generate event
in case channel value equals to actual timer count. PWM mode is used to generate
PWM signal on output pin.


## 1 Data Types

###  1.1 Handler (`struct MTIM_Handle`)
MTIM requires some RAM memory for its function to store information about
state of peripheral unit, ongoing read/write operations and for callback
functionality. All this information is stored in handle structure. Each
handle instance must be allocated in static memory by the user, as the handle
is registered for interrupt handling during initialization. Pointer to handle
instance is used as first parameter for all high level C interface functions.

### 1.2 Timer Configuration (`struct MTIM_TimerConfig`)

- `prescaler` - integer value defining prescaler value of input signal.
  Prescaler value depends on hardware implementation of given timer unit and
  often is limited to be value in power of two. If not supported value is
  requested, error value is returned during initialization.
- `reloadValue` - integer value defining upper timer limit of timer count
  register
- `direction` - count direction. Some timers support only one counting
  direction that is given by hardware implementation of given timer unit.
  This parameter is not used in encoder mode.
  - `MTIM_TIMER_DIRECTION_UP`
  - `MTIM_TIMER_DIRECTION_DOWN`
  - `MTIM_TIMER_DIRECTION_UP_DOWN` - for center aligned PWM
- `mode` - parimode selection
  - `MTIM_TIMER_MODE_TIMER` - timer count is based on internal clock bus
  - `MTIM_TIMER_MODE_COUNTER` - timer is counting edges of external signal
  - `MTIM_TIMER_MODE_ENCODER` - source of the timer count are two signals with phase shift (not implemented yet)

### 1.3 Timer Configuration for PWM generation (`struct MTIM_TimerConfigPWM`)
- `frequency` - desired frequency of PWM signal in Hz
- `pwmMode` - PWM mode selection.
  - `MTIM_TIMER_PWM_MODE_NORMAL`
  - `MTIM_TIMER_PWM_MODE_CENTER_ALIGNED`

### 1.4 Channel Configuration (`struct MTIM_ChannelConfig`)

- `mode` - channel mode of operation
  - `MTIM_CHANNEL_MODE_CAPTURE` 
  - `MTIM_CHANNEL_MODE_COMPARE`
  - `MTIM_CHANNEL_MODE_PWM`
- `captureTrigger` - edge selection for capture mode
  - `MTIM_CHANNEL_TRIGGER_DISABLED`
  - `MTIM_CHANNEL_TRIGGER_RISING`
  - `MTIM_CHANNEL_TRIGGER_FALLING`
  - `MTIM_CHANNEL_TRIGGER_RISING_FALLING`
- `compareAction` - action selection for compare mode
  - `MTIM_CHANNEL_COMPARE_ACTION_NONE` - no action, just event generation
  - `MTIM_CHANNEL_COMPARE_ACTION_CLEAR`
  - `MTIM_CHANNEL_COMPARE_ACTION_SET`
  - `MTIM_CHANNEL_COMPARE_ACTION_TOGGLE`

There are predefined channel configuration structures that can be used directly
for channel initialization:
- `MTIM_CHANNEL_PWM`
- `MTIM_CHANNEL_COMPARE`
- `MTIM_CHANNEL_CAPTURE_RISING`
- `MTIM_CHANNEL_CAPTURE_FALLING`
- `MTIM_CHANNEL_CAPTURE_RISING_FALLING`

### 1.5 Supported Events (`enum MCAL_Event`)
MTIM provides possibility to attach user callback function to different events:
- `MCAL_EVENT_UPDATE` - event when timer overflows/underflows
- `MCAL_EVENT_CHANNEL` - channel event in case of capture/compare triggers

For more information about event handling and callback function attachment,
please visit [MCAL Callback System](mcal_callback.md) page.


## 2 Interface Functions
For detailed description of interface functions visit [mtim.h](../mtim.h)

### 2.1 `MTIM_initTimer` / `MTIM_initTimerPWM` / `MTIM_initChannel`
Main initialization function that will prepare and configure peripheral unit.
If some settings need to be reconfigured, it is possible to call init with
different configuration. If requested configuration is not achievable on given
timer unit/channel of microcontroller, initialization function will return
error value. Before any operation with channel (including initialization),
appropriate timer must be initialized.

### 2.2 `MTIM_enable` / `MTIM_disable`
Functions for enabling/disabling of peripheral unit. After initialization the
peripheral unit is by default disabled, before any read/write operation it
needs to be enabled.

### 2.3 `MTIM_enableTimerEvent` / `MTIM_disableTimerEvent`
Functions for enabling/disabling `MCAL_EVENT_UPDATE` for timer. When enabling
event for callback for the first time, the callback is also registered to list of
callbacks located in handle instance. Disabling of the event keeps callback
registered but internal flag is cleared to not execute callback if the event occurs.

### 2.4 `MTIM_enableChannelEvent` / `MTIM_disableChannelEvent`
Functions for enabling/disabling `MCAL_EVENT_CHANNEL` for specific channel of timer.
For callback registering, the same principle applies here as for timer events.
Please note, that the same callback instance cannot be used for both the timer
and channel events, as they are registered to different callback lists.

### 2.5 `MTIM_readTimerCount` / `MTIM_writeTimerCount`
Functions for reading/writing of timer counter. Please note, that writing of timer
counter might not be supported by all types of microcontrollers.

### 2.6 `MTIM_readChannelValue` / `MTIM_writeChannelValue`
Functions for reading/writing capture/compare value of timer channel.

### 2.7 `MTIM_getChannelPinState`
Function for reading logical state of channel pin. This function can be used only if
the timer channel is associated with physical pin of the microcontroller.

## 3 Code Examples in C

### 3.1 Includes:
```c
#include "mcal_channels.h"
```

### 3.2 Initialization of timer:
```c
/* Initialization timer */
static struct MTIM_Handle TimerHandle;

const struct MTIM_TimerConfig TimerConfig = 
{
    1u,                         // Prescaler set to 1
    1000u,                      // Reload value
    MTIM_TIMER_DIRECTION_UP,
    MTIM_TIMER_MODE_TIMER
};

MTIM_initTimer(&TimerHandle, &MTIM1, &TimerConfig);
```

### 3.3 Initialization of timer channels:
Precondition is to have timer initialized from example 3.1
```c
/* Timer channel mapping to physical timer unit, channel and pin (example mapping for STM32 MCUs) */
const struct MTIM_Channel *compareChannel   = &MTIM1_CH0;
const struct MTIM_Channel *captureChannel   = &MTIM1_CH1_PA1;
const struct MTIM_Channel *pwmChannel       = &MTIM1_CH2_PA2;

/* Configure Channel 0 as compare - does not need external pin */
MTIM_initChannel(&TimerHandle, compareChannel, &MTIM_CHANNEL_COMPARE);

/* Configure Channel 1 as capture on rising edge on pin PA1 */
MTIM_initChannel(&TimerHandle, captureChannel, &MTIM_CHANNEL_CAPTURE_RISING);

/* Configure Channel 2 as PWM on pin PA2 */
MTIM_initChannel(&TimerHandle, pwmChannel, &MTIM_CHANNEL_PWM);
```

### 3.4 Reading/writing of timer/channel values:
Precondition is to have timer and appropriate channel initialized and channels defined.
```c
/* Reading of actual timer count value */
uint32_t timerCount = MTIM_readTimerCount(&TimerHandle);

/* Reading of timer channel value */
uint32_t channelValue = MTIM_readChannelValue(&TimerHandle, captureChannel);

/* Write timer channel value to 500 out of 1000 = 50% duty cycle*/
MTIM_writeChannelValue(&TimerHandle, pwmChannel, 500u);
```

### 3.5 Using timer for PWM generation:
```c
/* Timer and channel definitions */
static struct MTIM_Handle TimerHandle;
const struct MTIM_Channel *pwmChannel = &MTIM1_CH2_PA2;
const struct MTIM_TimerConfigPWM TimerConfig = 
{
    1000u,                      // PWM frequency in Hz
    MTIM_TIMER_PWM_MODE_NORMAL, // PWM mode
};

/* Initializing timer for PWM generation */
MTIM_initTimerPWM(&TimerHandle, &MTIM1, &TimerConfig);

/* Configure channel as PWM output */
MTIM_initChannel(&TimerHandle, pwmChannel, &MTIM_CHANNEL_PWM);

/* Write channel duty cycle: 128 = 50 % */
MTIM_writeChannelPWM(&TimerHandle, pwmChannel, 128u);
```

### 3.6 Callback function attachment:
Precondition is to have timer and appropriate channel initialized.
```c
/* Callback function for update event of timer */
void TimerCallbackFunction(void *objPtr, uint32_t eventFlags, const struct MCAL_EventResponse *eventResponse)
{
    /* User code */
}

/* Callback function for channel event (capture/compare) */
void ChannelCallbackFunction(void *objPtr, uint32_t eventFlags, const struct MCAL_EventResponse *eventResponse)
{
    /* User code */
}

/* Callback instance definition */
static MCAL_Callback_t TimerCallback;
static MCAL_Callback_t ChannelCallback;

/* Callback initialization */
MCAL_initCallback(&TimerCallback, TimerCallbackFunction, NULL);
MCAL_initCallback(&ChannelCallback, ChannelCallbackFunction, NULL);

/* Enabling of events and attaching them to callback */
MTIM_enableTimerEvent(&TimerHandle, &TimerCallback);
MTIM_enableChannelEvent(&TimerHandle, &MTIM1_CH0, &ChannelCallback);
```

[Back to MCAL Homepage](../README.md)
