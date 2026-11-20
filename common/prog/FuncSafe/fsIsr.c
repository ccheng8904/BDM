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
/** \file fsIsr.c
 *
 *  \ingroup  <DOXY_GROUP_FSF_STACK>
 *
 *  \brief    Implementation of package fsIsr (interrupt check) of library \n
 *            functional safety.
 *
 *  \details  Runtime and frequency of interrupts will be\n
 *            checked and compared against upper and lower limits.
 *\n
 *\n
 *<b>Functional Safety Interrupt Supervision Component - Top-Level Design</b>
 *\n
 *\n
 *\li<b>Package Model </b>\n
 *\image html Package_Isr.jpg
 *\n
 *<b>Description </b>\n
 *The fsIsr package provides the parts outlined in the model. By compilation switch, the Isr task handler may run
 *either in the usual operating mode or in test harness mode. When running in the usual operating mode, the test
 *harness part will not be linked to the package. When running in test harness mode, stub services are provided, so
 *the real Isr interfaces are separated from being called and only the test server will be able to call it.
 *\n
 *<b>Rationale </b>\n
 *By the test harness part, it is possible to test all functions inside the Isr handler fully isolated from other
 *parts of the whole system. This is an important issue, otherwise it would be very difficult to test the code at
 *asynchronous level.
 *\n
 *\n
 *\n
 *\li<b>State Model </b>\n
 *\image html StateModel_Isr.jpg
 *\n
 *<b>Description </b>\n
 *Checking interrupts on every call of the generic is not efficient, thus an artificial state, called "No Operation"
 *has been added. So, a check takes place on eveny 65 milliseconds. When elapsed, the Isr task handler passes all
 *states, one per call of the generic. If all states have been passed, the 65 milliseonds condition gets cleared and
 *the Isr task handler does a transition back into state "No Operation".
 *\n
 *<b>Rationale </b>\n
 *The Isr task handler does not have a its own initialisation routine. A pre-scheduling mechanism is provided by
 *the generic which has to be configured sufficiently by a concrete application. At its end, it must be guaraneed that
 *interrupt events can be measured correctly.
 *\n
 *\n
 *\n
 *\li<b>Scenario: Interrupt Supervision </b>\n
 *\image html SequenceChart_InterruptSupervision.jpg
 *\n
 *<b>Description </b>\n
 *Its clear, this scenario initially starts in state "No Operation", and it has to be noticed that interrupts can't be
 *and are not disabled during the whole process. They get disabled shortly when taking data. Each time the generic
 *calls the Isr task handler, the Isr task handler passes the state it is actually in, performs its partial task and
 *finally, for all kind of interrupts, it notifies ongoing to the generic.
 *\n
 *<b>Rationale </b>\n
 *In an operational system, interrupts occur on every moment in time. So, the Ist task handler is always busy to perform
 *its measurement. For that raeson, there is no successful notification to the generic and, by a concrete application,
 *Isr channel are not allowed to configure the run time limitation.
 *\n
 *\n
 *\n
 *\li<b>Input Document Reference(s) </b>\n
 *\n
 *<TABLE style="text-align: center">
 *<TR style="background-color:rgb(255,255,206)">
 *<TD>Reference</TD>
 *<TD>Ducument Title</TD>
 *<TD>Document Reference Name/Number</TD>
 *<TD>Version</TD>
 *<TD>Status</TD>
 *<TD>Release Date</TD>
 *</TR>
 *<TR>
 *<TD> [1] </TD>
 *<TD> Functional Safety Software Design document </TD>
 *<TD> FuncSafeSWSpec.pdf / 5560 0000003770 </TD>
 *<TD> Version 2.0 </TD>
 *<TD> Proposed </TD>
 *<TD> 08.05.2008 </TD>
 *</TR>
 *<TR>
 *<TD> [2] </TD>
 *<TD> Generic Functional Safety Software Specification</TD>
 *<TD> GenericFuncSafeSpec.pdf / 5560 0000007751 </TD>
 *<TD> Version 2.0 </TD>
 *<TD> Proposed </TD>
 *<TD> 08.05.2008 </TD>
 *</TR>
 *</TABLE>
 *\n
 */
