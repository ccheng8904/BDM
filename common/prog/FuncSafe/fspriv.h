/*******************************************************************************
 *  Copyright (c) 2019 BSH Hausgeraete GmbH,
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

#ifndef __FSPRIV_H

#define __FSPRIV_H

#ifdef __cplusplus
extern "C" {
#endif

/******************************************************************************/
/* INCLUDES                                                                   */
/******************************************************************************/
#include "bsh_stdinc.h"
#include "LibTypes.h"
#include "fSafe.h"
/*==============================================================================
    Definitions and Declarations
==============================================================================*/
/* For all 32bit platform use 4 byte size for Channel Cycle Timer
 * (FSF_ulCheckCycleTimer instead of FSF_ucCheckCycleTimer )
 */
#define FSF_CYCLE_TIM_SIZE_32BIT
/*=====================================================================================
    STATIC Macro definition
=====================================================================================*/

/**
While testing all parts must be visible
*/
#ifdef TH_FSCOM
#define STATIC
#else
#define STATIC  static
#endif

/*=====================================================================================
    Channel states
=====================================================================================*/
enum    FSF_CHANNEL_STATES {
        FSF_CH_STATE_IDLE,          //!< Channel state IDLE (MUST BE ZERO!)
        FSF_CH_STATE_ONGOING,       //!< Channel state ONGOING
        FSF_CH_STATE_SUCCESS,       //!< Channel state SUCCESS
        FSF_CH_STATE_ERROR          //!< Channel state ERROR
};

/*=====================================================================================
    Generic states
=====================================================================================*/
enum    FSF_STATES {
        FSF_STATE_BOOTED,           //!< Generic state BOOTED (MUST BE ZERO!)
        FSF_STATE_PRESCHEDULED,     //!< Generic state PRESCHEDULED (no FSCOM activity this state)
        FSF_STATE_STARTED           //!< Generic state STARTED
};

enum    FSF_SCH_STATES {
        FSF_SCH_INITIALISE,         //!< The pseuo scheduler is about to initialise
        FSF_SCH_CHANNEL,            //!< The pseuo scheduler is about to schedule channels
        FSF_SCH_GENERIC             //!< The pseuo scheduler is about to schedule the generic
};

enum    FSF_ISR_STATES {
        FSF_ISR_CHECK_LONG_INT,     //!< The ISR channel is about to check the long ISR's
        FSF_ISR_CHECK_SHORT_INT,    //!< The ISR channel is about to check the short ISR's
        FSF_ISR_MAINTAIN_CHANNEL    //!< The ISR channel is about to maintain the channel data
};

enum    FSF_ROM_STATES {
        FSF_ROM_CHECK_RESTART,      //!< The ROM channel is about to (re-)start the ROM check
        FSF_ROM_CHECK_ONGOING,      //!< The ROM channel is about to check the ROM
        FSF_ROM_CHECK_FINISH        //!< The ROM channel has stopped checking (ROM-at-once)
};

enum    FSF_STACK_STATES {
        FSF_STACK_INITIALIZE,       //!< The Stack channel is about to initialise the stack
        FSF_STACK_READY             //!< The Stack channel is ready for checking
};

enum    FSF_RAM_STATES {
        FSF_RAM_CHECK_RESTART,      //!< The RAM channel is about to (re-)start the RAM check
        FSF_RAM_CHECK_ONGOING,      //!< The RAM channel is about to check the RAM
        FSF_RAM_CHECK_CHANGE_SEGMENT, //!< The RAM channel has stopped checking
        FSF_RAM_CHECK_FINISH        //!< The RAM channel has stopped checking
};

/*=====================================================================================
    Timing Constraints
=====================================================================================*/
#define FSF_BASE_CYCLE_TIME     100u                  //!< Milliseconds (at least!)
#define FSF_CHECK_CYCLE_TIME    100u                  //!< Milliseconds
#define FSF_TIMHIST_DEPTH       4u                    //!< Entries in timing history

