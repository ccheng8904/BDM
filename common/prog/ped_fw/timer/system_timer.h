/*******************************************************************************
 *  Copyright (c) 2017 BSH Hausgeraete GmbH,
 *  Carl-Wery-Str. 34, 81739 Munich, Germany, www.bsh-group.de
 *
 *  All rights reserved. This program and the accompanying materials
 *  are protected by international copyright laws.
 *  Please contact copyright holder for licensing information.
 *
 *******************************************************************************
 *  PROJECT          System Timer, Generic SW
 *  COMP_ABBREV      STIM
 ******************************************************************************/

#ifndef SYSTEM_TIMER_H
#define SYSTEM_TIMER_H

#ifdef __cplusplus
 extern "C" {
#endif

/******************************************************************************/
/* DOCUMENTATION                                                              */
/******************************************************************************/
/*lint -save -e9059 "C comment contains C++ comment"
 * Codeblock with comments for doxygen needed. Tested with various parsers to work correctly
 */
 
/** \file
 *
 *  \brief    Public definitions and declarations for unit STIM.
 *
 *  \details  System timer library serves for single or periodic time
 *            measurement and callback execution with resolution down to one
 *            millisecond. System timer library contains two types of execution
 *            handlers, interrupt handler and scheduler handler.
 *
 *            Scheduler handler is used for processing of less accurate timers.
 *            Accuracy of these timers is given by time period of execution of
 *            this handler. If accuracy of these timers is not sufficient, then
 *            interrupt based timers (see next section) should be used instead.
 *
 *            Interrupt handler is called from hardware timer interrupt service
 *            routine with period of integer value in milliseconds. This
 *            handler is used for base time update and for processing of
 *            accurate interrupt base timers (including callback execution).
 *            For all ARM Cortex-M microcontrollers, SysTick timer is used by
 *            default for this purpose.
 *
 *            Interrupt based timers should only be used if really necessary,
 *            and respective callback functions should be kept as light as
 *            possible. Otherwise they can have impact on other real-time
 *            functions (UART/DBus handling, RTOS scheduling, ...)
 *
 *  Example of using system timer for precise single shot time measurement
 *  \code
 *  // Static definition of timer instance
 *  static struct STIM_Timer TimerInstance;
 *
 *  // Initialization and start of timer (500 ms)
 *  STIM_InitTimer(&TimerInstance, STIM_PROCESSING_INTERRUPT, 500u,
 *      STIM_MODE_SINGLE, true, NULL);
 *
 *  // Waiting until timer is down
 *  while(!STIM_IsTimerDown(&TimerInstance)) {}
 *  \endcode
 *
 *  Example of using system timer for continuous periodic callback function execution
 *  \code
 *  // Callback function definition
 *  int32_t TimerCallbackFunction(void *obj, uint32_t flags, int32_t data)
 *  {
 *      // User code ...
 *      return 0;
 *  }
 *
 *  // Static definitions of timer and callback instances (1 s)
 *  static struct STIM_Timer TimerInstance;
 *  static struct STDCB_Callback CallbackInstance;
 *
 *  // Initialization of callback and timer instances
 *  STIM_InitCallback(&CallbackInstance, TimerCallbackFunction, NULL,
 *      STIM_STATUS_TRIGGERED);
 *  STIM_InitTimer(&TimerInstance, STIM_PROCESSING_SCHEDULER, 1000u,
 *      STIM_MODE_PERIODIC, true, &CallbackInstance);
 *  \endcode
 *
 *  Example of using system timer as stopwatch:
 *  \code
 *  // Static definition of stopwatch instance
 *  static struct STIM_Timer StopwatchInstance;
 *
 *  // Initialization and start of stopwatch
 *  STIM_InitStopwatch(&StopwatchInstance, STIM_PROCESSING_INTERRUPT, true);
 *
 *  // User code ...
 *
 *  // Read measured time
 *  STIM_Time_t measuredTime_ms = STIM_StopStopwatch(&StopwatchInstance);
 *  \endcode
 */

/*lint -restore -e9059 */


/******************************************************************************/
/* INCLUDES                                                                   */
/******************************************************************************/
#include "bsh_stdinc.h"
#include <stdint.h>
#include <stdbool.h>
#include "std_lib/std_callback.h"


/******************************************************************************/
/* PUBLIC DEFINITIONS                                                         */
/******************************************************************************/

/** \brief    Timer status masks definitions
 *
 *  \details  These definitions are used when checking status of timer and for
 *            enabling callback events. Status missed is active, when timer
 *            handling misses one or more trigger events. This usually means,
 *            that target time is too short for given execution period of timer
 *            scheduler handle function.
 */
#define STIM_STATUS_TRIGGERED   0x01U   ///< Timer actual time has reached target time
#define STIM_STATUS_MISSED      0x02U   ///< Timer trigger event has occurred multiple times without handling
#define STIM_STATUS_OVERFLOW    0x04U   ///< Timer actual time has overflown its type - this should never occur
/* All status masks combined */
#define STIM_STATUS_ALL         (STIM_STATUS_TRIGGERED  | \
                                 STIM_STATUS_MISSED     | \
                                 STIM_STATUS_OVERFLOW)


