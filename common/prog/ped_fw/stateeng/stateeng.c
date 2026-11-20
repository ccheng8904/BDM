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
 *  \brief    Implementation of stateeng.c.
 *
 */


/******************************************************************************/
/* INCLUDES                                                                   */
/******************************************************************************/
#include "bsh_stdinc.h"
#include "stateeng.h"
#include "LibTypes.h"
#include <stddef.h>

/******************************************************************************/
/* PRIVATE DEFINITIONS                                                        */
/******************************************************************************/
#ifndef DEBUG //all debug specific definitions
#define DBG_STC
#else
#define DBG_STC   static
#endif

/******************************************************************************/
/* PRIVATE TYPE DEFINITIONS                                                   */
/******************************************************************************/

/******************************************************************************/
/* PRIVATE FUNCTION DECLARATIONS AND PRIVATE MACRO FUNCTION DEFINITIONS       */
/******************************************************************************/

static void vReportTransitions(const void *ExecutedTransition);

// search path from OriginState to DestinationState and do the entries and exits
static void vSearchPath(StateType OriginState, StateType DestinationState);

// handle the default transitions of State
static const StState* psDoDefaultTransition(StateType State);

//this function encapsulate the storage of state in: StTransition, StDefault and StState
static const StState* psGetState(StateType State);

//this function encapsulate the storage of condition in: StTransition, StDefault
static uchar bIsConditionTrue(ConditionType Condition);

//this function encapsulate the storage of action in: StTransition, StDefault
static void vDoAction(ActionType Action);

static uchar bDoesEventFit(EventType EventId, EventType FittingEvent);



/******************************************************************************/
/* PRIVATE DATA DEFINITIONS                                                   */
/******************************************************************************/

static StStateClient* STE_psCurrentClient;    //pointer to current client




/******************************************************************************/
/* PUBLIC FUNCTION DEFINITIONS                                                */
/******************************************************************************/

/*:BOOL STE_bHandleStateTable(StStateClient* psClient, EventType EventId, StateType *ActiveState, StateType *LastState)*/
BOOL STE_bHandleStateTable(StStateClient* psClient, EventType EventId, StateType *ActiveState, StateType *LastState)
{
   DBG_STC const StState* psSearchState;                          // pointer of state which has to search for
   DBG_STC StateType      SearchState;                            // state which has to search for
   DBG_STC StateType      NewState;                               // state to jump to, init with STE_ST_NO_STATE--> no jump
   DBG_STC StTransition*  psTransitionTable;                      // pointer to transition table of search state
   DBG_STC uchar          ucNumberOfTransitions;                  // number of transitions in search state
   BOOL bEventUsed = FALSE;

   NewState = STE_ST_NO_STATE;
   STE_psCurrentClient = psClient;
   if ( EventId != (EventType)STE_EV_NO_EVENT)// perform when there is any event
   {
      /*search fitting transition*/
      //**************** SEARCH FOR TRANSITION ****************************
      //Now start search in state tree. No condition because loop has to be done at least one time
      SearchState = *ActiveState;
      for (;;)
      {
         //-----------------------------------------------------------------------
         // load the next state to be observed and watch all transitions
         //-----------------------------------------------------------------------
         //calc pointer to SearchState with clients state table
         psSearchState         =   psGetState(SearchState);
         ucNumberOfTransitions =   psSearchState->NumberOfTrans;
         psTransitionTable     =   psSearchState->TransitionTable;
         //this loop search for fitting transitions in current search state
         //loop is finished when all transitions are search or a transition fit in search state
         while (ucNumberOfTransitions--)
         {
            if  ( bDoesEventFit(EventId, psTransitionTable->EvId) )//event fit?
            {
               if (bIsConditionTrue(psTransitionTable->Condition))
               {
                  /*HIT, perform the action and prepare transition*/
                  // !!!!!!!!!!!!! HIT !!!!!!!!!!!!!
                  //-----------------------------------------------------------------
                  // the actually observed transition has a hit for the active event
                  // and condition is true ->perform the transition
                  //-----------------------------------------------------------------
                  vDoAction(psTransitionTable->Action);         // do the action
                  NewState = psTransitionTable->NewState;           // set new State
                  vReportTransitions((const void*)psTransitionTable);     // report history for debugging
                  bEventUsed = TRUE;
                  break;//hit: so leave loop
               }
               else
               {
                  psTransitionTable++;

               }
            }
            else
            {
               psTransitionTable++;
            }
         }
         if ( (psSearchState->Depth==(uchar)STE_ROOT_LEVEL) || (NewState!=STE_ST_NO_STATE) )
         //if root not reached or transition hit
         //end condition could not be ask in for loop because lopp has to be done at least one time
         {
            break;//if root reached or transition is found
         }
         else
         {
            //continue with loop
         }
         SearchState = psSearchState->Father;
      }
      /*lint -e{923}  cast tested to work correctly on all relevant platforms*/
      if ((size_t)((uint32_t)NewState) > (size_t)STE_STAY_IN_STATE) //any transition to new state?
      {
         //**************************** DO THE TRANSITION ********************************
         /*lint -e{923}  cast tested to work correctly on all relevant platforms*/
         if ((size_t)((uint32_t)NewState) == (size_t)STE_BACK) // Back-Transition?   (see Glossary)
         {
            NewState = *LastState;// go back to last state
         }
         *LastState = SearchState;
         // save search state to make possible a back transition (STE_ST_BACK)
         // Hint: Engine will not jump back to active state.
         // To prevent lost of possibly key out events
         // engine will jump back to search state
         do
         {
            // now look for path to new state and do the Entries/Exits
            vSearchPath(*ActiveState, NewState);
            *ActiveState = NewState; //now change active state
            // look for default path and branch if it is necessary
            NewState = psDoDefaultTransition(*ActiveState);

         }while (NewState != *ActiveState);

      }
      else
      {
         //no change
      }
   }
   else
   {
     vDoAction(psGetState(*ActiveState)->Action);

   }
   return bEventUsed;

}

