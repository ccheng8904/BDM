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

#ifndef SCHEDULER_DEFINES_H_
#define SCHEDULER_DEFINES_H_

/**************************************************************************************************/
/* WORK WITH DEFINE FILES                                                                         */
/**************************************************************************************************/

/**************************************************************************************************/
/* INCLUDES                                                                                       */
/**************************************************************************************************/
#include "LibTypes.h"

/**************************************************************************************************/
/* PUBLIC DEFINITIONS                                                                        */
/**************************************************************************************************/

#define END_STAMP               TOTAL_MODULES
#define PRIO_NONE               HIGH_PRIORITY
#define MAX_BIT_INDEX           (BYTE_SIZE - 1U)
#define NO_OF_MODULES           ((uint8_t)TOTAL_MODULES)
#define NO_OF_MODULES_EXT       ((uint8_t)(NO_OF_MODULES + 1U))
#define NO_OF_MODULES_WRAPPED   ((NO_OF_MODULES + MAX_BIT_INDEX) / BYTE_SIZE) //< For a module representing a bit in a Byte, hence all modules accommodated in how many bytes?
#define DEBUG_ALL_MODULES       0U
#define DEBUG_SINGLE_MODULE     1U


#endif /* SCHEDULER_DEFINES_H_ */

/********************************************* End of File ******************************************************/
