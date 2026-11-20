/*******************************************************************************
*   Copyright (c) 2016 BSH Hausgeraete GmbH,
*   Carl-Wery-Str. 34, 81739 Munich, Germany, www.bsh-group.de
*
*   All rights reserved. This program and the accompanying materials
*   are protected by international copyright laws.
*   Please contact copyright holder for licensing information.
*
********************************************************************************
*   PROJECT          PP PED-Framework
*   COMP_ABBREV      SEG
*******************************************************************************/


/******************************************************************************/
/* DOCUMENTATION                                                              */
/******************************************************************************/
/** \file
 *
 *  \brief    Implementation of segment library
 */


/******************************************************************************/
/* INCLUDES                                                                   */
/******************************************************************************/
#include "bsh_stdinc.h"
#include <stdint.h>
#include "seg.h"
#include "segpriv.h"
#include "beat/beat.h"
#include "utility/utility.h"

/******************************************************************************/
/* PRIVATE DEFINITIONS                                                        */
/******************************************************************************/


#define SEG_INIT_FLAG  0x01u ///< Flag in SEG_ucStatus, set if module initialised



/******************************************************************************/
/* PRIVATE TYPE DEFINITIONS                                                   */
/******************************************************************************/

/******************************************************************************/
/* PRIVATE FUNCTION DECLARATIONS AND PRIVATE MACRO FUNCTION DEFINITIONS       */
/******************************************************************************/

/******************************************************************************/
/* PRIVATE DATA DEFINITIONS                                                   */
/******************************************************************************/



/******************************************************************************/
/* PUBLIC FUNCTION DEFINITIONS                                                */
/******************************************************************************/

uint8_t SEG_ucHandleTask(void)
{
    static uint8_t  SEG_ucStatus = 0u; ///< Task state of segment library
    if (SEG_ucStatus & SEG_INIT_FLAG)
    {
        _SEG_vHandleText();
        _SEG_vHandleBlinkingSeg();
    }
    else
    {
        if (_SEG_bGetInitConfig())
        {
            SEG_ucStatus|= SEG_INIT_FLAG;//change to SEG_WORKING
        }
    }
    return (SEG_ucStatus & SEG_INIT_FLAG);//TRUE if initialised
}


void SEG_vWriteText(uint8_t ucDigitStart, const uint8_t* pucText, uint8_t ucBlinkState)
{
    uint8_t* pucSegBufferPtr = &SEG_pucTextBuffer[ucDigitStart];//pointer to first digit to write
    uint8_t* pucBlinkDigBufferPtr = &SEG_pucTextBlinkBuffer[ucDigitStart];//pointer to first blink digit buffer
    uint8_t ucDigitCount = (SEG_ucNumberOfDigits - ucDigitStart);
    const uint8_t* pucPtr = pucText;
    
    if (ucBlinkState)//just write if not SEG_OFF
    {
        while ((*pucPtr > 0u) && (ucDigitCount > 0u))
        {   /*lint -save -e9049 increment/decrement operation combined with other operation with side-effects [MISRA 2012 Rule 13.3 advisory] */
            *pucSegBufferPtr++ = *pucPtr++;
            *pucBlinkDigBufferPtr++ = ucBlinkState;
            ucDigitCount--;
            /*lint -restore -e9049 */
        }
    }
    else
    {
        //just a refresh!
    }
    SEG_ucTextIndex = SEG_TEXT_START_WRITE;
}


void SEG_vSetSegment16(uint16_t segId, uint8_t ucSegStatus)
{
    uint16_t  uPhysId;
    const uint16_t* puPhysId;
    
    uPhysId = _SEG_uiGetPhysicalSegment(segId);//get physical ID
    if (uPhysId >= SEG_uiNumberOfSegments)
    {
        uPhysId -=  SEG_uiNumberOfSegments;//calculate group ID
        /*Axivion Next Line MisraC2012-11.5 : "Conversion between object pointer and void pointer */
        puPhysId =  _SEG_pGetPhysicalSegmentGroup((uint8_t)uPhysId);//get address of segment group
        if (puPhysId)
        {
            while (*puPhysId != (uint16_t)(-1))//do until endmark INVALID reached
            {
                _SEG_vSetSegment(*puPhysId,ucSegStatus);//output of each segment in group
                _SEG_vSetBlinkBuffer(*puPhysId,ucSegStatus);
    
                puPhysId++;
            }
        }
    }
    else
    {
        //output of physical segment
        _SEG_vSetSegment(uPhysId,ucSegStatus);
        _SEG_vSetBlinkBuffer(uPhysId,ucSegStatus);
    }
}


