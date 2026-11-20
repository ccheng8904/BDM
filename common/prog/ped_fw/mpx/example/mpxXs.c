/*******************************************************************************
 *  Copyright (c) 2017 BSH Hausgeraete GmbH,
 *  Carl-Wery-Str. 34, 81739 Munich, Germany, www.bsh-group.de
 *
 *  All rights reserved. This program and the accompanying materials
 *  are protected by international copyright laws.
 *  Please contact copyright holder for licensing information.
 *
 *******************************************************************************
 *  PROJECT          PP PED Framework
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
#include "utility/utility.h"
#include "mpxlib.h"
#include "mpxpriv.h"


/******************************************************************************/
/* PRIVATE DEFINITIONS                                                        */
/******************************************************************************/
#define MPX_COLUMN_COUNT 8///< Number of columns, \b Range: [1..16]
#define MPX_LINE_COUNT   8///< Number of lines,  \b Range: [1..16]



/******************************************************************************/
/* PRIVATE TYPE DEFINITIONS                                                   */
/******************************************************************************/

#if (MPX_COLUMN_COUNT > 8)
    typedef uint TcolumnMask;///< type for column, one bit for each column
#else
    typedef uchar TcolumnMask;///< type for Column, one bit for each column
#endif

#if (MPX_LINE_COUNT > 8)
    typedef uint TcolumnData;///< type for column data
#else
    typedef uchar TcolumnData;///< type for column data
#endif



/******************************************************************************/
/* PRIVATE DATA DEFINITIONS                                                   */
/******************************************************************************/
static uchar MPX_ucColumn;       ///< current sampled column of multiplexer


//definitions of values declared in mpypriv.h
const uchar MPX_ucNumberOfLines = MPX_COLUMN_COUNT;//number of lines
const uchar MPX_ucBufferSize    = MPX_BUFFER_SIZE; //buffer size
uchar MPX_pucDataBuffer[MPX_BUFFER_SIZE];          //multiplexer buffer


bool MPX_bGetInitConfig(void)
{
    MPX_vHandleEvent();// just to reference it, remove after registered as timer interupt
    return true;
}


// must be called by timer service
static void MPX_vHandleEvent(void)
{
   M_MPX_vSetChannel((TcolumnMask)(1<<MPX_ucColumn));      //output of column
   M_MPX_vSetLine(tGetColumnData(MPX_ucColumn));           //output of line
   M_MPX_vLoadTimer(MPX_MULTIPLEX_TIME);                   //reload timer for next Interrupt

   if (++MPX_ucColumn >= MPX_COLUMN_COUNT)
   {
      MPX_ucColumn = 0;
   }
}
