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
/** \file fsStack.c
 *
 *  \ingroup  <DOXY_GROUP_FSF_STACK>
 *
 *  \brief    Implementation of package fsStack of library functional safety.
 *
 *  \details  This function is monitoring the stack. Stack
overflow/underflow and correct location of the stack pointer will be sense. Multiple stack configurations are
considered too.
\n
\n
<b>Functional Safety Stack Supervision Component - Top-Level Design</b>
\n
\n
\li<b>Package Model </b>\n
\image html Package_Stack.jpg
\n
<b>Description </b>\n
The fsStack package provides the parts outlined in the model. By compilation switch, the Stack task handler may run
either in the usual operating mode or in test harness mode. When running in the usual operating mode, the test
harness part will not be linked to the package. When running in test harness mode, stub services are provided, so
the real stack area is separated from being manipulated and only the test server will be able to change it.
\n
<b>Rationale </b>\n
By the test harness part, it is possible to test all functions inside the stack task handler fully isolated from
other parts of the whole system. This is an important issue, otherwise there is a potential risk of a system crash
which is not wanted while test.
\n
\n
\n
\li<b>State Model </b>\n
\image html StateModel_Stack.jpg
\n
<b>Description </b>\n
Before checking the stack, it must be initialised correctly. This happens only once and when the generic calls the
stack task handler the first time. After this initialisation, the stack task handler transitions into the state
"FSF_STACK_READY", which will never be left during the life cycle of the whole system. In state "FSF_STACK_READY",
the stack gets checked on every call of the stack task handler.
\n
<b>Rationale </b>\n
None
\n
\n
\n
\li<b>Scenario: Stack Supervision </b>\n
\image html SequenceChart_StackSupervision.jpg
\n
<b>Description </b>\n
There is not that much to explain on this scenario, except that it make use of the generic's pre-condition mechanism.
\n
<b>Rationale </b>\n
None
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

#include "bsh_stdinc.h"
#include "LibTypes.h"
#include "FuncSafe/fSafe.h"
#include "FuncSafe/fSafeXS.h"
#include "FuncSafe/fspriv.h"
#include "processor.h"

/* Local macros ***************************************************************************************/

/* Local type definitions *****************************************************************************/

/* Local prototypes
************************************************************************/
STATIC  uint8_t * pucGetStackPointer (uint8_t ucStackIndex);
STATIC  void    vInitialzeStack(void);
STATIC  uint8_t   ucGetStackCount (void);

/* RAM/ROM Variable definitions ***********************************************************************/

/*=====================================================================================
    Stack channel workarea
=====================================================================================*/
//  This state variable is required in order to control initialisation and dispatching
STATIC  uint8_t  ucStackChannelState  = (uint8_t)FSF_STACK_INITIALIZE;
//  Initialised by 0 means first stack selected
STATIC  uint8_t   ucActualStack       = (uint8_t)0;


#ifdef  TH_FSCOM

/*=====================================================================================
    Stack channel test harness
=====================================================================================*/
#ifdef MULTIPLE_STACK
const uint8_t StackConfig = (uint8_t)1;
#else
const uint8_t StackConfig = (uint8_t)0;
#endif

//  This part is used to simulate single/multiple stacks
__ALIGNED(4) STATIC  uint8_t   TH_aucSimStack [20] = {0};   //  20 bytes are sufficient for stack simulation
#ifdef MULTIPLE_STACK
//  In multiple stack configurations, 2 stacks get
//  simulated
FSTH_CONST   T_Stack _FSF_Stack[2] = {
        {(uint16_t *)(&TH_aucSimStack[0]) ,(uint16_t *)(&TH_aucSimStack[8])},
        {(uint16_t *)(&TH_aucSimStack[10]),(uint16_t *)(&TH_aucSimStack[18])}
};
#else
//  In single stack configurations, only 1 stack
//  get simulated
FSTH_CONST   T_Stack _FSF_Stack[1] = {
        {(uint16_t *)(&TH_aucSimStack[0]),(uint16_t *)(&TH_aucSimStack[16])}};
#endif

//  Stack pointer location
STATIC  uint8_t * TH_FSF_StackPointer_0 = (uint8_t *)NULL;
STATIC  uint8_t * TH_FSF_StackPointer_1 = (uint8_t *)NULL;
//  Number of stacks available
STATIC  uint8_t   TH_ucStackCount     = (uint8_t)0;

#endif

/* static function definitions ************************************************************************/

