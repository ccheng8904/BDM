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
 *  COMP_ABBREV      DBGXTS
 ******************************************************************************/


#ifndef DBGXTS_H
#define DBGXTS_H

#ifdef __cplusplus
extern "C" {
#endif

/******************************************************************************/
/* DOCUMENTATION                                                              */
/******************************************************************************/
/** \file
 *
 *
 *  \brief    Public definitions and declarations for unit DBGS.
 *
 */


/******************************************************************************/
/* INCLUDES                                                                   */
/******************************************************************************/
#include <stdint.h>
#include "mcal_channels.h"

/******************************************************************************/
/*   DEPENDENCIES                                                             */
/******************************************************************************/
/** \brief    Dependencies for unit DBGS
 *
 */
#define DEP_DBGXTS


/******************************************************************************/
/* PUBLIC FUNCTION DECLARATIONS                                               */
/******************************************************************************/



/** \brief    Task handler
 *
 *  \return   TASK_TERMINATE
 */
extern uint8_t DBGXTS_handleTask(void);


#ifdef __cplusplus
}
#endif

#endif // DBGS_H
