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
 *  COMP_ABBREV      MPX
 ******************************************************************************/


/******************************************************************************/
/* DOCUMENTATION                                                              */
/******************************************************************************/
/** \file
 *
 *  \ingroup  PED-Framework
 *
 *  \brief    Project specific definitions for mpx
 *
 */


#ifndef MPXXS_H
#define MPXXS_H

#ifdef __cplusplus
extern "C" {
#endif

#include "LibTypes.h"

/* DEFINITIONS ****************************************************************************************/

/** Dependencies of library after configuration

If the configuration changes the dependencies of multiplexer, this shall adapted here.\n
The following example add a dependency to module EEP:\n
\code
#define DEP_MPX     DEPLIB_MPX  MOD_EEP, SCH_END_MARK
\endcode
\note There is no comma after DEPLIB_MPX
*/
#define DEP_MPX     DEPLIB_MPX SCH_END_MARK


/* TYPE DEFINITIONS ***********************************************************************************/

/** Result type of the input multiplexer

    The application can adapt this. Practical values are [uchar, uint, ulong]

    \note If the application needs (really) more than 32 inputs,
          you have to create another interface to get the result of input multiplexer.

\sa MPX_tGetInput()
*/
typedef uint8_t TmpxOut;



#ifdef __cplusplus
}
#endif

#endif