/**
   The stack channel initializer
<pre>
   --------------------------------------------------------------------------
   This stack channel requires local variables to be placed on the hardware
   stack. For the time being, the so called "compiled stack" is not supported
   --------------------------------------------------------------------------
</pre>
   This initializer writes two magic numbers. 16-Bit values are used in order to get better
   recognition, if the stack has been overwritten or the stack pointer was out of its
   pre-defined area. The whole stack is considered a character-based array its location
   and size is determined by an application. Due to the word-sized vaules written in the stack
   its length will be reduced by 4 bytes. If problems occurre due to this reduction, the stack
   is basically considered too small and has to be increased by the application. Usually, the
   first stack entry is written when calling main at the end of the startup routine. Since
   there shall no return to this point, as there is no application related functionality there,
   its possible to overwrite this return address by the magic entries. The last two bytes are
   of no problem, i assume.
    \return   bool
    \pre      Location and size of the stack provided correctly.
    \post     Magic numbers written correctly. Current stack pointer content checked correctly.
*/
STATIC  void    vInitialzeStack(void)
{
   //  Ask the application to return the stack pointer
   const uint16_t * pucP       = (uint16_t *)((uint32_t)pucGetStackPointer(ucActualStack)); /*lint !e923 cast from pointer to unsigned int16_t */
   uint16_t * pStBegin   = _FSF_Stack[ucActualStack].pui16FirstEntry;
   uint16_t * pStEnd     = _FSF_Stack[ucActualStack].pui16LastEntry;
   //  First of all, make sure the stack pointer is pointing into the stack,
   //  otherwise it does not make sense to continue.
   if ((pucP > pStBegin) && (pucP < pStEnd)) /*lint !e946 Relational or subtract operator applied to pointers */
   {
      // Place the magic numbers at the beginning ...
      *pStBegin = _FSF_ui16StackMagic;
      // and at the end too
      *pStEnd = _FSF_ui16StackMagic;
      FSF_vReportOnGoing((uint8_t)FSF_STACK_TEST);
/*    TP_FSF_STACK_Initialised_Correctly */
#ifdef TH_FSCOM
      TH_FSFP_PushOutPar(TP_FSF_STACK_Initialised_Correctly);
#endif
   }
   else
   {
      FSF_vReportError((uint8_t)FSF_STACK_TEST,(uint8_t)FSF_ERR_CHANNEL_ERROR,(uint8_t)0);
   }
}

/**
   Return the actual stack pointer
<pre>
   --------------------------------------------------------------------------
   This stack channel requires local variables to be placed on the hardware
   stack. For the time being, the so called "compiled stack" is not supported
   --------------------------------------------------------------------------
</pre>
    This observer returns the actual position of the stack pointer. Performing certain tasks
    in order to manipulate the stack pointer for test would result into more or less nonportable
    solutions, especially when testing the same piece of code and for different environments.
    Thus, for test harness purposes this value can be read from a reserved location.

    To be noticed: due to the preconditions above, for single stack configurations, only the
    address of an stack-located variable is returned. Its physical location will be some bytes
    deeeper inside the stack, as it is placed within the new stack frame of the called observer.
    Re-alignement is not done, as this would involve some porting problems due to different
    stack frame structures.
    For multiple stack configurations (i.e. separation into program stack and interrupt stack),
    the mechanism as described foregoing will not functions, so an application has to provide
    its own mechanism.

    \param    index of stack actually considered
    \return   actual location of the stack pointer
    \pre      Location and size of the stack provided correctly.
    \post     Magic numbers written correctly. Current stack pointer content checked correctly.
*/
STATIC  uint8_t * pucGetStackPointer (uint8_t ucStackIndex)
{
    (void)ucStackIndex;
#ifdef  TH_FSCOM
   TH_FSFP_PushOutPar(ucStackIndex);

   if (ucStackIndex)
   {
      return TH_FSF_StackPointer_1;
   }
   else
   {
      return TH_FSF_StackPointer_0;
   }
#else
#ifdef MULTIPLE_STACK
   return  _FSF_pucGetStackPointer(ucStackIndex);
#else
   return  _FSF_pucGetStackPointer();
#endif
#endif
}

/**
    In multiple stack configurations, this function provide the means to return the
    number of existing stacks. In signle stack configurations, this function has no
    meaning. As such, it will be removed by the optimizer.

    \return   number of existing stacks
*/
STATIC  uint8_t   ucGetStackCount (void)
{
#ifdef MULTIPLE_STACK
#ifdef TH_FSCOM
   //  Only create a test point without calling the application
   TH_FSFP_PushOutPar(TP_FSF_STACK_GetStackCount);
   TH_FSFP_PushOutPar(TH_ucStackCount);
   return TH_ucStackCount;
#else
   //  Observe the application
   //  stack count
   return _FSF_ucGetStackCount();
#endif
#else
   // Single stack configurations have only 1 stack
   return NUMBER_OF_STACKS;
#endif
}


/* external function definitions ***********************************************************************/