void SEG_vSetSegment8(uint8_t segId, uint8_t ucSegStatus)
{
    uint8_t  uPhysId;
    const uint8_t* puPhysId;
    
    uPhysId = (uint8_t)_SEG_uiGetPhysicalSegment(segId);//get physical ID
    if (uPhysId >= SEG_uiNumberOfSegments)
    {
        uPhysId -= (uint8_t)SEG_uiNumberOfSegments;//calculate group ID
        /*Axivion Next Line MisraC2012-11.5 : "Conversion between object pointer and void pointer */
        puPhysId = _SEG_pGetPhysicalSegmentGroup(uPhysId);//get address of segment group
        if (puPhysId)
        {
            while (*puPhysId != INVALID)//do until endmark INVALID reached
            {
                _SEG_vSetSegment(*puPhysId,ucSegStatus);//output of each segment in group
                _SEG_vSetBlinkBuffer(*puPhysId,ucSegStatus);
                puPhysId++;
            }
        }
    }
    else
    {
        //output of physical segment
        _SEG_vSetSegment(uPhysId,ucSegStatus);
        _SEG_vSetBlinkBuffer(uPhysId,ucSegStatus);
    }
}


void vHandleText(void)
{
    uint8_t j;
    uint32 tSegMask;
    uint8_t ucSegStatus;
    uint8_t ucSegBlinkStatus;
    uint16_t  uiPhysId;
    if (SEG_ucTextIndex != SEG_TEXT_READY)//any text to write
    {
        tSegMask = (uint32) _SEG_tGetDigitsSegmentCode(SEG_ucTextIndex);
        for (j = 0; j < SEG_ucSegmentCountForDigits; j++)
        {
            ucSegBlinkStatus = SEG_pucTextBlinkBuffer[SEG_ucTextIndex];
            if (tSegMask & 0x01u)//mask lowest bit
            {
                ucSegStatus = ucSegBlinkStatus;//set segment
            }
            else
            {
                ucSegStatus = 0;//reset segment
            }
            uiPhysId = _SEG_uiGetRoutingMask(SEG_ucTextIndex,j);
            if (ucSegBlinkStatus <= (uint8_t)SEG_ON)   //only ON/OFF segments should be written into buffer, blinking segments will be handle in _SEG_vSetBlinkBuffer
            {
                _SEG_vSetSegment(uiPhysId, ucSegStatus);
            }
            _SEG_vSetBlinkBuffer(uiPhysId,ucSegStatus);
    
            tSegMask >>= 1 ;//shift next segment to bit 0
        }
        if (++SEG_ucTextIndex >=SEG_ucNumberOfDigits)
        {
            SEG_ucTextIndex = SEG_TEXT_READY;
        }
    }
}



void vWriteBlinkText(uint8_t ucDigitStart, const uint8_t* pucText, uint8_t ucBlinkState)
{
   uint8_t* pucBlinkBufferPtr = &SEG_pucTextBlinkBuffer[ucDigitStart];//pointer to first digit to write
   while (*pucText)
   {
      *pucBlinkBufferPtr = ucBlinkState;
      pucBlinkBufferPtr++;
      pucText++;
   }
}


//Private but not static! may be overwritten with empty function when blinking not used
void vHandleBlinkingSeg(void)
{
   uint16_t uiSegment;
   uint8_t ucBlinkByte;
   uint8_t ucBlinkOnMask;
   uint8_t ucBlinkOffMask;
   if (_SEG_bIsBlinkStateChanged())//blink status changed ?
   {
      for (ucBlinkByte = 0; ucBlinkByte < SEG_ucNumberOfBlinkBytes; ucBlinkByte++)//do for all segments
      {
         ucBlinkOnMask  = _SEG_ucGetBlinkOnSegment(ucBlinkByte);  //calculate sgments to switch On
         ucBlinkOffMask = _SEG_ucGetBlinkOffSegment(ucBlinkByte);//calculate sgments to switch Off
         uiSegment = (uint16_t)ucBlinkByte * 8u;
         while ( (ucBlinkOnMask > 0u) || (ucBlinkOffMask >0u) ) 
         {
            if (ucBlinkOnMask & 1u)
            {
               _SEG_vSetSegment(uiSegment, (uint8_t)SEG_ON);//switch ON current segment
            }
            if (ucBlinkOffMask & 1u)
            {
               _SEG_vSetSegment(uiSegment, (uint8_t)SEG_OFF);//switch OFF current segment
            }
            uiSegment++;
            ucBlinkOnMask  >>= 1u; //shift next segment to LSB
            ucBlinkOffMask >>= 1u; //shift next blink bit to LSB
         }
      }
   }
}

