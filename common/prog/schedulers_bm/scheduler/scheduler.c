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
/// \file Implementation of module scheduler


/**************************************************************************************************/
/* INCLUDES                                                                                       */
/**************************************************************************************************/
#include "bsh_stdinc.h"
#include <stdbool.h>
#include "utility.h"
#include "scheduler.h"
#include "hwdt.h"
#include <string.h>
#ifdef   SCH_ACTIVATE_DEBUG_MODE
#include "scheduler_time_monitor.h"
#endif


/**************************************************************************************************/
/* LOCAL DEFINITIONS                                                                              */
/**************************************************************************************************/

/**************************************************************************************************/
/* LOCAL TYPE DEFINITIONS                                                                         */
/**************************************************************************************************/

/**************************************************************************************************/
/* EXTERN VARIABLE DEFINITIONS                                                                    */
/**************************************************************************************************/

/**************************************************************************************************/
/* LOCAL VAR DEFINITIONS                                                                          */
/**************************************************************************************************/
static struct SCH_DynamicInventory SCH_RuntimeCatalogue;
static struct SCH_ProjectInventory *SCH_ProjectCatalogue;

/**************************************************************************************************/
/* LOCAL FUNCTION DECLARATIONS                                                                    */
/**************************************************************************************************/
static void SCH_scheduleHPTasks(void);
static void SCH_scheduleLPTasks(void);
static void SCH_initializeHPTasks(void);
static void SCH_initializeLPTasks(void);
static void SCH_handleSchedulingPhase(void);
static void SCH_setupSchedulingLineups(void);
static void SCH_handleInitializationPhase(void);
static bool SCH_initializationPhaseNotOver(void);
static bool SCH_isTimerTaskInitialized(void);
static void SCH_handleTask(const TaskID task_id);
static uint8_t SCH_executeTask(const TaskID task_id);
static bool SCH_isItFinalLPTask(const TaskID task_id);
static TaskID SCH_getTaskID(const uint8_t task_index);
static void SCH_gatherSchedulerData(const TaskID task_id);
static bool SCH_areDependenciesSolved(const TaskID task_id);
static bool SCH_getTaskExecutionState(const TaskID task_id);
static bool SCH_areAllPrerequisitesMet(const TaskID task_id);
static TaskPriority SCH_getTaskPriority(const TaskID task_id);
static bool SCH_isLPTaskIndexOutOfRange(const uint8_t task_index);
static void SCH_setTaskInitState(const TaskID task_id, const bool init_state);
static bool SCH_isItReadyLPTask(const TaskID task_id, const int16_t last_task_id);
static void SCH_setTaskExecutionState(const TaskID task_id, const bool task_state);
static void SCH_resetRuntimeCatalogue(void);
#ifdef SCH_OPTIMIZE_PERFORMANCE
static TaskID SCH_getHPTaskID(const uint8_t task_index);
static TaskID SCH_getLPTaskID(const uint8_t task_index);
#endif

/**************************************************************************************************/
/* LOCAL FUNCTION DEFINITIONS                                                                     */
/**************************************************************************************************/
/**!
 *  \brief      Gathers the needed data about the scheduler
 *
 *  \param      none
 *
 *  \return     void
 *
 *  \details    Final low priority task id is retrieved
 */
static void SCH_gatherSchedulerData(const TaskID task_id)
{
    if((SCH_getTaskPriority(task_id) == LOW_PRIORITY))
    {
        SCH_RuntimeCatalogue.Info.FinalLowPrioTaskID = task_id;
    }
}


/**!
 *  \brief      The tables needed for Scheduling Phase are setup here
 *
 *  \param      none
 *
 *  \return     void
 *
 *  \details
 */