#include "bsh_stdinc.h"
#include "LibTypes.h"
#include "FuncSafe/fSafe.h"
#include "FuncSafe/fSafeXS.h"
#include "FuncSafe/fspriv.h"
#include "hsup.h"

//  The HCS12 compiler popped up a warning, if multiple function calls have been combined into one
//  subruotine. So, debugging such optimized code is getting worse or not possible.
#if defined(MC9S12GC96) && !defined(WIN32)
#pragma MESSAGE DISABLE C12056
#endif

/* Local macros ***************************************************************************************/

/* Local type definitions *****************************************************************************/

/* RAM/ROM Variable prototypes ************************************************************************/

/* Function prototypes ********************************************************************************/
STATIC  bool    bCheckInterrupts(TisrControl FSTH_CONST * pIsrCtrl, uint8_t ucIsrId);
STATIC  bool    bCheckShortInterrupts(TisrShortControl FSTH_CONST * pShortIsrCtrl, uint8_t ucIsrId);

/* RAM/ROM Variable definitions ***********************************************************************/

#ifdef TH_FSCOM
/*=====================================================================================
    ISR Channel Test Harness
=====================================================================================*/
/**
        Application related definitions for test purposes
*/
enum    FSF_eFullIsrControl{
                TH_ISR_0,
                TH_ISR_1,
                TH_ISR_2,
                TH_ISR_3,
                FS_ISR_NUMBER_OF_LARGE_ISR};

enum    FSF_eShortIsrControl{
                TH_SHISR_0,
                TH_SHISR_1,
                FS_ISR_NUMBER_OF_SHORT_ISR};

/**
        Application data area created for test purposes
*/

//  uiLimitLatency, uiLimitMaxFrequency, uiLimitMinFrequency, uiCycleTime, ucControl
FSTH_CONST TisrControl       FSF_atIsrCntrTbl[FS_ISR_NUMBER_OF_LARGE_ISR] = {
    {100,   0,    0, 10000/256, FS_ISR_CONTROL_NONE},        //  TH_ISR_0,
    {100,   0,    0, 10000/256, FS_ISR_CONTROL_NONE},        //  TH_ISR_1,
    {100,   0,    0, 10000/256, FS_ISR_CONTROL_NONE},         //  TH_ISR_2,
    {100,   0,    0, 10000/256, FS_ISR_CONTROL_NONE}         //  TH_ISR_3
    };

//  uiLimitMaxFrequency, uiLimitMinFrequency, uiCycleTime, ucControl
FSTH_CONST TisrShortControl  FSF_atIsrShortCntrTbl[FS_ISR_NUMBER_OF_SHORT_ISR] = {
    {       0,    0, 10000/256, FS_ISR_CONTROL_NONE},        //  TH_SHISR_0
    {       0,    0, 10000/256, FS_ISR_CONTROL_NONE}         //  TH_SHISR_1
    };

uint16_t  FSF_auiIsrTime      [FS_ISR_NUMBER_OF_LARGE_ISR];
uint16_t  FSF_auiIsrCount     [FS_ISR_NUMBER_OF_LARGE_ISR];
uint16_t  FSF_auiShortIsrCount[FS_ISR_NUMBER_OF_SHORT_ISR];

volatile    FSTH_CONST   uint8_t   FSF_ucNumberOfShortInterrupts   = (uint8_t)FS_ISR_NUMBER_OF_SHORT_ISR;
volatile    FSTH_CONST   uint8_t   FSF_ucNumberOfInterrupts        = (uint8_t)FS_ISR_NUMBER_OF_LARGE_ISR;
#endif

