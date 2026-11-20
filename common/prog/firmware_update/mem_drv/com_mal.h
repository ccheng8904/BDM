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
 ******************************************************************************/


#ifndef COMFLASH_H
#define COMFLASH_H

#ifdef __cplusplus
extern "C" {
#endif


/******************************************************************************/
/* DOCUMENTATION                                                              */
/******************************************************************************/
/** \file
 *
 *  \ingroup  mal
 *
 *  \brief    Public definitions and declarations for unit common flash interfaces.
 *
 *  \details  For simple flashes with a single memory segment use the inline functions.
 *            For flashes with more than one segment use \sa COMA_isAddrRangeValidx
 *            and \sa COMA_isBlockAddrValidx.
 *
 */



/******************************************************************************/
/* INCLUDES                                                                   */
/******************************************************************************/
#include <stdint.h>
#include <stdbool.h>
#include "SegmentDef.h"

/******************************************************************************/
/* PUBLIC DEFINITIONS                                                         */
/******************************************************************************/
#ifndef COMA_ERASE_BYTE
    #define  COMA_ERASE_BYTE 0xFFu
#endif

#if defined(APP_VARIANT) && defined(RX200)
    #define DRVIF_RAMCODE   SDEF_SetSegmentRW(RAMCODE)
#else
    #define DRVIF_RAMCODE
#endif

/******************************************************************************/
/* PUBLIC TYPE DEFINITIONS                                                    */
/******************************************************************************/


/** structure for memory segment description */ 
struct COMA_memSeg_s
{
    uint32_t blockSize; ///< block size inside memory segment
    uint32_t startAddr; ///< start of memory segment
    uint32_t endAddr;   ///< end address of memory segment
};

/******************************************************************************/
/* PUBLIC FUNCTION DECLARATIONS                                               */
/******************************************************************************/

/** \brief    Checks if start address and range is valid.
 *  \details This Interface shall be used if 
 *  \param    memSegs: pointer to memory segment description array
 *  \param    segCount: number of memory segments in memSegs
 *  \param    addr:  address to check
 *  \param    range: range to check
 *  \return   true if address and range is valid, else false
 */
bool COMA_isAddrRangeValidx
    (const struct COMA_memSeg_s* memSegs, uint32_t segCount, uint32_t addr, uint32_t range);


/** \brief    check if address is a valid block start.
 *
 *  \param    memSegs: pointer to memory segment description
 *  \param    segCount: number of memory segment in memSegs
 *  \param    addr:  flash address 
 *  \return   true if address is block-aligned, else false
 */
bool COMA_isBlockAddrValidx
    (const struct COMA_memSeg_s* memSegs, uint32_t segCount, uint32_t addr);


/** \brief   delivers the block size of the block containing specified address.
 *
 *  \param   memSegs: pointer to memory segment description
 *  \param   segCount: number of memory segment in memSegs
 *  \param   addr:  flash address 
 *  \return  block size
 */
uint32_t COMA_getBlockSize
        (const struct COMA_memSeg_s* memSegs, uint32_t segCount, uint32_t addr);


/** common read interface for internal memory.
 *  \note The upper sw layer is responsible to reserve at least size bytes
 *        inside buffer.
 *  \param addr: start address of block to read
 *  \param size: length of data to read (unused)
 *  \param buffer: buffer to copy the data (unused)
 *  \return true if read successfully, false otherwise
 */
bool COMA_read(uint32_t addr, uint32_t size, uint8_t* buffer);

/******************************************************************************/
/* PUBLIC INLINE FUNCTION DEFINITIONS                                         */
/******************************************************************************/

/** \brief    Checks if start address and range is valid.
 *
 *  \param    memSeg: pointer to memory segment description
 *  \param    addr:  address to check
 *  \param    range: range to check
 *  \return   true if address and range is valid, else false
 */
DRVIF_RAMCODE static inline bool COMA_isAddrRangeValid
    (const struct COMA_memSeg_s* memSeg, uint32_t addr, uint32_t range)
{
    return ((
        (addr  >= memSeg->startAddr) && (addr <= memSeg->endAddr) &&
        (range <=((memSeg->endAddr-addr)+1u))
        ) ? true : false);
}


/** \brief    check if address is a valid block start.
 *
 *  \param    memSeg: pointer to memory segment description
 *  \param    addr:  flash address 
 *  \return   true if address is block aligned, else false
 */
DRVIF_RAMCODE static inline bool COMA_isBlockAddrValid
    (const struct COMA_memSeg_s* memSeg, uint32_t addr)
{
    const uint32_t blockMask = (uint32_t)(memSeg->blockSize-1u);
    return ((
        ((blockMask & addr) == 0u)   && /* check block alignment */
        (addr  >= memSeg->startAddr) &&
        (addr  <= memSeg->endAddr)
         ) ? true : false);
}


#ifndef COMA_UNIT

/** \brief union to get block access and byte access to write unit
 *  \note COMA_wrUnit_t must be defined before including this file!!
 */ 
union COMA_wrUnit_u //lint !e9018 union keyword should not be used. MisraC2012 19.2 advisory
{
    COMA_wrUnit_t wrUnit; ///< write unit / access in whole
    uint8_t wrArray[sizeof(COMA_wrUnit_t)]; ///< byte access to write unit
};



/** \brief    check if an address is aligned to write unit.
 *
 *  \param    addr:  flash address 
 *  \return   true if address is aligned, else false
 */
DRVIF_RAMCODE static inline bool COMA_isAligned(uint32_t addr)
{
    const uint32_t WrBlockMask = (uint32_t)(sizeof(COMA_wrUnit_t)-1u);
    return ((WrBlockMask & addr) == 0u) ? true : false;//lint !e587 evaluates always true: false positive
}


/** \brief    converts a buffer to write unit.
 *  \details  left over bytes will be filled with the erase value
 *
 *  \param    size:  number of available bytes in buffer 
 *  \param    inputData: pointer to input buffer
 *  \return   unit to write
 */
DRVIF_RAMCODE static inline COMA_wrUnit_t COMA_charToWrUnit(uint32_t size, const uint8_t* const inputData)
{
    union COMA_wrUnit_u wrResult;
    for(uint32_t i=0; i<sizeof(COMA_wrUnit_t); i++)
    {
        wrResult.wrArray[i] = ((i < size) ? inputData[i] : (uint8_t)(COMA_ERASE_BYTE));
    }
    return wrResult.wrUnit;
}


/* Warning 522: highest operation, function 'COMA_copyToWrUnit', lacks side effects [MISRA 2012 Rule 2.2, advisory] */
/*lint -esym(522,COMA_copyToWrUnit) false positive! */


/** \brief    copy a buffer to write unit at specified address.
 *  \details  left over bytes will be filled with the erase value
 *
 *  \param    size:  number of available bytes in buffer 
 *  \param    inputData: pointer to input buffer
 *  \param    addr:  flash address 
 */
DRVIF_RAMCODE static inline void COMA_copyToWrUnit(uint32_t size, const uint8_t* const inputData, uint32_t addr)
{
    *(COMA_wrUnit_t *)addr = COMA_charToWrUnit(size, inputData); /*lint !e923 upper layer responsible  for alignment */
}


/** \brief    copy a byte to a specified address.
 *
 *  \param    inputData: data to copy
 *  \param    addr:  flash address
 */
DRVIF_RAMCODE static inline void COMA_copyU8(uint32_t addr, const uint8_t inputData)
{
    *((volatile uint8_t*)addr) = inputData; /*lint !e923 upper layer responsible  for alignment */
}


/** \brief    copy a word to a specified address.
 *
 *  \param    inputData: data to copy
 *  \param    addr:  flash address
 */
DRVIF_RAMCODE static inline void COMA_copyU16(uint32_t addr, const uint16_t inputData)
{
    *((volatile uint16_t*)addr) = inputData; /*lint !e923 upper layer responsible  for alignment */
}

/** \brief    copy a dword to a specified address.
 *
 *  \param    inputData: data to copy
 *  \param    addr:  flash address
 */
DRVIF_RAMCODE static inline void COMA_copyU32(uint32_t addr, const uint32_t inputData)
{
    *((volatile uint32_t*)addr) = inputData; /*lint !e923 upper layer responsible  for alignment */
}



/** \brief    Converts an address to a pointer to write unit.
 *  \note     The caller is responsible that given aligned \sa COMA_isAligned
 *
 *  \param    addr:  flash address 
 *  \return   pointer to write unit
 */
DRVIF_RAMCODE static inline COMA_wrUnit_t * COMA_addrToWrUnit(uint32_t addr)
{
    return (COMA_wrUnit_t *)(addr);/*lint !e928 !e923 upper layer must make sure that addr is aligned use COMA_isAligned() */
}


DRVIF_RAMCODE static inline bool COMA_isErased(uint32_t addr, uint32_t size)
{
    const COMA_wrUnit_t * ptr = COMA_addrToWrUnit(addr);
    COMA_wrUnit_t  eraseValue = (COMA_wrUnit_t)(-1);
    for (uint32_t i=0u; i< (size/sizeof(COMA_wrUnit_t)); i++)
    {
        if (ptr[i] != eraseValue){return false;}
    }
    return true;
}

#endif // COMA_UNIT

#ifdef __cplusplus
}
#endif

#endif // COMFLASH_H