/**
    The stack channel pre-condition
<pre>
   --------------------------------------------------------------------------
   This stack channel requires local variables to be placed on the hardware
   stack. For the time being, the so called "compiled stack" is not supported
   --------------------------------------------------------------------------
</pre>

    This precondition returns true, if the current stack location lies within its predefined
    area and the magic entries at the beginning and the end of the stack remain untouched.
    \return bool
*/
bool    FSF_bStackPrecondition(void)
{
   const uint16_t * pucP     = (uint16_t *)((uint32_t)pucGetStackPointer(ucActualStack)); /*lint !e923 cast from pointer to unsigned int16_t */
   const uint16_t * pStBegin = _FSF_Stack[ucActualStack].pui16FirstEntry;
   const uint16_t * pStEnd   = _FSF_Stack[ucActualStack].pui16LastEntry;

#ifdef TH_FSCOM
   TH_FSFP_PushOutPar(TP_FSF_bStackPrecondition);
#endif
   /*lint -save -e946 Relational or subtract operator applied to pointers */
   return (bool)   ((pStBegin            != pStEnd)                &&      // Begin/End position must differ
                   ((pucP > pStBegin)   && (pucP < pStEnd))        &&      // Is stack pointer location correct?
                   (*pStBegin           == _FSF_ui16StackMagic)    &&      // no underflow before?
                   (*pStEnd             == _FSF_ui16StackMagic)    &&      // no overflow before?
                   (ucActualStack       <  ucGetStackCount()));            // too much stacks counted?
   /*lint -restore -e946 */
   //  Msg(4:3415) The right hand operand of '&&' or '||' has side effects.
   //  MISRA-C:2004 Rule 12.4
   //  PCA comment: If this expression evaluates to false, control is transferred immediately to the safe
   //  state and other values do not make sense. Thus, this piece of code is tested and reviewed to work
   //  correctly. Thus, left unchanged.
}

/** The Stack Channel Task.
<pre>
   --------------------------------------------------------------------------
   This stack channel requires local variables to be placed on the hardware
   stack. For the time being, the so called "compiled stack" is not supported
   --------------------------------------------------------------------------
</pre>
    The stack channel make use of the pre-condtion mechanism provided by the FSCOM generic. When
    scheduled, it first reports ongoing. This is in order to notify that this channel is keeping
    alive. By FSCOM, its precondition get checks. If this check fails, the channel gets moved
    into error implicitely. Succes is notified to FSCOM otherwise. Special care has to be taken
    when interpreting the mechanism:\n
    while processing of interrupts, they might be nested, even if this is not wanted currently.
    Due to this fact, some hidden stack overflow/underflow conditions may occur, and, when
    control is returned back to FSCOM, the stack pointer may point to correct position again.
    For that reason, the stack channel continuousely checks both, the location and the magics.

    \return uint8_t   state of stack channel task

    \pre    None
    \post   Conditions for the stack are fulfilled
*/
uint8_t FSF_ucStackHandleTask(void)
{
   switch (ucStackChannelState)
   {
      case (uint8_t)FSF_STACK_INITIALIZE:
      {
         //  Initialze the actual statck
         vInitialzeStack();
         //  in case of multiple stacks, change to the next one
         if (ucGetStackCount() > (uint8_t)1)
         {
            //  Select the next stack configuration
            ucActualStack++;
            //  if all stacks got initialised, change the task state to continouos checking
            if (ucActualStack >= ucGetStackCount())
            {
               //  Start checking with the first stack configuration
               ucActualStack = (uint8_t)0;
               //  Change the state variable, so checking is possible
               ucStackChannelState = (uint8_t)FSF_STACK_READY;
            }
         }
         else
         {
            //  Change the state variable, so checking is possible
            ucStackChannelState = (uint8_t)FSF_STACK_READY;
         }
         break;
      }
      case (uint8_t)FSF_STACK_READY:
      {
         //  Test the actual stack
         //  Report ongoing check for the actual stack
         FSF_vReportOnGoing((uint8_t)FSF_STACK_TEST);
         //  In order to save execution time, do not check all stacks at once
         if (ucGetStackCount() > (uint8_t)1)
         {
            //  If more stack exit.....
            if (ucActualStack < (uint8_t)(ucGetStackCount() - (uint8_t)1))
            {
               //  Select the next stack configuration
               ucActualStack++;
            }
            else
            {
               //  As soon as all got checked, change back to
               //  the frst stack
               ucActualStack = (uint8_t)0;
            }
         }
         break;
      }
      default:
      {
         //  State variable of task handler accidently overwriten
         FSF_vReportError((uint8_t)FSF_STACK_TEST,(uint8_t)FSF_ERR_INVALID_STATE,(uint8_t)0);
         break;
      }
   }
   return (uint8_t)TASK_INITIALISED;
}
