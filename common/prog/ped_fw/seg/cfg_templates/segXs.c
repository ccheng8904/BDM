/*******************************************************************************
 *  Copyright (c) 2017 BSH Hausgeraete GmbH,
 *  Carl-Wery-Str. 34, 81739 Munich, Germany, www.bsh-group.de
 *
 *  All rights reserved. This program and the accompanying materials
 *  are protected by international copyright laws.
 *  Please contact copyright holder for licensing information.
 *
 *******************************************************************************
 *  PROJECT          PP PED-Framework
 *  COMP_ABBREV      SEG
 ******************************************************************************/


/******************************************************************************/
/* DOCUMENTATION                                                              */
/******************************************************************************/
/** \file
 *
 *  \ingroup  PED-Framework
 *
 *  \brief    Application specific implementation / configuration for segment library.
 *
 *  \details  This implementation just provides an empty implementation when seg is unused.
 *            Please use the example template when it will be used.
 *
 */

/******************************************************************************/
/* INCLUDES                                                                   */
/******************************************************************************/
#include "bsh_stdinc.h"
#include <stdint.h>
#include <stdbool.h>

#include "seg.h"
#include "segpriv.h"
#include "segXs.h"
#include "LibTypes.h"

const uint8_t SEG_ucNumberOfDigits     = SEG_NUMBER_OF_DIGITS;
const uint8_t SEG_ucNumberOfBlinkBytes = SEG_BUFFER_LENGTH;
const uint16_t SEG_uiNumberOfSegments  = SEG_NUMBER_OF_SEGMENTS;
uint8_t SEG_pucTextBlinkBuffer[SEG_NUMBER_OF_DIGITS];
uint8_t SEG_pucTextBuffer[SEG_NUMBER_OF_DIGITS];
uint8_t SEG_ucSegmentCountForDigits = 0x18u;
uint8_t SEG_ucTextIndex = 0u;


bool _SEG_bGetInitConfig(void)
{
    return true;
}


void _SEG_vSetSegment(uint16_t uiSegId, uint8_t ucSegStatus)
{
    (void)uiSegId;
    (void)ucSegStatus;
}


uint16_t _SEG_uiGetPhysicalSegment(uint16_t uiLogSeg)
{
    (void)uiLogSeg;
    return INVALID;
}


const void* _SEG_pGetPhysicalSegmentGroup(uint8_t ucGroup)
{
    (void)ucGroup;
    return ((const void*)0);
}


void _SEG_vHandleText(void)
{

}


uint16_t _SEG_uiGetRoutingMask(uint8_t ucDigit, uint8_t ucSegment)
{
    (void)ucDigit;
    (void)ucSegment;
    return 0u;
}


void _SEG_vHandleBlinkingSeg(void){}


bool _SEG_bIsBlinkStateChanged(void)
{
    return false;
}


void _SEG_vSetBlinkBuffer(uint16_t uiSegId, uint8_t ucSegStatus)
{
    (void)uiSegId;
    (void)ucSegStatus;
}


uint8_t _SEG_ucGetBlinkOnSegment(uint8_t ucBlinkByte)
{
    (void)ucBlinkByte;
    return 0u;
}


uint8_t _SEG_ucGetBlinkOffSegment(uint8_t ucBlinkByte)
{
    (void)ucBlinkByte;
    return 0u;
}


uint32_t _SEG_tGetDigitsSegmentCode(uint8_t Index)
{
    (void)Index;
    return 0u;
}
