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
 *  COMP_ABBREV      CRC
 ******************************************************************************/


/**************************************************************************************************/
/* DOCUMENTATION                                                                                  */
/**************************************************************************************************/
/** \file
 *
 *  \brief    Public definitions and declarations for module CrcCom.
 *
 *  \details  It provides a CRC package with some basic speed or size optimized CRC operations.
 *            CRC implementation for a specific platform may use this implementations to optimize
 *            for speed or size.
 *
 */


#ifndef CRCTBL_H
#define CRCTBL_H

#ifdef __cplusplus
extern "C" {
#endif

/**************************************************************************************************/
/* INCLUDES                                                                                       */
/**************************************************************************************************/
#include "bsh_stdinc.h"
#include <stdint.h>


/**************************************************************************************************/
/* PUBLIC DEFINITIONS                                                                             */
/**************************************************************************************************/
#define CRC_CCITT_INIT          ((uint16_t)0xFFFFU)       ///< Start value for CCITT CRC
#define CRC_ETHERNET_INIT       ((uint32_t)0xFFFFFFFFUL)  ///< Start value for Ethernet CRC
#define CRC_ETHERNET_XOR_OUT    ((uint32_t)0xFFFFFFFFUL)  ///< Final XOR value for Ethernet CRC
#define CRC_CCITT_POLYNOMIAL    ((uint32_t)0x00001021UL)  ///< Final XOR value for Ethernet CRC
#define CRC_ETHERNET_POLYNOMIAL ((uint32_t)0x04C11DB7UL)  ///< Final XOR value for Ethernet CRC

#define CRC_TABLE_SIZE      0x010U ///< Length of short lookup table
#define CRC_TABLE_SIZE_LONG 0x100U ///< Length of long lookup table

/**************************************************************************************************/
/* PUBLIC TYPE DEFINITIONS                                                                        */
/**************************************************************************************************/



/**************************************************************************************************/
/* GLOBAL VARIABLE DECLARATIONS                                                                   */
/**************************************************************************************************/
extern const uint16_t CRC_CCITT_LookUpTable       [CRC_TABLE_SIZE];     ///< short lookup table for CCITT CRC
extern const uint16_t CRC_CCITT_LookUpTableFast   [CRC_TABLE_SIZE_LONG];///< long  lookup table for CCITT CRC
extern const uint32_t CRC_Ethernet_LookUpTable    [CRC_TABLE_SIZE];     ///< short lookup table for Ethernet CRC
extern const uint32_t CRC_Ethernet_LookUpTableFast[CRC_TABLE_SIZE_LONG];///< long  lookup table for Ethernet CRC


/*lint -save -e9029 -e9044
  9044: "function parameter modified"
  9029: "Mismatched essential type categories for binary operator.."  reason: reported error of lint
*/
/**************************************************************************************************/
/* INLINE FUNCTION DECLARATIONS                                                                   */
/**************************************************************************************************/

/*!
 * \brief
 * Calculates the Ethernet-CRC for a single byte (optimised for speed and size)
 * 
 * This is the recommended implementation for 32 Bit machines.
 * \note
 * The implementation uses a table to calculate the crc 4-bit wise.
 * 
 *
 * \param[in] crc: current CRC value
 * \param[in] value: data value to update the CRC
 * 
 * \return new CRC value
 *
 */
static inline uint32_t CRC_EthernetCalcByteTbl( uint32_t crc, uint8_t value )
{
    crc ^= value;
    crc  = (crc >> 4U) ^ CRC_Ethernet_LookUpTable[crc & 0x0FUL];
    crc  = (crc >> 4U) ^ CRC_Ethernet_LookUpTable[crc & 0x0FUL];
    
    return crc;
}

/*!
 * \brief
 * Calculates the Ethernet-CRC for a single byte (optimised for speed)
 * 
 * This is the implementation optimised for speed.
 * \note
 * This implementation uses a CRC-Table and needs about 1100 bytes of ROM.
 * 
 *
 * \param[in] crc: current CRC value
 * \param[in] value: data value to update the CRC
 * 
 * \return new CRC value
 *
 */
static inline uint32_t CRC_EthernetCalcByteTblFast( uint32_t crc, uint8_t value )
{
    crc ^= value;
    crc  = (crc >> 8U) ^ CRC_Ethernet_LookUpTableFast[crc & 0xFFUL];
    
    return crc;
}


/*!
 * \brief
 * Calculates the CCITT-CRC for a single byte (optimised for speed and size)
 * 
 * This is the recommended implementation for 32 Bit machines.
 * \note
 * The implementation uses a table to calculate the crc 4-bit wise.
 * 
 *
 * \param[in] crc: current CRC value
 * \param[in] value: data value to update the CRC
 * 
 * \return new CRC value
 *
 */
static inline uint16_t CRC_CcittCalcByteTbl( uint16_t crc, uint8_t value )
{
    crc ^= (uint16_t)((uint16_t)value << 8U);
    crc  = (uint16_t)(crc << 4U) ^ CRC_CCITT_LookUpTable[crc >> 12U];
    crc  = (uint16_t)(crc << 4U) ^ CRC_CCITT_LookUpTable[crc >> 12U];

    return crc;
}


/*!
 * \brief
 * Calculates the CCITT-CRC for a single byte (optimised for size)
 * 
 * This is the implementation optimised for size. Not recommended for 8 bit machines
 * \note
 * The implementation may be smaller than the table based (depending on platform)
 * 
 *
 * \param[in] crc: current CRC value
 * \param[in] value: data value to update the CRC
 * 
 * \return new CRC value
 *
 */
static inline uint16_t CRC_CcittCalcByteOpt( uint16_t crc, uint8_t value )
{
    value  ^=  (uint8_t)(crc   >> 8U);
    value  ^=  (uint8_t)(value >> 4U);
    crc     = (uint16_t)((crc   << 8U) ^ ((uint16_t)value << 12U) ^ ((uint16_t)value << 5U) ^ (uint16_t)value); 
    return crc;
}

/*!
 * \brief
 * Calculates the CCITT-CRC for a single byte (fast implementation)
 * 
 * This is the implementation optimised for speed.
 * \note
 * This implementation uses a CRC-Table and needs about 600 bytes of ROM.
 * 
 *
 * \param[in] crc: current CRC value
 * \param[in] value: data value to update the CRC
 * 
 * \return new CRC value
 *
 */
static inline uint16_t CRC_CcittCalcByteTblFast( uint16_t crc, uint8_t value )
{
    crc ^= (uint16_t)((uint16_t)value << 8U);
    crc  = (uint16_t)(crc   << 8U) ^ CRC_CCITT_LookUpTableFast[crc >> 8U];

    return crc;
}


/*!
 * \brief
 * Calculates the CCITT CRC for a single byte
 * 
 * This is the default implementation for 8 Bit machines and usually delivers best results in speed.
 * For higher speed take \ref CRC_CcittCalcByteTblFast which cost about 600 bytes of code.
 * 
 *
 * \param[in] crc: current CRC value
 * \param[in] value: data value to update the CRC
 * 
 * \return new CRC value
 *
 */
extern uint16_t CRC_CcittCalcByte8BitMachine( uint16_t crc, uint8_t value );

//lint -restore -e9029 -e9044

#ifdef __cplusplus
}
#endif

#endif // __CRCTBL_H
