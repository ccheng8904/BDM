/*******************************************************************************
 *  Copyright (c) 2017 BSH Hausgeraete GmbH,
 *  Carl-Wery-Str. 34, 81739 Munich, Germany, www.bsh-group.de
 *
 *  All rights reserved. This program and the accompanying materials
 *  are protected by international copyright laws.
 *  Please contact copyright holder for licensing information.
 *
 *******************************************************************************
 *  PROJECT          SW_LIB
 *  COMP_ABBREV      FSF
 ******************************************************************************/

/******************************************************************************/
/* DOCUMENTATION                                                              */
/******************************************************************************/
/** \file fsCom.c
 *
 *  \ingroup  <DOXY_GROUP>
 *
 *  \brief    Implementation of package fsCom for functional safety.
 *
 *  \details  <b>Functional Safety Generic Component - Top-Level Design </b>
\n
\n
\li <b>Package Model </b>\n
\image html Package_fscom.jpg
\n
<b>Description </b>\n
The fscom package provides the parts outlined in the model. By compilation switch, the generic may run either in
the usual operating mode or in test harness mode. When running in the usual operating mode, the test harness part
will not be linked to the package. When running in test harness mode, a test server gets control on every call of
fscom.
\n
<b>Rationale </b>\n
By the test harness part, it is possible to test all functions inside fscom fully isolated from other parts of the
whole system. Moreover, passing parameters for certain tests and recording of the results is supported.
\n
\n
\n
\li <b>State Model </b>\n
\image html StateModel_PseudoScheduler.jpg
\n
<b>Description </b>\n
The pseudo-scheduler function basically schedules the task handlers for existing channels and maintains the channels.
There is one state implemented for each task. Before start of the scheduling, the system has to be initialised. So,
the initialisation state is entered by default (at completion of the startup phase).
\n
<b>Rationale </b>\n
Some application have not implemented the scheduler component the archtecture provides. For that raesons, the fscom
package aditionally has implemented a scheduling mechanism. With this scheduling mechanism, corrent implementation,
order of channel task handler scheduling and channel checking is guaranteed.
\n
\n
\n
\li <b>Scenario: Normal operational scheduling </b>\n
\image html SequenceChart_PseudoScheduler.jpg
\n
<b>Description </b>\n
The generic only provides two interfaces to an application: one provide a means to setup a certain table of tasks to
be scheduled continuousely; the second interface prvide a means to schedule the generic during an applications
operational work. As said in the headline, this scenario dos not take care on some exceptions.
\n
<b>Rationale </b>\n
The generic's execution path is split up into smaller pieces, one to be preformed on every call of the generic. This is,
because service of interrupts must be performed as fast as possible. So, there is some more to do within the generic's
interface, but ths is not an issue here. A concrete application need not make use of the generic's scheduling mechanism,
but it has to make sure that all tests have been completed --BEFORE-- the generic gets control.
\n
\n
\n
\li<b>Input Document Reference(s) </b>\n
\n
<TABLE style="text-align: center">
<TR style="background-color:rgb(255,255,206)">
<TD>Reference</TD>
<TD>Ducument Title</TD>
<TD>Document Reference Name/Number</TD>
<TD>Version</TD>
<TD>Status</TD>
<TD>Release Date</TD>
</TR>
<TR>
<TD> [1] </TD>
<TD> Functional Safety Software Design document </TD>
<TD> FuncSafeSWSpec.pdf / 5560 0000003770 </TD>
<TD> Version 2.0 </TD>
<TD> Proposed </TD>
<TD> 08.05.2008 </TD>
</TR>
<TR>
<TD> [2] </TD>
<TD> Generic Functional Safety Software Specification</TD>
<TD> GenericFuncSafeSpec.pdf / 5560 0000007751 </TD>
<TD> Version 2.0 </TD>
<TD> Proposed </TD>
<TD> 08.05.2008 </TD>
</TR>
</TABLE>
\n
*/
 /****************************************************************************************************************
   CHANGES
   %PL%
 *****************************************************************************************************************/
/*===================================================================================================================
    Includes
===================================================================================================================*/
#include "bsh_stdinc.h"
#include "LibTypes.h"
#include "utility/utility.h"
#include "FuncSafe/fSafe.h"
#include "FuncSafe/fSafeXS.h"
#include "FuncSafe/fspriv.h"
#include "FuncSafe/fsramcon.h"
#include "FuncSafe/fs_psm.h"
#include "processor.h"
//#if defined(STM32_ARM) && !defined(STM32G0) && !defined(STM32G4)  && !defined(STM32F3)
//#include "fsStm32f10x.h"  // for STM32 One Time RAM Check is called directly (because C language)
//#endif

/*===================================================================================================================
    Definitions and Declarations
===================================================================================================================*/

/*====================================================================================================
    Local macros and enumerations
====================================================================================================*/
/* Local macros ***************************************************************************************/

/*====================================================================================================
    Local type definitions
====================================================================================================*/

/* Local type definitions *****************************************************************************/
/*====================================================================================================
    Local prototypes
====================================================================================================*/

/* RAM/ROM Variable prototypes ************************************************************************/
/* Function prototypes ********************************************************************************/
#ifdef APP_VARIANT
STATIC  void    vPerformOneTimeChecks(void);
#endif
STATIC  uint8_t  ucGetFSCOM_State(void);
STATIC  void     vSetFSCOM_State(uint8_t newstate);
STATIC  uint8_t  ucMonitoringStart(void);
STATIC  uint8_t  ucNewMonitorCycle(uint8_t ucCheckId);
STATIC  uint8_t  ucHandlePreScheduling(void);
STATIC  uint8_t  ucHandleTask(void);
STATIC  void     vSetChannelOngoing(uint8_t ucCheckId);
STATIC  bool     bIsChannelIdValid(uint8_t ucCheckId);
STATIC  uint8_t  ucGetChannelState(uint8_t ucCheckId);
STATIC  bool     bIsChannelCycleOverdue(uint8_t ucCheckId);
STATIC  void     vSetChannelIdle(uint8_t ucCheckId);
STATIC  void     vSetChannelSuccess(uint8_t ucCheckId);
STATIC  bool     bMaintainCheckCycleTimer(void);
STATIC  void     vTriggerExtWatchdog(void);
#ifdef FSF_TIMHISTORY
STATIC  void     vMemorizeTimingValue(uint16_t uiTimVal);
#endif
STATIC  uint8_t  ucComHandleTask(uint8_t ucChannelId);
STATIC  void     vExtApplicationStartUp(void);
STATIC  void     vExtApplicationErrorNotify(uint8_t ucSubCheckId, uint8_t ucChannelId, uint8_t ucUserPar);
STATIC  void     vExtReportFatal(void);
#ifdef TH_FSCOM
STATIC  void     vExtBoot(void);
#endif
STATIC  void     vExtTriggerExternalWatchdog(void);
STATIC  bool     bExtReadExtWatchdog(void);

/* RAM/ROM Variable definitions ***********************************************************************/

//@{\name FSCOM local variable

/*=====================================================================================
    Local variables
=====================================================================================*/

/*======================================================================
    Configuration Support and Test Harness
======================================================================*/

//  Test support

//  This switch allows to call functions, that are part of the FSCOM generic, separately. When set,
//  the FSCOM task handler acts as a test server, and does not supervise associated channels.
//  For timing measurement FSCOM mainly depends on the interface TIM_uiGetCircleMicroSeconds.
//  In order to run the FSCOM's task handler, the system must be run in its operational state.
//  Under test conditions, the FSCOM is scheduled continuouesly but TASK_NOT_INITIALISED will be
//  returned in all cases. Channels have been defined to be dependent on the state of the FSCOM.
//  Due to the value returned back to the scheduler, channels will never be scheduled, so the
//  test harness is free to activate and test the channels.
#ifdef  TH_FSCOM
//  Used to external watchdog conditions
uint8_t    TH_FSF_ExtWatchdogState = (uint16_t)0;
//  Used to create timing conditions
uint8_t    TH_FSF_Circle256MicroSeconds = (uint8_t)0;
//  Used to create timing conditions
uint16_t   TH_FSF_CircleMicroSeconds = (uint16_t)0;
//  Test harness function to be executed by the next step.
//  Zero means, no function to execute.
uint8_t    TH_FSF_Function = (uint8_t)0;
//  Parameter passed in this test step
uint8_t    TH_FSF_InPar[TH_PAR_SIZE]  = {0};
//  In-Stack Pointer
uint8_t    TH_FSF_InSP = (uint8_t)0;
//  Return vector stored on test step completion
uint8_t    TH_FSF_OutPar[TH_RET_SIZE]  = {0};
//  Out-Stack Pointer
uint8_t    TH_FSF_OutSP = (uint8_t)0;
//  Timestamp at start of test step
uint16_t   TH_FSF_StartOfTest  = (uint16_t)0;
//  Timestamp at end of test step
uint16_t   TH_FSF_EndOfTest  = (uint16_t)0;
//  Execution time limit
uint16_t   TH_FSF_MaxExecTime  = (uint16_t)TH_EXEC_TIME;
uint8_t    TH_FSF_ApplError    = (uint8_t)0;

//  Type of ROM check
bool       TH_FSF_bROM_At_Once = false;
//  Scheduler Configuration
uint8_t    TH_FSF_HasScheduler = (uint8_t)true;
#endif
// macro is using to debug/adjust watchdog
//lint -save -e551
STATIC   uint16_t   uiWDTimViolation    = (uint16_t)0;                    //!< Counts watchdog timing violations
STATIC   uint8_t    ucPreSchedulingCtr  = (uint8_t)0;                   //!< Counts scheduling cycles before operational
STATIC   uint8_t    ucCurrentChannel    = (uint8_t)FSF_FIRST_APPL_TEST; //!< Current channel about to be checked
STATIC   uint8_t    ucSchedState        = (uint8_t)FSF_SCH_INITIALISE;  //!< After startup
STATIC   uint8_t    FSF_ucRetVal        = (uint8_t)TASK_NOT_INITIALISED; //!< Return value from last component

#ifdef  FSF_TIMHISTORY
//  The timing history:
//  this will simplify finding the correct timing. Each time functional safety is scheduled,
//  a time stamp is placed in this circular history, in terms of the result provided by the
//  interface TIM_uiGetCircleMicroSeconds
STATIC   uint16_t   uiStoredVal = (uint16_t)0;
STATIC   uint16_t   uiMinVal    = (uint16_t)UINT_MAX;
STATIC   uint16_t   uiMaxVal    = (uint16_t)0;
#endif

// definition of the memory block for RAM Consistency check
typedef struct _TFS_COM_RamConBlock {
    __ALIGNED(4) uint16_t   uiErrorCounter;                    //!< Counts reported errors
    __ALIGNED(4) uint8_t    ucCCElapsedCounter;                //!< Counts cycles elapsed
}TFS_COM_RamConBlock;

