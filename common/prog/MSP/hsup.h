/*******************************************************************************
*   Copyright (c) 2016 BSH Hausgeraete GmbH,
*   Carl-Wery-Str. 34, 81739 Munich, Germany, www.bsh-group.de
*
*   All rights reserved. This program and the accompanying materials
*   are protected by international copyright laws.
*   Please contact copyright holder for licensing information.
*
********************************************************************************
*   PROJECT          Generic SW
*   COMP_ABBREV      HSUP
*******************************************************************************/


/******************************************************************************/
/* DOCUMENTATION                                                              */
/******************************************************************************/
/** \file
*
*   \brief    Common HSUP definitions for all platforms.
*   \details  \section SUPifc1 Interface Package 1
*   <TABLE>
*   <TR><TD><b>Interface</TD>  <TD>Description</b></TD></TR>
*   <TR><TD>HSUP_vEnterLowPowerMode(void)</TD><TD>Enters the low power mode (with lowest power consumption).</TD></TR>
*   <TR><TD>HSUP_vDisableInt(void)</TD><TD>Disable all interrupts.</TD></TR>
*   <TR><TD>HSUP_vEnableInt(void)</TD><TD>Enable all interrupts.</TD></TR>
*   <TR><TD>HSUP_vDelay(uint8_t ucDelay)</TD><TD>Performs a busy wait for an arbitrary time.</TD></TR>
*   <TR><TD>HSUP_vNop(void)</TD><TD>Performs a NOP operation of the processor.</TD></TR>
*   <TR><TD>HSUP_ucMirror(uint8_t ucVal)</TD><TD>Mirror the passed parameter.</TD></TR>
*   <TR><TD>HSUP_ucGetHighByte(uint16_t uiVal)</TD><TD>Returns the high byte of the passed integer value.</TD></TR>
*   <TR><TD>HSUP_ucGetLowByte(uint16_t uiVal)</TD><TD>Returns the low byte of the passed integer value.</TD></TR>
*   <TR><TD>HSUP_vSetHighByte(uint16_t *puiVal1, uint8_t ucVal2)</TD><TD>Sets the high byte of  *puiVal1 with the passed value ucVal2.</TD></TR>
*   <TR><TD>HSUP_vSetLowByte(uint16_t *puiVal1, uint8_t ucVal2)</TD><TD>Sets the low byte of  *puiVal1 with the passed value ucVal2.</TD></TR>
*   <TR><TD>HSUP_uiGetHighWord(uint32_t ulVal)</TD><TD>Returns the high word of the passed long value.</TD></TR>
*   <TR><TD>HSUP_uiGetLowWord(uint32_t ulVal)</TD><TD>Returns the low word of the passed long value.</TD></TR>
*   <TR><TD>HSUP_vSetHighWord(uint32_t *pulVal1, uint16_t uiVal2)</TD><TD>Sets the high word of  *pulVal1 with the passed value uiVal2.</TD></TR>
*   <TR><TD>HSUP_vSetLowWord(uint32_t *pulVal1, uint16_t uiVal2)</TD><TD>Sets the low word of  *pulVal1 with the passed value uiVal2.</TD></TR>
*   <TR><TD>HSUP_ulProcToBigEndian(uint32_t ulVal)</TD><TD>Converts the passed value ulVal from processor to big endian data storage format.</TD></TR>
*   <TR><TD>HSUP_uiProcToBigEndian(uint16_t uiVal)</TD><TD>Converts the passed value uiVal from processor to big endian data storage format.</TD></TR>
*   <TR><TD>HSUP_ulBigEndianToProc(uint32_t ulVal)</TD><TD>Converts the passed value ulVal from big endian  to processor data storage format.</TD></TR>
*   <TR><TD>HSUP_uiBigEndianToProc(uint16_t uiVal)</TD><TD>Converts the passed value uiVal from big endian  to processor data storage format.</TD></TR>
*   </TABLE>
*   \n
*   The following picture visualises the Interface of HSUP:\n
*   \n
*   \image html hsup.png "Picture 1: Overview HAL SUP"
*
*/


#ifndef HSUP_H
#define HSUP_H

