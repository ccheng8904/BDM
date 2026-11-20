# MCAL Callback System
MCAL supports callback function attachment for most of the modules. Callback
system that is used in MCAL has been derived from standard callback library
(std_lib/std_callback.h) that is also used in [System Timer](../../../ped_fw/timer/README.md) library.

Because most of callbacks are executed directly from interrupt routines,
they can not be used for executing of time consuming or blocking functions.
Therefore callback functions should be kept as fast as possible so that interrupts of
lower priority and scheduler tasks are not blocked for long period
of time, otherwise some time critical functions might not work properly (e.g.
DBus might be compromised). If some time consuming computation on event needs
to be performed, it is advised to execute it in scheduler handle task and use
callback function only to signalize that event was triggered.

## 1 Data Types

### 1.1 Callback Function (`MCAL_CallbackFunction_t`)

To attach callback function to some event, event function of following type is required: 

```
typedef void (*MCAL_CallbackFunction_t) (void *objPtr, uint32_t eventFlags, const struct MCAL_EventResponse *eventResponse);
```
- `objPtr` - optional void pointer that can be used to identify for which callback link the event is executed
- `eventFlags` - information about which event is pending (used for internal function of callback system)
- `eventResponse` - pointer to event response structure containing additional information about event

### 1.2 Callback (`MCAL_Callback_t`)

Callback instance serves as a link between peripheral handler and callback
function. It is possible to attach multiple callback functions to the same
event as well as to attach single function to multiple events. Each link
between callback function and MCAL event usually requires separate instance
of `MCAL_Callback_t`, except the case when link is used to connect the same
callback function to the same handle instance with different event trigger
(e.g. `MCAL_EVENT_READ_BYTE` and `MCAL_EVENT_READ_COMPLETE` for the same MUART
handle instance). On the other hand, separate callback instance is required if
the same function is used to receive events from peripheral and also from
peripheral channels (e.g. overflow of timer and capture/compare event from
timer channel(s)) even though handle instance is the same.

### 1.3 Event Response (`struct MCAL_EventResponse`)
In the case one callback function is used to handle multiple events from different
MCAL modules (e.g. for centralized data collection about events), it is possible
to determine source of event and event type based on event response parameter.
Additionally, event response might contain additional data based on event type,
e.g. received data or ADC conversion result.
- `enum MCAL_Module eventSource` - module from which the event is originating
- `enum MCAL_Event eventType` - identification of event
- `struct MCAL_EventData eventData` - some events may pass data to callback function via this structure

### 1.4 Event data (`struct MCAL_EventData`)
Event can pass data of different type to callback function based on type of the
event. Specific event will always return the same type of data, so it is not
necessary to always check data type when reading actual data (e.g. `MCAL_EVENT_READ_BYTE`
event will always return `MCAL_DATA_TYPE_UINT8`). In the case that single callback
function is used for multiple events of different kind, function `MCAL_getEventDataType`
can be used to identify type of incoming data. If the event does not pass any
data, `MCAL_DATA_TYPE_NONE` will be returned. To cast incoming data to correct
type, function `MCAL_getEventData_XXX` can be used, where `XXX` is type of returned data.


## 2 Interface Functions

### 2.1 `MCAL_initCallback`
Function for initialization of callback instance. This function is 
- `callback` - pointer to callback instance to initialize
- `callbackFunction` - pointer to callback function
- `objPtr` - optional void pointer passed to callback function as first parameter

### 2.2 `MXXX_enableEvent` / `MXXX_disableEvent`
Functions for enabling/disabling of peripheral event. In the case peripheral supports
multiple events, last parameter is event type identificator.

### 2.2 `MXXX_enableChannelEvent` / `MXXX_disableChannelEvent`
Functions for enabling/disabling of peripheral channel event. Each channel can
have only one event source.


## 3 Code Examples
In following code examples dummy MCAL module MXXX is used to show general
principle of callback function attachment applicable for all MCAL modules.
For the purpose of simplification of the examples, initialization and
channel mapping of MXXX_Handle instances is not shown.

### 3.1 Includes:
```c
#include "mcal_channels.h"
```

### 3.2 Example of attaching single callback function to single event:
```c
void CallbackFunction(void *objPtr, uint32_t eventFlags, const struct MCAL_EventResponse *eventResponse)
{
    /* User code */
}

static struct MXXX_Handle MxxxHandle;

static MCAL_Callback_t Callback;

MCAL_initCallback(&Callback, CallbackFunction, NULL);

MXXX_enableEvent(&MxxxHandle, &Callback, MCAL_EVENT_TRIGGER);
```


### 3.3 Example of attaching single callback function to multiple events:
```c
void CallbackFunction(void *objPtr, uint32_t eventFlags, const struct MCAL_EventResponse *eventResponse)
{
    /* User code */
}

static struct MXXX_Handle MxxxHandle;

static MCAL_Callback_t Callback;

MCAL_initCallback(&Callback, CallbackFunction, NULL);

MXXX_enableEvent(&MxxxHandle, &Callback, MCAL_EVENT_TRIGGER);
MXXX_enableEvent(&MxxxHandle, &Callback, MCAL_EVENT_READ_COMPLETE);
```


### 3.4 Example of attaching multiple callback functions to single event:
```c
void CallbackFunction1(void *objPtr, uint32_t eventFlags, const struct MCAL_EventResponse *eventResponse)
{
    /* User code */
}

void CallbackFunction2(void *objPtr, uint32_t eventFlags, const struct MCAL_EventResponse *eventResponse)
{
    /* User code */
}

static struct MXXX_Handle MxxxHandle;

static MCAL_Callback_t Callback1;
static MCAL_Callback_t Callback2;

MCAL_initCallback(&Callback1, CallbackFunction1, NULL);
MCAL_initCallback(&Callback2, CallbackFunction2, NULL);

MXXX_enableEvent(&MxxxHandle, &Callback1, MCAL_EVENT_TRIGGER);
MXXX_enableEvent(&MxxxHandle, &Callback2, MCAL_EVENT_TRIGGER);
```


### 3.5 Example of attaching multiple handlers to the same callback function using object pointer as identification of handle instance:
```c
void CallbackFunction(void *objPtr, uint32_t eventFlags, const struct MCAL_EventResponse *eventResponse)
{
    /* Convert object pointer to handle */
    struct MXXX_Handle *handle = MXXX_getHandle(objPtr);
    
    if(handle != NULL)
    {
        /* User code */
    }
}

static struct MXXX_Handle MxxxHandle1;
static struct MXXX_Handle MxxxHandle2;

static MCAL_Callback_t Callback1;
static MCAL_Callback_t Callback2;

MCAL_initCallback(&Callback1, CallbackFunction, (void*)&MxxxHandle1);
MCAL_initCallback(&Callback2, CallbackFunction, (void*)&MxxxHandle2);

MXXX_enableEvent(&MxxxHandle1, &Callback1, MCAL_EVENT_TRIGGER);
MXXX_enableEvent(&MxxxHandle2, &Callback2, MCAL_EVENT_TRIGGER);
```

[Back to MCAL Homepage](../README.md)