/*=====================================================================================
    ISR Channel work area
=====================================================================================*/
STATIC uint8_t  ucIsrLastTimerValue;                    //!<Value of the timer last time. Needed for checking overflow.
STATIC uint16_t uiIsrTimeStamp;                         //!<Time stamp used by IsrDoEntry and IsrDoExit to calculate interrupt duration.
STATIC uint16_t uiIsrTimer[(uint8_t)FS_ISR_NUMBER_OF_LARGE_ISR];      ///<[65 ms]
STATIC uint16_t uiShortIsrTimer[(uint8_t)FS_ISR_NUMBER_OF_SHORT_ISR]; ///<[65 ms]
STATIC bool     bSynchronisedWithCirculatingTimer;      //-->private
STATIC bool     bCalcTimerCrossed65ms = false;
//  PCA: CR_001-16-02-2007
STATIC uint8_t  ucTimeOverflow = (uint8_t)(NO_ISR_TIME_OVERFLOW);     //!< In case of time overflow, id of interrupt(s)
STATIC uint8_t  ucCountOverflow = (uint8_t)(NO_ISR_COUNT_OVERFLOW);   //!< In case of count overflow, id of interrupt(s)
//  End of PCA: CR_001-16-02-2007

/* local function definitions ************************************************************************/

/**
* Local function called by fsIsr task handler.
*
* This function check so-called "long" running interrupts.
*
* \return   bool
*/
STATIC  bool    bCheckInterrupts(TisrControl FSTH_CONST * pIsrCtrl, uint8_t ucIsrId)
{   /*Axivion Disable MisraC2012-18.1 : "Access into array might be out of bounds" : false positive here */

   //  Has the configured latency elapsed ?
   if (uiIsrTimer[ucIsrId] >= pIsrCtrl->uiCycleTime)
   //  Msg(2:0506) Dereferencing pointer value that is possibly NULL.
   //  REFERENCE - ISO-6.3.3.2 Semantics
   //  PCA comment: code tested and reviewed to work correctly. Thus, left unchanged.
   {
      uiIsrTimer[ucIsrId] = 0u;
/*======================================================================
    Update uiCount and uiTime synchronized
======================================================================*/

      //  Make a local copy of data changed at asynchronous level
#ifdef TH_FSCOM
      TH_FSFP_PushOutPar(TP_FSF_ISR_INT_Disabled);
#else
      HSUP_vDisableInt();
#endif

      uint16_t    uiCount = FSF_auiIsrCount[ucIsrId]; //number of occurred events of current interrupt
      uint16_t    uiTime = FSF_auiIsrTime[ucIsrId];   //cumulative latency of current interrupt

      FSF_auiIsrCount[ucIsrId] = 0u;
      FSF_auiIsrTime[ucIsrId] = 0u;
    /*Axivion Enable MisraC2012-18.1 */
/*=======================================================
    Clear the ISR configuration
=======================================================*/
#ifdef FSF_ISRCONFIG
      FSF_vIsrClearConf(ucIsrId);
#endif


#ifdef TH_FSCOM
      TH_FSFP_PushOutPar(TP_FSF_ISR_INT_Enabled);
#else
      HSUP_vEnableInt();
#endif

/*======================================================================
    Latency check
======================================================================*/
      //*** Latency Time check ****
      //  Is time management active?
      if ((pIsrCtrl->ucControl & (uint8_t)(FS_ISR_LATENCY_CHECK)) != (uint8_t)0)
      {
         //  Prevent zero division
         if (uiCount != (uint16_t)0)
         {
            //  Has max. latency exceeded
            if ((uiTime/uiCount) > pIsrCtrl->uiLimitLatency)
            {
               FSF_vReportError((uint8_t)FSF_ISR_LATENCY_CHECK,(uint8_t)FSF_ERR_LATENCY_ERROR,ucIsrId);
               return false;
            }
         }
      }

/*======================================================================
    Max Frequency check
======================================================================*/
      //*** MAX Frequency Check ****
      if ((pIsrCtrl->ucControl & (uint8_t)(FS_ISR_MAXIMUM_CHECK)) != (uint8_t)0)
      {
         if (uiCount >= pIsrCtrl->uiLimitMaxFrequency)
         {
            FSF_vReportError((uint8_t)FSF_ISR_MAX_FREQUENCY_CHECK,(uint8_t)FSF_ERR_MAX_FREQ_ERROR,ucIsrId);

            return false;
         }
      }

/*======================================================================
    Min Frequency check
======================================================================*/
      //*** MIN Frequency Check ****
      if ((pIsrCtrl->ucControl & (uint8_t)(FS_ISR_MINIMUM_CHECK)) != (uint8_t)0)
      {
         if (uiCount < pIsrCtrl->uiLimitMinFrequency)
         {
            FSF_vReportError((uint8_t)FSF_ISR_MIN_FREQUENCY_CHECK,(uint8_t)FSF_ERR_MIN_FREQ_ERROR,ucIsrId);
            return false;
         }
      }
   }
   return true;
}

