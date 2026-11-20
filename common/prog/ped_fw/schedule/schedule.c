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
 *  COMP_ABBREV      SCH
 ******************************************************************************/


/******************************************************************************/
/* DOCUMENTATION                                                              */
/******************************************************************************/
/** \file
 *
 *  \brief  Implementation of scheduler library
 *
 */


/**************************************************************************************************/
/* INCLUDES                                                                                       */
/**************************************************************************************************/
#include "bsh_stdinc.h"
#include <stdint.h>
#include <stdbool.h>
#include "schedule.h"
#include "schpriv.h"
#include "utility/utility.h"

/* Definitions and Declarations*/

/* DEFINITIONS ******************************************************************************************/

/** enumeration for LP task search state */
enum SCH_searchState_e
{
   SEARCH_FIRST_LP,
   SEARCH_NEXT_LP,
   SEARCH_READY
};

enum
{//enumeration for Task Table status state
   TAB_INIT,        ///< state after reset
   TAB_CHANGED,     ///< task table was changed
   TAB_NON_CHANGED  ///< task table was'nt changed
};

/* TYPE DEFINITIONS*/

/* PROTOTYPES*/

/* FUNCTION PROTOTYPES *********************************************************************************/
static bool bCheckDependency(TtaskId tTaskId);///< checks if dependencies of task tTaskId are solved

/* VARIABLES ******************************************************************************************/
static const TschMainTaskTable*  SCH_ptMainTaskTable;///< Pointer to current main task table of application

static TtaskId SCH_tLowPriorityTaskId;///< Task Id of last called low priority module
static uint8_t SCH_ucTskTabStatus = TAB_INIT; ///< Task Table status flag [TAB_INIT, TAB_CHANGED, TAB_NON_CHANGED]
static bool SCH_bAllTskInitStatus = false; ///< All Task's Init status flag [true = all task are initialized, otherwise false]
static enum SCH_searchState_e SCH_searchState = SEARCH_FIRST_LP;
/* CONSTANTS ******************************************************************************************/

/* Static Functions*/
//Function for detecting correct dependencies of a module
static bool bCheckDependency(TtaskId tTaskId)
{
   TtaskId const * ptDependency;

   ptDependency = SCH_pptDependencyListPtrTable[tTaskId]; //startpoint for searching right dependencies
   while (*ptDependency != (TtaskId)SCH_END_MARK)
   {
      if (!UTI_bGetBit(SCH_pucInitStateTable,*ptDependency)) //dependent modules are not initialized?
      {
         return false; //dependency not solved
      }
      else
      {
         //dependency solved
      }
      /*lint -e{662} no out of bound access here! */
      ptDependency++; //no out-of-bounds pointer Misra 18.1, as end mark checked in while condition
   }
   return true; //no dependencies or dependencies solved

}

/* Global Functions*/


//function for standard-management of TaskHandlers
bool SCH_bHandleTask(void)
{
   TtaskId tTaskIndex;  // index in main task table
   TtaskId tPropertyId; // index in task property table
   uint8_t   ucByteIndex; // byte which contains current property bit
   uint8_t   ucBitMask;   // bitmask of current property

   SCH_searchState = SEARCH_FIRST_LP; // initialize low prior state: search for first
   SCH_bAllTskInitStatus = true; // default -> all task's are initialized

   for (tTaskIndex=0; ((SCH_ptMainTaskTable[tTaskIndex].TaskId != (TtaskId)SCH_END_MARK) && (SCH_ucTskTabStatus == (uint8_t)TAB_NON_CHANGED)); tTaskIndex++)
   {
      tPropertyId = SCH_ptMainTaskTable[tTaskIndex].TaskId;
      ucByteIndex = tPropertyId/(TtaskId)8;          //ByteIndex, it is used to detect the right Position in the Propertytable
      ucBitMask   = 1U << (tPropertyId % (TtaskId)8);   //BitMask, it is used to specify the right Bit in the Byte

      if ((SCH_pucDisableTable[ucByteIndex] & ucBitMask)) //current module disabled?
      {
         continue; //disabled; leaving 'for-loop'
      }

      if ((SCH_pucInitStateTable[ucByteIndex] & ucBitMask) == 0u) //module not initialized ?
      {
         if (bCheckDependency(tPropertyId) == false) //dependencies to other modules are not solved?
         {
            SCH_bAllTskInitStatus = false; //not all task's are initialized
            continue; //depend. not solved, leaving 'for-loop'
         }
         else
         {
            //depend. solved
         }
      }

      //NOW: current module is initialized or dependency is solved
      if (SCH_pucPriorityTable[ucByteIndex] & ucBitMask) //Is high priority task?
      {
         if (_SCH_ucDoTask(tPropertyId))
         {
            SCH_pucInitStateTable[ucByteIndex] |= ucBitMask; 
         }
         else
         {
            SCH_bAllTskInitStatus = false; //not all task's are initialized
         }
      }
      else
      {
         //searching next LowPriorty Task 
         if (SCH_searchState != SEARCH_FIRST_LP ) //Next LowPriority Task already found
         {
             SCH_searchState =  SEARCH_READY; //next LP-Task found
         }
         else
         {
            if (tTaskIndex>=SCH_tLowPriorityTaskId)  //is current task be located after the last LowPriortity task which carried out?
            {
               SCH_tLowPriorityTaskId = tTaskIndex; //set current LowPrior Task!
               SCH_searchState = SEARCH_NEXT_LP; //Looking for a further LP-Task
            }
         }
      }
   }

   //NOW: current task table was changed by executed handle task
   if (SCH_ucTskTabStatus == (TtaskId)TAB_CHANGED)
   {
      SCH_ucTskTabStatus = (uint8_t)TAB_NON_CHANGED;
      return false; // task table changed -> restart SCH_bHandleTask
   }

   //prepare next low prior task
   if (SCH_searchState != SEARCH_FIRST_LP)//any LP tasks?
   {
      tPropertyId = SCH_ptMainTaskTable[SCH_tLowPriorityTaskId].TaskId; //pointer to ID of the Task(handler)
      ucByteIndex = tPropertyId/(TtaskId)8;          //ByteIndex, it is used to detect the right Position in the Propertytable
      ucBitMask   = 1U << (tPropertyId % (TtaskId)8);   //BitMask, it is used to specify the right Bit in the Byte

      if (_SCH_ucDoTask(tPropertyId) != TASK_NOT_INITIALISED)
      {
         SCH_pucInitStateTable[ucByteIndex] |= ucBitMask; 
      }

      if (SCH_searchState == SEARCH_READY) //no further LowPriorityTask?
      {
         SCH_tLowPriorityTaskId++;
      }
      else
      {
         SCH_tLowPriorityTaskId=0;
      }
   }
   else
   {
      //there are no LP tasks
   }
   return false;
}