static void SCH_setupSchedulingLineups(void)
{
    TaskID task_id;
    uint8_t task_index = 0U;
    uint8_t task_index_hp = 0U;
    uint8_t task_index_lp = 0U;

    while((task_id = SCH_getTaskID(task_index)) != END_STAMP)//lint !e720 "result of assignment used" : not critical here
    {
        if(SCH_isModuleActive(task_id))
        {
            if((SCH_getTaskPriority(task_id) == HIGH_PRIORITY))
            {
#ifdef SCH_OPTIMIZE_PERFORMANCE
                SCH_RuntimeCatalogue.TaskLineup.HighPrio[task_index_hp] = task_id;
#endif
                task_index_hp++;
            }
            else
            {
#ifdef SCH_OPTIMIZE_PERFORMANCE
                SCH_RuntimeCatalogue.TaskLineup.LowPrio[task_index_lp] = task_id;
#endif
                task_index_lp++;
            }
        }

        task_index++;
    }

    SCH_RuntimeCatalogue.Info.ActiveHPTasks = task_index_hp;
    SCH_RuntimeCatalogue.Info.ActiveLPTasks = task_index_lp;

#ifdef SCH_OPTIMIZE_PERFORMANCE
    SCH_RuntimeCatalogue.TaskLineup.HighPrio[task_index_hp] = END_STAMP;
    SCH_RuntimeCatalogue.TaskLineup.LowPrio[task_index_lp]  = END_STAMP;
#endif
}


/**!
 *  \brief      Set the Task Active state table
 *
 *  \param      none
 *
 *  \return     void
 *
 *  \details    If the bit is true, task is active, else task is deactivated
 */
static void SCH_setTaskExecutionState(const TaskID task_id, const bool task_state)
{
    UTI_vSetBit(SCH_RuntimeCatalogue.StateTable.TaskExecution, (uint16_t)task_id, task_state);
}


/**!
 *  \brief      Set Init state of a module in InitStateTable
 *
 *  \param      uint8_t task_id
 *              uint8_t init_state
 *
 *  \return     void
 *
 *  \details
 */
static void SCH_setTaskInitState(const TaskID task_id, const bool init_state)
{
    UTI_vSetBit(SCH_RuntimeCatalogue.StateTable.TaskInit, (uint16_t)task_id, init_state);
    SCH_RuntimeCatalogue.Info.SystemInitState++;
}


/**!
 *  \brief      Get the priority of a task
 *
 *  \param      none
 *
 *  \return     task_priority   PRIO_HIGH or PRIO_LOW
 *
 *  \details
 */
static TaskPriority SCH_getTaskPriority(const TaskID task_id)
{
    return ((SCH_ProjectCatalogue->TasksList[task_id].Task_Priority));
}


/**!
 *  \brief      Get task ID from the main task table of the task
 *
 *  \param      none
 *
 *  \return     uint8_t task_id
 *
 *
 *  \details
 */
static TaskID SCH_getTaskID(const uint8_t task_index)
{
    return (SCH_ProjectCatalogue->TasksList[task_index].Task_ID);
}

/**!
 *  \brief      Get the corresponding task id from the HP_taskTable
 *
 *  \param      uint8_t     task_index
 *
 *  \return     uint8_t     task_id
 *
 *  \details
 */
#ifdef SCH_OPTIMIZE_PERFORMANCE
static TaskID SCH_getHPTaskID(const uint8_t task_index)
{
    return SCH_RuntimeCatalogue.TaskLineup.HighPrio[task_index];

}

/**!
 *  \brief      Get the corresponding task id from the LP_taskTable
 *
 *  \param      const uint8_t task_index
 *
 *  \return     uint8_t     task_id
 *
 *  \details
 */
static TaskID SCH_getLPTaskID(const uint8_t task_index)
{
    return SCH_RuntimeCatalogue.TaskLineup.LowPrio[task_index];
}
#endif

/**!
 *  \brief      Get the task execution state
 *
 *  \param      const uint8_t task_id
 *
 *  \return     bool    INACTIVE_STATE OR ACTIVE_STATE
 *
 *  \details
 */
static bool SCH_getTaskExecutionState(const TaskID task_id)
{
    bool state;

    state = UTI_bGetBit(SCH_RuntimeCatalogue.StateTable.TaskExecution, (uint16_t)task_id);

    return state;
}