/**
* Local function called by fsIsr task handler.
*
* This function will be replaced by an empty function definition in the excess template in case no short interrupt control is needed, according to the definition: FS_USE_SHORT_ISR_CONTROL
*
* \return bool
*/

STATIC  bool    bCheckShortInterrupts(TisrShortControl FSTH_CONST * pShortIsrCtrl, uint8_t ucIsrId)
{
   uint16_t uiCount;                       // number of occurred events of current interrupt

   //  Has the configured latency elapsed ?
   if (uiShortIsrTimer[ucIsrId] >= pShortIsrCtrl->uiCycleTime)
   //  Msg(2:0506) Dereferencing pointer value that is possibly NULL.
   //  REFERENCE - ISO-6.3.3.2 Semantics
   //  PCA comment: code tested and reviewed to work correctly. Thus, left unchanged.
   {
      uiShortIsrTimer[ucIsrId] = 0u;
/*======================================================================
    Update uiCount synchronized
======================================================================*/
      /* ISR changed data require some local copy
      */
#ifdef TH_FSCOM
      TH_FSFP_PushOutPar(TP_FSF_ISR_INT_Disabled);
#else
      HSUP_vDisableInt();
#endif

      uiCount = FSF_auiShortIsrCount[ucIsrId];
      FSF_auiShortIsrCount[ucIsrId] = 0;

/*=======================================================
    Clear the short ISR configuration
=======================================================*/
#ifdef FSF_ISRCONFIG
      FSF_vIsrClearShortConf(ucIsrId);
#endif

#ifdef TH_FSCOM
      TH_FSFP_PushOutPar(TP_FSF_ISR_INT_Enabled);
#else
      HSUP_vEnableInt();
#endif

/*======================================================================
    Max Frequency check
======================================================================*/
      if ((pShortIsrCtrl->ucControl & (uint8_t)(FS_ISR_MAXIMUM_CHECK)) != (uint8_t)0)
      {
         if (uiCount >= pShortIsrCtrl->uiLimitMaxFrequency)
         {
            FSF_vReportError((uint8_t)FSF_ISR_MAX_FREQUENCY_CHECK,(uint8_t)FSF_ERR_MAX_FREQ_ERROR,(uint8_t)(ucIsrId + FSF_ucNumberOfInterrupts));
            return false;
         }
      }

/*======================================================================
    Min Frequency check
======================================================================*/
      if ((pShortIsrCtrl->ucControl & (uint8_t)(FS_ISR_MINIMUM_CHECK)) != (uint8_t)0)
      {
         if (uiCount < pShortIsrCtrl->uiLimitMinFrequency)
         {
            FSF_vReportError((uint8_t)FSF_ISR_MIN_FREQUENCY_CHECK,(uint8_t)FSF_ERR_MIN_FREQ_ERROR,(uint8_t)(ucIsrId + FSF_ucNumberOfInterrupts));
            return false;
         }
      }
   }
   return true;
}

/* external function definitions ***********************************************************************/

