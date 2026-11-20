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
 *  COMP_ABBREV      DBGXTE
 ******************************************************************************/


#ifndef DBGS_H
#define DBGS_H

#ifdef __cplusplus
extern "C" {
#endif

/******************************************************************************/
/* DOCUMENTATION                                                              */
/******************************************************************************/
/** \file
 *
 *
 *  \brief    Public definitions and declarations for unit DBGXTE.
 *
 */


/******************************************************************************/
/* INCLUDES                                                                   */
/******************************************************************************/

#include <stdint.h>


/******************************************************************************/
/*   DEPENDENCIES                                                             */
/******************************************************************************/

/** \brief    Dependencies for unit DBGXTE */
#define DEP_DBGXTE


/******************************************************************************/
/* PUBLIC PREPROCESSOR DEFINITIONS                                            */
/******************************************************************************/

/************************************************************
* Unique exceptional error codes for DBGXTE: 0x0000F*** - 0x0000F***          */

/* Error solving actions should be ORed to the unique exceptional error code. */
/* DBGX_ERR_SOLVING_NONE     0x00000000 */
/* DBGX_ERR_SOLVING_RESET    0x10000000 */
/* DBGX_ERR_SOLVING_CALLBACK 0x20000000 */

#define DBGXTE_ERR_CALLBACK_TEST     ((uint32_t)0x0000F000 | DBGX_ERR_SOLVING_CALLBACK)
#define DBGXTE_ERR_RESET_TEST        ((uint32_t)0x0000F010 | DBGX_ERR_SOLVING_RESET)
// ...


/******************************************************************************/
/* PUBLIC FUNCTION DECLARATIONS                                               */
/******************************************************************************/


/** \brief    Task handler
 *
 *  \return   TASK_TERMINATE
 */
extern uint8_t DBGXTE_handleTask(void);


#ifdef __cplusplus
}
#endif

#endif // DBGS_H