/**!
 *  \brief      Get the task initialization state
 *
 *  \param      const uint8_t task_id
 *
 *  \return     bool    TASK_NOT_INITIALIZED/TASK_TERMINATED OR TASK_INITIALIZED
 *
 *  \details
 */
static bool SCH_getTaskInitState(const TaskID task_id)
{
    bool status;

    status = UTI_bGetBit(SCH_RuntimeCatalogue.StateTable.TaskInit, (uint16_t)task_id);

    return status;
}


/**!
 *  \brief      This function checks if all the dependencies of a task are resolved
 *
 *  \param      uint8_t task_id - Task Id of which the dependencies are needed to be checked
 *
 *  \return     bool    true or false
 *
 *  \details
 */
static bool SCH_areDependenciesSolved(const TaskID task_id)
{
    bool status = true;
    TaskID const *dependency;

    dependency = SCH_ProjectCatalogue->DependenciesList[task_id];

    while(*dependency != END_STAMP)
    {
        if (!SCH_isTaskInitialized(*dependency))
        {
            status = false;
            break;
        }
        /* Axivion Disable Style MisraC2012-18.1 : "Pointer arithmetic creates pointer outside array bounds": false positive */
        dependency++;
    }
    return status;
}

/**!
 *  \brief      Check if the provided task is the correct task to be initialized
 *
 *  \param      uint8_t task_id
 *              int8_t last_task_id
 *
 *  \return     true or false
 *
 *  \details
 */
static bool SCH_isItReadyLPTask(const TaskID task_id, const int16_t last_task_id)
{
    return ((int16_t)task_id > last_task_id);
}


/**!
 *  \brief      Check if the provided task is the final task in the main task table
 *
 *  \param      const uint8_t task_id
 *
 *  \return     true or false
 *
 *  \details
 */
static bool SCH_isItFinalLPTask(const TaskID task_id)
{
    return (task_id == SCH_RuntimeCatalogue.Info.FinalLowPrioTaskID);
}

/**!
 *  \brief      Check if the LP Task Index ran out of Range
 *
 *  \param      uint8_t task_index
 *
 *  \return     true or false
 *
 *  \details
 */
static bool SCH_isLPTaskIndexOutOfRange(const uint8_t task_index)
{
    return (task_index > SCH_RuntimeCatalogue.Info.ActiveLPTasks);
}


/**!
 *  \brief      Check if all the Prerequisites for performing the task are satisfied
 *
 *  \param      uint8_t task_id
 *
 *  \return     status TRUE or FALSE
 *
 *  \details
 */
static bool SCH_areAllPrerequisitesMet(const TaskID task_id)
{
    bool status = true;

    if(!SCH_isModuleActive(task_id))
    {
        status = false;
    }
    else
    {
        if(!SCH_isTaskInitialized(task_id))
        {
            if(!SCH_areDependenciesSolved(task_id))
            {
                status = false;
            }
        }
    }

    return status;
}


/**!
 *  \brief      Check if the Initialization phase is over or not
 *
 *  \param      none
 *
 *  \return     bool    true or false
 *
 *  \details    'true' return value determines the initialization phase is not over, 'false' meaning the initialization
 *              phase is over and scheduler should shift to scheduling phase.
 */
static bool SCH_initializationPhaseNotOver(void)
{
    return (!SCH_areAllModulesInitialized());
}


/**!
 *  \brief      Execute the task from TaskHandlers Table
 *
 *  \param      uint8_t task_id
 *
 *  \return     task_state  TASK_NOT_INITIALISED or TASK_INITIALISED or TASK_TERMINATE
 *
 *  \details
 */
static uint8_t SCH_executeTask(const TaskID task_id)
{
    return (SCH_ProjectCatalogue->TaskHandlersList[task_id])();
}


/**!
 *  \brief      This function returns InitState of Timer task.
 *
 *  \param      uint8_t task_id
 *
 *  \return     TRUE or FALSE
 *
 *  \details
 */
static bool SCH_isTimerTaskInitialized(void)
{
    return(SCH_getTaskInitState(MOD_TIM));
}


