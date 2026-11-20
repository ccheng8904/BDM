/*******************************************************************************
 *  Copyright (c) 2023 BSH Hausgeraete GmbH,
 *  Carl-Wery-Str. 34, 81739 Munich, Germany, www.bsh-group.de
 *
 *  All rights reserved. This program and the accompanying materials
 *  are protected by international copyright laws.
 *  Please contact copyright holder for licensing information.
 *
 *******************************************************************************
 *   PROJECT          PED Framework
 *   MODULE-PREFIX    CRC
 ******************************************************************************/

/******************************************************************************/
/* DOCUMENTATION                                                              */
/******************************************************************************/
/** \file  StdCrc.c
 * 
 *  \brief Implementation of StdCrc for SH30F9X71.
 */


/******************************************************************************/
/* INCLUDES                                                                   */
/******************************************************************************/
#include "bsh_stdinc.h"
#include <stdint.h>
#include <stdbool.h>
#include "StdCrc.h"
#include "CrcCom.h"
#include "processor.h"

/******************************************************************************/
/* PRIVATE DEFINITIONS                                                        */
/******************************************************************************/
#define CRC_DATA        CRC0->DR
#define CRC_DR_BYTE    *((volatile uint8_t *)&(CRC0->DR))
static const uint32_t CRC_WordAlignBytes = 0x04U; ///< Alignment in bytes for a 32-Bit Word

/** Settings for control register in CITT mode:   16bit polynomial,    no revin,            no revout,  */

#define CRC_CCITT_CTR_INIT                      (((uint32_t)CRC_MODE_CITT << CRC_CR_MODE_Pos)| ((uint32_t)1u << CRC_CR_RELOAD_Pos) )

/** Settings for control register in Eternet mode: 32bit polynomial,   read revin by bit,                write revin by bit and byte,    */

#define CRC_ETHERNET_CTR_INIT                   (((uint32_t)CRC_MODE_32 << CRC_CR_MODE_Pos) | ((uint32_t)1u << CRC_CR_RBITW_Pos) | ((uint32_t)1u << CRC_CR_RBITR_Pos)|((uint32_t)1u << CRC_CR_RBYTER_Pos)| ((uint32_t)1u << CRC_CR_RELOAD_Pos))

/******************************************************************************/
/* LOCAL FUNCTION DEFINITION                                                  */
/******************************************************************************/
/*lint -save -e9044 -e9049 -e928
   9044: function parameter modified (misra advisory)
   9049: increment/decrement operation combined with other operation (misra advisory)
   928:  explicit cast from 'volatile uint32_t *'to 'volatile uint8_t *'(misra advisory)
*/


/*!
 * \brief
 * Check if an byte pointer has word alignment or not
 *
 * This interface may be used to check the alignment of a byte pointer before casting it to a word pointer
 * Usually is not allowed to cast a pointer to another with a stricter alignment. When this check returns true
 * \param[in] date: pointer to check
 *
 * \return true if data has word alignment
 */
static bool CRC_isWordAligned(const uint8_t *data)
{
    /*lint -e{923}  cast is allowed as uintptr_t can hold address of pointer (C99) */
    return ( ( (uintptr_t)data % CRC_WordAlignBytes ) == 0U ) ? (true) : (false);
}


static void CRC_triggerWatchDogTimer(void)
{
    IWDT->CLR = 0xAAAAU;
    WWDT->CLR = 0x5555U;
}

/******************************************************************************/
/* PUBLIC FUNCTION DEFINITIONS                                                */
/******************************************************************************/
/*Axivion Disable MisraC2012-8.7 : Routine can be declared static in primary file. */
uint16_t CRC_Ccitt_Init( void )
{
    /* Unlock RCC registers */
    RCC_REGS_UNLOCK();
    RCC->AHBENR.V32    |= (uint32_t)RCC_AHB_CRC;
    /* Lock RCC registers */
    RCC_REGS_LOCK();
    CRC0->INIT          = CRC_CCITT_INIT;
    CRC0->CR.V32        = CRC_CCITT_CTR_INIT;
    return CRC_CCITT_INIT;
}

/*****************************************************************************/
uint16_t CRC_Ccitt_UpdateByte( uint16_t crc, uint8_t value )
{
    CRC0->INIT = crc;
    CRC0->CR.V32        = CRC_CCITT_CTR_INIT;
    CRC_DR_BYTE = value;
    return (uint16_t)CRC_DATA;
}

/*****************************************************************************/
/* same implementation as CRC_Ccitt_UpdateByte */
uint16_t CRC_Ccitt_UpdateByteFast( uint16_t crc, uint8_t value )
{
    CRC0->INIT = crc;
    CRC0->CR.V32        = CRC_CCITT_CTR_INIT;
    CRC_DR_BYTE = value;
    return (uint16_t)CRC_DATA;
}


