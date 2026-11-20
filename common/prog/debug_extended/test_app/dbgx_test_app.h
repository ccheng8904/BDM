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
 *  COMP_ABBREV      DBGXTA
 ******************************************************************************/


#ifndef DBGX_H
#define DBGX_H

#ifdef __cplusplus
extern "C" {
#endif

/******************************************************************************/
/* DOCUMENTATION                                                              */
/******************************************************************************/
/** \file
 *
 *
 *  \brief    Public definitions and declarations for unit DBGX.
 *
 */


/******************************************************************************/
/* INCLUDES                                                                   */
/******************************************************************************/
#include <stdint.h>


/******************************************************************************/
/*   DEPENDENCIES                                                             */
/******************************************************************************/
/** \brief    Dependencies for unit DBGX
 *
 */
#define DEP_DBGXTA


/******************************************************************************/
/* PUBLIC FUNCTION DECLARATIONS                                               */
/******************************************************************************/

/** \brief    Task handler
 *
 *  \return   TASK_TERMINATE
 */
extern uint8_t DBGXTA_handleTask(void);


#ifdef __cplusplus
}
#endif

#endif // DBGX_H