__ALIGNED(4) STATIC TFS_COM_RamConBlock FS_COM_RamConBlock;
__ALIGNED(4) STATIC TFS_COM_RamConBlock FS_COM_RamConBlockMirror;
STATIC uint8_t FS_COM_ucCheckValueRamConBlock; ///< Holds current consistency check value

/**
  \brief This variable holds the current memory consistency check value of FSF FSCOM.

    <table><tr><td width=30%><b>Physical Range:</b></td><td>
        n/a
    </td></tr><tr><td width=30%><b>Scaler:</b></td><td>
        n/a
    </td></tr><tr><td width=30%><b>Binary Range:</b></td><td>
        0x0..0xFF
    </td></tr><tr><td width=30%><b>Data Type:</b></td><td>
        uint8_t
    </td></tr></table>

*/

//@}

/* static function definitions ************************************************************************/

//@{\name FSCOM local functions
/** Perform the one-time check for all channels available

    \return     None\n
    \pre        None\n
    \post       All channels checked\n

*/
/*-----------------------------------------------------------------------
  Control Flow:
  Begin
      this function has to be called immediately on completion of the Register and
      the RAM check. The stack pointer is now set up correctly, hence local variables
      are allowed to use without restrictions.

      take the number of channels from the channel configuration;

      Loop for the rest of all channels actually configured:
      begin
          indirectly activate the one-time check handler;
      end;

  End
-------------------------------------------------------------------------*/
#ifdef APP_VARIANT
STATIC  void    vPerformOneTimeChecks(void)
{

   uint8_t  n = (uint8_t)FSF_FIRST_APPL_TEST;
   while (n < (uint8_t)FSF_ucNrOfChannels)
   {
      FSF_psChannelTab[n++]->FSF_pvOneTimeCheck();/*lint !e9049 : increment/decrement operation combined with other operation with side-effects [MISRA 2012 Rule 13.3, advisory] */
#ifdef TH_FSCOM
      TH_FSFP_PushOutPar((uint8_t)n);
#endif
   }
}
#endif
/** Observe the state of FSCOM

    Return the state of the FSCOM component.

    \return     uint8_t   the state of the FSCOM generic
    \pre        None\n
*/
STATIC  uint8_t    ucGetFSCOM_State(void)
{
   return FSF_ucTaskState;
}
/** Change the state of the FSCOM generic to the state passed here.

    \return     uint8_t   the state of the FSCOM generic
    \pre        None\n
    \post       Transition into new state completed
    \note       If an invalid state value has been passed a boot error will\n
                be created in
*/
STATIC  void    vSetFSCOM_State(uint8_t newstate)
{
   switch (newstate)
   {
      case (uint8_t)FSF_STATE_BOOTED:
      case (uint8_t)FSF_STATE_STARTED:
      case (uint8_t)FSF_STATE_PRESCHEDULED:
      {
         //  Take over the valid state
         FSF_ucTaskState = newstate;
         break;
      }
      default:
      {
         //  FSCOM state out-of-range
         FSF_vBootError();
         break; // needed to satisfy SCA
      }
   }
}

/** Start FSCOM channel monitoring.
    The scheduler component schedules FSCOM on a regular base. The 1st scheduling cycle
    is interpreted as monitoring about to start. This requires at least one scheduling
    cycle for the FSCOM BEFORE all other channels are scheduled. At the end of this
    cycle, the external watchdog is serviced at least once.

    \return uint8_t    the FSCOM state.\n\n
    \pre    FSCOM booted correctly, so ECU located parts are "ready-for-use"\n
    \post   All one-time checks have been performed successfully, that is the one-time
            Register check and the one-time RAM check. FSCOM data structures have been
            initialised correctly and the external watchdog has been serviced.\n
*/
STATIC  uint8_t    ucMonitoringStart(void)
{
   uint8_t   retval  = (uint8_t)FSF_STATE_BOOTED;      // the return value
   uint8_t   i;                                      // loop var
#ifdef TH_FSCOM
   TH_FSFP_PushOutPar((uint8_t)TP_FSF_ucMonitoringStart);
#endif
   //  Initialise all channel data
   if (FSF_ucNrOfChannels > (uint8_t)0)
   {
      //  Initialise the rest of all existing channels
      for (i = (uint8_t)FSF_REG_TEST;i < FSF_ucNrOfChannels;i++)
      {
         //  Channel initialisation happens only for channels scheduled regularely
         if (FSF_psChannelTab[i]->bIsRegularCheck != false)
         {
            //  Move the channel into the state IDLE
            vSetChannelIdle((uint8_t)i);
            // Initialize the channels check cycle
            FSF_sCH_CheckCycleTime[i].ui16CheckCycleTime = FSF_psChannelTab[i]->sCheckCycle.ui16CheckCycleTime;
            FSF_sCH_CheckCycleTime[i].ucCycleMultiplier  = FSF_psChannelTab[i]->sCheckCycle.ucCycleMultiplier;
         }
      }
      //  Finally, trigger the external watchdog
      vTriggerExtWatchdog();
      //  Load the pre-scheduling counter
      ucPreSchedulingCtr = FSF_ucPreSchedCycles;

      //  Change the FSCOM into state PRESCHEDULED
      retval = (uint8_t)FSF_STATE_PRESCHEDULED;
      //  Set up the check cycle timer
#ifdef FSF_CYCLE_TIM_SIZE_32BIT
      FSF_ulCheckCycleTimer = (uint32_t)(FSF_CHECK_CYCLE_TIME * 1000u);
#else
      FSF_ucCheckCycleTimer = (uint8_t)FSF_CHECK_CYCLE_TIME;
#endif
   }
   else
   {
      //  Channel configuration
      //  error
      FSF_vBootError();
   }

   return  retval;
}

/** Perform a channel monitoring cycle.

    \param      ucCheckId channel identifier
    \return     uint8_t     TASK_INITIALISED
    \pre        FSCOM initialised
    \post       - all channels returned into its IDLE state\n
                - channel check cycle time maintained\n
                - external watchdog triggered\n
    \note       If a channel remains in state IDLE an application error\n
                will be created with the error code FSF_ERR_CHANNEL_IDLE.\n
                if a channel check cycle has elapsed an application error\n
                will be created with the error code FSF_ERR_CYCLE_OVERDUE.\n
*/
/*-----------------------------------------------------------------------
  Control Flow:
  Begin

      For the actual channel

      if the affected channel is scheduled regularely then

          if the channel remains in state IDLE then

              inform the application by FSF_ERR_CHANNEL_IDLE

          fi

          if the channel check cycle completion is overdue then

              inform the application by FSF_ERR_CYCLE_OVERDUE

          fi

          move the channel into the state IDLE;

      fi

  End
-------------------------------------------------------------------------*/
STATIC  uint8_t    ucNewMonitorCycle(uint8_t ucCheckId)
{
#ifdef TH_FSCOM
   TH_FSFP_PushOutPar((uint8_t)TP_FSF_ucNewMonitorCycle);
   TH_FSFP_PushOutPar(ucCheckId);
#endif
   // Maintain configured channels
   // Channel supervision happens only for regularely scheduled channels
   if (FSF_psChannelTab[ucCheckId]->bIsRegularCheck != false)
   {
      // Channels that remain in state IDLE are considered dead
      if ((uint8_t)FSF_CH_STATE_IDLE == ucGetChannelState(ucCheckId))
      {
         vExtApplicationErrorNotify((uint8_t)FSF_ERR_CHANNEL_IDLE,ucCheckId,(uint8_t)0);
      }
      // For ongoing channel check cycles:
      if ((uint8_t)FSF_CH_STATE_ONGOING == ucGetChannelState(ucCheckId))
      {
         // If one scheduling cycle time has elapsed, then...
         if (FS_COM_RamConBlock.ucCCElapsedCounter > (uint8_t)0)
         {
            // Maintain the channel cycle time
            if (bIsChannelCycleOverdue(ucCheckId) != false)
            {
               vExtApplicationErrorNotify((uint8_t)FSF_ERR_CYCLE_OVERDUE,ucCheckId,(uint8_t)0);
            }
         }
      }
      //  Reset the channel back into its state IDLE
#ifdef TH_FSCOM
      TH_FSFP_PushOutPar((uint8_t)TP_FSF_vSetChannelIdle);
#endif
      vSetChannelIdle(ucCheckId);
   }
   //  If all channels have been checked, reset the counter of time packet that have elapsed during
   //  the last check cycle
   if (ucCheckId >= (uint8_t)(FSF_ucNrOfChannels - 1u))
   {
      FS_COM_RamConBlock.ucCCElapsedCounter = (uint8_t)0;
   }
   return (uint8_t)TASK_INITIALISED;
}

/** Change the state of the addressed channel to the state IDLE.

    \return     None
    \pre        None\n
    \post       Transition into the IDLE state completed
    \note       If an invalid channel address is given an application error\n
                will be created with the error code FSF_ERR_INVALID_RANGE.\n
*/
STATIC  void    vSetChannelIdle(uint8_t ucCheckId)
{
   if (bIsChannelIdValid(ucCheckId) != false)
   {
      UTI_vSetBit(FSF_ucCH_states[FSF_ONGOING_FIELD],(uint16_t)ucCheckId,false);
      UTI_vSetBit(FSF_ucCH_states[FSF_SUCCESS_FIELD],(uint16_t)ucCheckId,false);
   }
}

/** Change the state of the addressed channel to the state ONGOING.

    \return     None
    \pre        None\n
    \post       Transition into the ONGOING state completed
    \note       If an invalid channel address is given an application error\n
                will be created with the error code FSF_ERR_INVALID_RANGE.\n
*/
STATIC  void    vSetChannelOngoing(uint8_t ucCheckId)
{
   if (bIsChannelIdValid(ucCheckId) != false)
   {
      UTI_vSetBit(FSF_ucCH_states[FSF_SUCCESS_FIELD],(uint16_t)ucCheckId,false);
      UTI_vSetBit(FSF_ucCH_states[FSF_ONGOING_FIELD],(uint16_t)ucCheckId,true);
   }
}


/** Change the state of the addressed channel to the state SUCCESS.

    \return     None
    \pre        None\n
    \post       Transition into the SUCCESS state completed
    \note       If an invalid channel address is given an application error\n
                will be created with the error code FSF_ERR_INVALID_RANGE.\n
*/
STATIC  void    vSetChannelSuccess(uint8_t ucCheckId)
{
   if (bIsChannelIdValid(ucCheckId) != false)
   {
      UTI_vSetBit(FSF_ucCH_states[FSF_ONGOING_FIELD],(uint16_t)ucCheckId,true);
      UTI_vSetBit(FSF_ucCH_states[FSF_SUCCESS_FIELD],(uint16_t)ucCheckId,true);
      UTI_vSetBit(FSF_ucCH_states[FSF_SUCC_IN_PAST_FIELD],(uint16_t)ucCheckId,true);
   }
}

