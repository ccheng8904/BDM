/*******************************************************************************
 *  Copyright (c) 2017 BSH Hausgeraete GmbH,
 *  Carl-Wery-Str. 34, 81739 Munich, Germany, www.bsh-group.de
 *
 *  All rights reserved. This program and the accompanying materials
 *  are protected by international copyright laws.
 *  Please contact copyright holder for licensing information.
 *
 *******************************************************************************
 * PROJECT          Generic SW
 * COMP_ABBREV      PHYP
 ******************************************************************************/


#ifndef PHYXS_H
#define PHYXS_H

#ifdef __cplusplus
extern "C" {
#endif


/******************************************************************************/
/* DOCUMENTATION                                                              */
/******************************************************************************/
/** \file
 *
 *  \ingroup  Physpan
 *
 *  \brief    Configuration for physical panel library.
 *
 */


/******************************************************************************/
/* INCLUDES                                                                   */
/******************************************************************************/
#include "bsh_stdinc.h"
#include <stdint.h>


/******************************************************************************/
/*   DEPENDENCIES                                                             */
/******************************************************************************/
/** \brief    Dependencies for unit PHYXS
 *
 *  \details  If the unit is used in the PED Framework
 *            then the macro is needed in schedDef.c i.e.
 *            the config file of the scheduler library.
 *
 *  \attention  Add dependencies and description of dependencies!
 *              Do not forget the final comma with exception there are no dependencies!
 *              For example: #define DEP_XXX     MOD_YYY, MOD_ZZZ,
 *
 */
#define DEP_PHYP {DEPLIB_PHYP, SCH_END_MARK}


/******************************************************************************/
/* PUBLIC DEFINITIONS                                                         */
/******************************************************************************/

// !!!!!!!!!!! PLEASE SELECT NUMBER OF KEYS FOR EACH TYPE OF PANEL !!!!!!!!!

#define PHYP_DIGI_KEY_COUNT    6u ///< Number of mechanical/digital keys, set to zero if not present
#define PHYP_C_KEY_COUNT       5u ///< Number of capacitive keys, set to zero if not present

/** Number of keys all togeather */
#define PHYP_KEY_COUNT   ((uint8_t)(PHYP_DIGI_KEY_COUNT + PHYP_C_KEY_COUNT))


/******************************************************************************/
/* PUBLIC TYPE DEFINITIONS                                                    */
/******************************************************************************/



/******************************************************************************/
/* PUBLIC FUNCTION DECLARATIONS                                               */
/******************************************************************************/




#ifdef __cplusplus
}
#endif

#endif // PHYXS_H