/*=====================================================================================
    Some interrupt concerned definitions
=====================================================================================*/
#define NO_ISR_TIME_OVERFLOW    -1
#define NO_ISR_COUNT_OVERFLOW   -1

#ifdef TH_FSCOM
/*=====================================================================================
    Test Harness Definitions
=====================================================================================*/

/*======================================================================
    TH General Definitions
======================================================================*/
#define TH_PAR_SIZE     12      //!< Size of bytes reserved for input parameters
#define TH_RET_SIZE     40      //!< Size of bytes reserved for return parameters
#define TH_EXEC_TIME    10000   //!< measured in microseconds

/*======================================================================
    TH Function codes
======================================================================*/
//  Test Harness function codes
//  one function code exists per local/exported function

enum    {
    //  The NOP function. The server performs one test step. NOP is set on completion.
    //  Thus, each test step is performed only once and repetition is under control of
    //  the test equipment (edith).
    TH_FSF_no_operation,
    //  Local functions.
    //  functions NOT to be tested:
    //      vMemorizeTimingValue
    TH_FSF_vPerformOneTimeChecks,
    TH_FSF_ucGetFSCOM_State,
    TH_FSF_vSetFSCOM_State,
    TH_FSF_ucNewMonitorCycle,
    TH_FSF_ucComHandleTask,
    TH_FSF_vSetChannelIdle,
    TH_FSF_vSetChannelOngoing,
    TH_FSF_vSetChannelSuccess,
    TH_FSF_vTriggerExtWatchdog,
    TH_FSF_bIsChannelIdValid,
    TH_FSF_bIsChannelCycleOverdue,
    TH_FSF_ucGetChannelState,
    TH_FSF_bMaintainCheckCycleTimer,
    //  Exported services
    TH_FSF_vBoot,
    TH_FSF_vContinueStartUp,
    TH_FSF_ucMonitoringStart,
    TH_FSF_vBootError,
    TH_FSF_vReportOngoing,
    TH_FSF_vReportError,
    TH_FSF_vReportSuccess,
    TH_FSF_bSuccesInPast,
    TH_FSF_vReportFatal,
    TH_FSF_ucHandlePreScheduling,
    //  Functions of special meanings
    TH_FSF_CallChannelOneTimeCheck,
    TH_FSF_CallChannelPreCondition,
    //  Manipulation of In/Out stacks
    TH_FSF_ClearInPar,
    TH_FSF_ClearOutPar,
    //  Channel access support
    TH_FSF_vSetCycleCount,
    TH_FSF_vSetMultiplier,
    TH_FSF_vSetRegularCheck,
    //  ROM Channel support
    TH_FSF_uiCalculateCRC,
    TH_FSF_uiSetupNextROMAddress,
    TH_FSF_ucInitialiseROMChannel,
    TH_FSF_ucROMHandleTask,
    TH_FSF_bExtIsROMReady,
    TH_FSF_pucExtROMRead,
    //  Pseudo Scheduler
    TH_FSF_SCH_ucHandleTask,
    TH_FSF_SCH_vSetMainTable,
    //  Stack Channel support
    TH_FSF_vInitialzeStack,
    TH_FSF_bStackPrecondition,
    TH_FSF_ucStackHandleTask,
    //  ISR Channel(s)
    TH_FSF_bCheckShortInterrupts,
    TH_FSF_ucIsrHandleTask,
    TH_FSF_vIsrDoEntry,
    TH_FSF_vIsrDoExit,
    TH_FSF_vIsrDoShortEntry,
    TH_FSF_bCheckInterrupts,
    TH_FSF_bGetROMConfig,
    TH_FSF_bGetSchedulerConfig,
    //  Register Channel
    TH_FSF_ucRegHandleTask,
    //  More ROM channel related things
    TH_FSF_ucInternalROMTask,
    TH_FSF_bIsROMAtOnceAllowed,
    //  RAM channel related things
    TH_FSF_ucRAMHandleTask,
    TH_FSF_bGetRAMConfig,
    //  some more for the stack channel
    TH_FSF_GetucActualStack,
    TH_FSF_GetStackConfig,
    TH_FSF_GetSimStack,
    TH_FSF_GetStackPointer,
    TH_FSF_GetucStackCount,
    TH_FSF_pucGetStackPointer,
    //  still RAM channel related
    TH_FSF_vRam_OperationalException,
    TH_FSF_vRamOpercheck,
    //  RAM Consistency Check related
    TH_FSF_vGetRAMConsisCheck_State,
    TH_FSF_RAM_CON_vInitFsRamArea,
    TH_FSF_RAM_CON_vCalcCheckBasic,
    TH_FSF_RAM_CON_vSetUcharSecBasic,
    TH_FSF_RAM_CON_vSetUintSecBasic,
    TH_FSF_RAM_CON_vUpdateFsMirrorRam,
    TH_FSF_RAM_CON_bCheckFsRamRange,
//  MUST BE THE LAST ENTRY !!!
    TH_FSF_LASTENTRY
    };

