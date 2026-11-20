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
 *  COMP_ABBREV      UTI
 ******************************************************************************/

#ifndef UTILITY_H
#define UTILITY_H

#ifdef __cplusplus
extern "C" {
#endif

/******************************************************************************/
/* DOCUMENTATION                                                              */
/******************************************************************************/
/** \file
 *
 *  \brief    Public declarations and definitions of utility library
 *  
 *  \details  \section intro Introduction
 *
 *
 *   The utility library provides some common used functions and macros.\n
 *   Nearly all other libraries are using utility.lib.\n
 *   \n
 *   
 *   There are 3 groups of functions:
 *   
 *   \section group1 Memory Functions
 *   
 *   - UTI_vMemCopy(const uint8_t *pucSource, uint8_t *pucDestination, uint16_t uiLength)
 *   - UTI_bIsMemEqual(const uint8_t *pucSource1, const uint8_t *pucSource2, uint16_t uiLength)
 *   
 *   
 *   \section group2 Bit Functions/Macros
 *   
 *   - UTI_SET(VAR1,VAR2)
 *   - UTI_RES(VAR1,VAR2)
 *   - UTI_TEST(VAR1,VAR2)
 *   - UTI_vSetBit(uint8_t* pucBitField, uint16_t uiBitId, bool bOnOff)
 *   - UTI_bGetBit(const uint8_t* pucBitField, uint16_t uiBitId)
 *   - UTI_vSetBitField(uint8_t* pucBitField, uint16_t uiBitFieldId,  uint8_t ucBitCount, uint8_t ucValue);
 *   - UTI_ucGetBitField(const uint8_t* pucBitField, uint16_t uiBitFieldId, uint8_t ucBitCount)
 *   - UTI_ucShiftLeft(uint8_t ucValue, uint8_t ucShift);
 *   - UTI_ucShiftRight(uint8_t ucValue, uint8_t ucShift);
 *   
 *   \section group3 Filter Functions
 *   
 *   - UTI_vIIRFilter(uint16_t *pucAccu, uint8_t ucValue, uint8_t ucDampPlus, uint8_t ucDampMinus);
 *   
 *   
 */


/******************************************************************************/
/* INCLUDES                                                                   */
/******************************************************************************/
#include "bsh_stdinc.h"
#include <stdint.h>
#include <stdbool.h>





/* DEFINITIONS ****************************************************************************************/
/*lint -save -e9026 "Function-like macro defined..." cannot be solved with function */

#define UTI_SET(VAR1,VAR2)  ((VAR1)|=(VAR2)) ///< Macro sets bit mask VAR2 in VAR1
#define UTI_RES(VAR1,VAR2)  ((VAR1)&=~(VAR2))///< Macro resets bit mask VAR2 in VAR1
#define UTI_RES8(VAR1,VAR2)  ((VAR1)&=(uint8_t)~(VAR2))///< Macro resets bit mask VAR2 in VAR1 (8-Bit version)
#define UTI_RES16(VAR1,VAR2)  ((VAR1)&=(uint16_t)~(VAR2))///< Macro resets bit mask VAR2 in VAR1 (16-Bit version)
#define UTI_RES32(VAR1,VAR2)  ((VAR1)&=(uint32_t)~(VAR2))///< Macro resets bit mask VAR2 in VAR1 (32-Bit version)
#define UTI_TEST(VAR1,VAR2) ((VAR1)&(VAR2))  ///< Macro tests bit mask VAR2 in VAR1, if mask VAR1 is set in VAR1 result is !=0 (not 1!)

#define UTI_NELEMENTS(arr)  (sizeof(arr)/sizeof((arr)[0]))///< Macro computes number of elements of an array

#define UTI_MIN(a,b) (((a)<(b))?(a):(b)) ///< Macro computes the minimum from values a and b
#define UTI_MAX(a,b) (((a)>(b))?(a):(b)) ///< Macro computes the maximum from values a and b

#define UTI_WORD_FLOOR(size) ((size)&(~3u)) ///< Macro aligns input size to the nearest lower multiple of words (1 word = 4 bytes)
#define UTI_WORD_CEIL(size)  ((((size)%4u)!=0u)?(UTI_WORD_FLOOR((size)+4u)):(size)) ///< Macro aligns input size to the nearest higher multiple of words (1 word = 4 bytes)

/*lint -restore -e9026 */

/* GLOBAL FUNCTIONS **********************************************************************************/

/** 
Fills memory by desired value

\param pucDestination: Target of memory to copy\n
\b type: uint8_t * const

\param ucFillValue: Value to fill the array\n
\b type: const uint8_t

\param uiLength: Number of bytes to fill\n
\b type: uint16_t

\b Usage: \n
static unsigned int MAIN_auiArray[33];  \n
\n
UTI_vMemFill((const uint8_t*)MAIN_auiArray,0x55,sizeof(MAIN_auiArray));  \n
\n

\return None
*/
extern void UTI_vMemFill(uint8_t * const pucDestination, const uint8_t ucFillValue, uint16_t uiLength);


/** 
Copies memory from source to destination

\param pucSource: Source of memory to copy\n
\b type: const uint8_t* const

\param pucDestination: Target of memory to copy\n
\b type: uint8_t * const

\param uiLength: Number of bytes to copy\n
\b type: uint16_t

\return None
*/
extern void UTI_vMemCopy(const uint8_t * const pucSource, uint8_t * const pucDestination, uint16_t uiLength);


/** 
Compares ucLength bytes of buffer pucSource1 and pucSource2 and returns true if they are equal

\b Note: true means !0
\param pucSource1: first source of memory to compare\n
\b type: const uint8_t* const

\param pucSource2: second source of memory to compare\n
\b type: const uint8_t* const

\param uiLength: Number of bytes to compare\n
\b type:  uint16_t\n
\b range: 0..0xFFFF

\return result of comparison\n
\b type: bool\n
\b range: 0=not equal, all others = not equal

*/
extern bool UTI_bIsMemEqual(const uint8_t * const pucSource1, const uint8_t * const pucSource2, uint16_t uiLength);


/**
\brief Sets/resets one bit in an array of bits

This function supports bit-arrays up to 2048 bits (256Byte). Depend on bOnOff bit number uiBitId
will be set/reset in array pucBitField.

\param pucBitField: pointer to the array of bits\n
\b type: uint8_t*

\param uiBitId: ID (position) of bit to be set/reset\n
\b type: uint16_t\n
\b range: 0..2047 (0x0000..0x07FF)\n

\param bOnOff: specify weather set or reset the bit\n
\b type: bool\n
\b range: 0:reset, other: set\n

\return None

\sa UTI_bGetBit()
*/
extern void UTI_vSetBit(uint8_t* pucBitField, uint16_t uiBitId, bool bOnOff);


/**
Tests the state of a bit in an bit array.

This function supports bit-arrays up to 2048 bits (256Byte).\n
Returns the state of bit number uiBitId of bit-array pucBitField.

\param pucBitField: pointer to the array of bits\n
\b type: const uint8_t*

\param uiBitId: ID (position) of bit to be set/reset\n
\b type: uint16_t\n
\b range: 0..2047 (0x0000..0x07FF)\n

\return state of bit\n
\b type: bool\n
\b range: 0: bit cleared, other: bit set

\sa UTI_vSetBit()
*/
extern bool UTI_bGetBit(const uint8_t* pucBitField, uint16_t uiBitId);


/**
Copies n-bits in an arrray of n-bit values.

In the parameter \<ucValue\> there are \<ucBitCount\> right orientated bits. These bits are copied to the array \<pucBitField\>.\n
This function is used to save RAM by using all bits of a byte.\n 
If you need e.g. to save 100 4-Bit values you can use one byte for each value. Using this kind of bitfields you just need 50 bytes.

\param pucBitField: pointer to the array of ucValue-bits\n
\b type: uint8_t*

\param uiBitFieldId: Index (position) of bit field  to be set\n
\b type: uint16_t\n
\b range: (0..2047)/ \<ucBitCount\> \n

\param ucBitCount: number of bits to be bundled in array
\b type: uint8_t
\b range: [1..(8)], for 8 use normal  array (if known at compile time)!

\param ucValue: right orientated bitfield to be set in array
\b type: uint8_t
\b range: [0.. 1<<\<ucBitCount\> -1]


\note This function saves RAM but uses more ROM, so do not use this function to save just 2 bytes of RAM (use your MAP file to decide).
\sa UTI_ucGetBitField(), UTI_bGetBit()
*/
extern void UTI_vSetBitField(uint8_t* pucBitField, uint16_t uiBitFieldId,  uint8_t ucBitCount, uint8_t ucValue);


/**
Returns n-bits from an arrray of n-bit values.

This Function returns \<ucBitCount\> right orientated bits from an array of bitfields.
It is used to save RAM by using all bits of a byte.\n
If you need e.g. to save 100 4-Bit values you can use one byte for each value. Using this kind of bitfields you just need 50 bytes.

\param pucBitField: pointer to the array of ucValue-bits\n
\b type: const uint8_t*

\param uiBitFieldId: Index (position) of bit field  to be read\n
\b type: uint16_t\n
\b range: (0..2047)/ucBitCount\n

\param ucBitCount: number of bits to be bundled in array\n
\b type: uint8_t\n
\b range: [1..(8)], for 8 use normal  array (if known at compile time)!\n

\return right orientated bitfield from index position specified by  uiBitFieldId\n
\b type: uint8_t\n
\b range: [0.. 1<<(ucBitCount) -1]


\note This function saves RAM but uses more ROM, so do not use this function to save just 2 bytes of RAM (use your MAP file to decide).\n
      For 1 bit width fields use function UTI_vSetBit()!
\sa UTI_vSetBitField(), UTI_vSetBit()

*/
extern uint8_t UTI_ucGetBitField(const uint8_t* pucBitField, uint16_t uiBitFieldId, uint8_t ucBitCount);


/**
This functions implements an 8 bit level shifter to left.

UTI_ucShiftLeft shifts the value \<ucValue\> ucShift times to left.
For non HC08 controllers this is not more than a macro. For Metrowerks HC08 level shifting is optimized for speed.\n
When the parameter ucShift is a variable the codesize will be increase.\n
Using this function/macro will force a function call instead inlining again and again.\n
Use this macro/function, when parameter ucShift is not known at compile time, else use shift operator.

\param ucValue: value to be shifted\n
\b type: uint8_t

\param ucShift: number of shift operations to left\n
\b type: uint8_t\n
\b range: 0x00..0xFF (for more than 7, result is allways zero)

\sa UTI_ucShiftRight()
*/
static inline uint8_t UTI_ucShiftLeft(uint8_t ucValue, uint8_t ucShift)
{
    return ((uint8_t)(ucValue<<ucShift));
}


/**
This functions implements an 8 bit level shifter to right

UTI_ucShiftLeft shifts the value \<ucValue\> \<ucShift\>-times to right.
For non HC08 controllers this is not more than a macro. For Metrowerks HC08 level shifting is optimized for speed.\n
When the parameter ucShift is a variable the codesize will be increase.\n
Using this function/macro will force a function call instead inlining again and again.\n
Use this macro/function, when parameter ucShift is not known at compile time, else use shift operator.

\param ucValue: value to be shifted\n
\b type: uint8_t

\param ucShift: number of shift operations to right\n
\b type: uint8_t\n
\b range: 0x00..0xFF (for more than 7, result is allways zero)

\sa UTI_ucShiftLeft()
*/
static inline uint8_t UTI_ucShiftRight(uint8_t ucValue, uint8_t ucShift)
{
    return ((uint8_t)(ucValue>>ucShift));
}


/**
This is the implementation of an easy digital IIR filter.

IIR Filter: Yn+1 = Yn * (P-1)/P + (Xn+1)/P.\n
with P = ucDampPlus for Xn+1 >= Yn, P = ucDampMinus for Xn+1 < Yn\n
There are two different coefficients, one for rising and one for falling values.

\param pucAccu: input/output! last filter output before call/ new filter output after call\n
\b type       : uint16_t*\n
\b range      : 0--0xFFFF\n

\param ucValue: new input value for filter\n
\b type       : uint8_t\n
\b range      : 0..255\n

\param ucDampPlus: coefficient for positive changes\n
\b type       : uint8_t\n
\b range      : 0..255 (0: no filtering)\n

\param ucDampMinus: coefficient for negative changes\n
\b type       : uint8_t\n
\b range      : 0..255 (0: no filtering)\n


\return None


*/
extern void UTI_vIIRFilter(uint16_t *puiAccu, uint8_t ucValue, uint8_t ucDampPlus, uint8_t ucDampMinus);

extern void UTI_vIIRFilter16Bit(uint32_t *pulAccu, uint16_t uiValue, uint8_t ucDampPlus, uint8_t ucDampMinus);

#ifdef __cplusplus
}
#endif

#endif
