/*******************************************************************************
*   Copyright (c) 2017 BSH Hausgeraete GmbH,
*   Carl-Wery-Str. 34, 81739 Munich, Germany, www.bsh-group.de
*
*   All rights reserved. This program and the accompanying materials
*   are protected by international copyright laws.
*   Please contact copyright holder for licensing information.
*
********************************************************************************
*   PROJECT          HSI
*   COMP_ABBREV      BSH
*******************************************************************************/


/******************************************************************************/
/* DOCUMENTATION                                                              */
/******************************************************************************/


/******************************************************************************/
/* INCLUDES                                                                   */
/******************************************************************************/
#include "bsh_stdinc.h"
#include "hsi_basic.h"
#include "map_entry.h"
#include "dbus/dbus2_hsi_adaptor.h"
#include "dbusdll.h"
/******************************************************************************/
/* PRIVATE DEFINITIONS                                                        */
/******************************************************************************/

#define HSI_PROT_VERSION_MJ 2U
//#define HSI_PROT_VERSION_MN 0U

#ifndef HSI_MAX_RECEIVE_PAYLOAD_SIZE
#define HSI_MAX_RECEIVE_PAYLOAD_SIZE  (DBUS2_MAX_HSI_RECEIVE_LEN - sizeof(struct HSI_Message))
#endif
#ifndef HSI_MAX_TRANSMIT_PAYLOAD_SIZE
#define HSI_MAX_TRANSMIT_PAYLOAD_SIZE (DBUS2_MAX_HSI_TRANSMIT_LEN - sizeof(struct HSI_Message))
#endif

#define HSI_PROTOCOL_ID_MAP_2_0 UINT8_C(0x11)

#define HSI_STATUS_CODE_SUCCESS                 UINT8_C(0x00)
#define HSI_STATUS_CODE_PROTOCOL_VERSION_ERROR  UINT8_C(0x04)
#define HSI_STATUS_CODE_PROTOCOL_UNKNOWN        UINT8_C(0x11)

#if ((DBUS2_MAX_HSI_RECEIVE_LEN) < 64U)
#error HSI must support a message length of 64 bytes.
#endif

#if ((DBUS2_MAX_HSI_TRANSMIT_LEN) < 64U)
#error HSI must support a message length of 64 bytes.
#endif

/******************************************************************************/
/* PRIVATE TYPE DEFINITIONS                                                   */
/******************************************************************************/

struct LITE_Long
{
    uint8_t ll;
    uint8_t lh;
    uint8_t hl;
    uint8_t hh;
};


struct HSI_Message
{
    struct LITE_Long total_length;
    uint8_t hsi_prot_ver_mj;
    uint8_t protocol_id;
    uint8_t status_code;
    uint8_t reserved;
    uint8_t data[];/*lint !e9038 for axivion*/
};/*lint !e9038 Arrays with flexible length not liked by Lint, but they reflect reality. It is always being checked, whether we have enough space.*/


union HSI_PointerUnion //lint !e9018 "The union keyword should not be used. MisraC2012 19.2 advisory"
{
    struct HSI_Message *msgPtr;
    uint8_t *bytePtr;
    void *voidPtr;
};


/******************************************************************************/
/* PRIVATE FUNCTION DECLARATIONS AND PRIVATE MACRO FUNCTION DEFINITIONS       */
/******************************************************************************/

static inline uint32_t LITE_ToLong(const struct LITE_Long *const bytes);
static inline void LITE_SetLongBytes(struct LITE_Long *const bytes, uint32_t value);
static void HSI_ProcessMapMessage(union HSI_PointerUnion receiveMessage);//lint !e9018 it is intention to declare a union

/******************************************************************************/
/* PRIVATE DATA DEFINITIONS                                                   */
/******************************************************************************/


/******************************************************************************/
/* PUBLIC FUNCTION DEFINITIONS                                                */
/******************************************************************************/


