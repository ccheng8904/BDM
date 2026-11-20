/*******************************************************************************
 *  Copyright (c) 2017 BSH Hausgeraete GmbH,
 *  Carl-Wery-Str. 34, 81739 Munich, Germany, www.bsh-group.de
 *
 *  All rights reserved. This program and the accompanying materials
 *  are protected by international copyright laws.
 *  Please contact copyright holder for licensing information.
 *
 *******************************************************************************
 *  PROJECT          VEEPROM
 ******************************************************************************/

/******************************************************************************/
/* DOCUMENTATION                                                              */
/******************************************************************************/
/** \file
 * 8bit CRC for use in Volatile EEPROM library                                                                   
 *
 * This file provides a function for updating successively the calculated crc.
 * Used Polynom: #CRC_POLYNOM
 *
 * Example:
 * \code
   uint8_t ucCrc;
   uint8_t* pucCrc = &ucCrc;
   
   init crc
   ucCrc = 0;
   
   call routine each time, new data is processed
   CRC_vUpdateCrc(pucCrc, 1U);
   CRC_vUpdateCrc(pucCrc, 2U);
   CRC_vUpdateCrc(pucCrc, 3U);
   CRC_vUpdateCrc(pucCrc, 1U);
 
   ucCrc contains now the crc over the following data: 1, 2, 3, 1
 * \endcode
 */

/******************************************************************************/
/* INCLUDES                                                                   */
/******************************************************************************/
#include "LibTypes.h"
#include "crc.h"
#ifdef ASSERT_INCLUDED
#include "pedAssert.h"
#endif

/******************************************************************************/
/* PUBLIC FUNCTION DEFINITIONS                                                */
/******************************************************************************/
/**
 * \brief polynom used for calculation of 8bit-crc within function CRC_vUpdateCrc()
 */
#define CRC_POLYNOM ((uint8_t)0x84U)
/**
 * \brief updates the given crc with actual portion of data
 *
 * 
 * \param pucCrc pointer to crc. Here crc is updated.
 * \param ucData data that shall update the crc
 */
void CRC_vUpdateCrc( uint8_t *pucCrc, uint8_t ucData )
{
    uint8_t  ucBit;
    uint8_t  ucIndex;
#ifdef ASSERT_INCLUDED
    uint8_t ucCheckCrc = *pucCrc;
    uint8_t* pucCheckCrc = &ucCheckCrc;
    uint8_t ucCheckData = ucData;
#endif
    for( ucIndex = 8; ucIndex != 0U; ucIndex-- )
    {
        ucBit = (uint8_t)((*pucCrc ^ ucData) & 1U);
        *pucCrc >>= 1;
        if (0U != ucBit)
        {
            *pucCrc ^= CRC_POLYNOM;
        }
        else
        {
            ucData >>= 1;
        }
    }
#ifdef ASSERT_INCLUDED
    /* testing algorithm */
    ucIndex = 8;
    do
    {
        ucIndex--;
        ucBit = (uint8_t)((*pucCheckCrc ^ ucCheckData) & 1U);
        *pucCheckCrc >>= 1;
        if (0U != ucBit)
        {
            *pucCheckCrc ^= CRC_POLYNOM;
        }
        else
        {
            ucCheckData >>= 1;
        }
    }
    while ( ucIndex != 0U );
    ASSERT_EXPR( *pucCheckCrc == *pucCrc );

#endif
}

