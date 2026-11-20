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
/// \file Public application specific definitions and declarations scheduler_time_monitor

#ifndef SCHEDULER_TIME_MONITOR_H_
#define SCHEDULER_TIME_MONITOR_H_

/**************************************************************************************************/
/* INCLUDES                                                                                       */
/**************************************************************************************************/
#include "scheduler_types.h"

/**************************************************************************************************/
/* DEFINITIONS                                                                        */
/**************************************************************************************************/

/**************************************************************************************************/
/* PUBLIC TYPE DEFINITIONS                                                                        */
/**************************************************************************************************/

/**************************************************************************************************/
/* EXTERN VARIABLE DECLARATIONS                                                                   */
/**************************************************************************************************/
#if(DEBUG_MODE == DEBUG_SINGLE_MODULE)
    extern struct SCH_TM_TimeProperties SCH_TimeMonitor[DEBUG_SINGLE_MODULE];
#else
    extern struct SCH_TM_TimeProperties SCH_TimeMonitor[NO_OF_MODULES];
#endif
/**************************************************************************************************/
/* GLOBAL FUNCTION DECLARATIONS                                                                   */
/**************************************************************************************************/
/**!
 *  \brief      This function measures the Task Time Period and the Jitter
 *
 *  \param      TaskID task_id
 *
 *  \return     none
 *
 *  \details
 */
void SCH_TM_measureTaskTimeProperties(TaskID task_id);

/**!
 *  \brief      This function measures the Task Runtime and saves it in the Time Monitor Table
 *
 *  \param      TaskID task_id
 *
 *  \return     none
 *
 *  \details
 */
void SCH_TM_measureTaskRuntime(TaskID task_id);


#endif /* _SCHEDULER_TIME_MONITOR_H_ */


/********************************************* End of File ******************************************************/