uint8_t FSF_ucIsrHandleTask(void)
{
   uint8_t                       ucTime;                   //saves the time stamp, resolution 256 us
   uint8_t                       ucIsrId;                  //loop counter for all interrupts
   TisrShortControl FSTH_CONST *    pShortIsrCtrl;
   TisrControl      FSTH_CONST *    pIsrCtrl;

/*=======================================================
    Get current value of circulating timer (in pieces of 256 micro-seconds)
=======================================================*/
#ifdef TH_FSCOM
   ucTime = (uint8_t)TH_FSF_Circle256MicroSeconds;  // when in test harness mode, read this from variable content
#else
   ucTime = (uint8_t)(TIM_uiGetCircleMicroSeconds()>>8); //read high byte of circulating timer (1Tick = 256us)
#endif

/*=======================================================
    Did the circulating timer cross zero since last call (i.e. 65536us elapsed)?
=======================================================*/
   if (bCalcTimerCrossed65ms == false)
   {
      if (ucIsrLastTimerValue > ucTime)
      {
         //  PCA: CR_001-16-02-2007
         uint8_t ucActCountOverflow;
         uint8_t ucActTimeOverflow;

         //  Msg(3:3197) The initialiser for 'ucActCountOverflow' is always modified before being used. <next>
         //  Msg(3:3197) The initialiser for 'ucActTimeOverflow' is always modified before being used.
         //  PCA comment: code tested and reviewed to work correctly. Thus, left unchanged.
         bool  bIsOverflow         = false;

#ifdef TH_FSCOM
         TH_FSFP_PushOutPar(TP_FSF_ISR_INT_Disabled);
#else
         HSUP_vDisableInt();
#endif
         //  Get the actual values accessed at asynchronous level
         ucActTimeOverflow  = ucTimeOverflow;
         ucActCountOverflow = ucCountOverflow;

#ifdef TH_FSCOM
         TH_FSFP_PushOutPar(TP_FSF_ISR_INT_Enabled);
#else
         HSUP_vEnableInt();
#endif
         //  End of PCA: CR_001-16-02-2007

         // Increase the number of 65ms packets elapsed
         for(uint32_t i = 0; i < (uint8_t)FS_ISR_NUMBER_OF_LARGE_ISR; i++)
         {
            uiIsrTimer[i]++;
         }
         
         for(uint32_t i = 0; i < (uint8_t)FS_ISR_NUMBER_OF_SHORT_ISR; i++)
         {
            uiShortIsrTimer[i]++;
         }

         bCalcTimerCrossed65ms   = true;

         //  PCA: CR_001-16-02-2007
         //  Care has to be taken concerning initialisation: bSynchronisedWithCirculatingTimer set true means,
         //  the ISR task handler is in sync, otherwise handling an overflow conditions are not allowed!
         if((ucActTimeOverflow != (uint8_t)NO_ISR_TIME_OVERFLOW) && (bSynchronisedWithCirculatingTimer == true))
         {
            // Latency overflow. Report error!
            FSF_vReportError((uint8_t)FSF_ISR_LATENCY_CHECK,(uint8_t)FSF_ERR_LATENCY_ERROR,ucActTimeOverflow);
            // Signal overflow
            bIsOverflow = true;
         }
         else
         {
            if((ucActCountOverflow != (uint8_t)NO_ISR_COUNT_OVERFLOW) && (bSynchronisedWithCirculatingTimer == true))
            {
               // Interrupts count overflow! Report error!
               FSF_vReportError((uint8_t)FSF_ISR_MAX_FREQUENCY_CHECK,(uint8_t)FSF_ERR_MAX_FREQ_ERROR,ucActCountOverflow);

               // Signal overflow
               bIsOverflow = true;
            }
         }
         // Reset actual values
         if(bIsOverflow == true)
         {
            // In case of an overflow condition, reset both so only the first event get notified.
            // Stop counting events

#ifdef TH_FSCOM
            TH_FSFP_PushOutPar(TP_FSF_ISR_INT_Disabled);
#else
            HSUP_vDisableInt();
#endif
            // Reset the time overflow flag
            ucTimeOverflow = (uint8_t)(NO_ISR_TIME_OVERFLOW);

            // Reset the count overflow
            ucCountOverflow = (uint8_t)(NO_ISR_COUNT_OVERFLOW);
#ifdef TH_FSCOM
            TH_FSFP_PushOutPar(TP_FSF_ISR_INT_Enabled);
#else
            HSUP_vEnableInt();
#endif
            // Reset the elapsed indicator, so we signal only one error at a time
            bCalcTimerCrossed65ms = false;
         }
         //  End of PCA: CR_001-16-02-2007
      }
   }

   if (bCalcTimerCrossed65ms == true)
   {
/*=======================================================
    Check Short Interrupts
=======================================================*/
      //  Prepare a pointer to the short interrupt structure
      pShortIsrCtrl = FSF_atIsrShortCntrTbl;

#ifdef TH_FSCOM
      if (FSF_ucNumberOfShortInterrupts == (uint8_t)0)
      {
         TH_FSFP_PushOutPar(TP_FSF_ISR_No_short_ISR_control);
      }
#endif
      // uiIsrTimer has changed. Start working to check short interrupt control
      /*Axivion Next Line MisraC2012-14.2 : side effect due to incrementing pShortIsrCtrl (not only loop counter) */ 
      for (ucIsrId = 0;ucIsrId < FSF_ucNumberOfShortInterrupts; ucIsrId++, pShortIsrCtrl++)//lint !e9049 !e9008 !e415 : comma operator used [MISRA 2012 Rule 12.3, advisory]
      //  Msg(4:0489) Increment or decrement operation performed on pointer.
      //  MISRA-C:2004 Rule 17.4 <next>
      //  PCA comment: code tested and reviewed to work correctly. Thus, left unchanged.
      {
#ifdef TH_FSCOM
         TH_FSFP_PushOutPar(TP_FSF_ISR_Check_Short_ISR_ID);
          TH_FSFP_PushOutPar(ucIsrId);
#endif
         //no check the first time to synchronise counters with circulating timer
         if (bSynchronisedWithCirculatingTimer != false)
         {
            // Check the short interrupt
            if (bCheckShortInterrupts(pShortIsrCtrl,ucIsrId) == false)
            {
               break;
               //  Msg(2:0769) This break statement terminates an iteration statement.
               //  PCA comment: desired state and better readable in X32, code tested and reviewed to work correctly. Thus, left unchanged.
            }
         }
         else
         {
/*======================================================================
    Initialise some entries with ucIsrId
======================================================================*/
            /* This is in order to synchronise the first time  */

#ifdef TH_FSCOM
            TH_FSFP_PushOutPar(TP_FSF_ISR_INT_Disabled);
#else
            HSUP_vDisableInt();
#endif
            FSF_auiShortIsrCount[ucIsrId] = 0;

#ifdef TH_FSCOM
            TH_FSFP_PushOutPar(TP_FSF_ISR_INT_Enabled);
#else
            HSUP_vEnableInt();
#endif
         }
      }

/*=======================================================
    Check Long Interrupts
=======================================================*/
      //  Prepare a pointer to the interrupt structure
      pIsrCtrl = FSF_atIsrCntrTbl;
      // uiIsrTimer has changed. Start working to check full interrupt control
      /*Axivion Next Line MisraC2012-14.2 : side effect due to incrementing pIsrCtrl (not only loop counter) */ 
      for (ucIsrId = 0; ucIsrId < FSF_ucNumberOfInterrupts; ucIsrId++,pIsrCtrl++)//lint !e9049 !e9008 : comma operator used [MISRA 2012 Rule 12.3, advisory]
      //  Msg(4:0489) Increment or decrement operation performed on pointer.
      //  MISRA-C:2004 Rule 17.4
      //  PCA comment: code tested and reviewed to work correctly. Thus, left unchanged.
      {
#ifdef TH_FSCOM
         TH_FSFP_PushOutPar(TP_FSF_ISR_Check_ISR_ID);
         TH_FSFP_PushOutPar(ucIsrId);
#endif
         //no check the first time to synchronise counters with circulating timer
         if (bSynchronisedWithCirculatingTimer != false)
         {
            // Check the interrupt
            if (bCheckInterrupts(pIsrCtrl, ucIsrId) == false)
            {
               break;
               //  Msg(2:0769) This break statement terminates an iteration statement.
               //  PCA comment: desired state and better readable in X32, code tested and reviewed to work correctly. Thus, left unchanged.
            }
         }
         else
         {
/*======================================================================
    Initialise some entries with ucIsrId
======================================================================*/
            /* This is in order to synchronise the first time
            */
#ifdef TH_FSCOM
            TH_FSFP_PushOutPar(TP_FSF_ISR_INT_Disabled);
#else
            HSUP_vDisableInt();
#endif
            /*Axivion Disable MisraC2012-18.1 : "Access into array might be out of bounds" : false positive here */
            FSF_auiIsrTime[ucIsrId] = 0;
            FSF_auiIsrCount[ucIsrId] = 0;
            /*Axivion Enable MisraC2012-18.1 */
#ifdef TH_FSCOM
            TH_FSFP_PushOutPar(TP_FSF_ISR_INT_Enabled);
#else
            HSUP_vEnableInt();
#endif
         }
      }

/*=======================================================
    Maintain the Interrupt channels
=======================================================*/
      //  PCA: CR_001-16-02-2007
      //  At the end of the initialisation course, reset the overflow conditions. Do it
      //  only once!
      if (bSynchronisedWithCirculatingTimer == false)
      {
#ifdef TH_FSCOM
         TH_FSFP_PushOutPar(TP_FSF_ISR_INT_Disabled);
#else
         HSUP_vDisableInt();
#endif
         // Reset the time overflow flag
         ucTimeOverflow = (uint8_t)(NO_ISR_TIME_OVERFLOW);
         // Reset the count overflow
         ucCountOverflow = (uint8_t)(NO_ISR_COUNT_OVERFLOW);
#ifdef TH_FSCOM
         TH_FSFP_PushOutPar(TP_FSF_ISR_INT_Enabled);
#else
         HSUP_vEnableInt();
#endif
      }
      //  End of PCA: CR_001-16-02-2007

      bCalcTimerCrossed65ms               = false;
      bSynchronisedWithCirculatingTimer   = true;
   }
   else
   {
#ifdef TH_FSCOM
      TH_FSFP_PushOutPar(TP_FSF_ISR_Timer_less_then_65ms);
#endif
   }

   // Save current time to compare
   ucIsrLastTimerValue = ucTime;

/*=======================================================
    Report Ongoing for all ISR channels
=======================================================*/

   /* This task handler is used to control the different meanings around interrupts, that is
   - keeping alive in general
   - time elapsed until next scheduling (latency between scheduling cycles)
   - the number of interrupts to be expected (min. frequency)
   - the limit of interrupts allowed (max. frequency)
   */

   FSF_vReportOnGoing((uint8_t)FSF_ISR_CHECK);
   FSF_vReportOnGoing((uint8_t)FSF_ISR_LATENCY_CHECK);
   FSF_vReportOnGoing((uint8_t)FSF_ISR_MIN_FREQUENCY_CHECK);
   FSF_vReportOnGoing((uint8_t)FSF_ISR_MAX_FREQUENCY_CHECK);

   return TASK_INITIALISED;
}