/** Trigger the external watchdog.

    \return     none
    \pre        FSCOM initialised
    \post       The external watchdog has been triggered correctly\n
                A time stamp has been created for this moment in time\n
    \note       At boot time, the state of the watchdog is checked and, if it\n
                does not return the correct state, an application error is created\n
                with the error code FSF_ERR_BOOT_FAILURE.\n
                At operational time, the state of the watchdog is checked and, if it\n
                does not return the correct state, an application error is created\n
                with the error code FSF_ERR_WD_STATE_ERROR.\n
                If this call happens out of the correct moment in time, hence\n
                a violation of the timing of the the external watchdog has occurred,\n
                an application error is created with the error code\n
                FSF_ERR_WD_TIMING_VIOLATION.\n
*/
/*-----------------------------------------------------------------------
  Control Flow:

  The nominal moment in time when the watchdog trigger shall occure, lies just in
  the middle of time slot T2. This moment is computed as

      current time[us] + ((T1 + T3 + (T2/2)) * 1000)[us].

  Names of time slots are taken from chapter 7.5 of [1]. Interface TIM_uiGetCircleMicroSeconds() provides the current time in microsecond units. As
  this interface returns a 16-bit value, up to approx. 65ms may be measured. There
  is an overall system decision that one scheduling cycle is not allowed to exceed
  this limitation. Hence, computation of the moment in time when the next watchdog
  trigger has to happen can't exceed this limitation too. In order to avoid rounding
  problems, the watchdog time slots T1 until T3 will be brought into microsecond unit.

  Begin
      read in the current moment in time

      select FSCOM state

      Begin

          case BOOTED

              read the state of the external watchdog

              if this state does NOT show the safe state then

                  inform the application by FSF_ERR_BOOT_FAILURE

              fi

          esac

          case STARTED

              compute the time elapsed since last trigger of the external watchdog

              if the actual moment in time is out-of-range then

                  the processor runs too fast or too slow, so

                  inform the application by FSF_ERR_WD_TIMING_VIOLATION

              fi

              read the state of the external watchdog

              if this state does NOT show the operational state then

                  inform the application by FSF_ERR_WD_STATE_ERROR

              fi

          esac

          otherwise

              inform the application by FSF_ERR_INVALID_STATE
      end

      last WD trigger get the current moment in time
      trigger the external watchdog

  End


  Note: after notification of the application, there is NO return!

-------------------------------------------------------------------------*/
STATIC  void    vTriggerExtWatchdog(void)
{
#ifdef TH_FSCOM
   FSF_ui16Current = TH_FSF_CircleMicroSeconds;
#else
   FSF_ui16Current = TIM_uiGetCircleMicroSeconds();
#endif
   switch (ucGetFSCOM_State())
   {
      case (uint8_t)FSF_STATE_BOOTED:
      {
         if (bExtReadExtWatchdog() == false)
         {
            vExtApplicationErrorNotify((uint8_t)FSF_ERR_BOOT_FAILURE, (uint8_t)FSF_EXT_WDOG,(uint8_t)0);
         }
         break;
      }
      case (uint8_t)FSF_STATE_STARTED:
      {
         FSF_ui16delta_t = (uint16_t)(FSF_ui16Current - FSF_ui16LastWDTrigger);
         if ((FSF_ui16delta_t > (uint16_t)(FSF_ui16WDC1_2_3)) ||  (FSF_ui16delta_t < (uint16_t)(FSF_ui16WDC1_3)))
         {
            uiWDTimViolation++;
            vExtApplicationErrorNotify((uint8_t)FSF_ERR_WD_TIMING_VIOLATION, (uint8_t)FSF_EXT_WDOG,(uint8_t)0);
         }
         if (bExtReadExtWatchdog() != false)
         {
            vExtApplicationErrorNotify((uint8_t)FSF_ERR_WD_STATE_ERROR, (uint8_t)FSF_EXT_WDOG,(uint8_t)0);
         }
#ifdef FSF_TIMHISTORY
         // memorize the current time stamp
         vMemorizeTimingValue((uint16_t)FSF_ui16Current);
#endif
         break;
      }
      default:
      {
         vExtApplicationErrorNotify((uint8_t)FSF_ERR_INVALID_STATE, (uint8_t)FSF_EXT_WDOG,(uint8_t)0);
         break;
      }
   }
   // microseconds units !!
   FSF_ui16LastWDTrigger = FSF_ui16Current;
   vExtTriggerExternalWatchdog();
}

/** Check the channel address against invalid range

    \param      ucCheckId channel identifier
    \return     bool      true, if the channel is valid, false otherwise\n
    \pre        FSCOM initialised\n
    \post       None\n
    \note       If the channel address is onvalid an application error\n
                will be created with the error code FSF_ERR_INVALID_RANGE.\n
*/
STATIC  bool    bIsChannelIdValid(uint8_t ucCheckId)
{
   //  The FSF configuration hard data holds the number of channels available
   //  The channel identifier MUST be within this range
   if (ucCheckId < FSF_ucNrOfChannels)
   {
      return true;
   }
   else
   {
      vExtApplicationErrorNotify((uint8_t)FSF_ERR_INVALID_RANGE, ucCheckId,(uint8_t)0);
      return false;
   }
}

/** Check, if a channels check cycle completion is overdue.

    \param      ucCheckId channel identifier
    \return     bool      true, if the check cycle completion is overdue, false otherwise\n
    \pre        FSCOM initialised\n
    \post       None\n
*/
STATIC  bool    bIsChannelCycleOverdue(uint8_t ucCheckId)
{
   //  Some channels get scheduled on a regular base, but they don't want to be supervised
   //  In that case, configurable values have been set to zero

   //  Use pointers, this will improve access and save code space.
   T_FSCOM_CheckCycle FSTH_CONST * p = (T_FSCOM_CheckCycle FSTH_CONST *)(FSTH_CONST void*)(&FSF_psChannelTab[ucCheckId]->sCheckCycle); /*lint !e926 cast tested to work correctly */
   T_FSCOM_CheckCycle * q = (T_FSCOM_CheckCycle *)(void*)(&FSF_sCH_CheckCycleTime[ucCheckId]); /*lint !e926 cast tested to work correctly */

   // Does the actual channel use check cycle supervision?
   if (p->ui16CheckCycleTime != (uint16_t)0)
   {
      // Yes. Does the actual channel have sufficient packets?
      if(q->ui16CheckCycleTime >= (uint16_t)(FS_COM_RamConBlock.ucCCElapsedCounter))
      {
         // Yes. Simply count down the packets
         q->ui16CheckCycleTime -= (uint16_t)FS_COM_RamConBlock.ucCCElapsedCounter;
      }
      else
      {
         //  NO. Reload the packet again
         q->ui16CheckCycleTime = p->ui16CheckCycleTime;
         // No. Does the actual channels still have more packets
         if (q->ucCycleMultiplier == (uint8_t)0)
         {
            //  No. Check cycle elapsed before success.
            //  Reload the packet multiplier again
            q->ucCycleMultiplier = p->ucCycleMultiplier;
            //  Signal this event up to the caller
            return true;
         }
         else
         {
            //  Yes. Go on counting down the packets already left...
            q->ucCycleMultiplier--;
         }
      }
   }
   return false;
}

/** Return the state of a channel

    \param      ucCheckId channel identifier
    \return     uint8_t     IDLE or ONGOING or SUCCESS\n
    \pre        FSCOM initialised\n
    \post       None\n
*/
STATIC  uint8_t   ucGetChannelState(uint8_t ucCheckId)
{
   if (bIsChannelIdValid(ucCheckId) != false)
   {
      //  Getting values from a bit field is an expensive task. Thus, i'll take it only once!
      bool   bIsSuccess = (bool)UTI_bGetBit(FSF_ucCH_states[FSF_SUCCESS_FIELD],(uint16_t)ucCheckId);
      bool   bIsOngoing = (bool)UTI_bGetBit(FSF_ucCH_states[FSF_ONGOING_FIELD],(uint16_t)ucCheckId);

      if (bIsSuccess != false)
      {
         return (uint8_t)FSF_CH_STATE_SUCCESS;
      }
      else
      {
         // Success flag not set. Hence, returning state either ongoing or idle
         if (bIsOngoing != false)
         {
            return (uint8_t)FSF_CH_STATE_ONGOING;
         }
      }
   }
   return (uint8_t)FSF_CH_STATE_IDLE;
}

