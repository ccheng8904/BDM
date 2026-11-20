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
/// \file Public definitions and declarations for module scheduler

#ifndef SCHEDULER_H_
#define SCHEDULER_H_

/**************************************************************************************************/
/* INCLUDES                                                                                       */
/**************************************************************************************************/
#include "scheduler_types.h"
#include "scheduler/scheduler_cfg.h"
/**************************************************************************************************/
/* PUBLIC DEFINITIONS                                                                             */
/**************************************************************************************************/

/**************************************************************************************************/
/* PUBLIC TYPE DEFINITIONS                                                                        */
/**************************************************************************************************/

/**************************************************************************************************/
/* GLOBAL VARIABLE DECLARATIONS                                                                   */
/**************************************************************************************************/

/**************************************************************************************************/
/* GLOBAL FUNCTION DECLARATIONS                                                                   */
/**************************************************************************************************/
/**!
 *  \brief      Check if all the modules are initialized
 *
 *  \param      none
 *
 *  \return     uint8_t TRUE or FALSE
 *
 *  \details    this function returns TRUE if all modules in current task table are initialized, otherwise FALSE
 */
bool SCH_areAllModulesInitialized(void);


/**!
 *  \brief      Activate the previously deactivated Module
 *
 *  \param      uint8_t module_id
 *
 *  \return     void
 *
 *  \details
 */
void SCH_reactivateModule(const TaskID module_id);


/**!
 *  \brief      Deactivate the module
 *
 *  \param      uint8_t module_id
 *
 *  \return     void
 *
 *  \details
 */
void SCH_terminateModule(const TaskID module_id);


/**!
 *  \brief      Check if the module is deactivated
 *
 *  \param      uint8_t task_id
 *
 *  \return     TRUE or FALSE
 *
 *  \details
 */
bool SCH_isModuleActive(const TaskID module_id);


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
void SCH_changeModulePriority(const TaskID module_id, TaskPriority module_priority);


/**!
 *  \brief      This function sets up the corresponding tables for Scheduler and prepares the Scheduler
 *
 *  \param      SCH_ProjectInventory* project_catalog
 *
 *  \return     void
 *
 *  \details
 */
void SCH_setupScheduler(struct SCH_ProjectInventory* project_catalog);


/**!
 *  \brief      Scheduler Tasks Handler
 *
 *  \param      none
 *
 *  \return     void
 *
 *  \details
 */
void SCH_scheduler_entry(void);

/**!
 *  \brief      This function switches to the corresponding user define table for Scheduling and prepares the Scheduler
 *              also it reinitialises and checks the dependencies of tasks included in new table, it can be called from any task and any time
 *
 *  \param      SCH_ProjectInventory* project_catalog
 *
 *  \return     void
 *
 *  \details
 */
void SCH_switchProjectCatalogue(struct SCH_ProjectInventory* project_catalog);
/**!
 *  \brief      Get the current active system phase of the system
 *
 *  \param      none
 *
 *  \return     Initialization_phase OR Scheduling_phase
 *
 *  \details
 */
SchedulerPhase SCH_getSchedulerActivePhase(void);


/**!
 *  \brief      Gives the current active task ID 
 *
 *  \param      none
 *
 *  \return     TaskID
 *
 *  \details
 */

TaskID SCH_getActiveTaskID(void);


/**!
 *  \brief      This function returns InitState of a module.
 *
 *  \param      uint8_t task_id
 *
 *  \return     TRUE or FALSE
 *
 *  \details
 */
bool SCH_isTaskInitialized(const TaskID task_id);

#endif /* PROG_THREADX_INC_TX_SCHEDULER_H_ */

/********************************************* End of File ******************************************************/