/* void SCH_vSetMainStateTable(const TschMainTaskTable* ptMainTbl)*/
void SCH_vSetMainStateTable(const TschMainTaskTable* ptMainTbl)
{
    TtaskId tTaskIndex;         //index in main task table

    SCH_tLowPriorityTaskId = 0;             //start as top of the main task table
    SCH_ptMainTaskTable    = ptMainTbl;       //save pointer to applications main task table

    if (SCH_ucTskTabStatus == (uint8_t)TAB_INIT)
    {
        SCH_ucTskTabStatus = (uint8_t)TAB_NON_CHANGED;
    }
    else
    {
        SCH_ucTskTabStatus = (uint8_t)TAB_CHANGED;
    }
    //copy priority bit to table sorted by new main task table
    for (tTaskIndex=0; SCH_ptMainTaskTable[tTaskIndex].TaskId != SCH_END_MARK; tTaskIndex++)
    {
        bool bPriority = (SCH_ptMainTaskTable[tTaskIndex].Priority == 0u) ? false : true;
        UTI_vSetBit(SCH_pucPriorityTable, SCH_ptMainTaskTable[tTaskIndex].TaskId, bPriority);

    }
}

/* void SCH_vSwitchModule(TtaskId tTaskId, bool bOnOff)*/
//Function can change the disabling state of a module in true or false
void SCH_vSwitchModule(TtaskId tTaskId, bool bOnOff) 
{
    if ( ( UTI_bGetBit(SCH_pucDisableTable, tTaskId) == false ) && bOnOff )
    {   // don't do anything if activated task need to be activated
        return;
    }
    SCH_tLowPriorityTaskId = 0; // searching for next low prio task has to be restarted
    SCH_searchState = SEARCH_FIRST_LP;
    UTI_vSetBit(SCH_pucDisableTable, tTaskId, (bool)!bOnOff); //set disable Bit to OnOff (True/False) value      
}

/* void SCH_vSetPriority (TtaskId tTaskId, bool bPriority)*/
void SCH_vSetPriority (TtaskId tTaskId, bool bPriority)
{
   UTI_vSetBit(SCH_pucPriorityTable, tTaskId, bPriority); //set new priority

   if (bPriority)
   {
      SCH_tLowPriorityTaskId=0; //restart of the Low Priority
      SCH_searchState = SEARCH_FIRST_LP;
   }
}

/* bool SCH_bGetInitModuleStatus(TtaskId tTaskId)*/
//Function returns init state of a module tTaskId
bool SCH_bGetInitModuleStatus(TtaskId tTaskId) 
{
   return ((bool) UTI_bGetBit(SCH_pucInitStateTable, (uint16_t) tTaskId));//return init state of tTaskId task handler
}

/* bool SCH_bGetInitMainTaskTableStatus(void)*/
//Function returns true if all modules in current task table are initialized, otherwise false
bool SCH_bGetInitMainTaskTableStatus(void)
{
   return (SCH_bAllTskInitStatus);         // return init Tsk's status
}