#ifdef __cplusplus
extern "C" {
#endif


/******************************************************************************/
/* INCLUDES                                                                   */
/******************************************************************************/
#include "bsh_stdinc.h"
#include "LibTypes.h"
#if !defined(__PCGCC__) || defined(MOCK_IT)
#include "hsupMc.h"
#endif



/* DEFINITIONS ****************************************************************************************/
/*lint -esym(621,*_ADR_OFFSET) "Macro identifiers shall be distinct" Axivion false positive" */
#if defined(__BIG_ENDIAN_ADAPTATION) || (defined(__BYTE_ORDER__) && (__BYTE_ORDER__ == __ORDER_BIG_ENDIAN__))
#define HIGH_ADR_OFFSET  0 ///< Address offset for high byte/word (big endian)
#define LOW_ADR_OFFSET   1 ///< Address offset for low byte/word (big endian)
#else
#if defined(__LITTLE_ENDIAN_ADAPTATION) || (defined(__BYTE_ORDER__) && (__BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__))
#define HIGH_ADR_OFFSET  1 ///< Address offset for high byte/word (little endian)
#define LOW_ADR_OFFSET   0 ///< Address offset for low byte/word (little endian)
#else
#if   1
#error "Either '__LITTLE_ENDIAN_ADAPTATION' or '__BIG_ENDIAN_ADAPTATION' must be defined in build process in case the compiler does not define '__BYTE_ORDER__'!"
#endif
#endif
#endif

/*lint -esym(9026, HSUP_*) function-like macro*/


static inline uint8_t* HSUP_getBytePtr(uint16_t *value, uint32_t offset)
{   /* explicit cast from uint16_t* to uint8_t* [MISRA 2012 Rule 11.3, required], [MISRA 2012 Rule 11.5, required]
       Pointer arithmetic not allowed [+] MisraC2012 18.4 */
    return ((uint8_t*)value)+offset; /*lint !e928 !e9016 */ 
}

static inline uint16_t* HSUP_getWordPtr(uint32_t *value, uint32_t offset)
{   /* explicit cast from uint32_t* to uint16_t* [MISRA 2012 Rule 11.3, required], [MISRA 2012 Rule 11.5, required]
       Pointer arithmetic not allowed [+] MisraC2012 18.4 */
    return ((uint16_t*)value)+offset; /*lint !e929 !e9016 */ 
}


/** Accesses high byte of parameter word.

\b package: \n
 HSUPPackage1\n

\b description: \n
 This macro accesses the high byte of parameter word

\param word: high byte of word will be accessed, type of word shall be uint16_t

\return none
*/
#define HSUP_HIGH_BYTE(word) (*HSUP_getBytePtr(&(word),HIGH_ADR_OFFSET))

/** Accesses high word of parameter hs_long.

\b package: \n
 HSUPPackage1\n

\b description: \n
 This macro accesses the high word of parameter dword

\param dword: high word of dword will be accessed, type of dword shall be uint32_t

\return none
*/
#define HSUP_HIGH_WORD(dword) (*HSUP_getWordPtr(&(dword),HIGH_ADR_OFFSET))


/** Accesses low byte of parameter word.

\b package: \n
 HSUPPackage1\n

\b description: \n
 This macro accesses the  low byte of parameter word

\param word: low byte of word will be accessed, type of word uint16_t

\return none
*/
#define HSUP_LOW_BYTE(word) (*HSUP_getBytePtr(&(word),LOW_ADR_OFFSET))

/** Accesses low word of parameter dword.

\b package: \n
 HSUPPackage1\n

\b description: \n
 This macro accesses the low word of parameter dword

\param dword: low word of dword will be accessed, type of dword shall be uint32_t

\return none
*/
#define HSUP_LOW_WORD(dword) (*HSUP_getWordPtr(&(dword),LOW_ADR_OFFSET))


/* GLOBAL FUNCTIONS **********************************************************************************/

#if defined(__PCGCC__) && !defined(MOCK_IT)
static inline void HSUP_vDisableInt(void) {}
static inline void HSUP_vEnableInt(void) {}
#else

/** Returns the high byte of the passed uint16_t value.

\b description: \n
 This function returns the high byte of the passed uint16_t value.

\param val: The value where the high byte shall be returned.\n
 \b type: uint16_t\n
 \b range: 0x0000..0xFFFF

\return High byte of val.\n
 \b type: uint8_t\n
 \b range:0x00 .. 0xFF
*/
static inline uint8_t HSUP_ucGetHighByte(uint16_t val){return (uint8_t)(val >> 8U);}

/** Returns the low byte of the passed uint16_t value.

\b description: \n
 This function returns the low byte of the passed uint16_t value.

\param val: The value where the low byte shall be returned.\n
 \b type: uint16_t\n
 \b range: 0x0000..0xFFFF

\return Low byte of val.\n
 \b type: uint8_t\n
 \b range: 0x00 .. 0xFF
*/
static inline uint8_t HSUP_ucGetLowByte(uint16_t val){return (uint8_t)val;}

/** Sets the high byte of 16bit value.

\b description: \n
 This function sets the high byte of *val1 with the passed value val2.

\param val1: The pointer to the value where the high byte will be set.\n
 \b type: (uint16_t*)

\param val2: The new value of the high byte.\n
 \b type: uint8_t\n
 \b range: 0x00..0xFF

\return none
*/
static inline void HSUP_vSetHighByte(uint16_t* val1, uint8_t val2)
{
    HSUP_HIGH_BYTE(*(val1)) = val2;
}

/** Sets the low byte of 16bit value.

\b description: \n
 This function sets the low byte of *val1 with the passed value val2.

\param val1: The pointer to the value where the low byte will be set.\n
 \b type: (uint16_t*)

\param val2: The new value of the low byte.\n
 \b type: uint8_t\n
 \b range: 0x00..0xFF

\return none
*/
static inline void HSUP_vSetLowByte(uint16_t* val1, uint8_t val2)
{
    HSUP_LOW_BYTE(*(val1)) = val2;
}

/** Returns the high word of the passed uint32_t value.

\b description: \n
 This function returns the high word of the passed uint32_t value.


\param val: The value where the high word will be returned.\n
 \b type: uint32_t\n
 \b range: 0x00000000..0xFFFFFFFF

\return The high word of the value val.\n
 \b type: uint16_t\n
 \b range: 0x0000 .. 0xFFFF
*/
static inline uint16_t HSUP_uiGetHighWord(uint32_t val){return (uint16_t)(val >> 16UL);}

/** Returns the low word of the passed uint32_t value.


\b description: \n
 This function returns the low word of the passed long value.

\param ulVal: The value where the low word will be returned.\n
 \b type: uint32_t\n
 \b range: 0x00000000..0xFFFFFFFF

\return The low word of the value val.\n
 \b type: uint16_t\n
 \b range: 0x0000 .. 0xFFFF
*/
static inline uint16_t HSUP_uiGetLowWord(uint32_t val){return (uint16_t)val;}


/** Sets the high word of uint32_t value.

\b description: \n
 This function sets the high word of *val1 with the passed value val2.

\param val1: The pointer to the value where the high word will be set.\n
 \b type: (uint32_t*)

\param val2: The new value of the high word.\n
 \b type: uint16_t\n
 \b range: 0x0000..0xFFFF

\return none
*/
static inline void HSUP_vSetHighWord(uint32_t* val1, uint16_t val2)
{
    HSUP_HIGH_WORD(*(val1)) = val2;
}

/** Sets the low word of uint32_t value.

\b description: \n
 This function sets the low word of *val1 with the passed value val2.

\param val1: The pointer to the value where the low word will be set.\n
 \b type: (uint32_t*)

\param val2: The new value of the low word.\n
 \b type: uint16_t\n
 \b range: 0x0000..0xFFFF

\return none
*/
static inline void HSUP_vSetLowWord(uint32_t* val1, uint16_t val2)
{
    HSUP_LOW_WORD(*(val1)) = val2;
}

//lint -save -e14 "function without definition MisraC2012 8.6" interfaces defined in asm files

/** Converts the passed value from processor to big endian data storage format.

\b description: \n
 This function converts the passed value ulVal from processor to big endian data storage format.

\param value: The value to format to big endian.\n
 \b type: uint32_t\n
 \b range: 0x00000000..0xFFFFFFFF

\return Big endian representation of val.\n
 \b type: uint32_t\n
 \b range: 0x00000000..0xFFFFFFFF
*/
#ifdef __BIG_ENDIAN_ADAPTATION
#define HSUP_ulProcToBigEndian(value)  ((uint32_t)(value))
#else
#ifdef   __LITTLE_ENDIAN_ADAPTATION
extern uint32_t HSUP_ulProcToBigEndian(uint32_t value);
#else
#if   1
#error "Either '__LITTLE_ENDIAN_ADAPTATION' or '__BIG_ENDIAN_ADAPTATION' must be defined in build process!"
#endif
#endif
#endif


/** Converts the passed value from processor to big endian data storage format.

\b description: \n
 This function converts the passed value uiVal from processor to big endian data storage format.

\param value: The value to format to big endian.
 \b type: uint16_t\n
 \b range: 0x0000..0xFFFF

\return: Big endian representation of val.\n
 \b type: uint16_t\n
 \b range: 0x0000 .. 0xFFFF
*/
#ifdef __BIG_ENDIAN_ADAPTATION
#define HSUP_uiProcToBigEndian(value)  ((uint16_t)(value))
#else
#ifdef   __LITTLE_ENDIAN_ADAPTATION
extern uint16_t HSUP_uiProcToBigEndian(uint16_t value); //lint !e9046 'HSUP_uiBigEndianToProc' is typographically ambiguous with respect to 'HSUP_ulBigEndianToProc'
#else
#if   1
#error "Either '__LITTLE_ENDIAN_ADAPTATION' or '__BIG_ENDIAN_ADAPTATION' must be defined in build process!"
#endif
#endif
#endif


/** Converts the passed value from big endian to processor data storage format.

\b description: \n
 This function converts the passed value value from big endian to processor data storage format.

\param value: The value to format to processor data storage format.
 \b type: uint32_t\n
 \b range: 0x00000000..0xFFFFFFFF

\return Processor representation of value.
 \b type: uint32_t\n
 \b range: 0x00000000..0xFFFFFFFF
*/
#ifdef __BIG_ENDIAN_ADAPTATION
#define HSUP_ulBigEndianToProc(value)  ((uint32_t)(value))
#else
#ifdef   __LITTLE_ENDIAN_ADAPTATION
extern uint32_t HSUP_ulBigEndianToProc(uint32_t value);
#else
#if   1
#error "Either '__LITTLE_ENDIAN_ADAPTATION' or '__BIG_ENDIAN_ADAPTATION' must be defined in build process!"
#endif
#endif
#endif


/** Converts the passed value uiVal from big endian to processor data storage format.

\b description: \n
 This function converts the passed value value from big endian to processor data storage format.

\param value: The value to format to processor data storage format.\n
 \b type: uint16_t\n
 \b range: 0x0000..0xFFFF

\return Processor representation of value.\n
 \b type: uint16_t\n
 \b range: 0x0000 .. 0xFFFF
*/
#ifdef __BIG_ENDIAN_ADAPTATION
#define HSUP_uiBigEndianToProc(value)  ((uint16_t)(value))
#else
#ifdef   __LITTLE_ENDIAN_ADAPTATION
extern uint16_t HSUP_uiBigEndianToProc(uint16_t value); //lint !e9046 'HSUP_uiBigEndianToProc' is typographically ambiguous with respect to 'HSUP_ulBigEndianToProc'
#else
#if   1
#error "Either '__LITTLE_ENDIAN_ADAPTATION' or '__BIG_ENDIAN_ADAPTATION' must be defined in build process!"
#endif
#endif
#endif

#if !defined(NO_ISR_SUPPORT) || defined(MOCK_IT)
/** Disables all interrupts.

\b description: \n
 This function disables all interrupts.

\param none

\return none
*/
extern void HSUP_vDisableInt(void);


/** Enables all interrupts.

\b description: \n
 This function enables all interrupts.

\param none

\return none
*/
extern void HSUP_vEnableInt(void);
#else
#define HSUP_vDisableInt()
#define HSUP_vEnableInt()
#endif

/** Mirrors the passed parameter.

\b description: \n
 This function mirrors the passed parameter. The following bits will be exchanged:
   - bit 0 with bit 7
   - bit 1 with bit 6
   - bit 2 with bit 5
   - bit 3 with bit 4
 The result of the operation will be returned.

\param ucVal: value that shall be mirrored\n
 \b type: uint8_t\n
 \b range: 0x00..0xFF

\return: Mirrored result\n
 \b type: uint8_t\n
 \b range: 0x00..0xFF

\note This function is not implemented for all platforms. 
      It is required for those platforms where SPI does not support different bitorders (MSB/LSB-first)
*/
extern uint8_t HSUP_ucMirror(uint8_t ucVal);


#if !defined(LIBRARY_BUILD) && !defined(CORE_CLOCK) && !defined(MOCK_IT)
#error "Please define 'CORE_CLOCK' in Mhz"
#endif
#ifdef CORE_CLOCK
// macro HSUP_DelayWrp and HSUP_DelayLcl for internal use only
#define HSUP_DelayWrp(i,ucDelay)     HSUP_vDelay##i(ucDelay) //lint !e9024  '#/##' operators used [MISRA 2012 Rule 20.10 advisory]
#define HSUP_DelayLcl(i,ucDelay)     HSUP_DelayWrp(i,ucDelay)


/** Performs a busy wait for an arbitrary time.

\b description: \n
 This function performs a busy wait for an arbitrary time.

\param ucDelay: The function waits at least delay microseconds (0x1 .. 0xFF).

\return none
*/


#if !defined(HSUP_vDelay) && !defined(MOCK_IT)
#define HSUP_vDelay(ucDelay)   HSUP_DelayLcl(CORE_CLOCK,ucDelay)
#endif
#endif


/** Generates a CPU reset

\b description: \n
 This function generates a CPU reset

\param none

\return none
*/
extern void HSUP_vGenerateReset(void);
//lint -restore -e14

#endif // __PCGCC__

#ifdef __cplusplus
}
#endif

#endif