/** Dynamic data and execution structures are very hard to test. For that reasons, while
test the full code has been separeated into a piece of code to be executed when an interrupt
occurres, and another piece of code tested statically. When running under real target
conditions (no test harness bound) the successfully tested code gets executed.
*/

#ifndef TH_FSCOM
//  Normal entry point, no test harness
void FSF_vIsrDoEntry(uint8_t ucIsrId)
#else
//  Test harness entry point
void vIsrDoEntry(uint8_t ucIsrId)
#endif
{
   //  Does the user wants to check anything?
   if ((FSF_atIsrCntrTbl[ucIsrId].ucControl & (uint8_t)(FS_ISR_CONTROL_ALL)) != (uint8_t)0)
   {
      FSF_auiIsrCount[ucIsrId]++;
      if (FSF_auiIsrCount[ucIsrId] == 0u)
      {
         //  PCA: CR_001-16-02-2007

         // Interrupts count overflow! Report error!
         ucCountOverflow = ucIsrId;
         //  End of PCA: CR_001-16-02-2007
#ifdef TH_FSCOM
         TH_FSFP_PushOutPar((uint8_t)TP_FSF_ISR_EnISR_Count_Overflow);
         TH_FSFP_PushOutPar(ucIsrId);
#endif
      }
/*======================================================================
    Get current value of circulating timer
======================================================================*/
#ifdef TH_FSCOM
      uiIsrTimeStamp = (uint16_t)TH_FSF_CircleMicroSeconds;
#else
      uiIsrTimeStamp = TIM_uiGetCircleMicroSeconds();
#endif
   }

/*======================================================================
    Memorize entry of ISR
======================================================================*/
#ifdef FSF_ISRCONFIG
   FSF_vIsrEntryConf(ucIsrId);
#endif
}

