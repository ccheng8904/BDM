# Software timers
Framework contains two types of software timers:
 * TimerLib (legacy)
 * System Timer library

## TimerLib
This library handles the SW timers and the circulating timers.

This library decrements the memory content of special timer
segments. Several timer segments with different time bases may be defined.
To use SW-Timer an 8 or 16-Bit timer variable just has to be defined in
the corresponding memory segment.

For short time measurement additionally a circulating timer is supported.
Time differences with accuracy of 1µs can be calculated.

By default, the library uses 16-bit values for circulating timers, even if the timer supports 32-bit counting.
Users can use the macro `USE_32BIT_HWTIMER` to set the 32-bit HW circulating timer to the full 32-bit range. After that, users can use the `static inline uint32_t TIM_u32GetCircleMicroSeconds(void)` function to get the 32-bit counter value.
Also, the macro function `TIM_getDirectTimer32BitMicroseconds()` can be used to get the direct register value of the 32-bit timer in cases when using the standard function `TIM_u32GetCircleMicroSeconds` is not suitable. The static inline and macro functions are defined in the new platform/specific configuration header file timerMcXs.h that users must include to use them.

### Example of using TimerLib timer segments:
```c
#include "timer/timerlib.h"
#include "segmentdef.h"

// Create timer variable and place it in appropriate segment
SDEF_SetSegmentRW(TIMER8_10MS)
uint8_t timer_10ms
SDEF_SetSegmentRW_Default()

// Check if timer is done
if(TIM_ucIsTimerDown(timer_10ms))
{
    // Reload timer 1 s
    TIM_vLoadTimer(timer_10ms, 100u);
    ...
}

```

### Example of using TimerLib circulating timer:
```c
#include "timer/timerlib.h"

uint16_t timer;

// Load 1000 us (1 ms) into timer variable
TIM_LOAD_CIRCLE_TIMER_16(timer,1000)
...

// Check if time has been reached
if(TIM_IS_CIRCLE_TIMER_16_DOWN(timer))
{
    ...
}

```

## System Timer library
System timer library serves for single or periodic time
measurement and callback execution with resolution down to one
millisecond. System timer library contains two types of execution
handlers, interrupt handler and scheduler handler.

Scheduler handler is used for processing of less accurate timers.
Accuracy of these timers is given by time period of execution of
this handler. If accuracy of these timers is not sufficient, then
interrupt based timers (see next section) should be used instead.

Interrupt handler is called from hardware timer interrupt service
routine with period of integer value in milliseconds. This
handler is used for base time update and for processing of
accurate interrupt base timers (including callback execution).
For all ARM Cortex-M microcontrollers, SysTick timer is used by
default for this purpose.

Interrupt based timers should only be used if really necessary,
and respective callback functions should be kept as light as
possible. Otherwise they can have impact on other real-time
functions (UART/DBus handling, RTOS scheduling, ...)


### Example of using system timer for precise single shot time measurement:
```c
#include "timer/system_timer.h"

// Static definition of timer instance
static struct STIM_Timer TimerInstance;

// Initialization and start of timer (500 ms)
STIM_InitTimer(&TimerInstance, STIM_PROCESSING_INTERRUPT, 500u,
    STIM_MODE_SINGLE, true, NULL);

// Waiting until timer is down
while(!STIM_IsTimerDown(&TimerInstance)) {}
```


### Example of using system timer for continuous periodic callback function execution:
```c
#include "timer/system_timer.h"

// Callback function definition
int32_t TimerCallbackFunction(void *obj, uint32_t flags, int32_t data)
{
    // User code ...
    return 0;
}

// Static definitions of timer and callback instances (1 s)
static struct STIM_Timer TimerInstance;
static struct STDCB_Callback CallbackInstance;

// Initialization of callback and timer instances
STIM_InitCallback(&CallbackInstance, TimerCallbackFunction, NULL,
    STIM_STATUS_TRIGGERED);
STIM_InitTimer(&TimerInstance, STIM_PROCESSING_SCHEDULER, 1000u,
    STIM_MODE_PERIODIC, true, &CallbackInstance);
```


### Example of using system timer as stopwatch:
```c
#include "timer/system_timer.h"

// Static definition of stopwatch instance
static struct STIM_Timer StopwatchInstance;

// Initialization and start of stopwatch
STIM_InitStopwatch(&StopwatchInstance, STIM_PROCESSING_INTERRUPT, true);

// User code ...

// Read measured time
STIM_Time_t measuredTime_ms = STIM_StopStopwatch(&StopwatchInstance);
```
