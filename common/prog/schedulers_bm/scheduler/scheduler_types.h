/*******************************************************************************
 *  Copyright (c) 2017 BSH Hausgeraete GmbH,
 *  Carl-Wery-Str. 34, 81739 Munich, Germany, www.bsh-group.de
 *
 *  All rights reserved. This program and the accompanying materials
 *  are protected by international copyright laws.
 *  Please contact copyright holder for licensing information.
 *
 *******************************************************************************
 *   PROJECT          Scheduler NEW (Generic SW)
 *   AUTHOR           Raundal
 *   CREATED          12.05.2018
 ******************************************************************************/

/**************************************************************************************************/
/* DOCUMENTATION                                                                                  */
/**************************************************************************************************/
/// \file Public application specific definitions and declarations for scheduler

#ifndef SCHEDULER_TYPES_H_
#define SCHEDULER_TYPES_H_

/**************************************************************************************************/
/* INCLUDES                                                                                       */
/**************************************************************************************************/
#include "scheduler/scheduler_cfg.h"
#include "scheduler_defines.h"


/**************************************************************************************************/
/* DEFINITIONS                                                                        */
/**************************************************************************************************/

/**************************************************************************************************/
/* PUBLIC TYPE DEFINITIONS                                                                        */
/**************************************************************************************************/
/**!
 *  \brief      Function pointer for the TaskHandlers function pointer List
 *
 */
typedef uint8_t (*TaskHandlers)(void);


/**!
 *  \brief      Scheduler Execution Phases
 *
 *  \details    INITIALIZATION_PHASE - Scheduler is in Initialization Phase for only as long as not all tasks are initialized
 *              SCHEDULING_PHASE     - Scheduling Phase takes over soon as all tasks are initialized
 */
typedef enum SCH_Phases
{
    INITIALIZATION_PHASE,
    SCHEDULING_PHASE
}SchedulerPhase;


/**!
 *  \brief      Task Execution State
 *
 *  \details    TERMINATED_STATE - Task is in Terminated/Inactive state
 *              ACTIVE_STATE     - Task is in Active state
 *
 *  \Note       If other tasks has dependencies on this task, Task Termination allowed only after task initialization.
 */
typedef enum SCH_Execution_States
{
    TERMINATED_STATE,
    ACTIVE_STATE
}TaskExecutionState;


/**!
 *  \brief      Tasks Priorities HIGH or LOW
 *
 *  \details    PRIO_HIGH - Each Task with this priority is called during every call of the  corresponding function
 *              PRIO_LOW  - One sequential Task is called during every call of the corresponding function
 *
 *  \note       Numerically Lower Value implies higher Priority.
 *              Non-Preemptive!
 */
typedef enum SCH_Priorities
{
    HIGH_PRIORITY,
    LOW_PRIORITY
} TaskPriority;


/**!
 *  \brief      Every Module has 2 properties, a Task ID and Priority.
 *
 */
struct SCH_TaskProperties
{
    TaskID          Task_ID;            ///< ID of the Module
    TaskPriority    Task_Priority;      ///< Task Priority of the Module [PRIO_HIGH, PRIO_LOW]
};


/**!
 *  \brief      Main Project Inventory, it contains all the necessary Information needed about the
 *              project modules needed by the Scheduler.
 *
 */
struct SCH_ProjectInventory
{
    const TaskID* const DependenciesList[NO_OF_MODULES];
    const TaskHandlers TaskHandlersList[NO_OF_MODULES];
    struct SCH_TaskProperties TasksList[NO_OF_MODULES_EXT];
};


/**!
 *  \brief      Dynamic Inventory, it contains all the necessary Tables and Information which is required
 *              by the scheduler during runtime.
 *
 */
struct SCH_DynamicInventory
{
#ifdef SCH_OPTIMIZE_PERFORMANCE
    struct Task_Lineups
    {
        TaskID HighPrio[NO_OF_MODULES_EXT];
        TaskID LowPrio[NO_OF_MODULES_EXT];
    }TaskLineup;
#endif
    struct State_Tables
    {
        uint8_t TaskInit[NO_OF_MODULES_WRAPPED];
        uint8_t TaskExecution[NO_OF_MODULES_WRAPPED];
    }StateTable;

    struct Information
    {
        uint8_t SystemInitState;
        uint8_t ActiveHPTasks;
        uint8_t ActiveLPTasks;
        TaskID  FinalLowPrioTaskID;
        TaskID  CurrentRunningTaskID;
        SchedulerPhase CurrentActivePhase;
    }Info;
};

struct SCH_TM_TimeProperties
{
#if(DEBUG_MODE == DEBUG_SINGLE_MODULE)
    TaskID Task_ID;
#endif
    uint32_t time_periods[NUMBER_OF_SAMPLES];
    int32_t  task_jitter_average;
    uint32_t task_runtimes[NUMBER_OF_SAMPLES];
    uint8_t  sample_counter;
    
};

#endif /* SCHEDULER_TYPES_H_ */


/********************************************* End of File ******************************************************/
