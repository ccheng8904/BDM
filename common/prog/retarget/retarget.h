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
 *  COMP_ABBREV      RTG
 ******************************************************************************/


#ifndef RETARGET_H
#define RETARGET_H

#ifdef __cplusplus
extern "C" {
#endif


/******************************************************************************/
/* DOCUMENTATION                                                              */
/******************************************************************************/
/** \file
 *
 *  \ingroup  debug
 *
 *  \brief    Public definitions and declarations for unit retarget.
 *
 *  \details  .
 */


/******************************************************************************/
/* INCLUDES                                                                   */
/******************************************************************************/
#include "bsh_stdinc.h"
#include <stdint.h>


/******************************************************************************/
/* PUBLIC TYPE DEFINITIONS                                                    */
/******************************************************************************/


/******************************************************************************/
/* PUBLIC FUNCTION DECLARATIONS                                               */
/******************************************************************************/

/** \brief    Retargeting function for writing one character from STDIO.
 *
 *  \details  This function sends one byte trough debug interface from printf.
 *            If interface to which the byte is beeing sent is bussy, the
 *            function waits for last byte to be transmitted.
 *
 *  \param    byte: one byte to be sent
 */
extern void RTG_writeChar(char ch);


/** \brief    Retargeting function for reading one character to STDIO.
 *
 *  \details  This function receive one byte trough debug interface from scanf.
 *
 *  \return   byte: received character
 */
extern char RTG_readChar(void);


/** \brief    Function for retargeting failed assert functions.
 *
 *  \details  Weak definition of this function can be redefined in other SW module.
 *
 *  \param    expr: expression that failed assertion
 *  \param    file: file in which was the assert that failed
 *  \param    line: line at which was the assert that failed
 
 */
extern void RTG_assert(const char *expr, const char *file, int32_t line);


#ifdef __cplusplus
}
#endif

#endif // RETARGET_H