/*****************************************************************************/
uint16_t CRC_Ccitt_Update( uint16_t crc, uint32_t len, const uint8_t *data )
{
    CRC0->INIT = crc;
    CRC0->CR.V32        = CRC_CCITT_CTR_INIT;
    /* optimised for speed by reducing branches and loop decrement*/
    for(uint32_t length = len / sizeof(uint32_t); length > 0U; length--)
    {
        /* Hint: Word writing seems not to be possible with CCITT polynom */
        CRC_triggerWatchDogTimer();
        CRC_DR_BYTE = *data++;
        CRC_DR_BYTE = *data++;
        CRC_DR_BYTE = *data++;
        CRC_DR_BYTE = *data++;
    }

    /* Handle unaligned data at buffer end */
    for(uint32_t length = len % sizeof(uint32_t); length > 0U; length--)
    {
        CRC_DR_BYTE = *data++;
    }

    return (uint16_t)CRC_DATA;
}
/* Axivion Enable Style MisraC2012-8.7 */ 



/*****************************************************************************/
uint16_t CRC_Ccitt_Finish( uint16_t crc )
{
    return crc; // nothing to do for CCITT CRC
}

/*****************************************************************************/
uint16_t CRC_Ccitt_Calc( uint32_t len, const uint8_t *data )
{
    uint16_t crc;

    crc = CRC_Ccitt_Init();
    crc = CRC_Ccitt_Update( crc, len, data );
    // Hint: CRC_Ccitt_Finish not required for CCITT
    return crc;
}

/*****************************************************************************/
/*************************** Ethernet Interfaces *****************************/
/*****************************************************************************/
uint32_t CRC_Ethernet_Init( void )
{
    /* Unlock RCC registers */
    RCC_REGS_UNLOCK();
    RCC->AHBENR.V32    |= (uint32_t)RCC_AHB_CRC;
    /* Lock RCC registers */
    RCC_REGS_LOCK();
    CRC0->INIT          = CRC_ETHERNET_INIT;
    CRC0->CR.V32        = CRC_ETHERNET_CTR_INIT;
    return CRC_ETHERNET_INIT;
}

/*****************************************************************************/
uint32_t CRC_Ethernet_UpdateByte( uint32_t crc, uint8_t value )
{ 
 /* The following two lines would be required, if another CRC is calculated in parallel
    it will crash. Omitted to make the HW calculation not slower than a SW CRC!
    Compare implementation CRC_Ethernet_Update()
    CRC_INIT = crc;
    CRC_INIT = CRC_DR;
 */
    (void)crc;
    CRC_DR_BYTE = value;
    return CRC_DATA;
}

/*****************************************************************************/
uint32_t CRC_Ethernet_Update( uint32_t crc, uint32_t len, const uint8_t *data )
{
    const uint8_t  *byte_ptr = data;
    const uint32_t *word_ptr;
    
    uint32_t crc_reverse = 0;
    for(uint32_t i = 0U; i < 32U; i++)
    {
        if((crc >> i) & 0x01U)
        {
            crc_reverse |= ((uint32_t)1 << (31U - i));
        }
    }
    CRC0->INIT          = crc_reverse;
    CRC0->CR.V32        = CRC_ETHERNET_CTR_INIT;

    while ( ( CRC_isWordAligned(byte_ptr) == false ) && ( len > 0U ) )
    {   // handled unaligned start of buffer (maximal 3 bytes)
        CRC_DR_BYTE = *byte_ptr++;
        len--;
    }

    /* cast is allowed as buffer start is aligned (checked by CRC_isWordAligned()) */
    word_ptr = (const uint32_t*)byte_ptr; /*lint !e927 */

    for (uint32_t length = len / CRC_WordAlignBytes; length > 0U; length--)
    {   // in this loop is done the main work
        CRC_triggerWatchDogTimer();
        CRC_DATA = *word_ptr++;
    }
    /*lint -e{928}  cast is OK as alignment of word is stricter than pointer of byte */
    byte_ptr = (const uint8_t*)word_ptr;

    for (uint32_t length = len % CRC_WordAlignBytes; length > 0U; length--)
    {
        CRC_DR_BYTE = *byte_ptr++;
    }

    return CRC_DATA;
}

/*****************************************************************************/
uint32_t CRC_Ethernet_Finish( uint32_t crc )
{
    return crc ^ CRC_ETHERNET_XOR_OUT;
}

/*****************************************************************************/
uint32_t CRC_Ethernet_Calc( uint32_t len, const uint8_t *data )
{
    uint32_t crc;

    crc = CRC_Ethernet_Init();
    crc = CRC_Ethernet_Update( crc, len, data );
    return CRC_Ethernet_Finish( crc );
}

//lint -restore -e9044 -e9049 -e928