/** \brief    Timer base definitions
 *  \details  This definitions are used in target time calculations.
 */
#define STIM_TIME_BASE_MS       1U                          ///< millisecond time base
#define STIM_TIME_BASE_SEC      1000U                       ///< second time base
#define STIM_TIME_BASE_MIN      (STIM_TIME_BASE_SEC * 60U)  ///< minute time base
#define STIM_TIME_BASE_HOUR     (STIM_TIME_BASE_MIN * 60U)  ///< hour time base
#define STIM_TIME_BASE_DAY      (STIM_TIME_BASE_HOUR * 24U) ///< day time base

/*lint -save -e9026  "function like macro defined" */
/** \brief    Definitions used to calculate target time.
 */
#define STIM_TIME_MS(time)      (STIM_Time_t)(time)
#define STIM_TIME_SEC(time)     (STIM_Time_t)((time) * STIM_TIME_BASE_SEC)
#define STIM_TIME_MIN(time)     (STIM_Time_t)((time) * STIM_TIME_BASE_MIN)
#define STIM_TIME_HOUR(time)    (STIM_Time_t)((time) * STIM_TIME_BASE_HOUR)
#define STIM_TIME_DAY(time)     (STIM_Time_t)((time) * STIM_TIME_BASE_DAY)
/*lint -restore -e9026 */

/******************************************************************************/
/* PUBLIC TYPE DEFINITIONS                                                    */
/******************************************************************************/

/** \brief    System Timer time type.
 */
typedef uint32_t STIM_Time_t;   ///< Maximum measured time is 2^32 s (~49.71 days)


/** \brief    Timer repetition and protection mode.
 *
 *  \details  This enumeration type is used configure repetition and protection
 *            of single timer instance. In case of single shot timer, the timer
 *            is disabled when the target time is reached. When the timer is in
 *            periodic mode, the actual time is decremented by target time
 *            after reaching target time is reached and timer is kept enabled.
 *            TODO - Timer protection modes not implemented
 */
enum STIM_Mode
{
    STIM_MODE_SINGLE                ///< Single shot timer
    ,STIM_MODE_PERIODIC             ///< Periodic timer without protection (timer reset on missed trigger)
//  ,STIM_MODE_PERIODIC_PROTECTED   ///< Periodic timer with protection (timer is disabled and error is generated on missed trigger)
//  ,STIM_MODE_PERIODIC_CRITICAL    ///< Periodic timer with protection (microcontroller reset or endless loop on missed trigger)
};


/** \brief    Timer processing type.
 *
 *  \details  This enumeration type is used configure processing type of timer
 *            instance. When interrupt processing is selected, timer update
 *            and callback execution is done in background using hardware timer
 *            interrupt service routine. When scheduler processing is selected,
 *            timer update and callback execution is done in foreground using
 *            scheduler task.
 */
enum STIM_Processing
{
    STIM_PROCESSING_INTERRUPT,  ///< Process timer instance in interrupt handler
    STIM_PROCESSING_SCHEDULER   ///< Process timer instance in scheduler handle task
};

/*lint -esym(9045,*STIM_Timer)  "non-hidden definition of type struct STIM_Timer" */
/** \brief    Base timer structure.
 *
 *  \details  This structure is used for all system timer instances. System timer
 *            instance has to be placed in static memory, as it is added to
 *            linked list during initialization.
 */