/** Maintain a channels check cycle time.

    \return     bool      true, if the check cycle has elapsed, false otherwise.\n
    \pre        FSCOM initialised\n
    \post       None\n
    \note       This handler count down pieces of milliseconds. In order to save code
                space, milliseconds are measured by pieces of 1024 microseconds. So, the
                optimizer will use simple shift operations instead of code expensive
                computations. Moreover, the check cycle timer is maintained based on the
                range of an unsigned char, that is the maximum span for one cycle will be
                up to 255 milliseconds.
*/
//FSF_CHECK_CYCLE_TIME 100
STATIC  bool    bMaintainCheckCycleTimer(void)
{
   bool    retval   = false;
#ifdef FSF_CYCLE_TIM_SIZE_32BIT
   uint16_t  ui16TimeNow = (uint16_t) TIM_uiGetCircleMicroSeconds();
#ifdef TH_FSCOM
   uint16_t   ui16Current = (uint16_t)(TH_FSF_CircleMicroSeconds - FSF_ui16CycLastTime);
#else
   uint16_t   ui16Current =(uint16_t)(ui16TimeNow - FSF_ui16CycLastTime);
#endif
   FSF_ui16CycLastTime = ui16TimeNow;
   // Maintain the check cycle timer
   FSF_ulCheckCycleTimer -= (uint16_t)ui16Current;
   if ((int32_t)FSF_ulCheckCycleTimer < (int32_t)0)
   {
      FSF_ulCheckCycleTimer = (uint32_t)(FSF_CHECK_CYCLE_TIME * 1000u); //wertebereich gesprengt: evt 50000
#else   //#ifdef FSF_CYCLE_TIM_SIZE_32BIT
#ifdef TH_FSCOM
   uint8_t   ucCurrent = (uint8_t)(((uint16_t)(TH_FSF_CircleMicroSeconds - FSF_ui16LastWDTrigger))/(uint16_t)1024);
#else
   uint8_t   ucCurrent = (uint8_t)(((uint16_t)(TIM_uiGetCircleMicroSeconds() - FSF_ui16LastWDTrigger))/(uint16_t)1024);
#endif
   // Maintain the check cycle timer
   FSF_ucCheckCycleTimer -= (uint8_t)ucCurrent;   //platform-specific
   if ((schar)FSF_ucCheckCycleTimer < (schar)0)
   {
      FSF_ucCheckCycleTimer = (uint8_t)FSF_CHECK_CYCLE_TIME;
#endif
      // Count the number of cycle time packets elapsed since last measurement
      FS_COM_RamConBlock.ucCCElapsedCounter++;
      retval = true;
   }
   return retval;

}

#ifdef  FSF_TIMHISTORY
/** Memorize a timing value. Store it into the history and maintaib the history.
    \return     None\n
    \pre        FSCOM initialised\n
    \post       The history pointer points to the next free item.\n
*/
STATIC  void    vMemorizeTimingValue(uint16_t uiTimVal)
{
   uint16_t uiDiff = (uint16_t)(uiTimVal - uiStoredVal);
   uiStoredVal = uiTimVal;
   if (uiDiff < uiMinVal)
   {
      uiMinVal = uiDiff;
   }
   if (uiDiff > uiMaxVal)
   {
      uiMaxVal = uiDiff;
   }
}
#endif

/** Handle the per-scheduling state.
   Pass a number of scheduling cycles before normal operation.
   \return     uint8_t\n   FSF_STATE_PRESCHEDULED when counting scheduling cycles\n
                         FSF_STATE_STARTED      otherwise\n

    \pre        FSCOM runs in its boot state\n
    \post       The internal/external watchdogs not serviced.
*/
STATIC  uint8_t   ucHandlePreScheduling(void)
{
   uint8_t   retval = (uint8_t)FSF_STATE_PRESCHEDULED;
#ifdef TH_FSCOM
   TH_FSFP_PushOutPar((uint8_t)TP_FSF_ucHandlePreScheduling);
#endif
   if (ucPreSchedulingCtr > FSF_ucPreSchedCycles)
   {
      // Notify a boot failure
      //vExtApplicationErrorNotify(uint8_t ucSubCheckId, uint8_t ucChannelId, uint8_t ucUserPar)
      vExtApplicationErrorNotify((uint8_t)FSF_ERR_BOOT_FAILURE, (uint8_t)FSF_NOT_SPECIFIED_CHANNEL,(uint8_t)0);
      // If the application does not break off operation, reduce the variable to what
      // is configured
      ucPreSchedulingCtr = FSF_ucPreSchedCycles;

   }
   if ((ucPreSchedulingCtr > (uint8_t)0) && (ucPreSchedulingCtr <= FSF_ucPreSchedCycles))
   {
      ucPreSchedulingCtr--;
   }
   else
   {
      FSF_ui16LastWDTrigger = TIM_uiGetCircleMicroSeconds();
      retval = (uint8_t)FSF_STATE_STARTED;
   }

   return retval;
}

/** Schedule FSCOM.
   By this handler, FSCOM is scheduled on a regular base.

    \param      ucChannelId   channel identifier
    \return     uint8_t\n   TASK_NOT_INITIALISED:   channel monitoring not started yet\n
                          TASK_INITIALISED:       channel monitoring cycle completed\n
                          TASK_TERMINATE:         channel exception occurred\n

    \pre        FSCOM runs in its operational state\n
    \post       All configured channels checked. The external watchdog serviced correctly.
*/
/*-----------------------------------------------------------------------
  Control Flow:
  Begin
      if Test connected to FSCOM then

          call the test server to perform one test step
          return TASK_NOT_INITIALISED to the scheduler

      else


          select FSCOM state

          Begin

              case BOOTED

                  initialise all channel states,
                  determine the next FSCOM state
                  and set up the return code

              esac

              case PRESCHEDULED

                  count down the number of pre-scheduling cycles
                  this is required in order to get the system tuned stable
                  determine the next FSCOM state
                  and set up the return code

              case STARTED

                  manage the actual channel state
                  and set up the return code

              esac

              otherwise

                  create a fatal exception
                  set the return code to TASK_TERMINATE

          End

      End

      with the retrun code, return control to the scheduler;

  End
-------------------------------------------------------------------------*/
//  The real task handler
STATIC  uint8_t ucComHandleTask(uint8_t ucChannelId)
{
   uint8_t   retval      = (uint8_t)TASK_TERMINATE;
   uint8_t   nextState;
   switch (ucGetFSCOM_State())
   {
      case (uint8_t)FSF_STATE_BOOTED:
      {
         nextState = ucMonitoringStart();
         vSetFSCOM_State(nextState);
         retval = (uint8_t)TASK_NOT_INITIALISED;
         break;
      }
      case (uint8_t)FSF_STATE_PRESCHEDULED:
      {
         nextState = ucHandlePreScheduling();
         vSetFSCOM_State(nextState);
         if (nextState == (uint8_t)FSF_STATE_STARTED)
         {
            retval = (uint8_t)TASK_INITIALISED;
         }
         else
         {
            retval = (uint8_t)TASK_NOT_INITIALISED;
         }
         break;
      }
      case (uint8_t)FSF_STATE_STARTED:
      {
         retval = ucNewMonitorCycle(ucChannelId);
         break;
      }
      default:
      {
         // No other operating state allowed for FSCOM
         vExtApplicationErrorNotify((uint8_t)FSF_ERR_INVALID_STATE, (uint8_t)FSF_NOT_SPECIFIED_CHANNEL,(uint8_t)0);
         break;
      }
   }

   return retval;
}

/*=====================================================================================
    Stub interface implementations
=====================================================================================*/

//  This wrapper function separate the test harness from overloaded application
STATIC  void    vExtApplicationStartUp(void)
{
#ifdef TH_FSCOM
   TH_FSFP_PushOutPar((uint8_t)TP_FSF_vApplicationStartUp);
#else
   _FSF_vApplicationStartUp();
#endif
}

//  This wrapper function separate the test harness from overloaded application
STATIC  void    vExtApplicationErrorNotify(uint8_t ucSubCheckId, uint8_t ucChannelId, uint8_t ucUserPar)
{
#ifdef TH_FSCOM
   TH_FSFP_PushOutPar((uint8_t)TP_FSF_vApplicationErrorNotify);
   TH_FSFP_PushOutPar(ucSubCheckId);
   TH_FSFP_PushOutPar(ucChannelId);
   TH_FSFP_PushOutPar(ucUserPar);
   TH_FSF_ApplError++;
#else
   _FSF_vApplicationErrorNotify(ucSubCheckId,ucChannelId,ucUserPar);
#endif
}

//  This wrapper function separate the test harness from overloaded application
STATIC  void    vExtReportFatal(void)
{
#ifdef TH_FSCOM
   TH_FSFP_PushOutPar((uint8_t)TP_FSF_vReportFatal);
#else
   _FSF_vReportFatal();
#endif
}

//  This wrapper function separate the test harness from overloaded application
STATIC  void    vExtTriggerExternalWatchdog(void)
{
#ifdef TH_FSCOM
   TH_FSFP_PushOutPar((uint8_t)TP_FSF_vTriggerExtWatchdog);
#else
   _FSF_vTriggerExtWatchdog();
#endif
}

//  This wrapper function separate the test harness from overloaded application
STATIC  bool    bExtReadExtWatchdog(void)
{
#ifdef TH_FSCOM
   TH_FSFP_PushOutPar((uint8_t)TP_FSF_bExtReadExtWatchdog);
   TH_FSFP_PushOutPar((uint8_t)TH_FSF_ExtWatchdogState);
   return  TH_FSF_ExtWatchdogState;
#else
   return _FSF_bReadExtWatchdog();
#endif
}

//@}

/* external function definitions ***********************************************************************/

/* FSCOM BOOT FUNCTIONS **************************************************************************************/

//@{\name FSCOM exported boot services
/** Start of the FSCOM boot phase.

    This 1st activation of FSCOM is reserved in order to perform all one-time checks,
    which are destructive. At the end, FSCOM performs initialisation of its internal
    data structures. This service is NOT allowed to use during regular scheduling by
    the scheduler component, as RAM is a resource, which is to be checked for the
    whole system. Using this service as soon as the system has reached its operational
    state would be too late. This service may NOT be called, as this would require the
    stack that can't be assumed to be functional before completion of the RAM check.

    \pre    The stack pointer MUST be set on a valid range\n
    \post   All one-time checks have been performed successfully, that is the one-time
            Register check and the one-time RAM check.\n
    \note   This is added mainly for completeness. FSF_vBoot is only allowed to be called by
            immediate transfer of control, that is by GOTO or JUMP instruction as the stack
            can't be assumed to be functional before check. On completion, the start process
            has to be continued by usual function call. The return address gets lost. This
            doesn't matter as there is no return allowed back into the boot process.\n
*/

/*-----------------------------------------------------------------------
  Control Flow:
  Begin
      the boot procedure requires to stop processing of timers and interrupts.\n
      This is guaranteed this service FSCOM service is called at boot time\
      where interrupts and timers can't be processed that early moment in time;\n

      do the Register check;

      do the RAM check;

      do all other one-time checks;

      change FSCOM state into BOOTED;

      continue start up;

      To be considered:
      1.) an application shall not return to this point. If so, a fatal exception is
      created.
      2.) according ANSI-C the whole dynamic data area is filled with zero.
      Hence, channels are then all idle and the FSCOM remains in state BOOTED.
  End


-------------------------------------------------------------------------*/
#ifdef TH_FSCOM
void    FSF_vBoot(void)
{
   //  Continue the systems start up procedure.
   _FSF_vApplicationStartUp();
   //  Under test conditions, the library runs fully isolated from the
   //  environment, except some basic control structures, like boot tec..
   //  So, return control immediately, do nothing else!
}
void    vExtBoot(void)
#else
void    FSF_vBoot(void)
#endif
{
#ifdef VARIANT_BOOTMANAGER
    /* BootManager is running */
    FSF_setProgramCounterMonitoringVal(FSF_BM_RAM_TEST_COMPLETED);
    FSF_vRegistercheck();
    FSF_vStartUpRomCrcCheck();
    FSF_DoProgramCounterMonitoring(FSF_BM_RAM_TEST_COMPLETED,
                                   FSF_BM_PLATFORM_INITIALIZED);
#elif defined(APP_VARIANT)
    /* Application is running */

   //  Calling convention:
   //  ALL (!) checks are called like C-functions and, on return, it has to follow the conventions
   //  for C-functions too. But, as the stack can't be used for local variables Register and RAM
   //  checks are called directly, not using a loop variable.

   (* FSF_psChannelTab[FSF_REG_TEST]).FSF_pvOneTimeCheck();
//   (* FSF_psChannelTab[FSF_RAM_TEST]).FSF_pvOneTimeCheck(); // this test is moved into IntTblArmCM.c
   //  Perform all other checks configured for the application
   vPerformOneTimeChecks();
   //  Change FSCOM state into BOOTED
   FSF_ucTaskState = (uint8_t)FSF_STATE_BOOTED;
   //  Continue the systems start up procedure.
//   FSF_vContinueStartUp();
   //  NOTE:
   //  an application shall not return to this point!
//   vExtReportFatal();
#endif  // defined(APP_VARIANT)
}


/** End of the FSCOM boot phase.
    At the end of the FSCOM boot phase, control is transferred directly to this routine.
    Within this routine, start up has to continue depending on a project needs.

    \pre    FSCOM boot phase completed successfully\n
    \post   All one-time checks have been performed successfully.
    \note   By this routine, completion of the FSCOM process is clearly defined. For the
            application-specific start up continuation, please refer to the interfaces
            required by FSCOM.
*/
/*-----------------------------------------------------------------------
  Control Flow:
  Begin
      Chapter 6.5 (FSCOM Boot Procedure) requires to define one point where to continue
      start up, from FSCOM library point of view. Hence, the boot procedure has always
      the same point of completion and this is very helpful while debugging.

      continue the application specific system start up;

      NOTE: not return back here!
  End;


-------------------------------------------------------------------------*/
void    FSF_vContinueStartUp(void)
{
   vExtApplicationStartUp();
}

/** Report a boot error.
    This service is provided in order to move the system into the safe state.

    \pre    None\n
    \post   No return from this call.\n
*/
void    FSF_vBootError(void)
{
   //  Do not trigger the external watchdog again
   //  Pass control immediately to the application
   vExtApplicationErrorNotify((uint8_t)FSF_ERR_BOOT_FAILURE, (uint8_t)FSF_NOT_SPECIFIED_CHANNEL,(uint8_t)0);
   //  An application is not allowed to return control. Thus, enter an infinite loop.
   for (;;)
   {
      //  Trigger the internal watchdog
      vExtTriggerInternalWatchdog();
#ifdef TH_FSCOM
      TH_FSFP_PushOutPar((uint8_t)TP_FSF_vBootError);
      return;
#else
      //  Loop forever, do nothing else!
#endif
   }
}

//@}

/* FSCOM OPERATIONAL FUNCTIONS
**************************************************************************************/

//@{\name FSCOM exported operational services

/** Report ongoing check to FSCOM.
    When a channel starts checking this event is immediately reported to FSCOM. Each
    time the channel is processing it will report ongoing or success to FSCOM. If a
    channel does not report ongoing (or success), it is assumed not to work correctly.
    FSCOM keeps a flag for each channel. For ease of implementation it is recommended
    to set the ongoing flag also when reporting success (done directly by the report
    success service).
    \param  ucCheckId  identifier of the the addressed channel\n

    \pre    FSCOM initialised correctly. Channel identifier within the correct range.\n
    \post   For valid channels, internal state transitions, if any, have been completed.
            Invalid channel addressing immediately results into the safe state as this
            is not allowed to occur.\n
*/
/*-----------------------------------------------------------------------
  Control Flow:
  Begin

      transition from state IDLE into state ONGOING requires the pre-condition to be checked

      if the channel state is IDLE then

          check the pre-condition

          if this pre-condition returned false then

              inform the application by FSF_ERR_PRECONDITION_FALSE

          fi

      fi

      move the channel into the state ONGOING;

  End
-------------------------------------------------------------------------*/
void FSF_vReportOnGoing(uint8_t ucCheckId)
{
   uint8_t cstate = ucGetChannelState(ucCheckId);
#ifdef TH_FSCOM
   //  Do not service channels outside FSCOM
   if (ucCheckId >= (uint8_t)FSF_APP_CHANNEL_0)
   {
      return;
   }
#endif
   switch (cstate)
   {
      case (uint8_t)FSF_CH_STATE_IDLE:
      {
         if ((* FSF_psChannelTab[ucCheckId]).FSF_pbPrecondition() == false)
         {
            vExtApplicationErrorNotify((uint8_t)FSF_ERR_PRECONDITION_FALSE,ucCheckId,(uint8_t)0);
         }
         break;
      }
      case (uint8_t)FSF_CH_STATE_SUCCESS:
      case (uint8_t)FSF_CH_STATE_ONGOING:
      {
         break;
      }
      default:
      {
         vExtApplicationErrorNotify((uint8_t)FSF_ERR_INVALID_STATE,ucCheckId,(uint8_t)0);
         break;
      }
   }
   vSetChannelOngoing(ucCheckId);
}


/** Report a channel error to FSCOM.
    If a channel has stopped monitoring due to an error, this event is immediately
    reported to FSCOM.
    \param      ucCheckId      identifier of the the addressed channel / recognition of report ID (e.g. external watchdog)
    \param      ucSubCheckId   FSF_ERROR_CODE that describes the problem - please note that this parameter is not used, except when ucCheckId is an ID, but no channel.\n
    \param      ucUserPar      Parameter defined by user\n

    \pre        FSCOM initialised correctly. Channel identifier within the correct range.\n
    \post       Invalid channel addressing immediately results into the safe state as this
                is not allowed to occur.\n
*/
void FSF_vReportError(uint8_t ucCheckId, uint8_t ucSubCheckId, uint8_t ucUserPar)

{
   uint8_t errcode = (uint8_t)FSF_ERR_CHANNEL_ERROR;
#ifdef TH_FSCOM
   //  Do not service channels outside FSCOM
   if (ucCheckId >= (uint8_t)FSF_APP_CHANNEL_0)
   {
      return;
   }
#endif
   //  Is FSCOM ready to handle channel related problems
   if (ucGetFSCOM_State() == (uint8_t)FSF_STATE_STARTED)
   {
      if (ucCheckId < (uint8_t)FSF_LASTID)
      {
         if (ucCheckId < FSF_ucNrOfChannels)
         {
            //Channel
            switch (ucGetChannelState(ucCheckId))
            {
               case (uint8_t)FSF_CH_STATE_IDLE:
               case (uint8_t)FSF_CH_STATE_SUCCESS:
               case (uint8_t)FSF_CH_STATE_ONGOING:
               {
                  break;
               }
               default:
               {
                  errcode = (uint8_t)FSF_ERR_INVALID_STATE;
                  break;
               }
            }
         }
         else
         {
            if (ucCheckId > (uint8_t)FSF_NOCHANNEL)
            {
               //No channel, but valid ID (e.g. external watchdog)
               errcode = ucSubCheckId;

            }
            else
            {
               //Unused channel
               errcode = (uint8_t)FSF_ERR_INVALID_RANGE;
            }
         }
      }
      else
      {
         //Out of range
         errcode = (uint8_t)FSF_ERR_INVALID_RANGE;
      }
      _FSF_RAM_CON_vSetErrCounter();
      vExtApplicationErrorNotify(errcode,ucCheckId,ucUserPar);
   }
}

/** Report successfully completed check to FSCOM.
    If a channel has completed checking (partially or finally) successfully, this
    event is reported to FSCOM.
    \param  ucCheckId  identifier of the the addressed channel\n

    \pre    FSCOM initialised correctly. Channel identifier within the correct range.
    \post   For valid channels, internal state transitions, if any, have been completed.\n
            Invalid channel addressing immediately results into the safe state as this
            is not allowed to occur.\n
*/

void FSF_vReportSuccess(uint8_t ucCheckId)

{
#ifdef TH_FSCOM
   //  Do not service channels outside FSCOM
   if (ucCheckId >= (uint8_t)FSF_APP_CHANNEL_0)
   {
      return;
   }
#endif
   switch (ucGetChannelState(ucCheckId))
   {
      case (uint8_t)FSF_CH_STATE_IDLE:
      case (uint8_t)FSF_CH_STATE_SUCCESS:
      case (uint8_t)FSF_CH_STATE_ONGOING:
      {
         // Initialize the channels check cycle
         FSF_sCH_CheckCycleTime[ucCheckId].ui16CheckCycleTime = FSF_psChannelTab[ucCheckId]->sCheckCycle.ui16CheckCycleTime;
         FSF_sCH_CheckCycleTime[ucCheckId].ucCycleMultiplier  = FSF_psChannelTab[ucCheckId]->sCheckCycle.ucCycleMultiplier;
         vSetChannelSuccess(ucCheckId);
         break;
      }
      default:
      {
         vExtApplicationErrorNotify((uint8_t)FSF_ERR_INVALID_STATE,ucCheckId,(uint8_t)0);
         break;
      }
   }
}

/** Observe successful channel check in the past.
    A Boolean value is returned by this service. When at least one check cycle has
    been completed successfully true is returned, otherwise false is returned. See
    chapter 8.7 (Functional Safety Software Specification document) for an example of use.
    \param          ucCheckId  identifier of the the addressed channel\n

    \return         bool       true/false, see description.\n\n
    \pre            FSCOM initialised correctly. Channel identifier within the correct range.\n
    \post           Invalid channel addressing immediately results into the safe state as this
                    is not allowed to occur.\n
*/

bool    FSF_bSuccesInPast(uint8_t ucCheckId)
{
   bool retval = false;
   switch (ucGetChannelState(ucCheckId))
   {
      case (uint8_t)FSF_CH_STATE_IDLE:
      case (uint8_t)FSF_CH_STATE_SUCCESS:
      case (uint8_t)FSF_CH_STATE_ONGOING:
      {
         retval = UTI_bGetBit(FSF_ucCH_states[FSF_SUCC_IN_PAST_FIELD],ucCheckId);
         break;
      }
      default:
      {
         vExtApplicationErrorNotify((uint8_t)FSF_ERR_INVALID_STATE,ucCheckId,(uint8_t)0);
         break;
      }
   }
   return retval;
}

/** Report a fatal exception.
    This service is provided for channels that require IMMEDIATE transition into the safe state.
    For example, if the register channel has recognised stuck-at fault on the carry flag, then
    control immediately has to move to the safe state (Jump instruction).

    \pre    None\n
    \post   No return from this call.\n
*/
void    FSF_vReportFatal(void)
{
   vExtReportFatal();
}

//@}


/* FSCOM SCHEDULER FUNCTIONS
**************************************************************************************/

//@{\name FSCOM exported scheduling functions

/*======================================================================
    Real Scheduler Service(s)
======================================================================*/

/*=======================================================
    STATIC uint8_t   ucHandleTask(void);
=======================================================*/

/** Perform one execution cycle for ALL (!) channels actually registered.
    While operational, execution is stopped immediately, if one component
    did not return TASK_INITIALISED.

   \return    uint8_t     TASK_INITIALISED,     if all components returned this
                        TASK_NOT_INITIALISED, if at least one component has returned this

   \pre       scheduler has been initialised\n
   \post      all channels checked and returned into state IDLE
*/

STATIC uint8_t   ucHandleTask(void)
{
   _FSF_RAM_CON_vDoCheckInFsComModule();

/*======================================================================
    TP_FSF_SCH_ucHandleTask
======================================================================*/
#ifdef TH_FSCOM
   TH_FSFP_PushOutPar((uint8_t)TP_FSF_SCH_ucHandleTask);
#endif

   switch (ucSchedState)
   {
/*======================================================================
    Initialize the generic
======================================================================*/
      case (uint8_t)FSF_SCH_INITIALISE:
      {
         FSF_ucRetVal = ucComHandleTask((uint8_t)FSF_NOCHANNEL);
         if (FSF_ucRetVal == (uint8_t)TASK_INITIALISED)
         {
            //  Initialisation phase completed. Start generic scheduling
            ucSchedState = (uint8_t)FSF_SCH_GENERIC;
         }
         break;
      }
/*======================================================================
    Schedule the generic
======================================================================*/
      case (uint8_t)FSF_SCH_GENERIC:
      {
         //  Maintain the check cycle counter.
         (void)bMaintainCheckCycleTimer();
         // Walk through all channels that are actually configured
         for (ucCurrentChannel = (uint8_t)FSF_REG_TEST;
              ucCurrentChannel < (uint8_t)FSF_ucNrOfChannels;
              ucCurrentChannel++)
         {
            FSF_ucRetVal = ucComHandleTask(ucCurrentChannel);
            if (FSF_ucRetVal != (uint8_t)TASK_INITIALISED)
            {
               break;
               //  Msg(2:0769) This break statement terminates an iteration statement.
               //  PCA comment: desired state and better readable in X32, code tested and reviewed to work correctly. Thus, left unchanged.

            }
         }
         //  Finally, trigger the external watchdog
         vTriggerExtWatchdog();
         break;
      }
      default:
      {
/*======================================================================
    Task state accidently overwritten!
======================================================================*/
         // No other operating state allowed for FSCOM
         vExtApplicationErrorNotify((uint8_t)FSF_ERR_INVALID_STATE, (uint8_t)FSF_NOT_SPECIFIED_CHANNEL,(uint8_t)0);
         break;
      }
   }
   //  Service the internal watchdog
   vExtTriggerInternalWatchdog();

   /* calc new checksum at exit of task handler */
   _FSF_RAM_CON_vDoUpdateCheckInFsComModule();

   return FSF_ucRetVal;
}

//  Following interface is exported for systems that make use of the real scheduler
//  component. It is NOT called from within the functional safety component! Instead,
//  it is assumed the functional safety component gets executed by the scheduler component.

//  The task handler runs for different mode, depeneding on the compile switch
uint8_t FSF_ucComHandleTask(void)
{
/*======================================================================
    Schedule the test server
======================================================================*/
#ifdef TH_FSCOM
   return TH_FSFP_ucServer();
#endif
/*======================================================================
    Schedule the generic
======================================================================*/
#ifndef TH_FSCOM
   return ucHandleTask();
#endif
}

//@}

/*=====================================================================================
    Exported Stub interface implementations
=====================================================================================*/

//  This wrapper function separate the test harness from overloaded application
void    vExtTriggerInternalWatchdog(void)
{
#ifdef TH_FSCOM
   TH_FSFP_PushOutPar((uint8_t)TP_FSF_vTriggerInternalWatchdog);
#else
   _FSF_vTriggerInternalWatchdog();
#endif
}
/*lint -save -e926 -e928 conversion between pointer to pointer MisraC2012-11.5 : tested to work correctly here */
/*Axivion Disable MisraC2012-1.1 : Number of significant initial characters for external identifier not a problem with our tools */
void _FSF_RAM_CON_vFsComModuleCheckBySum(void) {
    _FSF_RAM_CON_vCheckBySum(FS_COM_ucCheckValueRamConBlock,
            (uint8_t) sizeof(FS_COM_RamConBlock),
            (const uint8_t*)(&FS_COM_RamConBlock),
            (uint8_t) (FSF_ERR_RAM_CON_IN_FSCOM));
}

void _FSF_RAM_CON_vFsComModuleCheckByMirror(void) {
    if (ucGetFSCOM_State() != (uint8_t) FSF_STATE_BOOTED) {
        _FSF_RAM_CON_vCheckByMirror((uint8_t) sizeof(FS_COM_RamConBlock),
                (const uint8_t*)(&(FS_COM_RamConBlock)),
                (const uint8_t*)(&(FS_COM_RamConBlockMirror)),
                (uint8_t) (FSF_ERR_RAM_CON_IN_FSCOM));
    } else {
        //do nothing
    }
}

void _FSF_RAM_CON_vFsComModuleUpdateCheckBySum(void) {
    _FSF_RAM_CON_vUpdateCheckBySum((uint8_t*)(&FS_COM_ucCheckValueRamConBlock),
            (uint8_t) sizeof(FS_COM_RamConBlock),
            (const uint8_t*)(&FS_COM_RamConBlock));
}

void _FSF_RAM_CON_vFsComModuleUpdateCheckByMirror(void) {
    _FSF_RAM_CON_vUpdateCheckByMirror((uint8_t) sizeof(FS_COM_RamConBlock),
            (const uint8_t*)(&(FS_COM_RamConBlock)),
            (uint8_t*)(&(FS_COM_RamConBlockMirror)));
}

void _FSF_RAM_CON_vSetErrCntBySum(void) {
    _FSF_RAM_CON_vSetUintBySum((uint16_t*)(void*)(&(FS_COM_RamConBlock.uiErrorCounter)),
            (uint8_t*)(&(FS_COM_ucCheckValueRamConBlock)),
            (FS_COM_RamConBlock.uiErrorCounter + 1u));
}

void _FSF_RAM_CON_vSetErrCntByMirror(void) {
    HSUP_vDisableInt();
    FS_COM_RamConBlock.uiErrorCounter += 1u;
    _FSF_RAM_CON_vUpdateCheckByMirror((uint8_t) sizeof(FS_COM_RamConBlock),
            (const uint8_t*)(&(FS_COM_RamConBlock)),
            (uint8_t*)(&(FS_COM_RamConBlockMirror)));
    HSUP_vEnableInt();
}
/*lint -restore -e926 -e928*/

void _FSF_RAM_CON_vSetErrCntWithoutRamCon(void) {
    FS_COM_RamConBlock.uiErrorCounter += 1u;
}
/*Axivion Enable MisraC2012-1.1 */
/*====================================================================================================
    Test Harness
====================================================================================================*/

/** FSCOM Test Harness.
    This test driver provide the means to access ALL functions the FSCOM implements from
    an externally connected device, that is EDITH or any comparable. The external device
    has to provide the necessary vector in order to get the requested function be called.
    Parameters are passed as part of a static area (TH_FSF_InPar) and return values, if any,
    will be stored in another static area (TH_FSF_OutPar). If FSCOM shall run under test conditions
    the compile switch TH_FSCOM must be known. This can easily be established by adding
    the -DTH_FSCOM to the compiler command line. When running under test conditions, the
    FSCOM task function acts as a test server which is scheduled on a regular base.
   \return     uint8_t\n   TASK_NOT_INITIALISED: in all cases\n

    \pre        FSCOM runs under test conditions\n
    \post       The internal/external watchdogs are serviced correctly.
*/

//@{\name FSCOM test harness in order to test the module within different environments
#ifdef      TH_FSCOM

#ifdef __GNUC__ // for gnu compiler - Cypress
#pragma pack()
#endif  //#ifdef __GNUC__
/*=====================================================================================
    General Test Harness
=====================================================================================*/

extern  void    (* const TH_FSF_fpTestVectorTable[(uint16_t)TH_FSF_LASTENTRY+(uint16_t)1])(void);

void    TH_FSFP_PushInPar(uint8_t val)
{
   TH_FSF_InPar[TH_FSF_InSP] = val;
   if (TH_FSF_InSP < (TH_PAR_SIZE - 1))
   {
      TH_FSF_InSP++;
   }
}

void    TH_FSFP_PushOutPar(uint8_t val)
{
   TH_FSF_OutPar[TH_FSF_OutSP] = val;
   if (TH_FSF_OutSP < (TH_RET_SIZE - 1))
   {
      TH_FSF_OutSP++;
   }
}

void    TH_FSFP_ClearInPar(void)
{
   uint8_t * pDst = TH_FSF_InPar;
   uint8_t   len = (uint8_t)sizeof(TH_FSF_InPar);
   while (len)
   {
      *pDst++ = (uint8_t)0;
      len--;
   }
   TH_FSF_InSP = (uint8_t)0;
}

#ifdef __GNUC__  // for gnu compiler - Cypress
#pragma pack()
#endif  //#ifdef __GNUC__
void    TH_FSFP_ClearOutPar(void)
{
   uint8_t * pDst = TH_FSF_OutPar;
   uint8_t   len = (uint8_t)sizeof(TH_FSF_OutPar);
   while (len)
   {
      *pDst++ = (uint8_t)0;
      len--;
   }
   TH_FSF_OutSP = (uint8_t)0;
}

void    TH_FSFP_no_operation (void)
{

}


/*=====================================================================================
    FSCOM Test Harness
=====================================================================================*/

void    TH_FSFP_vPerformOneTimeChecks(void)
{
   //  Do the test as requested
   vPerformOneTimeChecks();
}

void    TH_FSFP_ucGetFSCOM_State(void)
{
   //  Do the test as requested
   TH_FSFP_PushOutPar(ucGetFSCOM_State());
}

void    TH_FSFP_vSetFSCOM_State(void)
{
   //  Do the test as requested
   vSetFSCOM_State(TH_FSF_InPar[0]);
}

void   TH_FSFP_ucComHandleTask(void)
{
   //  Do the test as requested
   TH_FSFP_PushOutPar((uint8_t)ucComHandleTask(TH_FSF_InPar[0]));
}

void    TH_FSFP_ucNewMonitorCycle(void)
{
   //  Do the test as requested
   TH_FSFP_PushOutPar((uint8_t)ucNewMonitorCycle(TH_FSF_InPar[0]));
}

void    TH_FSFP_vSetChannelIdle(void)
{
   //  Do the test as requested
   vSetChannelIdle(TH_FSF_InPar[0]);
}

void    TH_FSFP_vSetChannelOngoing(void)
{
   //  Do the test as requested
   vSetChannelOngoing(TH_FSF_InPar[0]);
}

void    TH_FSFP_vSetChannelSuccess(void)
{
   //  Do the test as requested
   vSetChannelSuccess(TH_FSF_InPar[0]);
}

void    TH_FSFP_vTriggerExtWatchdog(void)
{
   //  Do the test as requested
   vTriggerExtWatchdog();
}

void    TH_FSFP_bIsChannelIdValid(void)
{
   //  Do the test as requested
   TH_FSFP_PushOutPar((uint8_t)bIsChannelIdValid(TH_FSF_InPar[0]));
}

void    TH_FSFP_bIsChannelCycleOverdue(void)
{
   //  Do the test as requested
   TH_FSFP_PushOutPar((uint8_t)bIsChannelCycleOverdue(TH_FSF_InPar[0]));
}

void    TH_FSFP_ucGetChannelState(void)
{
   //  Do the test as requested
   TH_FSFP_PushOutPar((uint8_t)ucGetChannelState(TH_FSF_InPar[0]));
}

void    TH_FSFP_bMaintainCheckCycleTimer(void)
{
   //  Do the test as requested
   TH_FSFP_PushOutPar((uint8_t)bMaintainCheckCycleTimer());
}

void    TH_FSFP_vBoot(void)
{
   //  Do the test as requested
   vExtBoot();
}

void    TH_FSFP_vContinueStartUp(void)
{
   //  Do the test as requested
   FSF_vContinueStartUp();
}

void    TH_FSFP_ucMonitoringStart(void)
{
   //  Do the test as requested
   TH_FSFP_PushOutPar((uint8_t)ucMonitoringStart());
}

void    TH_FSFP_vBootError(void)
{
   //  Do the test as requested
   FSF_vBootError();
}

void    TH_FSFP_vReportOngoing(void)
{
   //  Do the test as requested
   FSF_vReportOnGoing(TH_FSF_InPar[0]);
}

void    TH_FSFP_vReportError(void)
{
   //  Do the test as requested
   FSF_vReportError(TH_FSF_InPar[0],TH_FSF_InPar[1],TH_FSF_InPar[2]);
}

void    TH_FSFP_vReportSuccess(void)
{
   //  Do the test as requested
   FSF_vReportSuccess(TH_FSF_InPar[0]);
}

void    TH_FSFP_bSuccesInPast(void)
{
   //  Do the test as requested
   TH_FSFP_PushOutPar((uint8_t)FSF_bSuccesInPast(TH_FSF_InPar[0]));
}

void    TH_FSFP_vReportFatal(void)
{
   //  Do the test as requested
   FSF_vReportFatal();
}

void    TH_FSFP_ucHandlePreScheduling(void)
{
   //  Do the test as requested
   TH_FSFP_PushOutPar((uint8_t)ucHandlePreScheduling());
}

void    TH_FSFP_CallChannelOneTimeCheck(void)
{
   //  Do the test as requested
   (* FSF_psChannelTab[TH_FSF_InPar[0]]).FSF_pvOneTimeCheck();
}

void    TH_FSFP_CallChannelPreCondition(void)
{
   //  Do the test as requested
   TH_FSFP_PushOutPar((uint8_t)(* FSF_psChannelTab[TH_FSF_InPar[0]]).FSF_pbPrecondition());
}

void    TH_FSFP_LASTENTRY(void)
{

}

void    TH_FSFP_SetCycleCount(void)
{
   //  Do the test as requested
   uint16_t val = (uint16_t)((TH_FSF_InPar[1]*256) + TH_FSF_InPar[2]);
   FSF_sCH_CheckCycleTime[TH_FSF_InPar[0]].ui16CheckCycleTime = val;
}

void    TH_FSFP_SetMultiplier(void)
{
   //  Do the test as requested
   FSF_sCH_CheckCycleTime[TH_FSF_InPar[0]].ucCycleMultiplier = (uint8_t)(TH_FSF_InPar[1]);
}

void    TH_FSFP_SetRegularCheck(void)
{
   //  Do the test as requested
   T_FSCOM_Channel * pCH = (T_FSCOM_Channel *)FSF_psChannelTab[TH_FSF_InPar[0]];
   pCH->bIsRegularCheck = (bool)(TH_FSF_InPar[1]);
}

/*=====================================================================================
    ROM Channel Test Harness
=====================================================================================*/
void    TH_FSFP_uiCalculateCRC(void)
{
#ifdef FSF_CRC_32_BIT
    struct s_inparCrc {
        uint32_t ui32CRCvalue;
        uint32_t *pData;
        uint16_t ui16Length;
    } * psInparCrc = (struct s_inparCrc *)TH_FSF_InPar;
    union {
        uint8_t   aucOutpar[4];
        uint32_t  uiCRCValue;
    } sretval;

   //  Do the test as requested
   sretval.uiCRCValue = uiCalculateCRC32( psInparCrc->ui32CRCvalue, psInparCrc->pData, psInparCrc->ui16Length);
   //  Provide the result
   TH_FSFP_PushOutPar(sretval.aucOutpar[0]);
   TH_FSFP_PushOutPar(sretval.aucOutpar[1]);
   TH_FSFP_PushOutPar(sretval.aucOutpar[2]);
   TH_FSFP_PushOutPar(sretval.aucOutpar[3]);

#else
   //  Note: a portable input structure must be used in order to support different banking models
   struct s_inpar {
       uint16_t ui16CRCvalue;
       uint8_t  *pData;
       uint16_t ui16Length;
       } * psInpar = (struct s_inpar *)TH_FSF_InPar;

   union {
       uint8_t   aucOutpar[2];
       uint16_t  uiCRCValue;
       } sretval;

   //  Do the test as requested
   sretval.uiCRCValue = uiCalculateCRC( psInpar->ui16CRCvalue, psInpar->pData, psInpar->ui16Length);
   //  Provide the result
   TH_FSFP_PushOutPar(sretval.aucOutpar[0]);
   TH_FSFP_PushOutPar(sretval.aucOutpar[1]);
#endif
}

void    TH_FSFP_uiSetupNextROMAddress(void)
{

   //  Note: a portable input structure must be used in order to support different banking models
   union u_inpar {
       uint8_t       inpar[sizeof(uint16_t)];
       uint16_t      ui16Length;
       } * psInpar = (union u_inpar *)TH_FSF_InPar;

   union {
       uint8_t   aucOutpar[2];
       uint16_t  uiROMAddress;
       } sretval;

   //  Do the test as requested
   sretval.uiROMAddress = uiSetupNextROMAddress(psInpar->ui16Length);
   //  Provide the result
   TH_FSFP_PushOutPar(sretval.aucOutpar[0]);
   TH_FSFP_PushOutPar(sretval.aucOutpar[1]);
}

void    TH_FSFP_ucInitialiseROMChannel(void)
{
   //  Do the test as requested
   TH_FSFP_PushOutPar(ucInitialiseROMChannel());
}

void    TH_FSFP_ucROMHandleTask(void)
{
   //  Do the test as requested
   TH_FSFP_PushOutPar(FSF_ucRomHandleTask());
}

void    TH_FSFP_bExtIsROMReady(void)
{
   TH_FSFP_PushOutPar((uint8_t)bExtIsROMReady((uint8_t*)0x4711,(uint16_t)1));
}

void    TH_FSFP_pucExtROMRead(void)
{
   union {
         uint8_t   *p;
         uint8_t   outpar[sizeof(uint8_t*)];
         } opar;
   uint8_t i;

   opar.p = pucExtROMRead((uint8_t*)0x4711,(uint16_t)1);

   for (i=0;i < (uint8_t)(sizeof(uint8_t*));i++)
   {
      TH_FSFP_PushOutPar(opar.outpar[i]);
   }
}

void    TH_FSFP_bGetROMConfig(void)
{
   TH_FSFP_PushOutPar(ROMConfig[0]);
   TH_FSFP_PushOutPar(ROMConfig[1]);
}

void    TH_FSFP_ucInternalROMTask(void)
{
   TH_FSFP_PushOutPar(ucInternalROMTask());
}

void    TH_FSFP_bIsROMAtOnceAllowed(void)
{
   TH_FSFP_PushOutPar((uint8_t)bIsROMAtOnceAllowed());
}

void    TH_FSFP_SCH_ucHandleTask(void)
{
   TH_FSFP_PushOutPar(ucHandleTask());
}

void    TH_FSFP_SCH_vSetMainTable(void)
{

}

void    TH_FSFP_bGetSchedulerConfig(void)
{
   TH_FSFP_PushOutPar(TH_FSF_HasScheduler);
}

/*=====================================================================================
    Stack Channel Test Harness
=====================================================================================*/

void    TH_FSFP_vInitialzeStack(void)
{
   vInitialzeStack();
}

void    TH_FSFP_bStackPrecondition(void)
{
   TH_FSFP_PushOutPar((uint8_t)FSF_bStackPrecondition());
}

void    TH_FSFP_ucStackHandleTask(void)
{
   TH_FSFP_PushOutPar(FSF_ucStackHandleTask());
}

void    TH_FSFP_GetucActualStack(void)
{
   TH_FSFP_PushOutPar(ucActualStack);
}

void    TH_FSFP_GetStackConfig(void)
{
   TH_FSFP_PushOutPar(StackConfig);
}

void    TH_FSFP_GetSimStack(void)
{
   union {
       uint8_t   SimStack[sizeof(uint32_t)];
       uint8_t * pucSimStack;
       } uRetVal = {{0,0,0,0}};

   uRetVal.pucSimStack = (uint8_t *)(_FSF_Stack);

   TH_FSFP_PushOutPar(uRetVal.SimStack[0]);
   TH_FSFP_PushOutPar(uRetVal.SimStack[1]);
   TH_FSFP_PushOutPar(uRetVal.SimStack[2]);
   TH_FSFP_PushOutPar(uRetVal.SimStack[3]);
}

void    TH_FSFP_GetStackPointer(void)
{

   union {
       uint8_t   SimStack[sizeof(uint32_t)];
       uint8_t * pucSimStack;
       } uRetVal = {{0,0,0,0}};


   //  Return the actual content of the stack pointer

   if (TH_FSF_InPar[0])
   {
      uRetVal.pucSimStack = TH_FSF_StackPointer_1;
   }
   else
   {
      uRetVal.pucSimStack = TH_FSF_StackPointer_0;
   }

   TH_FSFP_PushOutPar(uRetVal.SimStack[0]);
   TH_FSFP_PushOutPar(uRetVal.SimStack[1]);
   TH_FSFP_PushOutPar(uRetVal.SimStack[2]);
   TH_FSFP_PushOutPar(uRetVal.SimStack[3]);
}

void    TH_FSFP_GetucStackCount(void)
{
   TH_FSFP_PushOutPar(TH_ucStackCount);
}

void    TH_FSFP_pucGetStackPointer(void)
{

   union {
       uint8_t   SimStack[sizeof(uint32_t)];
       uint8_t * pucSimStack;
       } uRetVal = {{0,0,0,0}};

   // Call this function under test
   uRetVal.pucSimStack = pucGetStackPointer (TH_FSF_InPar[0]);

   TH_FSFP_PushOutPar(uRetVal.SimStack[0]);
   TH_FSFP_PushOutPar(uRetVal.SimStack[1]);
   TH_FSFP_PushOutPar(uRetVal.SimStack[2]);
   TH_FSFP_PushOutPar(uRetVal.SimStack[3]);
}

/*=====================================================================================
    ISR Channel Test Harness
=====================================================================================*/

void    TH_FSFP_bCheckShortInterrupts(void)
{
   TH_FSFP_PushOutPar((uint8_t)bCheckShortInterrupts(&FSF_atIsrShortCntrTbl[TH_FSF_InPar[0]],TH_FSF_InPar[0]));
}

void    TH_FSFP_ucIsrHandleTask(void)
{
   TH_FSFP_PushOutPar(FSF_ucIsrHandleTask());
}

void    TH_FSFP_vIsrDoEntry(void)
{
   vIsrDoEntry(TH_FSF_InPar[0]);
}

void    TH_FSFP_vIsrDoExit(void)
{
   vIsrDoExit(TH_FSF_InPar[0]);
}

void    TH_FSFP_vIsrDoShortEntry(void)
{
   vIsrDoShortEntry(TH_FSF_InPar[0]);
}

void    TH_FSFP_bCheckInterrupts(void)
{
   TH_FSFP_PushOutPar((uint8_t)bCheckInterrupts(&FSF_atIsrCntrTbl[TH_FSF_InPar[0]],TH_FSF_InPar[0]));
}

/*=====================================================================================
    Register Channel Test Harness
=====================================================================================*/

void    TH_FSFP_ucRegHandleTask(void)
{
   TH_FSFP_PushOutPar(FSF_ucRegHandleTask());
}

/*=====================================================================================
    RAM Channel Test Harness
=====================================================================================*/

void    TH_FSFP_ucRamHandleTask(void)
{
   TH_FSFP_PushOutPar(FSF_ucRamHandleTask());
}

void    TH_FSFP_bGetRAMConfig(void)

{
      TH_FSFP_PushOutPar(RAMConfig[0]);
      TH_FSFP_PushOutPar(RAMConfig[1]);
}

void    TH_FSFP_vRam_OperationalException(void)
{
   FSF_vRam_OperationalException();
}

void    TH_FSPF_vRamOpercheck(void)
{
#if defined(HC08) || defined(HCS08)

   static  struct  _RAMpar {
                       uint8_t * pucRAM;
                       uint8_t * pucDEXBuffer;
                       uint8_t   ucPatNoInv;
                       uint8_t   ucPatInv;
                       } *pRAMpar = (struct _RAMpar *)TH_FSF_InPar;


   static  union   _SepStack   {
                       uint8_t           b[sizeof(uint8_t *)];
                       const uint8_t *   pSepStack;
                       } outpar;

   //  ***********************************************************************************
   //  Do the C high-level language coded task (its more efficient for
   //  those kind of controllers)
   //  ***********************************************************************************
   FSF_vRamOpercheck(  pRAMpar->pucRAM, pRAMpar->pucDEXBuffer, pRAMpar->ucPatNoInv, pRAMpar->ucPatInv);

   outpar.pSepStack = (uint8_t *)(SPRESERVED);

   TH_FSFP_PushOutPar(outpar.b[0]);
   TH_FSFP_PushOutPar(outpar.b[1]);
#endif
}

/*=====================================================================================
    RAM Consistency Test Harness
=====================================================================================*/

void    TH_FSFP_vGetRAMConsisCheck_State(void)
{
   TH_FSFP_PushOutPar(true);
}

void    TH_FSFP_RAM_CON_vInitFsRamArea(void)
{
   //FSF_vRamConInitFsRamArea();
}

/*-----------------------------------------------------------------------
  RAM Consistency Test using calculating the check value over a given memory area
-------------------------------------------------------------------------*/

//  Note: a portable input structure must be used in order to support different banking models
struct s_inpar {
    uint8_t ucLength;
    uint8_t ucDummy;           // due memory alignment
    uint8_t* pucStartAddress;
    } * psInpar = (struct s_inpar *)TH_FSF_InPar;
void TH_FSFP_RAM_CON_vCalcCheckBasic(void)
{
//   TH_FSFP_PushOutPar(FSF_RAM_CON_vCalcCheckBasic(psInpar->ucLength,psInpar->pucStartAddress));
    /// todo test harness code should be updated
}


//  Note: a portable input structure must be used in order to support different banking models
struct s_inpar1 {
    uint8_t *pucVariable;
    uint8_t  ucNewVal;
    uint8_t  ucDummy;           // due memory alignment
    uint8_t* pucCheckValue;
    } * psInpar1 = (struct s_inpar1 *)TH_FSF_InPar;
void TH_FSFP_RAM_CON_vSetUcharSecBasic(void)
{
   //FSF_RAM_CON_vSetUcharSecBasic(psInpar1->pucVariable,psInpar1->ucNewVal,psInpar1->pucCheckValue);
   /// todo test harness code should be updated
}

//  Note: a portable input structure must be used in order to support different banking models
struct s_inpar2 {
    uint16_t *puiVariable;
    uint16_t uiNewVal;
    uint8_t* pucCheckValue;
    } * psInpar2 = (struct s_inpar2 *)TH_FSF_InPar;
void TH_FSFP_RAM_CON_vSetUintSecBasic(void)
{
   //FSF_RAM_CON_vSetUintSecBasic(psInpar2->puiVariable,psInpar2->uiNewVal,psInpar2->pucCheckValue);
    /// todo test harness code should be updated
}

/*-----------------------------------------------------------------------
  RAM Consistency Test using RAM mirroring a given memory area
-------------------------------------------------------------------------*/

//  Note: a portable input structure must be used in order to support different banking models
struct s_inpar3 {
    uint8_t* pucStartAddress;
    uint8_t* pucEndAddress;
    } * psInpar3 = (struct s_inpar3 *)TH_FSF_InPar;
void    TH_FSFP_RAM_CON_vUpdateFsMirrorRam(void)
{
   //FSF_RAM_CON_vUpdateFsMirrorRam(psInpar3->pucStartAddress, psInpar3->pucEndAddress);
   /// todo test harness code should be updated
}

void TH_FSFP_RAM_CON_bCheckFsRamRange(void)
{
   //TH_FSFP_PushOutPar(FSF_RAM_CON_bCheckFsRamRange(psInpar3->pucStartAddress, psInpar3->pucEndAddress));
    /// todo test harness code should be updated
}

/*=====================================================================================
    Test Harness Vector Table
=====================================================================================*/
//  Test vectors, one for each function, so the server is able to perform the test step-by-step

void    (* const TH_FSF_fpTestVectorTable[(uint16_t)TH_FSF_LASTENTRY+(uint16_t)1])(void) = {
   TH_FSFP_no_operation,                     //  0
   TH_FSFP_vPerformOneTimeChecks,            //  1
   TH_FSFP_ucGetFSCOM_State,                 //  2
   TH_FSFP_vSetFSCOM_State,                  //  3
   TH_FSFP_ucNewMonitorCycle,                //  4
   TH_FSFP_ucComHandleTask,                  //  5
   TH_FSFP_vSetChannelIdle,                  //  6
   TH_FSFP_vSetChannelOngoing,               //  7
   TH_FSFP_vSetChannelSuccess,               //  8
   TH_FSFP_vTriggerExtWatchdog,              //  9
   TH_FSFP_bIsChannelIdValid,                //  10
   TH_FSFP_bIsChannelCycleOverdue,           //  11
   TH_FSFP_ucGetChannelState,                //  12
   TH_FSFP_bMaintainCheckCycleTimer,         //  13
   TH_FSFP_vBoot,                            //  14
   TH_FSFP_vContinueStartUp,                 //  15
   TH_FSFP_ucMonitoringStart,                //  16
   TH_FSFP_vBootError,                       //  17
   TH_FSFP_vReportOngoing,                   //  18
   TH_FSFP_vReportError,                     //  19
   TH_FSFP_vReportSuccess,                   //  20
   TH_FSFP_bSuccesInPast,                    //  21
   TH_FSFP_vReportFatal,                     //  22
   TH_FSFP_ucHandlePreScheduling,            //  23
   TH_FSFP_CallChannelOneTimeCheck,          //  24
   TH_FSFP_CallChannelPreCondition,          //  25
   TH_FSFP_ClearInPar,                       //  26
   TH_FSFP_ClearOutPar,                      //  27
   TH_FSFP_SetCycleCount,                    //  28
   TH_FSFP_SetMultiplier,                    //  29
   TH_FSFP_SetRegularCheck,                  //  30
   // ROM Channel support
   TH_FSFP_uiCalculateCRC,                   //  31
   TH_FSFP_uiSetupNextROMAddress,            //  32
   TH_FSFP_ucInitialiseROMChannel,           //  33
   TH_FSFP_ucROMHandleTask,                  //  34
   TH_FSFP_bExtIsROMReady,                   //  35
   TH_FSFP_pucExtROMRead,                    //  36
   // Pseudo Scheduler
   TH_FSFP_SCH_ucHandleTask,                 //  37
   TH_FSFP_SCH_vSetMainTable,                //  38
   // Stack Channel
   TH_FSFP_vInitialzeStack,                  //  39
   TH_FSFP_bStackPrecondition,               //  40
   TH_FSFP_ucStackHandleTask,                //  41
   //  ISR Channel(s)
   TH_FSFP_bCheckShortInterrupts,            //  42
   TH_FSFP_ucIsrHandleTask,                  //  43
   TH_FSFP_vIsrDoEntry,                      //  44
   TH_FSFP_vIsrDoExit,                       //  45
   TH_FSFP_vIsrDoShortEntry,                 //  46
   TH_FSFP_bCheckInterrupts,                 //  47
   TH_FSFP_bGetROMConfig,                    //  48
   TH_FSFP_bGetSchedulerConfig,              //  49
   //  Register Channel
   TH_FSFP_ucRegHandleTask,                  //  50
   //  More ROM channel related things
   TH_FSFP_ucInternalROMTask,                //  51
   TH_FSFP_bIsROMAtOnceAllowed,              //  52
    //  RAM channel related things
   TH_FSFP_ucRamHandleTask,                  //  53
   TH_FSFP_bGetRAMConfig,                    //  54
    //  some more for the stack channel
   TH_FSFP_GetucActualStack,                 //  55
   TH_FSFP_GetStackConfig,                   //  56
   TH_FSFP_GetSimStack,                      //  57
   TH_FSFP_GetStackPointer,                  //  58
   TH_FSFP_GetucStackCount,                  //  59
   TH_FSFP_pucGetStackPointer,               //  60
    //  still RAM channel related
   TH_FSFP_vRam_OperationalException,        //  61
   TH_FSPF_vRamOpercheck,                    //  62
    //  RAM Consistency Check related
   TH_FSFP_vGetRAMConsisCheck_State,         //  63
   TH_FSFP_RAM_CON_vInitFsRamArea,           //  64
   TH_FSFP_RAM_CON_vCalcCheckBasic,          //  65
   TH_FSFP_RAM_CON_vSetUcharSecBasic,        //  66
   TH_FSFP_RAM_CON_vSetUintSecBasic,         //  67
   TH_FSFP_RAM_CON_vUpdateFsMirrorRam,       //  68
   TH_FSFP_RAM_CON_bCheckFsRamRange,         //  69
   // Last entry
   TH_FSFP_LASTENTRY};                       //  68


/**
    The Test harness test server
*/
uint8_t   TH_FSFP_ucServer(void)
{
   if ((TH_FSF_Function > (uint8_t)TH_FSF_no_operation) && (TH_FSF_Function < (uint8_t)TH_FSF_LASTENTRY))
   {
      //  Timestamp at start of test step
      TH_FSF_StartOfTest  = TIM_uiGetCircleMicroSeconds();
      (* TH_FSF_fpTestVectorTable[TH_FSF_Function])();
      //  Timestamp at end of test step
      TH_FSF_EndOfTest  = TIM_uiGetCircleMicroSeconds();

      if (((uint16_t)(TH_FSF_EndOfTest - TH_FSF_StartOfTest)) > TH_FSF_MaxExecTime)
      {
         TH_FSFP_PushOutPar((uint8_t)TP_FSF_SCH_ExecTimeViolation);
      }
   }
   //  Make sure the current step has been performed only once
   TH_FSF_Function = (uint8_t)TH_FSF_no_operation;
   //  Trigger the external watchdog
   _FSF_vTriggerExtWatchdog();
   //  Trigger the internal watchdog
   _FSF_vTriggerInternalWatchdog();
   //  Return this value as this will not scheduled channels
   return  (uint8_t)TASK_INITIALISED;
}

#endif

//@}
