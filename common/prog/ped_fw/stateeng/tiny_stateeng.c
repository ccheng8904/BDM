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
 *  \brief    Implementation of tiny state engine library.
 *
 */

 
/******************************************************************************/
/* INCLUDES                                                                   */
/******************************************************************************/
#include "bsh_stdinc.h"
#include "tiny_stateeng.h"
#include "LibTypes.h"


/* DEFINITIONS ****************************************************************************************/

//#define STATIC   static
#define STATIC


/* TYPE DEFINITIONS ***********************************************************************************/


/* PROTOTYPES *****************************************************************************************/


/* VARIABLES ******************************************************************************************/


/* CONSTANTS ******************************************************************************************/





extern const TState* STE_ptActiveState;
void STE_vHandleTinyStateTable(uchar ucEventId)

{
   STATIC const TState*       ptSearchState;      // state which has to search for
   STATIC const Ttransition*  ptTransition;       // pointer to transition table of search state

   STATIC uchar               ucNumberOfTransitions;    // number of transitions in search state

   if ( ucEventId != STE_EV_NO_EVENT)// perform when there is any event
   {
      //**************** SEARCH FOR TRANSITION ****************************
      ptSearchState = STE_ptActiveState;
      do
      {
         ucNumberOfTransitions = ucGetStateNumberOfTransitions(ptSearchState); //get number of transitions in search state
         ptTransition          = ptGetStateTransitionTable(ptSearchState);     //get pointer to transition table

         //this loop search for fitting transitions in current search state
         while (ucNumberOfTransitions--)
         {
            if( bIsTransitionEvent(ptTransition, ucEventId) )//event fit?

            {
               if (bIsTransitionCondition(ptTransition))
               {
                  // !!!!!!!!!!!!! HIT !!!!!!!!!!!!!
                  //-----------------------------------------------------------------
                  // the actually observed transition has a hit for the active event 
                  // and condition is true ->perform the transition                  
                  //-----------------------------------------------------------------

                  if (ptGetTransitionNewState(ptTransition) != STE_STAY_IN_STATE)
                  {
                     vDoStateExit(STE_ptActiveState);
                     vDoStateEntry(ptGetTransitionNewState(ptTransition));
                     STE_ptActiveState = ptGetTransitionNewState(ptTransition);

                  }
                  vDoTransitionAction(ptTransition);
                  //vReportTransitions(ptTransition);// report history for debugging

                  return;//Transition done
               }
            }
            ptTransition++;

         }
         ptSearchState = ptGetStateFather(ptSearchState);
      }
      while (ptSearchState!=STE_NO_STATE);//do until root is reached

   }
   else
   {
      vDoStateAction(STE_ptActiveState);

   }
}


BOOL STE_bAlways(void)
{
   return TRUE;
}

void STE_vNoAction(void)
{

}

//********************************************* GLOSSARY ***********************************************************/
/* Always-Transition:                                                                                              */
/*       Normaly for a change of state an event is necessary. The condition have to be true.                       */
/*       An Always-Transition doesn´t need an event. If the condition is true a transition will be performed       */
/*       In the Transition-Table this special Event is named STE_EV_ALWAYS                                         */
/*       to prevent a lot of calculation time a search of this STE_EV_ALWAYS-transitions only starts when a        */
/*       special timer is dowm.                                                                                    */
/*                                                                                                                 */
/* Back Transition:                                                                                                */
/*       A Back-Transition will be branch to the origin of last transition                                         */
/*       Use the back-transition to unite two or more states.                                                      */
/*******************************************************************************************************************/