struct STIM_Timer
{
    volatile STIM_Time_t    currentTime_ms; ///< Actual time
    STIM_Time_t             targetTime_ms;  ///< Target time
    
    volatile uint8_t        status;         ///< Timer status
    volatile bool           enabled;        ///< Timer enable flag
    enum STIM_Mode          mode;           ///< Timer mode selection

    struct STDCB_Callback   *cbList;        ///< Timer callback list
    struct STIM_Timer       *nextTimer;     ///< Next timer instance in linked list
};


/******************************************************************************/
/* PUBLIC FUNCTION DECLARATIONS                                               */
/******************************************************************************/

/** \brief    Function for initializing SystemTimer module.
 */
extern int32_t STIM_InitModule(void);


/** \brief    Function for reading of circular time variable of system timer.
 *
 *  \retval   actual time in ms
 */
extern uint32_t STIM_GetTick(void);


/** \brief    Function for initializing callback structure.
 *
 *  \details  Callback structures can be initialized directly by static
 *            initialization or using this initialization function.
 *
 *  \param    callback:     pointer to callback instance
 *  \param    functionPtr:  pointer to callback function
 *  \param    objectPtr:    pointer to user object (NULL if not used)
 *  \param    statusMask:   status mask for enabling callbacks on certain events
 *
 *  \retval   returns 0 if success, otherwise -1
 */
static inline int32_t STIM_InitCallback(struct STDCB_Callback *callback, STDCB_CallbackFunction_t functionPtr, void *objectPtr, uint8_t statusMask)
{
    return STDCB_InitCallback(callback, functionPtr, objectPtr, (uint32_t) statusMask);
}


/** \brief    Function for attaching callback structure to timer instance.
 *
 *  \details  One timer can have multiple callbacks attached, for example one
 *            callback for trigger event and one for event error handling. One
 *            callback instance can be assigned only to one timer instance.
 *            This function is protected from attaching already attached callback
 *            instance. Be careful when attaching callback instance, as it has
 *            to be defined in static memory.
 *
 *  \param    timer:        pointer to timer instance
 *  \param    callback:     pointer to callback instance
 *
 *  \retval   returns 0 if success, -1 if callback is already attached
 */
static inline int32_t STIM_AttachCallback(struct STIM_Timer *timer, struct STDCB_Callback *callback)
{
    return STDCB_AttachCallback(&timer->cbList, callback);
}


/** \brief    System timer interrupt handle task.
 *
 *  \details  This function increments system time and handle processing
 *            of all timers assigned to processing by interrupts.
 *
 *  \param    interruptPeriod_ms: period of interrupt timer in milliseconds.
 */
extern void STIM_InterruptHandler(uint16_t interruptPeriod_ms);


/** \brief    System timer scheduler handle task.
 *
 *  \details  This function handles handles processing of all timers assigned 
 *            to processing by scheduler task.
 *
 *  \retval   required by scheduler and always true
 */
extern void STIM_SchedulerHandleTask(void);


/** \brief    Timer initialization function.
 *
 *  \details  This function initializes timer instance, appends it to appropriate
 *            timer linked list, sets target time and list
 *
 *  \param    timer:          pointer to timer instance
 *  \param    processingType: interrupt or scheduler processing type of timer instance
 *                STIM_PROCESSING_INTERRUPT - timer instance will processed in interrupt handler
 *                STIM_PROCESSING_SCHEDULER - timer instance will processed in scheduler handle task
 *  \param    targetTime_ms:  target time in milliseconds
 *  \param    timerMode:      mode of timer - periodicity and protection
 *                STIM_MODE_SINGLE - timer will be stopped after reaching target time
 *                STIM_MODE_PERIODIC - timer will be periodically triggered
 *  \param    immediateStart: if true, timer will be started during initialization
 *  \param    callback:       pointer to callback instance, use NULL callback is not used
 *
 *  \retval   returns 0 if success, otherwise -1
 */
extern int32_t STIM_InitTimer(struct STIM_Timer *timer, enum STIM_Processing processingType,
    STIM_Time_t targetTime_ms, enum STIM_Mode timerMode, bool immediateStart, struct STDCB_Callback *callback);


/** \brief    Reads time left of timer instance.
 *
 *  \param    timer: pointer to timer instance
 *
 *  \retval   time left in milliseconds
 */