/*======================================================================
    TH Data area
======================================================================*/
//  Used to external watchdog conditions
extern  uint8_t    TH_FSF_ExtWatchdogState;

//  Used to create timing conditions
extern  uint8_t    TH_FSF_Circle256MicroSeconds;

//  Used to create timing conditions
extern  uint16_t   TH_FSF_CircleMicroSeconds;

//  Test harness function to be executed by the next step.
//  Zero means, no function to execute.
extern  uint8_t    TH_FSF_Function;

//  Parameter passed in this test step
extern  uint8_t    TH_FSF_InPar[TH_PAR_SIZE];
//  In-Stack Pointer
extern  uint8_t    TH_FSF_InSP;


//  Return vector stored on test step completion
extern  uint8_t    TH_FSF_OutPar[TH_RET_SIZE];
//  Out-Stack Pointer
extern  uint8_t    TH_FSF_OutSP;

//  Timestamp at start of test step
extern  uint16_t  TH_FSF_StartOfTest;

//  Timestamp at end of test step
extern  uint16_t  TH_FSF_EndOfTest;

//  Stack pointer location
extern  uint8_t * TH_FSF_StackPointer;

//  Reflects the actual scheduler configuration
extern  uint8_t   TH_FSF_HasScheduler;

//  Type of ROM check
extern  bool      TH_FSF_bROM_At_Once;

/*======================================================================
    TH External Declarations
======================================================================*/

/**
General Test Harness Support
*/
extern  void    TH_FSFP_PushInPar(uint8_t val);
extern  void    TH_FSFP_PushOutPar(uint8_t val);
extern  void    TH_FSFP_ClearInPar(void);
extern  void    TH_FSFP_ClearOutPar(void);

/**
Specific to FSCOM
*/
extern  void    TH_FSFP_SetCycleCount(void);
extern  void    TH_FSFP_SetMultiplier(void);
extern  void    TH_FSFP_SetRegularCheck(void);

/**
Specific to the ROM Channel
*/
extern  void    TH_FSFP_uiCalculateCRC(void);
extern  void    TH_FSFP_uiSetupNextROMAddress(void);
extern  void    TH_FSFP_ucInitialiseROMChannel(void);
extern  void    TH_FSFP_ucROMHandleTask(void);
extern  void    TH_FSFP_bExtIsROMReady(void);
extern  void    TH_FSFP_pucExtROMRead(void);
extern  void    TH_FSFP_bGetROMConfig(void);
extern  void    TH_FSFP_ucInternalROMTask(void);
extern  void    TH_FSFP_bIsROMAtOnceAllowed(void);


extern  const   uint8_t   ROMConfig[2];
extern  uint8_t ucInternalROMTask(void);
extern  bool    bIsROMAtOnceAllowed(void);
extern  void    TH_FSFP_bGetSchedulerConfig(void);

/**
Specific to the Stack Channel
*/
extern  FSTH_CONST   uint16_t  _FSF_ui16StackMagic;
extern          uint8_t   ucActualStack;
extern  const   uint8_t   StackConfig;
extern          uint8_t   TH_SimStack [20];

