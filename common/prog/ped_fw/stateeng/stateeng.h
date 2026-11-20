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
 *  \ingroup  stateeng
 *
 *  \brief    Public definitions and declarations of state engine library
 *
 */

#ifndef STATEENG_H
#define STATEENG_H

#ifdef __cplusplus
extern "C" {
#endif


/******************************************************************************/
/* INCLUDES                                                                   */
/******************************************************************************/
#include "LibTypes.h"
#include <stddef.h>

/******************************************************************************/
/* PUBLIC DEFINITIONS                                                         */
/******************************************************************************/


//define for empty action
#define STE_NO_ACTION          NULL ///< define for empty action

//define for condition, which is always true
#define STE_ST_ALWAYS          ((ConditionType){0,0}) ///< define for condition, which is always true

/** define for address of undefined transition, used in states without transitions */
#define STE_NO_TRANSITION      NULL

/** define for address of undefined default transition, used in states without default transitions */
#define STE_NO_DEFAULT         NULL


//****defines for special transitions*****

/** do the transition, but don't change the state, don't do any entry/exit-function or default transition */
#define STE_ST_STAY_IN_STATE   ((StateType)(STE_STAY_IN_STATE))

/** return to last active state; useful to return from error state, when error repaired */
#define STE_ST_BACK            ((StateType)(STE_BACK))

/** father of root state */
#define STE_ST_NO_STATE        ((StateType)(STE_NO_STATE))

/** definition for state-level, substate of root state has level STE_LEVEL_1... */
enum{
       STE_ROOT_LEVEL,
       STE_LEVEL_1,
       STE_LEVEL_2,
       STE_LEVEL_3,
       STE_LEVEL_4,
       STE_LEVEL_5,
       STE_LEVEL_6,
       STE_LEVEL_7
};

#define MAX_LEVEL      STE_LEVEL_7 ///< max supported depth for state-table 


/** Special definitions for events */
enum{
       STE_EV_ALWAYS,                  ///< special event used in transitions which fits always (not implemented)
       STE_EV_NO_EVENT,                ///< No event happened
       STE_FIRST_REGULAR_EVENT         ///< this has to be last entry in enumeration with this client starts enumeration of events
};

/** Special definitions for conditions */
enum{
       STE_ALWAYS,                     ///< condition always true
       STE_FIRST_REGULAR_CONDITION     ///< with STE_FIRST_REGULAR_CONDITION client starts enumeration of condition ids
};


/** Special definitions for states */
enum{
       STE_NO_STATE,                  ///< no state, e.g. for root which has no father state
       STE_STAY_IN_STATE,             ///< target of a transition is the same state without perform exit/entry
       STE_BACK,                      ///< target of a transition is the last active state
       STE_FIRST_REGULAR_STATE        ///< this has to be last entry in enumeration
};



/******************************************************************************/
/* PUBLIC DATA DEFINITIONS                                                    */
/******************************************************************************/

extern const uchar STE_ucHistoryDepth;   ///<  number of history transitions to be saved (must be defined outside library)
extern const void *STE_pvHistoryTable[]; ///<  buffer to save history (must be defined outside library)



/******************************************************************************/
/* PUBLIC TYPE DEFINITIONS                                                    */
/******************************************************************************/


//type definitions for function pointers
typedef uint8_t (*ConditionFunc)(void);   ///< function pointer to condition (guard)
typedef void (*ActionFunc)(void);    ///< function pointer to action
typedef void (*EntryFunc)(void);     ///< function pointer to entry function
typedef void (*ExitFunc)(void);      ///< function pointer to exit function


//type definitions for stored functions (to save for example ID
//currently hole pointer will be saved for conditions and actions 
//typedef ConditionFunc       ConditionType;
/** type definition for conditions */
typedef struct structCondition
{
    const TbitField     TrueCondition :1;   ///< 1 if result TRUE fit, else FALSE 
    const TbitField     ConditionID   :15;   ///< id of condition function
} ConditionType;

typedef ActionFunc          ActionType;  ///< the action
typedef const struct structState *StateType; ///< pointer to state
#ifndef stateXL
typedef uint8_t             EventType;   ///< the event
#else
typedef uint16_t            EventType;   ///< the event
#endif


/** type definition for Default-Transition */
typedef const struct  structDefaultTransition
{
    const ConditionType           Condition;       ///< condition
    const ActionType              Action;          ///< action
    const StateType               NewState;        ///< target state
} StDefault;



/** type definition for Transition */
typedef const struct  structTransition
{
    const ConditionType           Condition;   ///< condition
    const ActionType              Action;      ///< action
    const StateType               NewState;    ///< target state
    const EventType               EvId;        ///< the event id
} StTransition;


/** type definition for State */
typedef struct  structState
{
    const EntryFunc              Entry;                      ///< entry action
    const ExitFunc               Exit;                       ///< exit action
    const ActionType             Action;                     ///< will be performed each call of stateengine
    StDefault             *DefaultTable;                     ///< pointer to first default
    StTransition          *TransitionTable;                  ///< pointer to the first transition
    const StateType              Father;                     ///< Father-State
    const uint8_t                Depth;                      ///< needed for search to deepest path
    const uint8_t                NumberOfDef;                ///< count of defaults
    const uint8_t                NumberOfTrans;              ///< count of transitions
} StState;




/** type structure for the state client  */
typedef struct StructStateClient
{
   const EventType           **ppCompoundEventTable;  ///< pointer to compound event table, if NULL, there are no Cmpd-Events
   EventType                   LastCompoundEvent;     ///< index of last Cmpd-Event, if ZERO there are no Cmpd-Events
   const ConditionFunc        *ppfConditionTable;     ///< pointer to clients condition table (ptr to func-pointer)
}StStateClient;


/******************************************************************************/
/* PUBLIC FUNCTION DECLARATIONS                                               */
/******************************************************************************/

/** \brief    Handles the given transition state table
 *
 *  \details  handles the client state table, perform transitions and
 *            default-transitions   
 *
 *  \param[in]    psClient:    pointer to client State-Transition-Table
 *  \param[in]    EventId:     Id of current Event
 *  \param[out]   ActiveState: pointer to active state of client state table
 *  \param[in]    LastState:   Last active state of client state table
 *
 */
void STE_vHandleStateTable
(
    StStateClient* psClient,
    EventType EventId,
    StateType *ActiveState,
    StateType *LastState
);


/** \brief    Handles the given transition state table
 *
 *  \details  handles the client state table, perform transitions and
 *            default-transitions and returns true if the event was used  
 *
 *  \param[in]    psClient:    pointer to client State-Transition-Table
 *  \param[in]    EventId:     Id of current Event
 *  \param[out]   ActiveState: pointer to active state of client state table
 *  \param[in]    LastState:   Last active state of client state table
 *  \return   TRUE: event was used, FALSE: event unused
 *
 */
BOOL STE_bHandleStateTable
(
    StStateClient* psClient,
    EventType EventId,
    StateType *ActiveState,
    StateType *LastState
);



/** \brief    performs the entry action and handle the default transitions.
 *
 *  \details  this function shall be called before \sa STE_bHandleStateTable or
              \sa STE_vHandleStateTable.
 *
 *  \param[in]  psClient:    pointer to client State-Transition-Table
 *  \param[out] ActiveState: pointer to active state of client state table
 *  \param[in]  EntryState:  Last active state of client state table
 *
 */
void STE_vInitStateTable
(
    StStateClient* psClient,
    StateType *ActiveState,
    StateType EntryState
);



#ifdef __cplusplus
}
#endif

#endif

