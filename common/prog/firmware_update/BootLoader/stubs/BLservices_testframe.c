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
 *  COMP_ABBREV      BLSRV
 ******************************************************************************/


/******************************************************************************/
/* DOCUMENTATION                                                              */
/******************************************************************************/
/* Testframe for BLmessageLoop unit */


#define UNIT_UNDER_TEST
#include "bsh_stdinc.h"
#include "stdint.h"

static enum BLCOM_rxResult BLSRV_jumpLoader_Stub (uint16_t address, uint16_t length);
static enum BLCOM_rxResult BLSRV_storeData_Stub  (uint16_t address, uint16_t length);


/* test engine needs wrappper for return value enum */
static uint32_t BLSRV_doCommandService_(const uint8_t *buffer);
static uint32_t BLSRV_setPage_         (uint16_t address, uint16_t length);
static uint32_t BLSRV_invalidCommand_  (uint16_t address, uint16_t length);
static uint32_t BLSRV_setBaudRate_     (uint16_t address, uint16_t length);
static uint32_t BLSRV_selfIdent_       (uint16_t address, uint16_t length);
static uint32_t BLSRV_jumpLoader_      (uint16_t address, uint16_t length);
static uint32_t BLSRV_storeData_       (uint16_t address, uint16_t length);
static uint32_t BLSRV_jumpApplication_ (uint16_t address, uint16_t length);
static uint32_t BLSRV_pageErase_       (uint16_t address, uint16_t length);
static uint32_t BLSRV_massErase_       (uint16_t address, uint16_t length);
static uint32_t BLSRV_programmeData_   (uint16_t address, uint16_t length);
static uint32_t BLSRV_readData_        (uint16_t address, uint16_t length);
static uint32_t BLSRV_calcCrc_         (uint16_t address, uint16_t length);


#include "BLservices.c" // include the unit under test

static const uint8_t BLSRV_TestCmdStoreData[]      = {0x0A, 0x22, 0x55, 0xAA};
static const uint8_t BLSRV_TestCmdJumpLoaderData[] = {0x1A, 0x00, 0x44, 0xBB};
static const uint8_t BLSRV_TestCmdInvalid[]        = {0x7, 0x00, 0x44, 0xBB};
static const uint8_t BLSRV_TestAddress[]           = {0x11, 0x22, 0x33, 0x44};
static uint8_t BLSRV_TestBuffer[60];

const T_TM_Anchor BLSRV_atTestFuncPtr[] = 
{
    (T_TM_Anchor)BLSRV_init,
    (T_TM_Anchor)BLSRV_doCommandService_,
    (T_TM_Anchor)BLSRV_setPage_,
    (T_TM_Anchor)BLSRV_invalidCommand_,
    (T_TM_Anchor)BLSRV_setBaudRate_,
    (T_TM_Anchor)BLSRV_selfIdent_,
    (T_TM_Anchor)BLSRV_jumpLoader_,
    (T_TM_Anchor)BLSRV_storeData_,
    (T_TM_Anchor)BLSRV_jumpApplication_,
    (T_TM_Anchor)BLSRV_pageErase_,
    (T_TM_Anchor)BLSRV_massErase_,
    (T_TM_Anchor)BLSRV_programmeData_,
    (T_TM_Anchor)BLSRV_readData_,
    (T_TM_Anchor)BLSRV_calcCrc_,
    (T_TM_Anchor)BLSRV_u16FromBuffer,
    (T_TM_Anchor)BLSRV_u32FromBuffer,
    (T_TM_Anchor)BLSRV_getTargetAddress,
    (T_TM_Anchor)BLSRV_getAdressFromFrame,
    (T_TM_Anchor)BLSRV_storeData_Stub,
    (T_TM_Anchor)BLSRV_jumpLoader_Stub,
    (T_TM_Anchor)BLSRV_TestCmdStoreData,
    (T_TM_Anchor)BLSRV_TestCmdJumpLoaderData,
    (T_TM_Anchor)BLSRV_TestCmdInvalid,
    (T_TM_Anchor)BLSRV_TestAddress
};

extern const T_TM_Anchor TM_TEST_SupportFuncPtr[];


T_TM_Anchor const * TM_aptAnchor[] =
{
        BLSRV_atTestFuncPtr,
        TM_TEST_SupportFuncPtr
};


#ifndef __NOT_TARGET_CC__
void ITBL_vResetHandler(void)
{
    TM_vInit();
    HSUP_vGenerateReset();
}
#endif


static uint32_t BLSRV_doCommandService_(const uint8_t *buffer)
{
    return (uint32_t)BLSRV_doCommandService(buffer);
}


static uint32_t BLSRV_setPage_(uint16_t address, uint16_t length)
{
    return (uint32_t)BLSRV_setPage(address,length);
}