/*void STE_vHandleStateTable(StStateClient* psClient, EventType EventId, StateType *ActiveState, StateType *LastState)*/
void STE_vHandleStateTable(StStateClient* psClient, EventType EventId, StateType *ActiveState, StateType *LastState)
{
   (void)(STE_bHandleStateTable(psClient, EventId, ActiveState, LastState));
}

/*void STE_vInitStateTable(StStateClient* psClient, StateType *ActiveState, StateType EntryState)*/
void STE_vInitStateTable(StStateClient* psClient, StateType *ActiveState, StateType EntryState)
{
   DBG_STC StateType     NewState;      // state to jump to
   STE_psCurrentClient = psClient;
   *ActiveState        = EntryState;
   NewState            = EntryState;
   do
   {
      // now look for path to new state and do the Entries/Exits, in first loop just do the entry
      vSearchPath(*ActiveState, NewState);
      *ActiveState = NewState; //now change active state
      // look for default path and branch if it is necessary
      NewState = psDoDefaultTransition(*ActiveState);
   }
   while (NewState != *ActiveState);

}

/*~+:********************************************* GLOSSARY ***********************************************************/
/*~+:Always-Transition:*/
/*~+:       Normaly for a change of state an event is necessary. The condition have to be true.*/
/*~+:       An Always-Transition doesn´t need an event. If the condition is true a transition will be performed*/
/*~+:       In the Transition-Table this special Event is named STE_EV_ALWAYS*/
/*~+:       to prevent a lot of calculation time a search of this STE_EV_ALWAYS-transitions only starts when a*/
/*~+:       secial timer is dowm.*/
/*~+:*/
/*~+:Back Transition:*/
/*~+:       A Back-Transition will be branch to the origin of last transition*/
/*~+:       Use the back-transition to unite two or more states.*/

/******************************************************************************/
/* PRIVATE FUNCTION DEFINITIONS                                               */
/******************************************************************************/

/*:encapsulating functions*/

/*the following functions encapsulate the accesss to special structure elements*/
/*This is usefull if structures have to be changed for memory saving,*/
/*or special features like compound actions/conditions wished to implemented.*/
/*Later this functions will be created from database, so don't optimize here, (the compiler/linker will make it)*/
/*StState* psGetState(StateType State)*/

static const StState* psGetState(StateType State)
{
   //this function encapsulate the storage of state in: StTransition, StDefault and StState
   //currently just return the entry
   /*lint -e{929}  cast tested to work correctly on all relevant platforms*/
   return (const StState*)State;
}

/*uchar bIsConditionTrue(ConditionType Condition)*/
static uchar bIsConditionTrue(ConditionType Condition)
{
   uint16_t ConditionId = Condition.ConditionID;
   //this function encapsulate the storage of condition in: StTransition, StDefault
   const ConditionFunc *pfCondition;
   if (Condition.ConditionID != (TbitField)STE_ALWAYS)
   {
      ConditionId-=(uchar)STE_FIRST_REGULAR_CONDITION;
      //get pointer to client condition table
      pfCondition  = STE_psCurrentClient->ppfConditionTable;
      if (pfCondition[ConditionId]())//calculate condition
      {
         //true-condition
         return ( (Condition.TrueCondition == 0u) ? FALSE : TRUE );
      }
      else
      {
         //false-condition
         return ( (Condition.TrueCondition == 0u) ? TRUE : FALSE );
      }
   }
   else
   {
      //cond. is always true
      return(TRUE);
   }

}

/*void vDoAction(ActionType Action)*/
static void vDoAction(ActionType Action)
{
   //this function encapsulate the storage of action in: StTransition, StDefault, StState
   //currently ActionType is a function pointer so just do the action
   if (Action!=STE_NO_ACTION)
   {
      Action();
   }
}