/**!
 *  \brief      Perform the particular task and handle task states
 *
 *  \param      uint8_t task_id
 *
 *  \return     void
 *
 *  \details
 */
static void SCH_handleTask(const TaskID task_id)
{
    uint8_t task_state = TASK_NOT_INITIALISED;

#ifdef SCH_ACTIVATE_DEBUG_MODE
    if(SCH_isTimerTaskInitialized())
    {
#if(DEBUG_MODE == DEBUG_SINGLE_MODULE)
        if(task_id == DEBUG_MOD_ID)
        {
            SCH_TM_measureTaskTimeProperties(task_id);
        }
#else
            SCH_TM_measureTaskTimeProperties(task_id);
#endif
    }
#endif

#ifdef SCH_OPTIMIZE_PERFORMANCE
        SCH_RuntimeCatalogue.Info.CurrentRunningTaskID = task_id;
        task_state = SCH_executeTask(task_id);
#else
    if(SCH_isModuleActive(task_id))
    {
        SCH_RuntimeCatalogue.Info.CurrentRunningTaskID = task_id;
        task_state = SCH_executeTask(task_id);
    }
#endif


#ifdef SCH_ACTIVATE_DEBUG_MODE
    if(SCH_isTimerTaskInitialized())
    {
#if(DEBUG_MODE == DEBUG_SINGLE_MODULE)
        if(task_id == DEBUG_MOD_ID)
        {
            SCH_TM_measureTaskRuntime(task_id);
        }
#else
            SCH_TM_measureTaskRuntime(task_id);
#endif
    }
#endif
   
    if(task_state == TASK_INITIALISED)
    {
        if(!SCH_isTaskInitialized(task_id))
        {
            SCH_setTaskInitState(task_id, (bool)true);
        }
    }
    else if(task_state == TASK_TERMINATE)
    {
        SCH_terminateModule(task_id);
    }
    else
    {
        //do nothing
    }
}


/**!
 *  \brief      Initialize High Priority Tasks
 *
 *  \param      none
 *
 *  \return     void
 *
 *  \details    This task handler searches all the Tasks that have High Priority
 *              in the main task table and initialized them one by one if the dependencies are resolved.
 */
static void SCH_initializeHPTasks(void)
{
    TaskID task_id;
    uint8_t task_index = 0U;

    while((task_id = SCH_getTaskID(task_index)) != (END_STAMP))//lint !e720 "result of assignment used" : not critical here
    {
        if((SCH_getTaskPriority(task_id) == HIGH_PRIORITY))
        {
            if((SCH_areAllPrerequisitesMet(task_id)))
            {
                SCH_handleTask(task_id);
            }
        }

        task_index++;
    }
}


/**!
 *  \brief      Initialize Low Priority Tasks
 *
 *  \param      none
 *
 *  \return     void
 *
 *  \details    For each call of this task handler, one low priority task is initialized from the main
 *              task table and for the next call, next low priority task.
 */
static void SCH_initializeLPTasks(void)
{
    TaskID task_id;
    uint8_t task_index = 0U;
    const int16_t reset_LP_task = -1;
    static int16_t last_LP_task = -1;

    while((task_id = SCH_getTaskID(task_index)) != END_STAMP)//lint !e720 "result of assignment used" : not critical here
    {
        if((SCH_getTaskPriority(task_id) == LOW_PRIORITY))
        {
            if((SCH_areAllPrerequisitesMet(task_id)) && (SCH_isItReadyLPTask(task_id, last_LP_task)))
            {
                SCH_handleTask(task_id);

                if(SCH_isItFinalLPTask(task_id))
                {
                    last_LP_task = reset_LP_task;
                }
                else
                {
                    last_LP_task = (int16_t)task_id;
                }

                break;
            }
            else
            {
                if(SCH_isItFinalLPTask(task_id))
                {
                    last_LP_task = reset_LP_task;
                }
            }
        }

        task_index++;
    }
}


