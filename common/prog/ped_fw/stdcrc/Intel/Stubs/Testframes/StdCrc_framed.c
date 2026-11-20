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
#include "stdcrc/StdCrc.h"
#include "stdcrc/CrcCom.h"
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
    CRC_Ccitt_Init_Line[0]++; return CRC_CCITT_INIT;
}

/******************************************************************************/
uint16_t CRC_Ccitt_UpdateByte( uint16_t crc, uint8_t value )
{
    #ifdef __8BIT_MACHINE    
        crc = CRC_CcittCalcByte8BitMachine( crc, value ); CRC_Ccitt_UpdateByte_Line[0]++;
    #else
        crc = CRC_CcittCalcByteOpt( crc, value ); CRC_Ccitt_UpdateByte_Line[1]++;  
    #endif
    CRC_Ccitt_UpdateByte_Line[2]++; return crc;
}

/******************************************************************************/
uint16_t CRC_Ccitt_UpdateByteFast( uint16_t crc, uint8_t value )
{
    crc = CRC_CcittCalcByteTblFast( crc, value ); CRC_Ccitt_UpdateByteFast_Line[0]++;
    CRC_Ccitt_UpdateByteFast_Line[1]++; return crc;
}

/******************************************************************************/
uint16_t CRC_Ccitt_Update( uint16_t crc, uint32_t len, const uint8_t *data )
{
    while( len--  > 0U )
    {
    #ifdef __TRIGGER_WDG
        HWDT_vTrigger(); CRC_Ccitt_Update_Line[0]++;
    #endif
    #ifdef StdCrc_fast
        crc = CRC_CcittCalcByteTblFast( crc, *data++ ); CRC_Ccitt_Update_Line[1]++; 
    #elif defined(__8BIT_MACHINE)
        crc = CRC_CcittCalcByte8BitMachine( crc, *data++ ); CRC_Ccitt_Update_Line[2]++; 
    #else
        crc = CRC_CcittCalcByteOpt( crc, *data++ ); CRC_Ccitt_Update_Line[3]++;
    #endif
    }

    CRC_Ccitt_Update_Line[4]++; return crc;
}


/******************************************************************************/
uint16_t CRC_Ccitt_Finish( uint16_t crc )
{
    CRC_Ccitt_Finish_Line[0]++; return crc; // nothing else to do for CCITT CRC, no XOR-out
}

/******************************************************************************/
uint16_t CRC_Ccitt_Calc( uint32_t len, const uint8_t *data )
{
    uint16_t crc = CRC_CCITT_INIT; CRC_Ccitt_Calc_Line[0]++;
    
    crc = CRC_Ccitt_Update( crc, len, data );CRC_Ccitt_Calc_Line[1]++;

    CRC_Ccitt_Calc_Line[2]++; return crc;
}

/******************************************************************************/
/*************************** Ethernet CRC Interfaces **************************/
/******************************************************************************/
uint32_t CRC_Ethernet_Init( void )
{
    CRC_Ethernet_Init_Line[0]++; return CRC_ETHERNET_INIT;
}

/******************************************************************************/
uint32_t CRC_Ethernet_UpdateByte( uint32_t crc, uint8_t value )
{
    crc = CRC_EthernetCalcByteTbl( crc, value ); CRC_Ethernet_UpdateByte_Line[0]++;
    CRC_Ethernet_UpdateByte_Line[1]++; return crc;
}

/******************************************************************************/
uint32_t CRC_Ethernet_Update( uint32_t crc, uint32_t len, const uint8_t *data )
{
    while( len-- > 0U )
    {
        #ifdef __TRIGGER_WDG
            HWDT_vTrigger(); CRC_Ethernet_Update_Line[0]++;
        #endif
        #ifdef StdCrc_fast
            crc = CRC_EthernetCalcByteTblFast( crc, *data++ ); CRC_Ethernet_Update_Line[1]++;
        #else
            crc = CRC_EthernetCalcByteTbl( crc, *data++ ); CRC_Ethernet_Update_Line[2]++;
        #endif
    }

    CRC_Ethernet_Update_Line[3]++; return crc;
}

/******************************************************************************/
uint32_t CRC_Ethernet_Finish( uint32_t crc )
{
    CRC_Ethernet_Finish_Line[0]++; return crc ^ CRC_ETHERNET_XOR_OUT;
}

/******************************************************************************/
uint32_t CRC_Ethernet_Calc( uint32_t len, const uint8_t *data )
{   
    uint32_t crc; CRC_Ethernet_Calc_Line[0]++;
   
    crc = CRC_Ethernet_Init();CRC_Ethernet_Calc_Line[1]++;
    crc = CRC_Ethernet_Update( crc, len, data );CRC_Ethernet_Calc_Line[2]++;
    CRC_Ethernet_Calc_Line[3]++; return CRC_Ethernet_Finish( crc );
}



//lint -restore -e9044 -e9049
