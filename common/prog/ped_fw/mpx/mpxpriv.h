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


#ifndef MPXPRIV_H
#define MPXPRIV_H

#ifdef __cplusplus
extern "C" {
#endif

/******************************************************************************/
/* DOCUMENTATION                                                              */
/******************************************************************************/
/** \file
 *
 *  \brief    Private definitions and declarations of library multiplexer
 *           (used for configuration).
 *
 */

/******************************************************************************/
/* INCLUDES                                                                   */
/******************************************************************************/
#include "bsh_stdinc.h"
#include "LibTypes.h"


/******************************************************************************/
/* PUBLIC DEFINITIONS                                                         */
/******************************************************************************/

#define MPX_BUFFER_SIZE (((MPX_LINE_COUNT+7U)/8U) * MPX_COLUMN_COUNT)///< multiplexer buffer size


/******************************************************************************/
/* PUBLIC DATA DECLARATIONS                                                   */
/******************************************************************************/

/**
 *  Number of lines, to be defined in library template mpxsXS.c,
 *  \b Range: [1..255]
 */
extern const uint8_t MPX_ucNumberOfLines;
extern const uint8_t MPX_ucBufferSize;     ///< Number of bytes in multiplexer buffer


/** Data buffer for multiplexer
 
The buffer is allways byte orientated. Even if just 4 lines are used a column needs a hole byte.\n
If there are 9 to 16 lines 2 bytes are used for each column.\n

Application must define this array as follows:
\code
 uchar MPX_pucDataBuffer[((MPX_LINE_COUNT+7)/8) * MPX_COLUMN_COUNT];
\endcode
*/
extern uint8_t MPX_pucDataBuffer[];

/** 
 *  Counter must be decremented in interrupt handler (when function #MPX_bIsReady() used), until zero reached.
 */
extern uint8_t MPX_ucCountDown;


/******************************************************************************/
/* PUBLIC FUNCTION DECLARATIONS                                               */
/******************************************************************************/

/** Configuration handler, called by task handler

This Function shall be overwritten by application.\n
The configuration handler will be called by task handler until a non zero value will be returned.\n
If configuration is finished this function must return true (!FALSE).\n
The application specific implementation of this function shall initilise the timer interrupt for multiplexer.
Any runtime configuration may be handled in this function.

\return state of initialistion (command for task handler)\n
\b Type: BOOL\n
\b Range: 0 = call me again, all others = inititialisation done

\note This is \b not a interface function, library will call it for configuration.\n
      Application must define this function.

*/
bool MPX_bGetInitConfig(void);


#ifdef __cplusplus
}
#endif

#endif