/**!
 *  \brief      Schedule all high priority tasks
 *
 *  \param      none
 *
 *  \return     void
 *
 *  \details    This task handler searches all the Tasks that belong to high priority
 *              in the high priority task table and executes them one by one.
 */
static void SCH_scheduleHPTasks(void)
{
    TaskID task_id;
    uint8_t task_index = 0U;

#ifdef SCH_OPTIMIZE_PERFORMANCE
    while((task_id = SCH_getHPTaskID(task_index)) != END_STAMP)
    {
        SCH_handleTask(task_id);
        task_index++;
    }
#else 
    while((task_id = SCH_getTaskID(task_index)) != END_STAMP)//lint !e720 "result of assignment used" : not critical here
    {
        if(SCH_getTaskPriority(task_id) == HIGH_PRIORITY)
        {
            SCH_handleTask(task_id);
        }
        
        task_index++;
    }
#endif
}


/**!
 *  \brief      Schedule a low priority tasks
 *
 *  \param      none
 *
 *  \return     void
 *
 *  \details    When called, this task handler searches one low priority task from low priority task table
 *              and executes it and for each next call, the next task in the table.
 */
static void SCH_scheduleLPTasks(void)
{
    TaskID task_id;
    static uint8_t task_index    = 0U;


#ifdef SCH_OPTIMIZE_PERFORMANCE
    task_id = SCH_getLPTaskID(task_index);

    if((task_id == END_STAMP) || (SCH_isLPTaskIndexOutOfRange(task_index)))
    {
        task_id = SCH_getLPTaskID(task_index = 0U);
    }

    SCH_handleTask(task_id);
    task_index++;
#else
    static uint8_t task_countLP  = 0U;
    while((task_id = SCH_getTaskID(task_index)) != END_STAMP)//lint !e720 "result of assignment used" : not critical here
    {
        task_index++;  
        if(SCH_getTaskPriority(task_id) == LOW_PRIORITY)
        {
            SCH_handleTask(task_id);
            task_countLP++;
            break;
        }
    }
   
    if((SCH_getTaskID(task_index) == END_STAMP) || (SCH_isLPTaskIndexOutOfRange(task_countLP)))
    {
        task_index = 0U;
        task_countLP = 0U;
    }
#endif

}


/**!
 *  \brief      Initialization phase handler
 *
 *  \param      none
 *
 *  \return     void
 *
 *  \details
 */
static void SCH_handleInitializationPhase(void)
{
    SCH_RuntimeCatalogue.Info.CurrentActivePhase = INITIALIZATION_PHASE;
    SCH_initializeHPTasks();
    SCH_initializeLPTasks();
}


/**!
 *  \brief      Scheduling phase handler
 *
 *  \param      none
 *
 *  \return     void
 *
 *  \details
 */
static void SCH_handleSchedulingPhase(void)
{
    SCH_RuntimeCatalogue.Info.CurrentActivePhase = SCHEDULING_PHASE;

    if(SCH_RuntimeCatalogue.Info.ActiveHPTasks)
    {
        SCH_scheduleHPTasks();
    }

    if(SCH_RuntimeCatalogue.Info.ActiveLPTasks)
    {
        SCH_scheduleLPTasks();
    }
    
}


/**!
 *  \brief      This function resets the run time info
 *
 *  \param      none
 *
 *  \return     void
 *
 *  \details
 */
static void SCH_resetRuntimeCatalogue(void)
{
    memset(&SCH_RuntimeCatalogue, 0, sizeof(SCH_RuntimeCatalogue));
}
/**************************************************************************************************/
/* GLOBAL FUNCTION DEFINITIONS                                                                    */
/**************************************************************************************************/
/**!
 *  \brief      Scheduler Tasks Handler
 *
 *  \param      none
 *
 *  \return     void
 *
 *  \details
 */
void SCH_scheduler_entry(void)
{
    while(SCH_initializationPhaseNotOver())
    {
        SCH_handleInitializationPhase();
    }
    for(;;)
    {
       SCH_handleSchedulingPhase();
    }
}