#ifdef MULTIPLE_STACK
extern    FSTH_CONST    T_Stack  _FSF_Stack[2];
#else
extern    FSTH_CONST    T_Stack  _FSF_Stack[1];
#endif

extern          uint8_t * TH_FSF_StackPointer_0;
extern          uint8_t * TH_FSF_StackPointer_1;
extern          uint8_t   TH_ucStackCount;

extern          void    vInitialzeStack(void);
extern          bool      FSF_bStackPrecondition(void);
extern          uint8_t * pucGetStackPointer (uint8_t ucStackIndex);

extern          void    TH_FSFP_vInitialzeStack(void);
extern          void    TH_FSFP_bStackPrecondition(void);
extern          void    TH_FSFP_ucStackHandleTask(void);

/**
Specific to the ISR Channel(s)
*/

//  The component will take place inside the real target environment and interrupts will
//  occurre asynchronousely. Under such cicumsdances, there is no component possible.
//  Thus, interrupt entry/exit functions get wrapped depending on the test harness switch.
extern  void    vIsrDoEntry(uint8_t IsrId);
extern  void    vIsrDoExit(uint8_t IsrId);
extern  void    vIsrDoShortEntry(uint8_t IsrId);
extern  bool    bCheckInterrupts(TisrControl FSTH_CONST * pIsrCtrl, uint8_t ucIsrId);
extern  bool    bCheckShortInterrupts(TisrShortControl FSTH_CONST * pShortIsrCtrl, uint8_t ucIsrId);


extern  void    TH_FSFP_bCheckInterrupts(void);
extern  void    TH_FSFP_ucIsrHandleTask(void);
extern  void    TH_FSFP_bCheckShortInterrupts(void);
extern  void    TH_FSFP_vIsrDoEntry(void);
extern  void    TH_FSFP_vIsrDoExit(void);
extern  void    TH_FSFP_vIsrDoShortEntry(void);

/**
Specific to the Register Channel
*/
extern  void    TH_FSFP_ucRegHandleTask(void);

/**
Specific to the RAM Channel
*/
extern  const   uint8_t RAMConfig[2];
extern          void    TH_FSFP_ucRamHandleTask(void);
extern          void    TH_FSFP_bGetRAMConfig(void);
extern          void    TH_FSFP_vRam_OperationalException(void);
extern          void    TH_FSFP_vRamOpercheck(void);

extern          uint8_t * pucRAM_Saved;
extern          uint8_t * pucDEXBuffer_Saved;
extern          uint8_t   ucPatNoInv_Saved;
extern          uint8_t   ucPatInv_Saved;

/*======================================================================
    TH Test Points
======================================================================*/
//  Note: zero is no valid test point
//  There are some interfaces where control is not returned back to the caller.
//  Test points are specified in order to identify if such an interface has
//  been reached. If the test harness is connected to FSCOM, a test point is
//  stored inside the output buffer and control is returned back to the test
//  server. Thus, the server is able continue with the next step.

//  FSCOM RAM Channel
#define     TP_FSF_FSF_vRamOpercheck            0x80
#define     TP_FSF_RAM_INT_Disabled             0x81
#define     TP_FSF_RAM_INT_Enabled              0x82
#define     TP_FSF_RAM_Pattern_changed          0x83
#define     TP_FSF_RAM_Segment_changed          0x84
#define     TP_FSF_RAM_LowLevelCheck_S0         0x85
#define     TP_FSF_RAM_LowLevelCheck_S1         0x86
#define     TP_FSF_RAM_LowLevelCheck_S2         0x87
#define     TP_FSF_RAM_LowLevelCheck_S3         0x88
#define     TP_FSF_RAM_LowLevelCheck_S4         0x89
#define     TP_FSF_RAM_LowLevelCheck_S5         0x8a

//  FSCOM Register Channel
#define     TP_FSF_vReg_OperCheck               0x90