#ifdef TH_FSCOM
//  Application entry point while test
void FSF_vIsrDoEntry(uint8_t ucIsrId)
{
   ucIsrId = ucIsrId;
}
#endif

#ifndef TH_FSCOM
//  Normal entry point, no test harness
void FSF_vIsrDoExit(uint8_t ucIsrId)
#else
//  Test harness entry point
void vIsrDoExit(uint8_t ucIsrId)
#endif
{
   //  Does the user wants to do the latency check?
   if ((FSF_atIsrCntrTbl[ucIsrId].ucControl & (uint8_t)(FS_ISR_LATENCY_CHECK)) != (uint8_t)0)
   {
      uint16_t uiTime;
      volatile uint8_t ucIdx = ucIsrId;

/*======================================================================
    Compute the cumulative latency
======================================================================*/
#ifdef TH_FSCOM
      uiTime = FSF_auiIsrTime[ucIdx] + (uint16_t)(TH_FSF_CircleMicroSeconds - uiIsrTimeStamp);  //calculate cumulative latency
#else
      uint16_t uiCircleMicroSeconds = TIM_uiGetCircleMicroSeconds();
      uiTime = FSF_auiIsrTime[ucIdx] + (uint16_t)(uiCircleMicroSeconds - uiIsrTimeStamp);  //calculate cumulative latency
#endif

      if (uiTime < FSF_auiIsrTime[ucIsrId])
      {
         //  PCA: CR_001-16-02-2007
         // Latency overflow. Report error!
         ucTimeOverflow = ucIsrId;

         //  End of PCA: CR_001-16-02-2007
#ifdef TH_FSCOM
         TH_FSFP_PushOutPar((uint8_t)TP_FSF_ISR_ExISR_Runlength_Overflow);
         TH_FSFP_PushOutPar(ucIsrId);
#endif
      }
      else
      {
         FSF_auiIsrTime[ucIsrId] = uiTime;

#ifdef TH_FSCOM
         TH_FSFP_PushOutPar((uint8_t)TP_FSF_ISR_ExISR_ComputedLatency);
         TH_FSFP_PushOutPar((uint8_t)(((uint16_t)uiTime)/256));
         TH_FSFP_PushOutPar((uint8_t)(((uint16_t)uiTime)%256));
#endif
      }
   }

/*======================================================================
    Memorize exit of ISR
======================================================================*/
#ifdef FSF_ISRCONFIG
   FSF_vIsrExitConf(ucIsrId);
#endif
}