static inline STIM_Time_t STIM_ReadTimer(const struct STIM_Timer *timer)
{
    return timer->targetTime_ms - timer->currentTime_ms;
}


/** \brief    Reads timer status flags.
 *
 *  \param    timer: pointer to timer instance
 *
 *  \retval   timer status flags
 */
static inline uint8_t STIM_GetStatus(const struct STIM_Timer *timer)
{
    return timer->status;
}

/** \brief    Clears timer status flags.
 *
 *  \param    timer: pointer to timer instance
 */
static inline void STIM_ClearStatus(struct STIM_Timer *timer)
{
    timer->status = 0U;
}


/** \brief    Checks if timer is triggered.
 *
 *  \details  Calling this function will not clear trigger flag.
 *
 *  \param    timer: pointer to timer instance
 *
 *  \retval   true if timer is triggered
 */
static inline bool STIM_IsTimerDown(const struct STIM_Timer *timer)
{
    return (STIM_GetStatus(timer) & STIM_STATUS_TRIGGERED) != 0u;
}


/** \brief    This function starts timer.
 *
 *  \details  If the timer is initialized with manual start, this function needs
 *            to be called in order to start timer.
 *
 *  \param    timer: pointer to timer instance
 */
static inline void STIM_EnableTimer(struct STIM_Timer *timer)
{
    timer->enabled = true;
}


/** \brief    This function stops timer.
 *
 *  \details  Actual time measured is retained, update process is disabled.
 *
 *  \param    timer: pointer to timer instance
 */
static inline void STIM_DisableTimer(struct STIM_Timer *timer)
{
    timer->enabled = false;
}


/** \brief    This function resets timer.
 *
 *  \details  Actual time measured and status is cleared. Timer keeps its
 *            working state.
 *
 *  \param    timer: pointer to timer instance
 */
static inline void STIM_ResetTimer(struct STIM_Timer *timer)
{
    timer->currentTime_ms = 0U;
    STIM_ClearStatus(timer);
}


/** \brief    This function sets timer mode.
 *
 *  \param    timer: pointer to timer instance
 *  \param    mode:  enumeration type of timer mode - single or periodic
 */
static inline void STIM_SetTimerMode(struct STIM_Timer *timer, enum STIM_Mode mode)
{
    timer->mode = mode;
}


/** \brief    Function setting target time of timer instance.
 *
 *  \param    timer:    pointer to timer instance
 *  \param    time_ms:  desired time to be set
 */
static inline void STIM_SetTargetTime(struct STIM_Timer *timer, STIM_Time_t time_ms)
{
    timer->targetTime_ms = time_ms;
}


/** \brief    Function reloading target time and resetting of timer.
 *
 *  \param    timer:    pointer to timer instance
 *  \param    time_ms:  desired time to be set in milliseconds
 */
static inline void STIM_ReloadTimer(struct STIM_Timer *timer, STIM_Time_t time_ms)
{
    STIM_SetTargetTime(timer, time_ms);
    STIM_ResetTimer(timer);
}


/** \brief    This function initializes timer with stopwatch functionality.
 *
 *  \details  Timer is initialized as one shot timer with no target time.
 *            Maximum time measured is based on type of STIM_Time_t. If the
 *            processing type is set to interrupt, the stopwatch timer is
 *            handled on the background and time measured is exact, but with
 *            the cost of increased processing time. Stopwatch timer can be
 *            started automatically after initialization using parameter start.
 *
 *  \param    timer:          pointer to timer instance
 *  \param    processingType: interrupt or scheduler processing of stopwatch timer
 *                STIM_PROCESSING_INTERRUPT - timer instance will processed in interrupt handler
 *                STIM_PROCESSING_SCHEDULER - timer instance will processed in scheduler handle task
 *  \param    immediateStart: if true, timer will be started during initialization
 *
 *  \retval   returns 0 if success, otherwise -1
 */
static inline int32_t STIM_InitStopwatch(struct STIM_Timer *timer, enum STIM_Processing processingType, bool immediateStart)
{
    return STIM_InitTimer(timer, processingType, 0u, STIM_MODE_SINGLE, immediateStart, (struct STDCB_Callback *)NULL);
}