//  FSCOM Interrupt channel(s)
#define     TP_FSF_ISR_No_short_ISR_control     0xa0
#define     TP_FSF_ISR_Timer_less_then_65ms     0xa1
#define     TP_FSF_ISR_Check_ISR_ID             0xa2
#define     TP_FSF_ISR_INT_Enabled              0xa3
#define     TP_FSF_ISR_INT_Disabled             0xa4
#define     TP_FSF_ISR_EnISR_Count_Overflow     0xa5
#define     TP_FSF_ISR_ExISR_Runlength_Overflow 0xa6
#define     TP_FSF_ISR_ExISR_ComputedLatency    0xa7
#define     TP_FSF_ISR_EnShISR_Count_Overflow   0xa8
#define     TP_FSF_ISR_Check_Short_ISR_ID       0xa9

//  FSCOM Stack Channel
#define     TP_FSF_STACK_Initialised_Correctly  0xb0
#define     TP_FSF_STACK_GetStackCount          0xb1


//  Pseudo Scheduler
#define     TP_FSF_SCH_vSetMainStateTable       0xc0
#define     TP_FSF_SCH_ucHandleTask             0xc1
#define     TP_FSF_SCH_ComponentNotInitialised  0xc2
#define     TP_FSF_SCH_RestartScheduling        0xc3
#define     TP_FSF_SCH_EmptyTaskTable           0xc4
#define     TP_FSF_SCH_SwitchedNextComponent    0xc5
#define     TP_FSF_SCH_FSF_ucPseudoHandler_1    0xc6
#define     TP_FSF_SCH_FSF_ucPseudoHandler_2    0xc7
#define     TP_FSF_SCH_FSF_ucPseudoHandler_3    0xc8
#define     TP_FSF_SCH_ExecTimeViolation        0xc9

//  FSCOM ROM Channel
#define     TP_FSF_ROM_Segment_Changed          0xd0
#define     TP_FSF_ROM_Module_Changed           0xd1
#define     TP_FSF_ROM_Check_Restart            0xd2
#define     TP_FSF_ROM_Not_Ready                0xd3
#define     TP_FSF_ROM_No_Configuration         0xd4
#define     TP_FSF_ROM_Data_Read                0xd5
#define     TP_FSF_ROM_Is_Ready                 0xd6
#define     TP_FSF_ROM_Check_Finished           0xd7
#define     TP_FSF_ROM_Check_At_Once            0xd8
#define     TP_FSF_ROM_Check_Fragmented         0xd9
#define     TP_FSF_ROM_Check_Refused            0xda
#define     TP_FSF_ROM_Check_Allowed            0xdb

//  FSCOM Generic
#define     TP_FSF_vApplicationStartUp          0xe1
#define     TP_FSF_vApplicationErrorNotify      0xe2
#define     TP_FSF_vReportFatal                 0xe3
#define     TP_FSF_vBootError                   0xe4
#define     TP_FSF_vTriggerExtWatchdog          0xe5
#define     TP_FSF_vTriggerInternalWatchdog     0xe6
#define     TP_FSF_bDefaultPreconditionTRUE     0xe7
#define     TP_FSF_vDefaultOneTimeCheck         0xe8
#define     TP_FSF_bStackPrecondition           0xe9
#define     TP_FSF_vRegister_OneTimeCheck       0xea
#define     TP_FSF_vRam_OneTimeCheck            0xeb
#define     TP_FSF_ucMonitoringStart            0xec
#define     TP_FSF_ucNewMonitorCycle            0xed
#define     TP_FSF_ucHandlePreScheduling        0xee
#define     TP_FSF_bExtReadExtWatchdog          0xef
#define     TP_FSF_vSetChannelIdle              0xf0

#endif

/*====================================================================================================
    TYPE DEFINITIONS
====================================================================================================*/

/*====================================================================================================
    DECLARATIONS (to be (re)defined in library template)
====================================================================================================*/

