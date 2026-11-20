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
 *
 *  \brief    Implementation of mpx library.
 *
 *
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
#define BITS_PER_BYTE 8u


/******************************************************************************/
/* PRIVATE TYPE DEFINITIONS                                                   */
/******************************************************************************/


typedef struct _TIndexMask{
  uint8_t ucIndex;   ///< Index in buffer #MPX_pucDataBuffer
  uint8_t ucBitMask; ///< bit mask to access a bit in #MPX_pucDataBuffer with index ucIndex
}TIndexMask;       ///< internal structure (mask and index) to set/reset a bit in multiplexer buffer



/******************************************************************************/
/* PRIVATE FUNCTION DECLARATIONS AND PRIVATE MACRO FUNCTION DEFINITIONS       */
/******************************************************************************/

/** Calculates the index im memory buffer for a point in the matrix

 \param ucIdentifier: \n
        identifier to calculate mask for\n
     \b Type: uint8_t\n

 \return index and mask in MPX_pucDataBuffer\n
         b\ Type: TIndexMask

*/
static TIndexMask MPX_tCalcIndexAndMask(uint8_t ucIdentifier);
/******************************************************************************/
/* PRIVATE FUNCTION DECLARATIONS AND PRIVATE MACRO FUNCTION DEFINITIONS       */
/******************************************************************************/


/******************************************************************************/
/* PRIVATE DATA DEFINITIONS                                                   */
/******************************************************************************/

uint8_t MPX_ucCountDown;//published in mpxpriv.h!


/******************************************************************************/
/* PUBLIC FUNCTION DEFINITIONS                                                */
/******************************************************************************/


uint8_t MPX_ucHandleTask(void)
{
   if (MPX_bGetInitConfig()!= false)//do application specific configuration, e.g initialisation of timer interrupt
   /* Msg(2:3416) This boolean expression contains side effects. 
      This side effect is used to init the module
   */
   {
      return TASK_TERMINATE;
   }
   else
   {
      return TASK_NOT_INITIALISED;
   }

}

void MPX_vSet(uint8_t ucIdentifier, bool bOnOff)

{
   TIndexMask tIndexMask;
   tIndexMask = MPX_tCalcIndexAndMask(ucIdentifier);
   if (bOnOff)
   {
      MPX_pucDataBuffer[tIndexMask.ucIndex] |= tIndexMask.ucBitMask;
   }
   else
   {
      MPX_pucDataBuffer[tIndexMask.ucIndex] &= (uint8_t)(~tIndexMask.ucBitMask);
   }

}


bool MPX_bGet(uint8_t ucIdentifier)
{
   TIndexMask tIndexMask;
   tIndexMask =MPX_tCalcIndexAndMask(ucIdentifier);
   return ((uint8_t)(MPX_pucDataBuffer[tIndexMask.ucIndex] & tIndexMask.ucBitMask) != 0u);

}


void MPX_vToggle(uint8_t ucIdentifier)
{
   TIndexMask tIndexMask;
   tIndexMask = MPX_tCalcIndexAndMask(ucIdentifier);

   MPX_pucDataBuffer[tIndexMask.ucIndex] ^= tIndexMask.ucBitMask;
}


void MPX_vSetPoint(uint8_t ucColumn, uint8_t ucLine, bool bOnOff)
{
   MPX_vSet((uint8_t)((ucColumn * MPX_ucNumberOfLines) + ucLine), bOnOff);
}


bool MPX_bGetPoint(uint8_t ucColumn, uint8_t ucLine)
{
   return MPX_bGet((uint8_t)((ucColumn * MPX_ucNumberOfLines) + ucLine));
}


void MPX_vTogglePoint(uint8_t ucColumn, uint8_t ucLine)
{
   MPX_vToggle((uint8_t)((ucColumn * MPX_ucNumberOfLines) + ucLine));
}


void MPX_vSetMpxBuffer(const uint8_t* pucBuffer)
{
   UTI_vMemCopy(pucBuffer, MPX_pucDataBuffer, MPX_ucBufferSize);
}


const uint8_t* MPX_pucGetMpxBuffer(void)
{
   return (MPX_pucDataBuffer);
}


void MPX_vClearMpx(void)
{
   uint8_t ucLoop    = MPX_ucBufferSize;

   while (ucLoop!=0u)
   {
      --ucLoop;
      MPX_pucDataBuffer[ucLoop] = 0;
   }

}


bool MPX_bIsReady(void)
{
   return ((bool)(MPX_ucCountDown > 0u));
}


void MPX_vSetCountDown(uint8_t ucValue)
{
   MPX_ucCountDown = ucValue;
}

/******************************************************************************/
/* PRIVATE FUNCTION DEFINITIONS                                               */
/******************************************************************************/

static TIndexMask MPX_tCalcIndexAndMask(uint8_t ucIdentifier)
{
   TIndexMask tIndexMask;
   /* For each column a new bytes starts, e.g. 20 lines takes 3 bytes for each column */
   tIndexMask.ucIndex =(uint8_t)((uint8_t)(ucIdentifier / MPX_ucNumberOfLines)*(uint8_t)((MPX_ucNumberOfLines+((BITS_PER_BYTE)-1u))/(BITS_PER_BYTE)));
   tIndexMask.ucBitMask  =(uint8_t)(ucIdentifier % MPX_ucNumberOfLines);

   /* addaption if there are more than 8 lines */
   tIndexMask.ucIndex += tIndexMask.ucBitMask / BITS_PER_BYTE;
   tIndexMask.ucBitMask %= BITS_PER_BYTE;

   /* create the mask form bit position */
   tIndexMask.ucBitMask = (uint8_t)(1u<<tIndexMask.ucBitMask);

   return tIndexMask;
}