/**!
 *  \brief      This function switches to the corresponding use define tables for Scheduling and prepares the Scheduler
 *
 *  \param      none
 *
 *  \return     void
 *
 *  \details
 */
void SCH_switchProjectCatalogue(struct SCH_ProjectInventory* project_catalog)
{

    SCH_resetRuntimeCatalogue();
    SCH_setupScheduler(project_catalog);
    SCH_scheduler_entry();

}
/**!
 *  \brief      This function sets up the corresponding tables for Scheduler and prepares the Scheduler
 *
 *  \param      none
 *
 *  \return     void
 *
 *  \details
 */
void SCH_setupScheduler(struct SCH_ProjectInventory* project_catalog)
{
    TaskID task_id;
    uint8_t task_index = 0U;

    SCH_ProjectCatalogue = project_catalog;

    while((task_id = SCH_getTaskID(task_index)) != END_STAMP)//lint !e720 "result of assignment used" : not critical here
    {
        SCH_setTaskExecutionState(task_id, (bool)true);
        SCH_gatherSchedulerData(task_id);
        task_index++;
    }

    SCH_setupSchedulingLineups();
}


/**!
 *  \brief      This function is used for changing the priority of a module during runtime
 *
 *  \param      uint8_t module_id
 *              bool    module_priority
 *
 *  \return     void
 *
 *  \details
 */
void SCH_changeModulePriority(const TaskID module_id, TaskPriority module_priority)
{
    SCH_ProjectCatalogue->TasksList[module_id].Task_Priority = module_priority;
    SCH_setupSchedulingLineups();
}


/**!
 *  \brief      Get the current active system phase of the system
 *
 *  \param      none
 *
 *  \return     Initialization_phase OR Scheduling_phase
 *
 *  \details
 */
SchedulerPhase SCH_getSchedulerActivePhase(void)
{
    return SCH_RuntimeCatalogue.Info.CurrentActivePhase;
}


/**!
 *  \brief      Check if the module is deactivated
 *
 *  \param      uint8_t module_id
 *
 *  \return     true or false
 *
 *  \details
 */
bool SCH_isModuleActive(const TaskID module_id)
{
    bool state;

    state = SCH_getTaskExecutionState(module_id);

    return state;
}


/**!
 *  \brief      Check if all the modules are initialized.
 *
 *  \param      void
 *
 *  \return     uint8_t TRUE or FALSE
 *
 *  \details    This function returns TRUE if all modules in current task table are initialized, otherwise FALSE
 */
bool SCH_areAllModulesInitialized(void)
{
    return (SCH_RuntimeCatalogue.Info.SystemInitState == NO_OF_MODULES);
}


/**!
 *  \brief      Activate the previously deactivated Module
 *
 *  \param      uint8_t module_id
 *
 *  \return     void
 *
 *  \details    Module is reactivated and for scheduling phase the tables are set up again
 */
void SCH_reactivateModule(const TaskID module_id)
{
    SCH_setTaskExecutionState(module_id, (bool)true);
    SCH_setupSchedulingLineups();
}


/**!
 *  \brief      Terminate the module
 *
 *  \param      uint8_t module_id
 *
 *  \return     void
 *
 *  \details    Module is deactivated and for scheduling phase the tables are set up again
 */
void SCH_terminateModule(const TaskID module_id)
{
    SCH_setTaskExecutionState(module_id, (bool)false);
    SCH_setupSchedulingLineups();
}


/**!
 *  \brief      Gives the current active task ID 
 *
 *  \param      none
 *
 *  \return     TaskID
 *
 *  \details
 */
TaskID SCH_getActiveTaskID(void)
{
    return(SCH_RuntimeCatalogue.Info.CurrentRunningTaskID);
}



/**!
 *  \brief      This function returns InitState of a module.
 *
 *  \param      uint8_t task_id
 *
 *  \return     TRUE or FALSE
 *
 *  \details
 */
bool SCH_isTaskInitialized(const TaskID task_id)
{
    return (SCH_getTaskInitState(task_id));
}

/********************************************* End of File ******************************************************/