/** FSCOM configuration data.
FSCOM is considered a library. As such, it doesn't have data (neither hard data nor dynamic data).
Both types of data have to be provided by an application. The internal structure will be the same
for different types of controller; its physical representation may differ between types of
controllers. Data defined for the FSCOM library are separated by its attributes:
\li <b>Hard Data</b>\n
located in a constant data part (flash or ROM) carrying all configuration unchanged while operation
\li <b>Dynamic Data</b>\n
located in RAM carrying all state dpenedent parts while operation, even for the set of configured
channels.
*/

/** FSCOM dynamic data area.
*/

// FSCOM task state
// Moment in time of last watchdog trigger
// Channel states. Structured and size: [FSF_NR_OF_FLAGS][FSCOM_FLAG_FIELD]
// Pieces of base cycles elapsed. Structured and size: [FSF_NR_OF_CHANNELS]
// Check cycle timer (count units of 100 milliseconds)
// Moment in time when FSCOM is scheduled
// Delta value from last watchdog trigger until current scheduling
extern  uint8_t             FSF_ucTaskState;
extern  uint16_t            FSF_ui16LastWDTrigger;
extern  uint8_t             FSF_ucCH_states[FSF_NR_OF_FLAGS][FSCOM_FLAG_FIELD];
extern  T_FSCOM_CheckCycle  FSF_sCH_CheckCycleTime[];
#ifdef FSF_CYCLE_TIM_SIZE_32BIT
extern  uint16_t            FSF_ui16CycLastTime;
extern  uint32_t            FSF_ulCheckCycleTimer;
#else
extern  uint8_t             FSF_ucCheckCycleTimer;
#endif
extern  uint16_t            FSF_ui16Current;
extern  uint16_t            FSF_ui16delta_t;

/** FSCOM hard data area.
*/

//  Table of references to configured channels
//  Size of table of configured channels. Structured and size: [FSF_NR_OF_CHANNELS]
//  Watchdog time slots (T1, T2, T3)
//  The FSCOM Default Channel configuration
//  The number of prescheduling cycles passed before normal operation
extern  FSTH_CONST   T_FSCOM_Channel * FSTH_CONST FSF_psChannelTab[];
extern  FSTH_CONST   uint8_t                 FSF_ucNrOfChannels;
extern  FSTH_CONST   uint16_t                FSF_ui16WDC1_2_3;
extern  FSTH_CONST   uint16_t                FSF_ui16WDC1_3;
extern  FSTH_CONST   T_FSCOM_Channel         FSF_sDefaultChannel;
extern  FSTH_CONST   uint8_t                 FSF_ucPreSchedCycles;

/*====================================================================================================
    GLOBAL FUNCTIONS (to be (re)defined in library template)
====================================================================================================*/

//@{\name FSCOM required interfaces

/** Start or continue the application-specific start up process.
    In order to start or continue an applications start up phase, control is transferred to
    this routine.

    \pre        FSCOM initialised correctly. The external watchdog initialised.\n

    \note       may be overloaded according to a project specific needs
*/
extern  void    _FSF_vApplicationStartUp(void);

/** Notify an error to the application.
    Notify an application about a problem and this call happens as part of safe state, that
    is control can't be returned to FSCOM. If so, FSCOM subsequently calls its internal
    error handling routine.
    \param      ucSubCheckId   FSF_ERROR_CODE that describes the problem\n
    \param      ucChannelId    Identifer of the affected channel or FSF_NOCHANNEL
    \param      ucUserPar      User defined parameter

    \note       may be overloaded according to a project specific needs
*/
extern  void    _FSF_vApplicationErrorNotify(uint8_t ucSubCheckId, uint8_t ucChannelId, uint8_t ucUserPar);

/** Trigger the external watchdog.
    Trigger the external watchdog according the requirements for T3

    \note       may be overloaded according to a project specific needs\n
                Reference chapter 7.5 of [1]
*/
extern  void    _FSF_vTriggerExtWatchdog(void);

