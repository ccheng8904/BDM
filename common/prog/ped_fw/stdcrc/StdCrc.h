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

#ifndef STDCRC_H
#define STDCRC_H

#ifdef __cplusplus
extern "C" {
#endif



/**************************************************************************************************/
/* DOCUMENTATION                                                                                  */
/**************************************************************************************************/
/**
\if StdCrc
\file 
 \brief Public declarations and definitions of StdCrc library
\endif
*/
/**
\if StdCrc
\mainpage Common used Functions and Definitions
\else
\file 
 \brief Public declarations and definitions of StdCrc library
\endif

\section intro Introduction

The StdCrc library provides functions for Ethernet-CRC and CCITT-CRC.\n
There is a generic SW-Implementation as well as implementations using a peripheral CRC.
The provided interfaces should be used instead of project specific solutions.
The HW-implementation is optimized for speed, but generic SW-implementation is more suitable for projects with reentrance or mutiple processes using the CRC unit.

\section ccitt 16-Bit CCITT-CRC Interfaces

- uint16_t CRC_Ccitt_Init( void )
- uint16_t CRC_Ccitt_Update( uint16_t crc, uint32_t len, const uint8_t *data )
- uint16_t CRC_Ccitt_UpdateByte( uint16_t crc, uint8_t value )
- uint16_t CRC_Ccitt_UpdateByteFast( uint16_t crc, uint8_t value )
- uint16_t CRC_Ccitt_Finish( uint16_t crc )
- uint16_t CRC_Ccitt_Calc( uint32_t len, const uint8_t *data )


\section ethernet 32-Bit Ethernet-CRC Interfaces

- uint32_t CRC_Ethernet_Init( void )
- uint32_t CRC_Ethernet_Update( uint32_t crc, uint32_t len, const uint8_t *data )
- uint32_t CRC_Ethernet_UpdateByte( uint32_t crc, uint8_t value )
- uint32_t CRC_Ethernet_Finish( uint32_t crc )
- uint32_t CRC_Ethernet_Calc( uint32_t len, const uint8_t *data )


*/

/*lint -save -e9059 "C comment contains C++ comment"
 * Codeblock with comments needed. Tested with various parsers to work correctly
 */

 /*------------------------  CRC calculation example start ------------------------
How to calculate CRC16 (Ethernet type) value by using HW implementation.

1. CRC16 is possible to calculate over a specific data area by using function CRC_Ccitt_Calc(length in bytes, pointer to data),
which return a calculated result.

Pointer = (uint32_t *)0x10000000;           // pointer to a specific address
Crc16Result = CRC_Ccitt_Calc(8,Pointer);

For a big data areas is better to use partial calculation.

2. CRC16 is possible to calculate over a specific data area partially by using function CRC_Ccitt_Update(Crc result, lenght in bytes, pointer to data),
it is possible to specify the length of data in bytes, which will be used to calculation.
To use mentioned function, it is required to call CRC_Ccitt_Init() before the implementation and
CRC_Ccitt_Finish() after the implementation to get successful result.

Pointer = (uint32_t *)0x10000000;           // pointer to a specific address
Crc16Result  = CRC_Ethernet_Init();
    for(i=0;i<required length;i++)    // cycle over a specific memory area
    {
    Crc16Result  = CRC_Ccitt_Update(Crc16Result, SizeOf(Pointer) , Pointer);
    *Pointer++;  }
Crc16Result  = CRC_Ccitt_Finish(Crc16Result);

3. CRC16 is possible to calculate over a specific data area partially by using function CRC_Ccitt_UpdateByte(Crc result, one byte variable),
this function is used to calculate CRC16 byte per byte.
To use mentioned function, it is required to call CRC_Ccitt_Init() before the implementation
and CRC_Ccitt_Finish() after the implementation to get successful result.

Pointer = (uint8_t *)0x10000000;
Crc16Result  = CRC_Ccitt_Init();
    for(i=0;i<required length;i++)    // cycle over a specific memory area
    {
    ByteVariable = *Pointer;
    Crc16Result  = CRC_Ccitt_UpdateByte(Crc16Result, ByteVariable);
    *Pointer++;  }
Crc16Result  = CRC_Ccitt_Finish(Crc16Result);

------------------------------------------------------------------------------------

How to calculate CRC32 (Ethernet type) value by using HW implementation.

1. CRC32 is possible to calculate over a specific data area by using function CRC_Ethernet_Calc(length in bytes, pointer to data),
which return a calculated result.

Pointer = (uint32_t *)0x10000000;           // pointer to a specific address
Crc32Result = CRC_Ethernet_Calc(8,Pointer);

For a big data areas is better to use partial calculation. Calculation over whole flash 0x10000000 to 0x100FFFF0 was tested successful.

2. CRC32 is possible to calculate over a specific data area partially by using function CRC_Ethernet_Update(Crc result, lenght in bytes, pointer to data),
it is possible to specify the length of data in bytes, which will be used to calculation.
To use mentioned function, it is required to call CRC_Ethernet_Init() before the implementation and
CRC_Ethernet_Finish() after the implementation to get successful result.

Pointer = (uint32_t *)0x10000000;           // pointer to a specific address
Crc32Result  = CRC_Ethernet_Init();
    for(i=0;i< required length ;i++)        // cycle over a specific memory area
    {
    Crc32Result  = CRC_Ethernet_Update(Crc32Result, SizeOf(Pointer) , Pointer);
    *Pointer++;  }
Crc32Result  = CRC_Ethernet_Finish(Crc32Result);

3. CRC32 is possible to calculate over a specific data area partially by using function CRC_Ethernet_UpdateByte(Crc result, one byte variable),
this function is used to calculate CRC32 byte per byte.
To use mentioned function, it is required to call CRC_Ethernet_Init() before the implementation
and CRC_Ethernet_Finish() after the implementation to get successful result.

Pointer = (uint8_t *)0x10000000;          // pointer to a specific address
Crc32Result  = CRC_Ethernet_Init();
    for(i = 0;i< required length ;i++)    // cycle over a specific memory area 
    {
    ByteVariable = *Pointer;
    Crc32Result  = CRC_Ethernet_UpdateByte(Crc32Result, ByteVariable);
    *Pointer++;  }
Crc32Result  = CRC_Ethernet_Finish(Crc32Result);

------------------------  CRC calculation example end ------------------------*/
/*lint -restore -e9059 */

/**************************************************************************************************/
/* INCLUDES                                                                                       */
/**************************************************************************************************/
#include "bsh_stdinc.h"
#include <stdint.h>


/******************************************************************************/
/* PUBLIC TYPE DEFINITIONS                                                    */
/******************************************************************************/

#ifdef crc_16bit_width
typedef uint16_t CRC_crc_t; ///< data type for platform crc
#else
typedef uint32_t CRC_crc_t; ///< data type for platform crc
#endif


/******************************************************************************/
/* PUBLIC FUNCTION DECLARATIONS                                               */
/******************************************************************************/
/*!
 * \brief
 * Initializes the CCITT CRC calculation
 *
 * \note This function also initializes a possible used CRC hardware unit
 * 
 * \return initializer for CRC
 *
 */
uint16_t CRC_Ccitt_Init( void );


/*!
 * \brief
 * Updates the CCITT CRC for a data block
 * 
 * This interface shall be used if the CRC cannot be calculated in a single step.
 * For instance if it takes to much time or complete data unknown.
 * 
 * \note \ref CRC_Ccitt_Init must be called before calling this interface. After all data is processed interface \ref CRC_Ccitt_Finish must be used.\n
 *
 *
 * \param[in] crc: current CRC value
 * \param[in] len: length of data block
 * \param[in] data: pointer to data block
 * 
 * \return new CRC value
 *
 */
uint16_t CRC_Ccitt_Update( uint16_t crc, uint32_t len, const uint8_t *data );


/*!
 * \brief
 * Updates the CCITT CRC for a single byte
 * 
 * This interface shall be used if the CRC cannot be calculated in a single step.
 * For instance if CRC must be updated on the fly for an ongoing data reception
 *
 * \note \ref CRC_Ccitt_Init must be called before calling this interface. After all data is processed interface \ref CRC_Ccitt_Finish must be used.\n
 *
 * \param[in] crc: current CRC value
 * \param[in] value: data value to update the CRC
 * 
 * \return new CRC value
 *
 */
uint16_t CRC_Ccitt_UpdateByte( uint16_t crc, uint8_t value );

/*!
 * \brief
 * Updates the CCITT CRC for a single byte (optimized for speed)
 * 
 * This interface shall be used if the CRC cannot be calculated in a single step.
 * For instance if CRC must be updated on the fly for an ongoing data reception
 *
 * \note \ref CRC_Ccitt_Init must be called before calling this interface. After all data is processed interface \ref CRC_Ccitt_Finish must be used.\n
 *
 * \param[in] crc: current CRC value
 * \param[in] value: data value to update the CRC
 * 
 * \return new CRC value
 *
 */
uint16_t CRC_Ccitt_UpdateByteFast( uint16_t crc, uint8_t value );


/*!
 * \brief
 * Finishes the CCITT CRC calculation.
 * 
 * This interface finishes the CRC calculation after \ref CRC_Ccitt_UpdateByte or \ref CRC_Ccitt_Update was used.
 *
 * \note In case of CCITT this interface does no influence the CRC.
 *
 * \param[in] crc: current CRC value
 * 
 * \return new CRC value
 *
 */
uint16_t CRC_Ccitt_Finish( uint16_t crc );


/*!
 * \brief
 * Calculates the CCITT CRC for the complete data block
 * 
 * \param[in] len: length of data block
 * \param[in] data: pointer to data block
 * 
 * \return CRC value
 */
uint16_t CRC_Ccitt_Calc( uint32_t len, const uint8_t *data );



/*!
 * \brief
 * Initializes the Ethernet CRC calculation
 *
 * \note This function also initializes a possible used CRC hardware unit
 * 
 * \return initializer for CRC
 *
 */
uint32_t CRC_Ethernet_Init( void );


/*!
 * \brief
 * Updates the Ethernet CRC for a single byte
 * 
 * This interface shall be used if the CRC cannot be calculated in a single step.
 * For instance if CRC must be updated on the fly for an ongoing data reception
 *
 * \note \ref CRC_Ethernet_Init must be called before calling this interface. After all data is processed interface \ref CRC_Ethernet_Finish must be used.\n
 *
 * \param[in] crc: current CRC value
 * \param[in] value: data value to update the CRC
 * 
 * \return new CRC value
 *
 */
uint32_t CRC_Ethernet_Update( uint32_t crc, uint32_t len, const uint8_t *data );


/*!
 * \brief
 * Updates the Ethernet CRC for a single byte
 * 
 * This interface shall be used if the CRC cannot be calculated in a single step.
 * For instance if CRC must be updated on the fly for an ongoing data reception
 *
 * \note \ref CRC_Ethernet_Init must be called before calling this interface. After all data is processed interface \ref CRC_Ethernet_Finish must be used.\n
 *
 * \param[in] crc: current CRC value
 * \param[in] value: data value to update the CRC
 * 
 * \return new CRC value
 *
 */
uint32_t CRC_Ethernet_UpdateByte( uint32_t crc, uint8_t value );


/*!
 * \brief
 * Finishes the Ethernet CRC calculation.
 * 
 * This interface finishes the CRC calculation after \ref CRC_Ethernet_UpdateByte or \ref CRC_Ethernet_Update was used.
 *
 * \param[in] crc: current CRC value
 * 
 * \return new CRC value
 */
uint32_t CRC_Ethernet_Finish( uint32_t crc );


/*!
 * \brief
 * Calculates the Ethernet CRC for the complete data block
 * 
 * \param[in] len: length of data block
 * \param[in] data: pointer to data block
 * 
 * \return CRC value
 *
 */
uint32_t CRC_Ethernet_Calc( uint32_t len, const uint8_t *data );


/*!
 * \brief
 * Initializes the platform CRC calculation
 *
 * \note This function also initializes a possible used CRC hardware unit
 * 
 * \return initializer for CRC
 *
 */
#ifndef UNIT_TEST_INST
static inline CRC_crc_t CRC_Init( void )
{
#ifdef crc_16bit_width
    return CRC_Ccitt_Init();
#else
    return CRC_Ethernet_Init();
#endif
}
#else
extern CRC_crc_t CRC_Init( void );
#endif

/*!
 * \brief
 * Updates the platform CRC for a data block
 * 
 * This interface shall be used if the CRC cannot be calculated in a single step.
 * For instance if it takes to much time or complete data unknown.
 * 
 * \note \ref CRC_Init must be called before calling this interface. After all data is processed interface \ref CRC_Finish must be used.\n
 *
 *
 * \param[in] crc: current CRC value
 * \param[in] len: length of data block
 * \param[in] data: pointer to data block
 * 
 * \return new CRC value
 *
 */
#ifndef UNIT_TEST_INST
static inline CRC_crc_t CRC_Update( CRC_crc_t crc, uint32_t len, const uint8_t *data )
{
#ifdef crc_16bit_width
    return CRC_Ccitt_Update(crc, len, data );
#else
    return CRC_Ethernet_Update(crc, len, data );
#endif
}
#else
extern CRC_crc_t CRC_Update( CRC_crc_t crc, uint32_t len, const uint8_t *data );
#endif

/*!
 * \brief
 * Updates the platform CRC for a single byte
 * 
 * This interface shall be used if the CRC cannot be calculated in a single step.
 * For instance if CRC must be updated on the fly for an ongoing data reception
 *
 * \note \ref CRC_Init must be called before calling this interface. After all data is processed interface \ref CRC_Finish must be used.\n
 *
 * \param[in] crc: current CRC value
 * \param[in] value: data value to update the CRC
 * 
 * \return new CRC value
 *
 */
#ifndef UNIT_TEST_INST
static inline CRC_crc_t CRC_UpdateByte( CRC_crc_t crc, uint8_t value )
{
#ifdef crc_16bit_width
    return CRC_Ccitt_UpdateByte(crc, value );
#else
    return CRC_Ethernet_UpdateByte(crc, value );
#endif
}
#else
extern CRC_crc_t CRC_UpdateByte( CRC_crc_t crc, uint8_t value );
#endif

/*!
 * \brief
 * Finishes the platform CRC calculation.
 * 
 * This interface finishes the CRC calculation after \ref CRC_Ethernet_UpdateByte or \ref CRC_Ethernet_Update was used.
 *
 * \param[in] crc: current CRC value
 * 
 * \return new CRC value
 */
#ifndef UNIT_TEST_INST
static inline CRC_crc_t CRC_Finish( CRC_crc_t crc )
{
#ifdef crc_16bit_width
    return CRC_Ccitt_Finish( crc );
#else
    return CRC_Ethernet_Finish( crc );
#endif
}
#else
extern CRC_crc_t CRC_Finish( CRC_crc_t crc );
#endif


/*!
 * \brief
 * Calculates the platform CRC for the complete data block
 * 
 * \param[in] len: length of data block
 * \param[in] data: pointer to data block
 * 
 * \return CRC value
 *
 */
#ifndef UNIT_TEST_INST
static inline CRC_crc_t CRC_Calc( uint32_t len, const uint8_t *data )
{
#ifdef crc_16bit_width
    return CRC_Ccitt_Calc(len, data );
#else
    return CRC_Ethernet_Calc(len, data );
#endif
}
#else
extern CRC_crc_t CRC_Calc( uint32_t len, const uint8_t *data );
#endif


#ifdef __cplusplus
}
#endif

#endif // STDCRC_H