/** \brief    This function reads actual time measured.
 *
 *  \param    timer: pointer to timer instance
 *
 *  \retval   time measured in milliseconds
 */
static inline STIM_Time_t STIM_ReadStopwatch(const struct STIM_Timer *timer)
{
    return timer->currentTime_ms;
}


/** \brief    This function starts time measuring.
 *
 *  \param    timer: pointer to timer instance
 */
static inline void STIM_StartStopwatch(struct STIM_Timer *timer)
{
    STIM_EnableTimer(timer);
}


/** \brief    This function resets stopwatch timer and reads actual time.
 *
 *  \param    timer: pointer to timer instance
 *
 *  \retval   time measured in milliseconds
 */
static inline STIM_Time_t STIM_ResetStopwatch(struct STIM_Timer *timer)
{
    STIM_Time_t timerTime = STIM_ReadStopwatch(timer);
    STIM_ResetTimer(timer);
    return timerTime;
}


/** \brief    This function stops, resets and reads actual time of timer.
 *
 *  \param    timer: pointer to timer instance
 *
 *  \retval   time measured in milliseconds
 */
static inline STIM_Time_t STIM_StopStopwatch(struct STIM_Timer *timer)
{
    STIM_DisableTimer(timer);
    return STIM_ResetStopwatch(timer);
}


/** \brief    This function stops, and reads actual time of timer, time is retained.
 *
 *  \param    timer: pointer to timer instance
 *
 *  \retval   time measured in milliseconds
 */
static inline STIM_Time_t STIM_PauseStopwatch(struct STIM_Timer *timer)
{
    STIM_DisableTimer(timer);
    return STIM_ReadStopwatch(timer);
}


#ifdef __cplusplus
}
#endif

/******************************************************************************/
/* PUBLIC CLASS DECLARATIONS                                                  */
/******************************************************************************/
#ifdef __cplusplus


namespace STIM
{
    class Timer
    {
        private:
            struct STIM_Timer timer;
        public:
            Timer() {};
            
            int32_t init(enum STIM_Processing processingType, STIM_Time_t targetTime_ms, enum STIM_Mode timerMode, bool immediateStart)
            {
                return STIM_InitTimer(&this->timer, processingType, targetTime_ms, timerMode, immediateStart, (struct STDCB_Callback *)NULL);
            }
            
            int32_t init(enum STIM_Processing processingType, STIM_Time_t targetTime_ms, enum STIM_Mode timerMode, bool immediateStart, struct STDCB_Callback *callback)
            {
                return STIM_InitTimer(&this->timer, processingType, targetTime_ms, timerMode, immediateStart, callback);
            }
            
            void enable()
            {
                STIM_EnableTimer(&this->timer);
            }
            
            void disable()
            {
                STIM_DisableTimer(&this->timer);
            }
            
            STIM_Time_t read()
            {
                return STIM_ReadTimer(&this->timer);
            }
            
            void setTargetTime(STIM_Time_t time)
            {
                STIM_SetTargetTime(&this->timer, time);
            }
            
            void reload(STIM_Time_t time)
            {
                STIM_ReloadTimer(&this->timer, time);
            }
            
            uint8_t getStatus()
            {
                return STIM_GetStatus(&this->timer);
            }
            
            void clearStatus()
            {
                STIM_ClearStatus(&this->timer);
            }
            
            int32_t attachCallback(struct STDCB_Callback *callback)
            {
                return STIM_AttachCallback(&this->timer, callback);
            }
    };
    
    class Stopwatch
    {
        private:
            struct STIM_Timer stopwatch;
        public:
            Stopwatch() {};
            
            int32_t init(enum STIM_Processing processingType, bool immediateStart)
            {
                return STIM_InitStopwatch(&this->stopwatch, processingType, immediateStart);
            }
            
            void start()
            {
                STIM_EnableTimer(&this->stopwatch);
            }
            
            STIM_Time_t stop()
            {
                return STIM_StopStopwatch(&this->stopwatch);
            }
            
            STIM_Time_t reset()
            {
                return STIM_ResetStopwatch(&this->stopwatch);
            }
            
            STIM_Time_t read()
            {
                return STIM_ReadStopwatch(&this->stopwatch);
            }
    };
}

#endif // __cplusplus


#endif // SYSTEM_TIMER_H