void HSI_DataIndication(void* const data, uint32_t data_length)
{
    union HSI_PointerUnion receiveMessage;//lint !e9018 it is intention to declare a union
    uint32_t totalLenStatedInHsiMsg;

    receiveMessage.voidPtr=data;
    totalLenStatedInHsiMsg=LITE_ToLong(&(receiveMessage.msgPtr)->total_length);

    if((data_length>=sizeof(*receiveMessage.msgPtr))&&(data_length==totalLenStatedInHsiMsg))
    {
        if(receiveMessage.msgPtr->hsi_prot_ver_mj!=HSI_PROT_VERSION_MJ)
        {
            /*wrong version -> send whole message back to sender, only changing the status code*/
            receiveMessage.msgPtr->status_code=HSI_STATUS_CODE_PROTOCOL_VERSION_ERROR;
            DBPL_vSendHsiProtocolResponse(receiveMessage.voidPtr, (uint8_t)data_length);
        }
        else if(receiveMessage.msgPtr->protocol_id!=HSI_PROTOCOL_ID_MAP_2_0)
        {
            /*only MAP is supported -> send whole message back to sender, only changing the status code*/
            receiveMessage.msgPtr->status_code=HSI_STATUS_CODE_PROTOCOL_UNKNOWN;
            DBPL_vSendHsiProtocolResponse(receiveMessage.voidPtr, (uint8_t)data_length);
        }
        else
        {
            HSI_ProcessMapMessage(receiveMessage);
        }
    }
    else
    {
        DLL_vReleaseDataFromReceivedMessage();
    }
}


/******************************************************************************/
/* PRIVATE FUNCTION DEFINITIONS                                               */
/******************************************************************************/
static inline uint32_t LITE_ToLong(const struct LITE_Long *const bytes)
{
    return (((uint32_t)bytes->hh<<24U)|((uint32_t)bytes->hl<<16U)|((uint32_t)bytes->lh<<8U)|(uint32_t)bytes->ll);
}

static inline void LITE_SetLongBytes(struct LITE_Long *const bytes, uint32_t value)
{
    bytes->hh=(uint8_t)(value>>24U);
    bytes->hl=(uint8_t)(value>>16U);
    bytes->lh=(uint8_t)(value>>8U);
    bytes->ll=(uint8_t)value;
}

/*lint -save -e934 "Taking address of near auto variable 'xx' [MISRA 2012 Rule 1.3, required]" does not make sense on MCU platforms */
static void HSI_ProcessMapMessage(union HSI_PointerUnion receiveMessage)/*lint !e9018 It is the intention to declare a union.*/
{
    union HSI_PointerUnion transmitMessage;/*lint !e9018 It is the intention to declare a union*/
    uint8_t transmitBuffer[DBUS2_MAX_HSI_TRANSMIT_LEN];
    uint16_t dataLengthTxMap;
    uint32_t dataLengthRxMap;
    uint32_t totalHsiLength;

    transmitMessage.bytePtr=transmitBuffer;
    dataLengthTxMap=(uint16_t)(sizeof(transmitBuffer)-sizeof(*transmitMessage.msgPtr));
    dataLengthRxMap=LITE_ToLong(&receiveMessage.msgPtr->total_length)-sizeof(*receiveMessage.msgPtr);

    MAP_ProcessMessage(receiveMessage.msgPtr->data, (uint16_t)HSI_MAX_RECEIVE_PAYLOAD_SIZE, (uint16_t)dataLengthRxMap,transmitMessage.msgPtr->data, (uint16_t)HSI_MAX_TRANSMIT_PAYLOAD_SIZE, &(dataLengthTxMap));

    totalHsiLength=(uint32_t)((uint32_t)dataLengthTxMap+(uint32_t)sizeof(*transmitMessage.msgPtr));
    LITE_SetLongBytes(&transmitMessage.msgPtr->total_length, totalHsiLength);
    transmitMessage.msgPtr->hsi_prot_ver_mj=HSI_PROT_VERSION_MJ;
    transmitMessage.msgPtr->protocol_id=HSI_PROTOCOL_ID_MAP_2_0;
    transmitMessage.msgPtr->status_code=HSI_STATUS_CODE_SUCCESS;
    transmitMessage.msgPtr->reserved = 0U;

    DBPL_vSendHsiProtocolResponse(transmitMessage.voidPtr, (uint8_t)totalHsiLength);
}
/*lint -restore -e934 */

