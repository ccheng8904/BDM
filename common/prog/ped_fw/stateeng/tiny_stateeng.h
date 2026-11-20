/*******************************************************************************
 *  Copyright (c) 2017 BSH Hausgeraete GmbH,
 *  Carl-Wery-Str. 34, 81739 Munich, Germany, www.bsh-group.de
 *
 *  All rights reserved. This program and the accompanying materials
 *  are protected by international copyright laws.
 *  Please contact copyright holder for licensing information.
 *
 *******************************************************************************
 *  PROJECT          PP PED-Framework
 *  COMP_ABBREV      STE
 ******************************************************************************/


/******************************************************************************/
/* DOCUMENTATION                                                              */
/******************************************************************************/
/** \file
 *
 *  \ingroup  tiny stateeng
 *
 *  \brief    Public definitions and declarations of tiny state engine library
 *
 */

#ifndef __TINY_STATEENG_H
#define __TINY_STATEENG_H

#ifdef __cplusplus
extern "C" {
#endif

#include "LibTypes.h"

//type definitions for function pointers
typedef unsigned char (*pfConditionFunc)(void);
typedef void (*pfActionFunc)(void);
typedef void (*pfEntryFunc)(void);
typedef void (*pfExitFunc)(void);


typedef const struct _Tstate  TstateType;

//typedef const struct structState *StateType;



// the definition of a Default Transition (unused for tiny Stateengine)
typedef const struct  TdefTransition
{
    pfConditionFunc         pfCondition;   // IF: condition is true THEN-->
    pfActionFunc            pfAction;      // DO:  action
    TstateType*             ptNewState;    // SET: new state
}TdefTransition;

#define bIsDefCondition(ptTransition)   (ptTransition->pfCondition())
#define vDoDefAction(ptTransition)      (ptTransition->pfAction())
#define ptGetDefNewState(ptTransition)  (ptTransition->ptNewState)

// the definition of a Transition
typedef const struct  _Ttransition
{
    uchar                   ucEvId;        // IF: current event is EvId AND
    pfConditionFunc         pfCondition;   // IF: condition is true THEN-->
    pfActionFunc            pfAction;      // DO:  action
    TstateType*                 ptNewState;    // SET: new state
} Ttransition;

//Access macros for transition structure (Member functions)
#define bIsTransitionEvent(ptTransition,ucEvent)  ((ptTransition)->ucEvId==(ucEvent))
#define bIsTransitionCondition(ptTransition)      ((ptTransition)->pfCondition())
#define vDoTransitionAction(ptTransition)         ((ptTransition)->pfAction())
#define ptGetTransitionNewState(ptTransition)     ((ptTransition)->ptNewState)



// the definition of a State
typedef struct  _Tstate
{
    pfEntryFunc            pfEntry;           // entry action
    pfExitFunc             pfExit;            // exit action
    pfActionFunc           pfAction;          // performed each call of stateengine without Event
    Ttransition*           ptTransitionTable; // pointer to the first transition
    uchar                  ucNumberOfTrans;   // count of transitions
    TstateType*            ptFather;          // Father-State
} TState;

#define vDoStateEntry(ptState)                    ((ptState)->pfEntry())
#define vDoStateExit(ptState)                     ((ptState)->pfExit())
#define vDoStateAction(ptState)                   ((ptState)->pfAction())
#define ptGetStateTransitionTable(ptState)        ((ptState)->ptTransitionTable)
#define ucGetStateNumberOfTransitions(ptState)    ((ptState)->ucNumberOfTrans)
#define ptGetStateFather(ptState)                 ((ptState)->ptFather)
#define ucGetStateLevel(ptState)                  ((uchar)(0))
#define ptGetStateDefaultTable(ptState)           ((Tstate)*(0))
#define ucGetStateNumberOfDefTransitions(ptState) ((uchar)(0))

#define STE_EV_NO_EVENT 0U


//define for emptye action
#define STE_NO_ACTION          (STE_vNoAction)

//define for condition, which is always true
#define STE_ST_ALWAYS          (STE_bAlways)

//define for address of undefined transition, used in states without transitions
#define STE_NO_TRANSITION      ((Ttransition*)(0))

//define for address of undefined default transition, used in states without default transitions
#define STE_NO_DEFAULT         ((TdefTransition*)(0))


//****defines for special transitions*****

//do the transition, but don't change the state, don't do any entry/exit-function or default transition
#define STE_STAY_IN_STATE   ((TState*)(0))
#define STE_GO_STATE_BACK   ((TState*)(1))//be carefull with this address:)


//father of root state
#define STE_NO_STATE        ((TState*)(0))


//void STE_vHandleStateTable(uchar ucEventId, const TState** pptActiveState);
extern void STE_vHandleTinyStateTable(uchar ucEventId);
extern BOOL STE_bAlways(void);
extern void STE_vNoAction(void);


//extern const uchar STE_ucHistoryDepth;   //$CHB $name: number of history transitions to be saved
//extern void       *STE_pvHistoryTable[]; //$VHB $name: buffer to save history

#ifdef __cplusplus
}
#endif

#endif