/*uchar bDoesEventFit(EventType EventId, EventType FittingEvent)*/
static uchar bDoesEventFit(EventType EventId, EventType FittingEvent)
{
   DBG_STC const EventType *pCompoundEvent;
   DBG_STC uchar ucCmpEventId;
   if (FittingEvent > (EventType)STE_psCurrentClient->LastCompoundEvent)
   {
      //no compount event
      return ((EventId == FittingEvent) ? TRUE : FALSE);
   }
   else
   {
      //compount event
      ucCmpEventId = FittingEvent-(EventType)STE_FIRST_REGULAR_EVENT;
      //set pointer to start of compondevent event table
      pCompoundEvent=
      STE_psCurrentClient->ppCompoundEventTable[ucCmpEventId];
      //now set pointer to given compound event
      //ppCompoundEvent+= ((FittingEvent-STE_FIRST_REGULAR_EVENT));
      //now ppCompoundEvent point to compound event
      //all entries until the terminating zero are fitting events
      //now start search if any entry fit
      //search until terminating zero (like C-string)
      while (*pCompoundEvent!=(EventType)0x00)
      {
         if (  (*(pCompoundEvent++)) == EventId)
         {
            return(TRUE);//there is a fitting entry!
         }
      }
      return(FALSE);
   }
}

/*void vSearchPath(StateType OriginState, StateType DestinationState)*/
// search path from OriginState to DestinationState and do the entries and exits
static void vSearchPath(StateType OriginState, StateType DestinationState)
{
   DBG_STC EntryFunc       EntryStack[MAX_LEVEL+1]; // to save entry path to destination
   DBG_STC const StState  *psSearchStateEntry;      // pointer to current entry search state
   DBG_STC const StState  *psSearchStateExit;       // pointer to current exit search state
   DBG_STC StateType       SearchStateEntry;        // index from current entry search state
   DBG_STC StateType       SearchStateExit;         // index from current exit search state
   DBG_STC uchar EntryIndex;                    // loop counter for entry

   EntryIndex = 0;
   EntryStack[EntryIndex] = STE_NO_ACTION;
   SearchStateEntry = DestinationState;     // init entry state, search entry path
   SearchStateExit  = OriginState;          // init exit state, search exit path
   
   
   do
   {	  
      psSearchStateEntry = psGetState(SearchStateEntry); // get pointer to entry state
      psSearchStateExit =  psGetState(SearchStateExit);  // get pointer to exit state
      if (psSearchStateEntry->Depth > psSearchStateExit->Depth) // depend on depth look for entry or exit path
      {
         // save entry to make them in invers order
         EntryStack[EntryIndex] = psSearchStateEntry->Entry;
         EntryIndex++;
         SearchStateEntry = psSearchStateEntry->Father;
      }

      else
      {
         if (psSearchStateExit->Exit != STE_NO_ACTION)
         {
            // exit do at once
            psSearchStateExit->Exit();
         }

         if (psSearchStateExit->Depth)

         {
            SearchStateExit = psSearchStateExit->Father;
         }
         else
         {
            EntryStack[EntryIndex] = psSearchStateEntry->Entry;
            EntryIndex++;
            break;
            //root of tree reached
         }
      }
   }
   while (SearchStateEntry != SearchStateExit);// do until EntryState = ExitState

   while (EntryIndex)//do entries in invers order
   {
        EntryIndex--;
        if (EntryStack[EntryIndex]!=STE_NO_ACTION)
        {
            EntryStack[EntryIndex]();
        }
    }
}

/*StState* psDoDefaultTransition(StateType State)*/
// handle the default transitions of State
static const StState* psDoDefaultTransition(StateType State)
{
   DBG_STC uchar       DefaultIndex;
   DBG_STC StDefault*  psDefaultTable;
   DBG_STC StateType   DefaultState;
   DefaultState   = State;
   psDefaultTable = psGetState(DefaultState)->DefaultTable;
   DefaultIndex   = psGetState(DefaultState)->NumberOfDef;
   while (DefaultIndex--)
   {
      if( bIsConditionTrue(psDefaultTable->Condition) )
      {
         vDoAction(psDefaultTable->Action);           // condition is true, so do the action
         DefaultState = psDefaultTable->NewState;     // and finaly change the state
         vReportTransitions((const void*)psDefaultTable);   // report history for debugging
         break;
      }
      else
      {
         psDefaultTable++;
      }
   }
   /*lint -e{929}  cast tested to work correctly on all relevant platforms*/
   return((const StState*)DefaultState);
}

/** \brief    reports a performed transition to save it in a ring buffer.
 *
 *  \details  this function is used for debugging. It helps to find out the history 
              of a state engine.
 *
 *  \param[in]  ExecutedTransition:    pointer to performed transition
 *
 */ 
static void vReportTransitions(const void *ExecutedTransition)
{
    static uchar STE_ucCurrentHistoryIndex = 0;
    
    STE_pvHistoryTable[STE_ucCurrentHistoryIndex] = ExecutedTransition;

    if ( STE_ucCurrentHistoryIndex >= (STE_ucHistoryDepth-1u) )
    {
        STE_ucCurrentHistoryIndex =0u;
    }
    else
    {
        STE_ucCurrentHistoryIndex++;
    }

}


