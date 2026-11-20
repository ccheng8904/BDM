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


/******************************************************************************/
/* DOCUMENTATION                                                              */
/******************************************************************************/
/** \file
 *  \brief Generic Implementation of StdCrc.
 *
 *  \details    This implementation is platform independent.
 *              For optimization for or using HW CRC there may be a platform specific solution
 *
 */


/******************************************************************************/
/* INCLUDES                                                                   */
/******************************************************************************/
#include "bsh_stdinc.h"
#include <stdint.h>
#include "StdCrc.h"
#include "CrcCom.h"
#ifdef __TRIGGER_WDG
    #include "hwdt.h"
#endif


/******************************************************************************/
/* GLOBAL FUNCTION DEFINITIONS                                                */
/******************************************************************************/
/*lint -save -e9044 -e9049
   9044: function parameter modified (misra advisory)
   9049: increment/decrement operation combined with other operation (misra advisory)
*/
/******************************************************************************/
/*************************** CCITT CRC Interfaces *****************************/
/******************************************************************************/
uint16_t CRC_Ccitt_Init( void )
{
    return CRC_CCITT_INIT;
}

/******************************************************************************/
uint16_t CRC_Ccitt_UpdateByte( uint16_t crc, uint8_t value )
{
    #ifdef __8BIT_MACHINE    
        crc = CRC_CcittCalcByte8BitMachine( crc, value );
    #else
        crc = CRC_CcittCalcByteOpt( crc, value );  
    #endif
    return crc;
}

/******************************************************************************/
uint16_t CRC_Ccitt_UpdateByteFast( uint16_t crc, uint8_t value )
{
    crc = CRC_CcittCalcByteTblFast( crc, value );
    return crc;
}

/******************************************************************************/
uint16_t CRC_Ccitt_Update( uint16_t crc, uint32_t len, const uint8_t *data )
{
    while( len--  > 0U )
    {
    #ifdef __TRIGGER_WDG
        HWDT_vTrigger();
    #endif
    #ifdef StdCrc_fast
        crc = CRC_CcittCalcByteTblFast( crc, *data++ ); 
    #elif defined(__8BIT_MACHINE)
        crc = CRC_CcittCalcByte8BitMachine( crc, *data++ ); 
    #else
        crc = CRC_CcittCalcByteOpt( crc, *data++ );
    #endif
    }

    return crc;
}


/******************************************************************************/
uint16_t CRC_Ccitt_Finish( uint16_t crc )
{
    return crc; // nothing else to do for CCITT CRC, no XOR-out
}

/******************************************************************************/
uint16_t CRC_Ccitt_Calc( uint32_t len, const uint8_t *data )
{
    uint16_t crc = CRC_CCITT_INIT;
    
    crc = CRC_Ccitt_Update( crc, len, data );

    return crc;
}

/******************************************************************************/
/*************************** Ethernet CRC Interfaces **************************/
/******************************************************************************/
uint32_t CRC_Ethernet_Init( void )
{
    return CRC_ETHERNET_INIT;
}

/******************************************************************************/
uint32_t CRC_Ethernet_UpdateByte( uint32_t crc, uint8_t value )
{
    crc = CRC_EthernetCalcByteTbl( crc, value );
    return crc;
}

/******************************************************************************/
uint32_t CRC_Ethernet_Update( uint32_t crc, uint32_t len, const uint8_t *data )
{
    while( len-- > 0U )
    {
        #ifdef __TRIGGER_WDG
            HWDT_vTrigger();
        #endif
        #ifdef StdCrc_fast
            crc = CRC_EthernetCalcByteTblFast( crc, *data++ );
        #else
            crc = CRC_EthernetCalcByteTbl( crc, *data++ );
        #endif
    }

    return crc;
}

/******************************************************************************/
uint32_t CRC_Ethernet_Finish( uint32_t crc )
{
    return crc ^ CRC_ETHERNET_XOR_OUT;
}

/******************************************************************************/
uint32_t CRC_Ethernet_Calc( uint32_t len, const uint8_t *data )
{   
    uint32_t crc;
   
    crc = CRC_Ethernet_Init();
    crc = CRC_Ethernet_Update( crc, len, data );
    return CRC_Ethernet_Finish( crc );
}



//lint -restore -e9044 -e9049