#ifdef TH_FSCOM
//  Application entry point while test
void FSF_vIsrDoExit(uint8_t ucIsrId)
{
   ucIsrId = ucIsrId;
}
#endif

#ifndef TH_FSCOM
//  Normal entry point, no test harness
void FSF_vIsrDoShortEntry(uint8_t ucIsrShortId)
#else
//  Test harness entry point
void vIsrDoShortEntry(uint8_t ucIsrShortId)
#endif
{
   // Does the user wants to check anything?
   if ((FSF_atIsrShortCntrTbl[ucIsrShortId].ucControl & (uint8_t)(FS_ISR_CONTROL_SHORT)) != (uint8_t)0)
   {
      FSF_auiShortIsrCount[ucIsrShortId]++;
      if (FSF_auiShortIsrCount[ucIsrShortId] == 0u)
      {
         //  PCA: CR_001-16-02-2007
         // Interrupt count overflow! Report error!
         ucCountOverflow = (uint8_t)(ucIsrShortId + FSF_ucNumberOfInterrupts);
         //  End of PCA: CR_001-16-02-2007

#ifdef TH_FSCOM
         TH_FSFP_PushOutPar((uint8_t)TP_FSF_ISR_EnShISR_Count_Overflow);
         TH_FSFP_PushOutPar(ucIsrShortId);
#endif
      }
   }

/*======================================================================
    Memorize entry of short ISR
======================================================================*/
#ifdef FSF_ISRCONFIG
   FSF_vIsrShortEntryConf(ucIsrShortId);
#endif
}

#ifdef TH_FSCOM
//  Application test entry point
void FSF_vIsrDoShortEntry(uint8_t ucIsrShortId)
{
   ucIsrShortId = ucIsrShortId;
}
#endif

#if defined(MC9S12GC96) && !defined(WIN32)
#pragma MESSAGE WARNING C12056
#endif