static uint32_t BLSRV_invalidCommand_(uint16_t address, uint16_t length)
{
    return (uint32_t)BLSRV_invalidCommand(address,length);
}


static uint32_t BLSRV_setBaudRate_(uint16_t address, uint16_t length)
{
    return (uint32_t)BLSRV_setBaudRate(address,length);
}


static uint32_t BLSRV_selfIdent_(uint16_t address, uint16_t length)
{
    return (uint32_t)BLSRV_selfIdent(address,length);
}


static uint32_t BLSRV_jumpLoader_(uint16_t address, uint16_t length)
{
    return (uint32_t)BLSRV_jumpLoader(address,length);
}


static uint32_t BLSRV_storeData_(uint16_t address, uint16_t length)
{
    return (uint32_t)BLSRV_storeData(address,length);
}


static uint32_t BLSRV_jumpApplication_(uint16_t address, uint16_t length)
{
    return (uint32_t)BLSRV_jumpApplication(address,length);
}


static uint32_t BLSRV_pageErase_(uint16_t address, uint16_t length)
{
    return (uint32_t)BLSRV_pageErase(address,length);
}


static uint32_t BLSRV_massErase_(uint16_t address, uint16_t length)
{
    return (uint32_t)BLSRV_massErase(address,length);
}

static uint32_t BLSRV_programmeData_(uint16_t address, uint16_t length)
{
    return (uint32_t)BLSRV_programmeData(address,length);
}

static uint32_t BLSRV_readData_(uint16_t address, uint16_t length)
{
    return (uint32_t)BLSRV_readData(address,length);
}

static uint32_t BLSRV_calcCrc_(uint16_t address, uint16_t length)
{
    return (uint32_t)BLSRV_calcCrc(address,length);
}







volatile uint16_t BLSRV_jumpLoader_address;
volatile uint16_t BLSRV_jumpLoader_length;
volatile enum BLCOM_rxResult BLSRV_jumpLoader_RET;
static enum BLCOM_rxResult BLSRV_jumpLoader_Stub (uint16_t address, uint16_t length)
{
    BLSRV_jumpLoader_address = address;
    BLSRV_jumpLoader_length  = length;
    return (enum BLCOM_rxResult)BLSRV_jumpLoader_RET;
}


volatile uint16_t BLSRV_storeData_address;
volatile uint16_t BLSRV_storeData_length;
volatile enum BLCOM_rxResult BLSRV_storeData_RET;
static enum BLCOM_rxResult BLSRV_storeData_Stub  (uint16_t address, uint16_t length)
{
    BLSRV_storeData_address = address;
    BLSRV_storeData_length  = length;
    return (enum BLCOM_rxResult)BLSRV_storeData_RET;
}



volatile uint8_t BLCOM_sendByte_data;
void BLCOM_sendByte(uint8_t data)
{
    BLCOM_sendByte_data = data;
}


volatile const uint8_t *BLCOM_sendBuffer_buffer;
volatile uint16_t BLCOM_sendBuffer_length;
void BLCOM_sendBuffer(const uint8_t *buffer, uint16_t length)
{
    BLCOM_sendBuffer_buffer = buffer;
    BLCOM_sendBuffer_length = length;
    for (uint16_t i=0; i < length; i++)
    {
        if (i< sizeof(BLSRV_TestBuffer)){BLSRV_TestBuffer[i]=buffer[i];}
    }
}

volatile uint16_t BLCOM_setBaudrate_bautrate;
void BLCOM_setBaudrate(uint16_t bautrate)
{
    BLCOM_setBaudrate_bautrate = bautrate;
}


volatile enum BLCOM_rxResult BLCOM_receiveFrame_RET;
volatile uint8_t *BLCOM_receiveFrame_buffer;
volatile uint16_t BLCOM_receiveFrame_length;
volatile uint16_t BLCOM_receiveFrame_timeout;
enum BLCOM_rxResult BLCOM_receiveFrame(uint8_t *buffer, uint16_t length, uint16_t timeout)
{
    BLCOM_receiveFrame_buffer = buffer;
    BLCOM_receiveFrame_length = length;
    BLCOM_receiveFrame_timeout = timeout;
    return (enum BLCOM_rxResult)BLCOM_receiveFrame_RET;
}


volatile uint32_t CRC_Ethernet_Calc_len;
volatile const uint8_t *CRC_Ethernet_Calc_data;
volatile uint32_t CRC_Ethernet_Calc_RET;
uint32_t CRC_Ethernet_Calc( uint32_t len, const uint8_t *data )
{
    CRC_Ethernet_Calc_len  = len;
    CRC_Ethernet_Calc_data = data;
    return CRC_Ethernet_Calc_RET;
    
}

volatile uint8_t HSUP_vGenerateReset_TP;
void HSUP_vGenerateReset(void)
{
    HSUP_vGenerateReset_TP = 1;
}