/** Get external watchdog state.
    Read in the state of the external watchdog.

    \return     bool    true, the watchdog remains in its safe state, false otherwise\n

    \pre        None\n
    \post       Depending on how the external watchdog is really implemented, when triggering
                the external watchdog and subsequently reading back its state again there might
                be some hysteresis to be taken into condiderations. This has to be solved at
                application level as this depends on a specific project.
    \note       may be overloaded according to a project specific needs\n
                Reference chapter 7.5 of [1]
*/
extern  bool    _FSF_bReadExtWatchdog(void);

/** Trigger the internal watchdog.
    Trigger the internal watchdog

    \note       may be overloaded according to a project specific needs\n
                Reference chapter 7.4 of [1]
*/
extern  void    _FSF_vTriggerInternalWatchdog(void);

/*lint -esym(9046,*FSF_vReportFatal)  typographically ambiguous with respect to 'FSF_vReportFatal' */
/** Report a fatal exception.
    Report a fatal exception.

    \note       may be overloaded according to a project specific needs\n
                Reference chapter 7.2 of [1]
*/
extern  void    _FSF_vReportFatal(void);
extern void _FSF_RAM_CON_vDoCheckInRomModule(void);
extern void _FSF_RAM_CON_vDoUpdateCheckInRomModule(void);
/*Axivion Disable MisraC2012-1.1 : Number of significant initial characters for external identifier not a problem with our tools */

extern void _FSF_RAM_CON_vRomModuleCheckBySum(void);
extern void _FSF_RAM_CON_vRomModuleCheckByMirror(void);
extern void _FSF_RAM_CON_vRomModuleUpdateCheckBySum(void);
extern void _FSF_RAM_CON_vRomModuleUpdateCheckByMirror(void);


extern void _FSF_RAM_CON_vDoCheckInFsComModule(void);
extern void _FSF_RAM_CON_vDoUpdateCheckInFsComModule(void);

extern void _FSF_RAM_CON_vFsComModuleCheckBySum(void);
extern void _FSF_RAM_CON_vFsComModuleCheckByMirror(void);
extern void _FSF_RAM_CON_vFsComModuleUpdateCheckBySum(void);
extern void _FSF_RAM_CON_vFsComModuleUpdateCheckByMirror(void);
/*Axivion Enable MisraC2012-1.1 : Number of significant initial characters for external identifier not a problem with our tools */

extern void _FSF_RAM_CON_vSetErrCntBySum(void);
extern void _FSF_RAM_CON_vSetErrCntByMirror(void);
extern void _FSF_RAM_CON_vSetErrCntWithoutRamCon(void);
extern void _FSF_RAM_CON_vSetErrCounter(void);

#ifdef TH_FSCOM

/** TH_FSFP_ucServer
    Perform one test step.

    \return     TASK_NOT_INITIALISED in all cases

    \note       Parameters are taken from the test harness data area\n
*/
extern  uint8_t    TH_FSFP_ucServer(void);

#ifdef FSF_CRC_32_BIT
#if defined(FM4) && defined(TH_FSCOM)
// for FM4 the packed structure has to be used, because of unaligned accesses -> SpuriousIsr is called
extern uint32_t  uiCalculateCRC32  (uint32_t CRC32Value, __packed FSTH_CONST uint32_t *pData, uint16_t ui16length);
#else
extern uint32_t  uiCalculateCRC32  (uint32_t CRC32Value, FSTH_CONST uint32_t *pData, uint16_t ui16length);
#endif
#else
extern  uint16_t  uiCalculateCRC  (uint16_t CRC16Value, FSTH_CONST uint8_t *pData, uint16_t ui16length);
#endif
extern  uint8_t   ucInitialiseROMChannel  (void);
extern  uint32_t  uiSetupNextROMAddress   (uint16_t ui16length);
extern  bool      bExtIsROMReady (uint8_t *pucData, uint16_t ui16length);
extern  uint8_t   *pucExtROMRead (uint8_t *pucData, uint16_t ui16length);

#endif

//@}

//  This wrapper function separate the test harness from overloaded application

extern  void    vExtTriggerInternalWatchdog(void);

#ifdef __cplusplus
}
#endif

#endif
