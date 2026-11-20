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
 *  \brief    Configuration template for mpx.
 *
 *  \details  This is an example implementation
 */

/******************************************************************************/
/* INCLUDES                                                                   */
/******************************************************************************/
#include "bsh_stdinc.h"
#include "utility.h"
#include "mpxlib.h"
#include "mpxpriv.h"

//lint -e9026 Function-like macro
//lint -e751  local typedef '' not referenced
/******************************************************************************/
/* PRIVATE DEFINITIONS                                                        */
/******************************************************************************/
#define MPX_COLUMN_COUNT       8u       ///< Number of columns, \b Range: [1..16]
#define MPX_LINE_COUNT         8u       ///< Number of lines,  \b Range: [1..16]
#define M_MPX_vSetChannel(Channel)      // needs to be replaced by project function
#define M_MPX_vSetLine(ColumnData)      // needs to be replaced by project function
#define M_MPX_vLoadTimer(MultiplexTime) // needs to be replaced by project function

/******************************************************************************/
/* PRIVATE TYPE DEFINITIONS                                                   */
/******************************************************************************/

#if (MPX_COLUMN_COUNT > 8u)
typedef uint16_t TcolumnMask;///< type for column, one bit for each column
#else
typedef uint8_t  TcolumnMask;///< type for Column, one bit for each column
#endif

#if (MPX_LINE_COUNT > 8u)
typedef uint16_t TcolumnData;///< type for column data
#else
typedef uint8_t  TcolumnData;///< type for column data
#endif

/******************************************************************************/
/* PRIVATE FUNCTION DECLARATIONS                                              */
/******************************************************************************/
static void MPX_vHandleEvent(void);

/******************************************************************************/
/* PRIVATE DATA DEFINITIONS                                                   */
/******************************************************************************/


//definitions of values declared in mpypriv.h
const uint8_t MPX_ucNumberOfLines = MPX_COLUMN_COUNT;         //number of lines
const uint8_t MPX_ucBufferSize    = (uint8_t)MPX_BUFFER_SIZE; //buffer size
uint8_t MPX_pucDataBuffer[MPX_BUFFER_SIZE];                   //multiplexer buffer


bool MPX_bGetInitConfig(void)
{
    MPX_vHandleEvent();// just to reference it, remove after registered as timer interupt
    return true;
}

static void MPX_vHandleEvent(void)
{
    static uint8_t MPX_ucColumn = 0;       ///< current sampled column of multiplexer

    M_MPX_vSetChannel((TcolumnMask)(1u << MPX_ucColumn));   //output of column
    M_MPX_vSetLine(tGetColumnData(MPX_ucColumn));           //output of line
    M_MPX_vLoadTimer(MPX_MULTIPLEX_TIME);                   //reload timer for next Interrupt

    if (++MPX_ucColumn >= MPX_COLUMN_COUNT)
    {
        MPX_ucColumn = 0;
    }
}
