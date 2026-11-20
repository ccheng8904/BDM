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
/// \file Private User application specific definitions and declarations for scheduler_config


/**************************************************************************************************/
/* INCLUDES                                                                                       */
/**************************************************************************************************/
#include "bsh_stdinc.h"
#include "scheduler_auto.h"
#include "scheduler_types.h"

<$foreach(var,$(scheduler_tasks),ifdef(var.include_file,foreach(inc_file,var.include_file,macro(macro.include,inc_file))))>
/* USER CODE BEGIN Include */
/*! place your module includes here */
/* USER CODE END Include */


/***************************************************************************************************
 * DEPENDENCY ARRAYS
 * *************************************************************************************************
 * \brief: Create Module dependency arrays for each module here, terminate the array with END_STAMP
 * *************************************************************************************************
 * \note: No Circular Dependencies allowed
 ***************************************************************************************************/
<$define(macro_sheduler_dep_def,"const TaskID $1_dep[]\t= {$2\tEND_STAMP};\n")>
<$foreach(var,$(scheduler_tasks),macro(macro_sheduler_dep_def,$(var.name),$(var.dependency)))>
/* USER CODE BEGIN Dependency */
/*! place your module dependencies here */
/* USER CODE END Dependency */


/**!
 * \brief       Main Project Catalog Structure
 *
 * \details     It contains following Tables,
 *              TasksList           - Task line-up for scheduling
 *              TaskHandlersList    - TaskHandler functions
 *              DependenciesList    - Module Dependency
 */
struct SCH_ProjectInventory SCH_DefaultProjectCatalogue =
{
    /***************************************************************************************************
     * MAIN TASKS LIST
     * *************************************************************************************************
     * \brief: Add ModuleIDs from SCH_ModuleIDs and assign them appropriate priorities here
     ***************************************************************************************************/
    .TasksList =
    {
<$define(table_sheduler_prio_tbl,{"low":"LOW_PRIORITY","high":"HIGH_PRIORITY"})>
<$define(macro_sheduler_task_tbl,"        {MOD_$1, $2},\n")>
<$foreach(var,$(scheduler_tasks),macro(macro_sheduler_task_tbl,$(var.name),table(table_sheduler_prio_tbl,$(var.priority))))>
        /* USER CODE BEGIN ModulePriority */
        /*! place your project modules and their priorities here */
        /* USER CODE END ModulePriority */

        {END_STAMP, PRIO_NONE}
    },


    /***************************************************************************************************
     * TASK HANDLERS LIST
     * *************************************************************************************************
     * \brief: Add corresponding task handlers for each module here
     ***************************************************************************************************/
    .TaskHandlersList =
    {
<$define(macro_sheduler_handle,"        [MOD_$1] = $2,\n")>
<$foreach(var,$(scheduler_tasks),macro(macro_sheduler_handle,$(var.name),$(var.handle_function)))>
        /* USER CODE BEGIN TaskHandlers */
        /*! place your task handlers here */
        /* USER CODE END TaskHandlers */
    },


    /***************************************************************************************************
     * DEPENDENCIES LIST
     * *************************************************************************************************
     * \brief: Add above module dependency arrays in the following Table
     * *************************************************************************************************
     * \note: maintain the same order as enumeration SCH_ModuleIDs in scheduler_config.h
     ***************************************************************************************************/
    .DependenciesList =
    {
<$define(macro_sheduler_dep_tbl,"        [MOD_$1] = $1_dep,\n")>
<$foreach(var,$(scheduler_tasks),macro(macro_sheduler_dep_tbl,$(var.name)))>
        /* USER CODE BEGIN ModuleDependency */
        /*! place your module dependencies arrays here */
        /* USER CODE END ModuleDependency */
    },
};


/********************************************* End of File ******************************************************/
