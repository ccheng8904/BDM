/*******************************************************************************
 *  Copyright (c) 2017 BSH Hausgeraete GmbH,
 *  Carl-Wery-Str. 34, 81739 Munich, Germany, www.bsh-group.de
 *
 *  All rights reserved. This program and the accompanying materials
 *  are protected by international copyright laws.
 *  Please contact copyright holder for licensing information.
 *
 *******************************************************************************
 *  PROJECT          Generic SW
 *  COMP_ABBREV      TIM
 ******************************************************************************/


#ifndef TIMERLIB_H
#define TIMERLIB_H

#ifdef __cplusplus
extern "C" {
#endif

/******************************************************************************/
/* DOCUMENTATION                                                              */
/******************************************************************************/
/** \file
 *
 *  \ingroup  PED-FW
 *
 *  \brief This library handles the SW timers and the circulating timers.
 *
 *  \details This library decrements the memory content of special timer
 *  segments. Several timer segments with different time bases may be defined.
 *  To use SW-Timer an 8- or 16-Bit timer variable just has to be defined in
 *  the corresponding memory segment.\n
 *  For short time measurement additionally a circulating timer is supported.
 *  Time differences with accuracy of 1us can be calculated.\n
 *
 *  \section timsec1 An example Using a SW-Timer using segmentation
 *  \code
 *  #include "SegmentDef.h"
 *  #include "timer/timerlib.h"
 *  
 *
 *  SDEF_SetSegmentRW(TIMER8_10MS)
 *  static Ttimer8 EXMPL_Timer10ms_8Bit;
 *
 *  SDEF_SetSegmentRW(TIMER16_100MS)
 *  static Ttimer16 EXMPL_Timer100ms_16Bit;
 *
 *  SDEF_SetSegmentRW_Default()
 *  
 *
 *  void EXMPL_loadTimers(void)
 *  {
 *      EXMPL_Timer10ms_8Bit   = 10u;
 *      EXMPL_Timer100ms_16Bit = 25u;
 *  }
 *  
 *  void EXMPL_handletask(void)
 *  {
 *      if(TIM_IsTimerDown(EXMPL_Timer10ms_8Bit))
 *      {
 *         do_something1();
 *      }
 *      if(TIM_IsTimerDown(EXMPL_Timer100ms_16Bit))
 *      {
 *         do_something2();
 *      }
 *  }
 *  \endcode
 *
 *  \section timsec2 Supported timer segments
 *  The timer segments are project specific, however the framework supports a list of timers out of the box.
 *  It is recommanded not to change it.
 *  \subsection timsubsec1 8-Bit timers
 *   \b TIMER8_10MS, TIMER8_20MS, TIMER8_100MS, TIMER8_200MS, TIMER8_1S, \b TIMER8_2S,
 *      TIMER8_10S, TIMER8_20S, TIMER8_1M, TIMER8_2M\n
 *   Bold printed timers are obligatory needed by the framework itself.
 *
 *  \subsection timsubsec2 16-Bit timers
 *  \b TIMER16_10MS, TIMER16_20MS, TIMER16_100MS, TIMER16_100MS, \b TIMER16_1S, TIMER16_2S,
 *     TIMER16_10S, TIMER16_20S, \b TIMER16_1M, TIMER16_2M, TIMER16_10M, TIMER16_20M\n
 *  It is recommanded to use bold printed 16-Bit timers only
 *  
 *  \section timsec3 Changing timer segments
 *  It is not recommanded to change the timer segments.\n
 *   However, when it is required you should adapt
 *  TIM_ptTimer8Config for 8-Bit timers and TIM_ptTimer16Config for 16-Bit timers.\n
 *  \sa TIM_ptTimer8Config, TIM_ptTimer16Config
 *  Aditionally the linker script must be adapted.
 *  \note The order of segments in linker script should be te same as in the config table
 *        TIM_ptTimer8Config and TIM_ptTimer16Config.\n
 *        The prescaler segment should be followed by the according timer segment.\n
 *        For instance START8_1 is follwed by TIMER8_10MS.
 */

/******************************************************************************/
/* INCLUDES                                                                   */
/******************************************************************************/
#include "bsh_stdinc.h" 
#include "LibTypes.h"
#include "SegmentDef.h"

#ifdef MCAL_MPCM_INCLUDED
#include "mcal/mcal.h"
#endif

/******************************************************************************/
/*   DEPENDENCIES                                                             */
/******************************************************************************/
#define DEP_TIM ///< dependencies for timer task used from scheduler, timer has no dependencies


/******************************************************************************/
/* PUBLIC DEFINITIONS                                                         */
/******************************************************************************/
//lint -save -e9026       (9026: function like macro, legacy code)
#define TIM_vLoadTimer(Timer,Time)      do{(Timer)=(Time);}while(0)///< This loads an 8 bit SW-Timer
#define TIM_vLoad16BitTimer(Timer,Time) do{(Timer)=(Time);}while(0)///< This loads a 16 bit SW-Timer
#define TIM_ucReadTimer(Timer)          (Timer)      ///< Returns the current value of an 8 bit timer
#define TIM_uiRead16BitTimer(Timer)     (Timer)      ///< Returns the current value of a 16 bit timer
#define TIM_ucIsTimerDown(Timer)        ((Timer)==0u)   ///< Test if 8 bit timer is down, legacy
#define TIM_ucIs16BitTimerDown(Timer)   ((Timer)==0u)   ///< Test if 16 bit timer is down, legacy
#define TIM_IsTimerDown(Timer)          ((Timer)==0u)   ///< Test if a timer is down (8 or 16 bit)
/*lint -restore -e9026 */

#define TIM_MILLI_SECOND 1000UL ///< ticks of TIM_uiGetCircleMicroSeconds() for 1ms


#define tTimer8  Ttimer8   ///< Define for downward compatibility with old timer module, do not use.
#define tTimer16 Ttimer16  ///< Define for downward compatibility with old timer module, do not use.

#define TIM_END_MARK 0x00u  ///< Endmark to be used for terminating prescaler in config table

/** 
  \brief Function-Macro is used to load an 16Bit-timer.

 The macro is using the 16Bit circulating timer is used, so 1 tick is 1us.
 \b Note: Periodicity is 32.ms not 65.5ms\n
 32ms after timer is down TIM_IS_CIRCLE_TIMER_16_DOWN() will return FALSE again!!\n
\param timer: Timer to be load. This timer will elapse in \e time us \n \b type: uint16_t\n
\param time:  Time until macro TIM_IS_CIRCLE_TIMER_16_DOWN() will return true.\n \b type: uint16_t\n

\b Example:
\code
TIM_LOAD_CIRCLE_TIMER_16(MAI_uiTimer,1000)
...

if (TIM_IS_CIRCLE_TIMER_16_DOWN(MAI_uiTimer))
{
...
}
\endcode
*/
#define TIM_LOAD_CIRCLE_TIMER_16(timer,time)  do {(timer) = ((uint16_t) (TIM_uiGetCircleMicroSeconds() + (uint16_t)(time)));} while(0u)


/** 
  \brief Returns true if timer elapsed.

 The circulating 16Bit timer is used, so 1 tick is 1us.
 \b Note: Periodicity is 32.ms not 65.5ms\n
 32ms after timer is down TIM_IS_CIRCLE_TIMER_16_DOWN will return FALSE again!!\n
 Use function macro TIM_LOAD_CIRCLE_TIMER_16() to load timer.\n
\param timer: Timer to test if is down.
\return true if timer is down
*/
#define TIM_IS_CIRCLE_TIMER_16_DOWN(timer)    ((int16)(TIM_uiGetCircleMicroSeconds() - (timer)) >=0u)


/******************************************************************************/
/* PUBLIC TYPE DEFINITIONS                                                    */
/******************************************************************************/
typedef struct TtimerConfig{   ///< Structure to configure a timer segment.
  volatile uchar* const pucPrescalerPtr;///< Pointer to prescaler of timer segment
  uchar ucPrescaler;    ///< value for prescaler, 0 marks last segment
}TtimerConfig;
/** 
  \struct TtimerConfig
  Structure to configure a timer segment.

 \b Example: Configuration of timer Segments
\code
const TtimerConfig TIM_ptTimer8Config[] =
{
           {&TIM_uc8Start1,    10},
           {&TIM_uc8Start2,     2},
           {&TIM_uc8Start3,     5},
           {&TIM_uc8Start4,     2},
           {&TIM_uc8Start5,     5},
           {&TIM_ucStatus,      0}
};
\endcode
*/
#ifndef __IAR_SYSTEMS_ICC__
typedef  volatile uchar  Ttimer8;  ///< Type for 8 bit timers
typedef  volatile uint16 Ttimer16; ///< Type for 16 bit timers
#else
#define Ttimer8 volatile uchar    ///< Type for 8 bit timers
#define Ttimer16 volatile uint16  ///< Type for 16 bit timers
#endif

 
/* VARIABLES TO BE DEFINED BY APPLICATION *******************************************************************/
SDEF_SetSegmentRW(END8)     // place in following data in segment END8 
extern tTimer8 TIM_uc8End; ///<End mark of last 8 bit timer segment
SDEF_SetSegmentRW(END16)    // place in following data in segment END16
extern tTimer8 TIM_uc16End;///<End mark of last 8 bit timer segment
SDEF_SetSegmentRW_Default() // switch back to default RW segment

#ifndef TIM_NO_TABLE_DECLARATION
extern const TtimerConfig TIM_ptTimer8Config[]; ///< configuration of 8 bit timers (example in lib template)
extern const TtimerConfig TIM_ptTimer16Config[];///< configuration of 8 bit timers (example in lib template)
#endif




/******************************************************************************/
/* PUBLIC FUNCTION DECLARATIONS                                               */
/******************************************************************************/

/** 
  \brief Task Handler of timer module.

  This function has to be called periodically from the scheduler. The Handler returns TASK_NOT_INITIALISED while timer library is not
  initialised. Call this task handler if just 8-Bit timers are used.

\return \n
     Current state of timer (info/command for scheduler)\n
  \b Type: uchar\n
  \b Range: [TASK_NOT_INITIALISED, TASK_INITIALISED, TASK_TERMINATE(unused)]

*/
extern BOOL TIM_bHandleTask(void);


/** 
  \brief Task Handler of timer module.

  This function has to be called periodically from the scheduler. The Handler returns TASK_NOT_INITIALISED while timer library is not
  initialised. Call this task handler if 8 and 16-Bit timers are used.

\return \n
     Current state of timer (info/command for scheduler)\n
  \b Type: uchar\n
  \b Range: [TASK_NOT_INITIALISED, TASK_INITIALISED, TASK_TERMINATE(unused)]

*/
extern BOOL TIM_bHandleTask16(void);


/**
 *  \brief    Wrapper function which is used by RTD and the new scheduler, returns the 
 *            uint8_t value instead of BOOL from the original Handle Task
 *
 *  \param    none
 *
 *  \return   TASK_INITIALIZED(1) or TASK_NOT_INITIALIZED (0)
 *
 *  \details  TASK_TERMINATE is not valid for timer application
 */
static inline uint8_t TIM_handleTask16(void)
{
    return (uint8_t)TIM_bHandleTask16();
}


/**
 *  \brief    Wrapper function which is used by RTD and the new scheduler, returns the 
 *            uint8_t value instead of BOOL from the original Handle Task
 *
 *  \param    none
 *
 *  \return   TASK_INITIALIZED(1) or TASK_NOT_INITIALIZED (0)
 *
 *  \details  TASK_TERMINATE is not valid for timer application
 */
static inline uint8_t TIM_handleTask(void)
{
    return (uint8_t)TIM_bHandleTask();
}


/* INTERNAL FUNCTION TO OVERWRITE *********************************************************************/

/** 
  \brief Initialise the timer module

 Function must be implemented platform specific!\n
 Define this function, to configure timer and interrupt (look for template in timerMcXS.c)\n
 The function must return true when initialisation is done.\n
 _TIM_bGetInitConfig() will be called from library itself until true will be returned.

\returns initialisation done or not\n
 type       : #BOOL\n
 range      : 0=false, all other= true

 \b Example: Configuration using the HAL (Hardware Abstraction Layer)
\code
 BOOL _TIM_bGetInitConfig(void)
 {
  HCCT0_vInit(0);
  return TRUE;
 }
\endcode
*/
extern BOOL _TIM_bGetInitConfig(void);


/** 
 \brief Returns true if timer which library is based on is down.

 Function must be implemented platform specific!\n
 Examples are in the template timerMcXS.c\n
 The function must return true when timer is down.
 The content is depend on the timer mode.\n 
 There are 2 different modes which are supported:\n
 \b A: Base time comes from circulating timer\n
 \b B: Base time comes from periodic timer interrupt\n
\return Basetimer down or not\n
 type       : BOOL\n
 range      : 0=false, all other= true

\b Example (implementation with circulating counter):
\code
BOOL _TIM_bIsBaseTimerDown(void)
   if ((uint16_t)(TIM_uiGetCircleMicroSeconds()-TIM_uiTimeOld) > BASE_TIME_IN_US)
   {
      TIM_uiTimeOld   += BASE_TIME_IN_US;
      return TRUE;
   }
   return FALSE;
}
\endcode
*/
extern BOOL _TIM_bIsBaseTimerDown(void);



/* INTERFACE FUNCTIONS TO OVERWRITE *******************************************************************/

/** 
  \brief Interface must be implemented platform specific: Returns a 32bit circulating counter value.

 The counter counts with 1Mhz frequency upward from 0x00 to 0xFFFFFFFF and starts again with 0x00.\n
 This is an optional library function. Define it if a 32bit with timer is required.\n
 Normally this version of circulating counter is used when time measurement with periods of
 more than 65ms is required.\n
 \b Recommendation: Just implement if required.\n
 For implementation please look examples in timerMcXS.c.
\return Circulating 32bit counter value\n
 type       : #uint32_t\n
 range      : 0x00000000...0xFFFFFFFF
*/
#if !defined(USE_32BIT_HWTIMER)
extern uint32_t TIM_u32GetCircleMicroSeconds(void);
#endif


/** 
  \brief Interface must be implemented platform specific: Returns a 16bit circulating counter value.

 The counter counts with 1Mhz frequency upward from 0x00 to 0xFFFF and starts again with 0x00.\n
 This is an obligatory library function. Several libraries are using this interface.\n
 \b Note: Special care should be taken using this function for long period time measurement.
 The period of this timer is 65ms!
 For implementation please look examples in timerXS.c.
\return Circulating 16bit counter value\n
 type       : #uint16_t\n
 range      : 0x0000...0xFFFF\n

\b Example: (implementation using HAL interface):
\code

uint16_t TIM_uiGetCircleMicroSeconds(void)
{
   return (HCCT0_uiGetTimer()*HW_TIMER_US_PER_TICK);
}
\endcode
*/
extern uint16_t TIM_uiGetCircleMicroSeconds(void);


#ifdef MCAL_MPCM_INCLUDED
/**
 *  \brief Callback function from MPCM module. Re-initialize timer when user switches power mode via MPCM.
 *
 *  \param obj: pointer to handle instance
 *  \param flags: pending MPCM_STATUS_XXX event flags
 *  \param eventResponse: response from event sender
 *
 */
extern void TIM_MpcmEventCallback(void *obj, uint32_t flags, const struct MCAL_EventResponse *eventResponse);
#endif

#ifdef __cplusplus
}
#endif

#endif
