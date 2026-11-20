/*******************************************************************************
 *  Copyright (c) 2022 BSH Hausgeraete GmbH,
 *  Carl-Wery-Str. 34, 81739 Munich, Germany, www.bsh-group.de
 *
 *  All rights reserved. This program and the accompanying materials
 *  are protected by international copyright laws.
 *  Please contact copyright holder for licensing information.
 *
 *******************************************************************************
 *  PROJECT          Generic SW
 *  COMP_ABBREV      DBCDBUS
 ******************************************************************************/


/******************************************************************************/
/* DOCUMENTATION                                                              */
/******************************************************************************/
/** \file
 *
 *  \ingroup dbuscan
 *
 *  \brief   Implementation of drivers controlling DBus part of the DBusCAN chip
 *
 */


/******************************************************************************/
/* INCLUDES                                                                   */
/******************************************************************************/
#include "bsh_stdinc.h"
#include <stdint.h>
#include "dbuscan_dbus.h"
#include "dbuscan_drv.h"
#include "dbuscan.h"
#include "utility.h"
#include "dbus_rtos_interface.h"
#include "mcal/mcal_assert.h"

#if defined (__CC_ARM)
    #pragma push
    #pragma anon_unions    /* allow anonymous structures and unions */
#endif


/******************************************************************************/
/* PRIVATE DEFINITIONS                                                        */
/******************************************************************************/

/** \brief   Definition of buffer size needed for data reception
 */
#define DBCDBUS_READ_BUFFER_SIZE            (DBC_DBUS_RXF_HDR_SIZE + BUS_MESSAGE_ID_LEN + __MESSAGE_INPUT_BUFFER_SIZE + DBC_DBUS_CRC_SIZE)

/** \brief   Number of node ID filters free to write
 *
 *  \details The very last node ID filter (DBC_DBUS_NF14_ADDR) is occupied by the default DBus configuration
 *           (to receive messages for all subsystems on default dbus node address).
 *           There are still 14 node ID filters free to write (DBC_DBUS_NF0_ADDR - DBC_DBUS_NF13_ADDR).
 */
#define DBCDBUS_FREE_NODE_FILTER_COUNT      (DBC_DBUS_NF_COUNT - 1u)

/** \brief   Definition for DBCDBUS_rxFramePtr variable if error occurs during readout of received DBus message from DBusCAN
 */
#define DBCDBUS_READ_FRAME_ERROR            ((TbusMessage*)0xFFFFFFFFu)


/******************************************************************************/
/* PRIVATE TYPE DEFINITIONS                                                   */
/******************************************************************************/

/** \brief   Type definition of buffer for data reception from the DBusCAN chip over SPI
 *
 *  \details Consists of 4-byte SPI header + data
 */
typedef PACKED struct
{
    uint8_t reserved[DBC_SPI_HDR_SIZE]; // a reserved word due to the SPI header data
    PACKED union
    {
        uint8_t        payload[DBCDBUS_READ_BUFFER_SIZE];
        DBC_DBUS_RXF_t dbusRxfHdr;
    };
} DBCDBUS_ReadBuf_t;

/** \brief   Type definition of structure for reading DBus Tx status data (from the DBusCAN chip) over SPI
 *
 *  \details Consists of 4-byte SPI header + DBus Tx status data
 */
struct DBCDBUS_TxStatus
{
    uint8_t         reserved[DBC_SPI_HDR_SIZE]; ///< Reserved word due to the SPI header data
    DBC_DBUS_TXSF_t txStatus;                   ///< DBus Tx status data
};


/******************************************************************************/
/* PRIVATE FUNCTION DECLARATIONS AND PRIVATE MACRO FUNCTION DEFINITIONS       */
/******************************************************************************/
static enum DBC_Error DBCDBUS_writeNodeFilter(enum DBC_RegAddr filterAddr, const uint32_t value);
static void DBCDBUS_readTxStatus(void);
static void DBCDBUS_sendFrameCallback(void *obj, uint32_t flags, const struct MCAL_EventResponse *eventResponse);    ///< Routine with state machine for sequential non-blocking SPI communication to send a DBus message
static void DBCDBUS_readFrameCallback(void *obj, uint32_t flags, const struct MCAL_EventResponse *eventResponse);    ///< Routine with state machine for sequential non-blocking SPI communication to read a DBus message
static void DBCDBUS_decrementNewMsgCount(void);
static void DBCDBUS_setMspiCallbackFunction(MCAL_CallbackFunction_t funcPtr);                                        ///< This function is used to reconfigure callback function for MSPI communication
static void DBCDBUS_writeRegWithCallback(enum DBC_RegAddr addr, uint32_t data, MCAL_CallbackFunction_t funcPtr);     ///< Writes register of the DBusCAN chip via non-blocking SPI communication and sets callback function to be executed afterwards

// Callback functions for interrupt handling
static void DBCDBUS_irqCbHandleGlobalFlags(void *obj, uint32_t flags, const struct MCAL_EventResponse *eventResponse); ///< Handles and clears global interrupt flags via non-blocking SPI communication. It is called automatically after interrupt flags are read from DbusCAN chip
static void DBCDBUS_irqCbReadSpiFlags(void *obj, uint32_t flags, const struct MCAL_EventResponse *eventResponse);      ///< Reads SPI status flags via non-blocking SPI communication. It is called automatically after global interrupt flags are cleared in DbusCAN chip
static void DBCDBUS_irqCbClearSpiFlags(void *obj, uint32_t flags, const struct MCAL_EventResponse *eventResponse);     ///< Clears SPI status flags via non-blocking SPI communication if SPI error occurred. It is called automatically after SPI status flags are read from DbusCAN chip
static void DBCDBUS_irqCbClearDbusFlags(void *obj, uint32_t flags, const struct MCAL_EventResponse *eventResponse);    ///< Clears DBus interrupt flags via non-blocking SPI communication. It is called automatically after SPI status flags are cleared in DbusCAN chip
static void DBCDBUS_irqCbHandleDbusFlags(void *obj, uint32_t flags, const struct MCAL_EventResponse *eventResponse);   ///< Handles DBus interrupt flags. It is called automatically after DBus interrupt flags are cleared in DbusCAN chip
static void DBCDBUS_irqCbHandleDbusWakeFlag(void *obj, uint32_t flags, const struct MCAL_EventResponse *eventResponse);///< Handler for check wake event for DBusCAN. DBusCAN set to NORMAL mode if wake event is detected
static void DBCDBUS_irqCbFinishIrqHandling(void *obj, uint32_t flags, const struct MCAL_EventResponse *eventResponse); ///< This function is called when interrupt handling is finished


/******************************************************************************/
/* PRIVATE DATA DEFINITIONS                                                   */
/******************************************************************************/
static volatile uint8_t DBCDBUS_newMsgCount = 0u;     ///< This variable is incremented when there is an interrupt from the DBusCAN chip of a new DBus message received. It is decremented when DBus message is read from the DBusCAN chip.
static volatile bool DBCDBUS_isSpiLock = false;       ///< This variable is used to signalize acquiring SPI line for data transfer
static volatile bool DBCDBUS_isIrqHandlingFinished = false;   ///< Indicates whether interrupt handling is complete
static volatile bool DBCDBUS_isFrameSent = false;     ///< Indicates whether DBus frame (to be transmitted) has been delivered to DBusCAN chip
#ifndef RTOS_DBUS_EVENTDRIVEN
static bool DBCDBUS_isMsgTxFinished = false;          ///< Variable for indicating whether transmission of DBus message from the DBusCAN chip is completed
#endif
static MCAL_Callback_t DBCDBUS_mspiCallback;          ///< Callback instance for setting callback function for non-blocking SPI communication
static union DBC_IrqBuf DBCDBUS_irqBuffer;            //lint !e9018 union usage needed and not dangerous
static struct DBCDBUS_TxStatus DBCDBUS_txStatus;      ///< Variable for reading DBus Tx status data from the DBusCAN chip over SPI
static TbusMessage* DBCDBUS_rxFramePtr = NULL;        ///< Pointer where received DBus message will be saved


/******************************************************************************/
/* PUBLIC DATA DEFINITIONS                                                    */
/******************************************************************************/
DBC_WriteBuf_t DBCDBUS_txBuf;                         ///< Buffer for writing data to the DBusCAN chip over SPI


/******************************************************************************/
/* PUBLIC FUNCTION DEFINITIONS                                                */
/******************************************************************************/

bool DBCDBUS_handleIrq(void)
{
    if(!DBCDBUS_isIrqHandlingFinished)
    {
        if(!DBCDBUS_isSpiLock)
        {
            MCAL_initCallback(&DBCDBUS_mspiCallback, DBCDBUS_irqCbHandleGlobalFlags, (void*)&DBCDRV_mspiHandle);
            if(MCAL_OK == MSPI_enableEvent(&DBCDRV_mspiHandle, &DBCDBUS_mspiCallback, MCAL_EVENT_TRANSFER_COMPLETE))
            {
                DBCDBUS_isSpiLock = true;
                if(DBC_OK != DBCDRV_readIrq(&DBCDBUS_irqBuffer))
                {
                    (void)MSPI_disableEvent(&DBCDRV_mspiHandle, &DBCDBUS_mspiCallback, MCAL_EVENT_TRANSFER_COMPLETE);
                    DBCDBUS_isSpiLock = false;
                }
            }
        }
    }
    else
    {
        DBCDBUS_isIrqHandlingFinished = false;
        return true;
    }
    return false;
}

bool DBCDBUS_sendFrame(void)
{
    if(!DBCDBUS_isFrameSent)
    {
        if(!DBCDBUS_isSpiLock)
        {
            MCAL_initCallback(&DBCDBUS_mspiCallback, DBCDBUS_sendFrameCallback, (void*)&DBCDRV_mspiHandle);
            if (MCAL_OK == MSPI_enableEvent(&DBCDRV_mspiHandle, &DBCDBUS_mspiCallback, MCAL_EVENT_TRANSFER_COMPLETE))
            {
                DBCDBUS_isSpiLock = true;
                DBCDBUS_sendFrameCallback(NULL, 0u, NULL);
            }
        }
    }
    else
    {
        DBCDBUS_isFrameSent = false;
        return true;
    }
    return false;
}

TbusMessage* DBCDBUS_readFrame(void)
{
    static bool isMsgReadoutFinished = true;
    TbusMessage* readFramePtr = DBCDBUS_rxFramePtr;

    if((isMsgReadoutFinished == true) && (DBCDBUS_newMsgCount != 0u) && (DBCDBUS_isSpiLock == false))
    {
        isMsgReadoutFinished = false;
        MCAL_initCallback(&DBCDBUS_mspiCallback, DBCDBUS_readFrameCallback, (void*)&DBCDRV_mspiHandle);
        if (MCAL_OK == MSPI_enableEvent(&DBCDRV_mspiHandle, &DBCDBUS_mspiCallback, MCAL_EVENT_TRANSFER_COMPLETE))
        {
            DBCDBUS_isSpiLock = true;
            DBCDBUS_readFrameCallback(NULL, 0u, NULL); // start read message from DBusCAN
        }
        else
        {
            isMsgReadoutFinished = true; // need to try again
        }
    }
    else if(NULL != readFramePtr) // the message was read from DBusCAN (or error occurred)
    {
        if(readFramePtr == DBCDBUS_READ_FRAME_ERROR) //lint !e923 cast from uint32_t to pointer
        {
            readFramePtr = NULL;
        }
        DBCDBUS_rxFramePtr = NULL;
        isMsgReadoutFinished = true;
    }
    else { }
    return readFramePtr;
}

enum DBC_Error DBCDBUS_sendWakeUpPulse(void)
{
    return DBCDRV_writeReg32(DBC_DBUS_DPA_ADDR, DBC_DBUS_DPA_TX_WK_PULSE_MASK);
}

enum DBC_Error DBCDBUS_setNodeFilter(enum DBC_RegAddr filterAddr, bool ackEn, uint8_t node, uint16_t subsystMask)
{
    uint32_t filter;
    const uint8_t MAX_NODE_ADDR = (uint8_t)(DBC_DBUS_NF_PID_MASK >> DBC_DBUS_NF_PID_POS);
    if ((filterAddr < DBC_DBUS_NF0_ADDR) || (filterAddr > DBC_DBUS_NF13_ADDR) || (node > MAX_NODE_ADDR))
    {
        return DBC_ERROR;
    }
    filter  = DBC_DBUS_NF_FLT_VALID_MASK;
    filter |= ((true == ackEn) ? DBC_DBUS_NF_ACK_EN_MASK : 0uL);
    filter |= (((uint32_t)node << DBC_DBUS_NF_PID_POS) & DBC_DBUS_NF_PID_MASK);
    filter |= subsystMask;
    return DBCDBUS_writeNodeFilter(filterAddr, filter);
}

enum DBC_Error DBCDBUS_resetNodeFilter(enum DBC_RegAddr filterAddr)
{
    if ((filterAddr < DBC_DBUS_NF0_ADDR) || (filterAddr > DBC_DBUS_NF13_ADDR))
    {
        return DBC_ERROR;
    }
    return DBCDBUS_writeNodeFilter(filterAddr, 0uL);
}

enum DBC_Error DBCDBUS_resetAllNodeFilters(void)
{
    const uint8_t nodeFilterSize = (uint8_t)(DBCDBUS_FREE_NODE_FILTER_COUNT * DBCDRV_REG_SIZE);
    memset((void*)DBCDBUS_txBuf.payload, 0, nodeFilterSize);

    DBC_RETURN_ON_ERROR(DBCDRV_enableCfgDbus());
    DBC_RETURN_ON_ERROR(DBCDRV_write(DBC_DBUS_NF0_ADDR, &DBCDBUS_txBuf, nodeFilterSize, DBC_WRITE_H));
    DBC_RETURN_ON_ERROR(DBCDRV_disableCfgDbus());

    return DBC_OK;
}

#ifndef RTOS_DBUS_EVENTDRIVEN
bool DBCDBUS_isMsgTransmitted(void)
{
    if(true == DBCDBUS_isMsgTxFinished)
    {
        DBCDBUS_isMsgTxFinished = false;
        return true;
    }
    return false;
}

#else
uint8_t DBCDBUS_getNewMsgCount(void)
{
    return DBCDBUS_newMsgCount;
}

const TbusMessage* DBCDBUS_getRxFramePtr(void)
{
    return DBCDBUS_rxFramePtr;
}
#endif // RTOS_DBUS_EVENTDRIVEN

uint8_t DBCDBUS_getReceivedAck(void)
{
    return DBCDBUS_txStatus.txStatus.statusField.ACK;
}

enum DBC_Error DBCDBUS_enableAck(void)
{
    uint32_t regVal;

    DBC_RETURN_ON_ERROR(DBCDRV_enableCfgDbus());

    // Enable acknowledge for main node/subnode ID
    regVal = DBCDRV_readReg32(DBC_DBUS_SIDFC_ADDR);
    if (DBCDRV_readReg32(DBC_REVISION_ADDR) >= DBC_REVISION_WITH_ALL_FEATURES)
    {
        if (DBC_DBUS_SIDFC_ACK_EN_MASK != (regVal & DBC_DBUS_SIDFC_ACK_EN_MASK))
        {
            regVal |= DBC_DBUS_SIDFC_ACK_EN_MASK;
            DBC_RETURN_ON_ERROR(DBCDRV_writeReg32(DBC_DBUS_SIDFC_ADDR, regVal));
        }
    }
    else
    {   //set back node and subnode ID
        DBC_Cfg_t cfg = DBCDRV_getConfig();
        UTI_RES(regVal, (DBC_DBUS_SIDFC_PID_MASK| DBC_DBUS_SIDFC_SID_MASK));
        UTI_SET(regVal, ((uint32_t)cfg.NODE_ID << DBC_DBUS_SIDFC_PID_POS));
        UTI_SET(regVal, ((uint32_t)cfg.SUBNODE_ID));
        DBC_RETURN_ON_ERROR(DBCDRV_writeReg32(DBC_DBUS_SIDFC_ADDR, regVal));
    }

    // Enable acknowledge for node filter used for reception on all subsystems
    regVal = DBCDRV_readReg32(DBC_DBUS_NF14_ADDR);
    if (DBC_DBUS_NF_ACK_EN_MASK != (regVal & DBC_DBUS_NF_ACK_EN_MASK))
    {
        regVal |= DBC_DBUS_NF_ACK_EN_MASK;
        DBC_RETURN_ON_ERROR(DBCDRV_writeReg32(DBC_DBUS_NF14_ADDR, regVal));
    }

    // Set reception of acknowledged frames only
    regVal  = DBCDRV_readReg32(DBC_DBUS_RXC_ADDR) & ~DBC_DBUS_RXC_RX_FLTR_MASK;
    regVal |= DBC_DBUS_RXC_RX_FLTR_W_ACK_OK;
    DBC_RETURN_ON_ERROR(DBCDRV_writeReg32(DBC_DBUS_RXC_ADDR, regVal));

    DBC_RETURN_ON_ERROR(DBCDRV_disableCfgDbus());

    return DBC_OK;
}

enum DBC_Error DBCDBUS_disableAck(void)
{
    uint32_t regVal;

    DBC_RETURN_ON_ERROR(DBCDRV_enableCfgDbus());

    // Disable acknowledge for main node/subnode ID
    regVal = DBCDRV_readReg32(DBC_DBUS_SIDFC_ADDR);
    if (DBCDRV_readReg32(DBC_REVISION_ADDR) >= DBC_REVISION_WITH_ALL_FEATURES)
    {
        if (DBC_DBUS_SIDFC_ACK_EN_MASK == (regVal & DBC_DBUS_SIDFC_ACK_EN_MASK))
        {
            regVal &= ~DBC_DBUS_SIDFC_ACK_EN_MASK;
            DBC_RETURN_ON_ERROR(DBCDRV_writeReg32(DBC_DBUS_SIDFC_ADDR, regVal));
        }
    }
    else
    {   // only reset node and subnode ID to zero (broadcast)
        UTI_RES(regVal, (DBC_DBUS_SIDFC_PID_MASK| DBC_DBUS_SIDFC_SID_MASK));
        DBC_RETURN_ON_ERROR(DBCDRV_writeReg32(DBC_DBUS_SIDFC_ADDR, regVal));
    }

    // Disable acknowledge for node filter used for reception on all subsystems
    regVal = DBCDRV_readReg32(DBC_DBUS_NF14_ADDR);
    if (DBC_DBUS_NF_ACK_EN_MASK == (regVal & DBC_DBUS_NF_ACK_EN_MASK))
    {
        regVal &= ~DBC_DBUS_NF_ACK_EN_MASK;
        DBC_RETURN_ON_ERROR(DBCDRV_writeReg32(DBC_DBUS_NF14_ADDR, regVal));
    }

    // Set reception of addressed but not acknowledged frames
    regVal  = DBCDRV_readReg32(DBC_DBUS_RXC_ADDR) & ~DBC_DBUS_RXC_RX_FLTR_MASK;
    regVal |= DBC_DBUS_RXC_RX_FLTR_WO_ACK_OK;
    DBC_RETURN_ON_ERROR(DBCDRV_writeReg32(DBC_DBUS_RXC_ADDR, regVal));

    DBC_RETURN_ON_ERROR(DBCDRV_disableCfgDbus());

    return DBC_OK;
}


/******************************************************************************/
/* PRIVATE FUNCTION DEFINITIONS                                               */
/******************************************************************************/

static enum DBC_Error DBCDBUS_writeNodeFilter(enum DBC_RegAddr filterAddr, const uint32_t value)
{
    DBC_RETURN_ON_ERROR(DBCDRV_enableCfgDbus());
    DBC_RETURN_ON_ERROR(DBCDRV_writeReg32(filterAddr, value));
    DBC_RETURN_ON_ERROR(DBCDRV_disableCfgDbus());
    return DBC_OK;
}

static void DBCDBUS_readTxStatus(void)
{
    (void)DBCDRV_readNbl(DBC_DBUS_TXSF_ADDR, (uint8_t*)(void*)&DBCDBUS_txStatus, DBC_DBUS_TXSF_SIZE, DBC_READ_L);
}

/*lint -esym(818,obj) Pointer parameter 'obj' (line 61) could be declared as pointing to const [MISRA 2012 Rule 8.13, advisory] */
static void DBCDBUS_sendFrameCallback(void *obj, uint32_t flags, const struct MCAL_EventResponse *eventResponse)
{
    (void)*(uint8_t*)obj; (void)flags; (void)*eventResponse;

    enum _stateTxFrame
    {
        STATE_TX_FRAME_CHECK_STATUS,
        STATE_TX_FRAME_CHECK_FIFO,
        STATE_TX_FRAME_WRITE_START,
        STATE_TX_FRAME_WRITE_DONE
    };
    static enum _stateTxFrame stateTxFrame = STATE_TX_FRAME_CHECK_STATUS;
    uint8_t txFifoFreeLvl;
    uint32_t regVal;

    switch(stateTxFrame)
    {
        case STATE_TX_FRAME_CHECK_STATUS:
            stateTxFrame = STATE_TX_FRAME_CHECK_FIFO;
            if(DBC_OK == DBCDRV_readReg32Nbl(DBC_DBUS_TXEFS_ADDR))
            {
                return;
            }
            break;
        case STATE_TX_FRAME_CHECK_FIFO:
            // Verify that Tx Status FIFO has free element to accept new status
            regVal = DBCDRV_getReadReg32Nbl();
            if((DBC_DBUS_TXEFS_TEFFL_MASK & regVal) < DBC_DBUS_TXEFS_TEFFL_MAX)
            {
                stateTxFrame = STATE_TX_FRAME_WRITE_START;
                if(DBC_OK == DBCDRV_readReg32Nbl(DBC_DBUS_TXFQS_ADDR))
                {
                    return;
                }
            }
            break;
        case STATE_TX_FRAME_WRITE_START:
            // Verify that Tx FIFO has free element to accept new frame
            regVal = DBCDRV_getReadReg32Nbl();
            txFifoFreeLvl = (uint8_t)(regVal & DBC_DBUS_TXFQS_TFFL_MASK);
            if((txFifoFreeLvl > 0u) && (txFifoFreeLvl <= DBC_DBUS_FIFO_MAX_MSG_CNT))
            {
                uint16_t frameSize = DBCDBUS_txBuf.dbusTxfHdr.MSG_LEN;
                // Verify that Tx FIFO has enough space for the whole frame
                if(frameSize <= ((DBC_DBUS_TXFQS_TFDA_MASK & regVal) >> DBC_DBUS_TXFQS_TFDA_POS))
                {
                    frameSize = (uint16_t)UTI_WORD_CEIL((uint32_t)frameSize + DBC_DBUS_TXF_HDR_SIZE);
                    stateTxFrame = STATE_TX_FRAME_WRITE_DONE;
                    if(DBC_OK == DBCDRV_writeNbl(DBC_DBUS_RX_TX_FIFO_ADDR, &DBCDBUS_txBuf, frameSize, DBC_WRITE_L))
                    {
                        return;
                    }
                }
            }
            break;
        case STATE_TX_FRAME_WRITE_DONE:
            DBCDBUS_isFrameSent = true;
            break;
        default: break;
    }
    stateTxFrame = STATE_TX_FRAME_CHECK_STATUS;
    (void)MSPI_disableEvent(&DBCDRV_mspiHandle, &DBCDBUS_mspiCallback, MCAL_EVENT_TRANSFER_COMPLETE);
    DBCDBUS_isSpiLock = false;
}

static void DBCDBUS_readFrameCallback(void *obj, uint32_t flags, const struct MCAL_EventResponse *eventResponse)
{
    (void)*(uint8_t*)obj; (void)flags; (void)*eventResponse;

    enum _stateRxFrame
    {
        STATE_RX_FRAME_CHECK_STATUS,
        STATE_RX_FRAME_CHECK_AND_READ,
        STATE_RX_FRAME_PROCESS,
        STATE_RX_FRAME_CHECK_STATUS_AGAIN
    };
    static enum _stateRxFrame stateRxFrame = STATE_RX_FRAME_CHECK_STATUS;
    static DBCDBUS_ReadBuf_t rxBuf; // Buffer for reading data from the DBusCAN chip over SPI

    uint32_t regVal;
    uint32_t msgSize;
    uint8_t newMsgCnt;

    switch(stateRxFrame)
    {
        case STATE_RX_FRAME_CHECK_STATUS:
            stateRxFrame = STATE_RX_FRAME_CHECK_AND_READ;
            DBCDBUS_rxFramePtr = NULL;
            (void)DBCDRV_readReg32Nbl(DBC_DBUS_RXF0S_ADDR);
            return;
        case STATE_RX_FRAME_CHECK_AND_READ:
            regVal = DBCDRV_getReadReg32Nbl();
            newMsgCnt = (uint8_t)(regVal & DBC_DBUS_RXF0S_RF0FL_MASK);
            if((newMsgCnt <= DBC_DBUS_FIFO_MAX_MSG_CNT) && (DBCDBUS_newMsgCount != newMsgCnt))
            {
                DBCDBUS_newMsgCount = newMsgCnt;
            }
            msgSize = (regVal & DBC_DBUS_RXF0S_RF0MS_MASK) >> DBC_DBUS_RXF0S_RF0MS_POS;
            // Verify that message is available in RX buffer
            if((0u != msgSize) && (msgSize <= DBCDRV_DBUS_RX_FIFO_SIZE))
            {
                stateRxFrame = STATE_RX_FRAME_PROCESS;
                msgSize = UTI_WORD_CEIL(msgSize);
                if(msgSize > sizeof(rxBuf.payload))
                {
                    msgSize = WORD_SIZE; // make only a partial read to discard too big frame from Rx FIFO
                    stateRxFrame = STATE_RX_FRAME_CHECK_STATUS_AGAIN;
                }
                // Read the received frame from the DBus Rx FIFO (0x4400)
                if(DBC_OK != DBCDRV_readNbl(DBC_DBUS_RX_TX_FIFO_ADDR, (uint8_t*)(void*)&rxBuf, (uint16_t)msgSize, DBC_READ_L))
                {
                    DBCDBUS_rxFramePtr = DBCDBUS_READ_FRAME_ERROR; //lint !e923 cast from uint32_t to pointer
                    break;
                }
                return;
            }
            DBCDBUS_rxFramePtr = DBCDBUS_READ_FRAME_ERROR; //lint !e923 cast from uint32_t to pointer
            DBCDBUS_decrementNewMsgCount();
            break;
        case STATE_RX_FRAME_PROCESS:
            DBCDBUS_rxFramePtr = (TbusMessage*)(void*)&rxBuf.dbusRxfHdr.rxfField.MSG_LEN;
            DBCDBUS_decrementNewMsgCount();
            break;
        case STATE_RX_FRAME_CHECK_STATUS_AGAIN:
            stateRxFrame = STATE_RX_FRAME_CHECK_AND_READ;
            DBCDBUS_decrementNewMsgCount();
            (void)DBCDRV_readReg32Nbl(DBC_DBUS_RXF0S_ADDR);
            return;
        default: break;
    }
    stateRxFrame = STATE_RX_FRAME_CHECK_STATUS;
    (void)MSPI_disableEvent(&DBCDRV_mspiHandle, &DBCDBUS_mspiCallback, MCAL_EVENT_TRANSFER_COMPLETE);
    DBCDBUS_isSpiLock = false;
}

static void DBCDBUS_decrementNewMsgCount(void)
{
    if (DBCDBUS_newMsgCount > 0u)
    {
        DBCDBUS_newMsgCount--;
    }
}

static void DBCDBUS_setMspiCallbackFunction(MCAL_CallbackFunction_t funcPtr)
{
    MCAL_assert(funcPtr != NULL);
    /*lint -e{929} "cast from pointer to pointer [MISRA 2012 Rule 11.3, 11.5 required]" */
    /*Axivion Next Line MisraC2012-11.1 : "Conversion between incompatible function pointer types", Tested to work correctly */
    DBCDBUS_mspiCallback.functionPtr = (STDCB_CallbackFunction_t)funcPtr;
}

static void DBCDBUS_writeRegWithCallback(enum DBC_RegAddr addr, uint32_t data, MCAL_CallbackFunction_t funcPtr)
{
    MCAL_assert(funcPtr != NULL);
    DBCDBUS_setMspiCallbackFunction(funcPtr);
    if(DBC_OK != DBCDRV_writeReg32Nbl(addr, data))
    {
        funcPtr(NULL, 0u, NULL);
    }
}

// Callback functions for interrupt handling
static void DBCDBUS_irqCbHandleGlobalFlags(void *obj, uint32_t flags, const struct MCAL_EventResponse *eventResponse)
{
    const uint32_t globalFlags = DBCDBUS_irqBuffer.irq.globalFlags;
    if(0u != (~DBC_IF_DBUS_CAN_MASK & globalFlags))
    {
        uint32_t globalFlagsToClear = globalFlags & ~(DBC_IF_GLOBAL_FAULT_FLAG_MASK | DBC_IF_UVCC_MASK | DBC_IF_MODE_SLEEP_MASK); // omit clearing read-only flags
        if (0u != (DBC_IF_PWRON_MASK & globalFlagsToClear))
        {
            DBCDRV_notifyPowerOnReset();
            // no sense to check for other interrupt flags, device is in reset
            (void)MSPI_disableEvent(&DBCDRV_mspiHandle, &DBCDBUS_mspiCallback, MCAL_EVENT_TRANSFER_COMPLETE);
            DBCDBUS_isSpiLock = false;
            DBCDBUS_isIrqHandlingFinished = true;
            return;
        }
        if(0u != globalFlagsToClear)
        {
            // clear global interrupt flags
            DBCDBUS_writeRegWithCallback(DBC_IF_ADDR, globalFlagsToClear, DBCDBUS_irqCbReadSpiFlags);
            return;
        }
        DBCDBUS_irqCbReadSpiFlags(obj, flags, eventResponse);
        return;
    }
    DBCDBUS_irqCbClearDbusFlags(obj, flags, eventResponse);
}

static void DBCDBUS_irqCbReadSpiFlags(void *obj, uint32_t flags, const struct MCAL_EventResponse *eventResponse)
{
    if(0u != (DBC_IF_SPIERR_MASK & DBCDBUS_irqBuffer.irq.globalFlags))
    {
        DBCDBUS_setMspiCallbackFunction(DBCDBUS_irqCbClearSpiFlags);
        (void)DBCDRV_readReg32Nbl(DBC_STATUS_ADDR);
        return;
    }
    DBCDBUS_irqCbClearDbusFlags(obj, flags, eventResponse);
}

static void DBCDBUS_irqCbClearSpiFlags(void *obj, uint32_t flags, const struct MCAL_EventResponse *eventResponse)
{
    (void)*(uint8_t*)obj; (void)flags; (void)*eventResponse;
    uint32_t regVal = DBCDRV_getReadReg32Nbl();
    DBCDBUS_writeRegWithCallback(DBC_STATUS_ADDR, regVal, DBCDBUS_irqCbClearDbusFlags);
}

static void DBCDBUS_irqCbClearDbusFlags(void *obj, uint32_t flags, const struct MCAL_EventResponse *eventResponse)
{
    if (0u != DBCDBUS_irqBuffer.irq.dbusFlags)
    {
        // clear DBus interrupt flags
        DBCDBUS_writeRegWithCallback(DBC_DBUS_IR_ADDR, DBCDBUS_irqBuffer.irq.dbusFlags, DBCDBUS_irqCbHandleDbusFlags);
        return;
    }
    DBCDBUS_irqCbFinishIrqHandling(obj, flags, eventResponse);
}

static void DBCDBUS_irqCbHandleDbusFlags(void *obj, uint32_t flags, const struct MCAL_EventResponse *eventResponse)
{
    if (0u != (DBC_DBUS_IR_RF0N_MASK & DBCDBUS_irqBuffer.irq.dbusFlags))
    {
        DBCDBUS_newMsgCount++;
        DBR_RunEventdrivenDbusTask(DBR_ED_TASK_DBPL_RX);
    }
    if (0u != (DBC_DBUS_IR_TEFN_MASK & DBCDBUS_irqBuffer.irq.dbusFlags))
    {
      #ifndef RTOS_DBUS_EVENTDRIVEN
        DBCDBUS_isMsgTxFinished = true;
      #endif
        DBR_SetEventflag(DBR_DLL_TX_DONE_FLAG);
        DBCDBUS_setMspiCallbackFunction(DBCDBUS_irqCbHandleDbusWakeFlag);
        DBCDBUS_readTxStatus();
        return;
    }
    DBCDBUS_irqCbHandleDbusWakeFlag(obj, flags, eventResponse);
}

static void DBCDBUS_irqCbHandleDbusWakeFlag(void *obj, uint32_t flags, const struct MCAL_EventResponse *eventResponse)
{
    enum _powerModeChangeState
    {
        POWER_MODE_GET_CURRENT,
        POWER_MODE_SET_NORMAL
    };
    static volatile enum _powerModeChangeState powerModeChangeState = POWER_MODE_GET_CURRENT;
    static volatile uint32_t regVal;
    uint32_t chipMode;

    if (0u != (DBC_DBUS_IR_WK_EVENT_MASK & DBCDBUS_irqBuffer.irq.dbusFlags))
    {
        switch(powerModeChangeState)
        {
            case POWER_MODE_GET_CURRENT:
                {
                    DBCDBUS_setMspiCallbackFunction(DBCDBUS_irqCbHandleDbusWakeFlag);
                    powerModeChangeState = POWER_MODE_SET_NORMAL;
                    if(DBC_OK == DBCDRV_readReg32Nbl(DBC_MOPC_ADDR))
                    {
                        return;
                    }
                }
                break;
            case POWER_MODE_SET_NORMAL:
                regVal = DBCDRV_getReadReg32Nbl();
                chipMode = (regVal & DBC_MOPC_MODE_SEL_MASK) >> DBC_MOPC_MODE_SEL_POS;
                if(chipMode == (uint32_t)DBC_POWER_MODE_NORMAL)
                {
                    break;  // NORMAL mode is already set
                }
                regVal &= ~DBC_MOPC_MODE_SEL_MASK;
                regVal |= (uint32_t)DBC_POWER_MODE_NORMAL << DBC_MOPC_MODE_SEL_POS;
                powerModeChangeState = POWER_MODE_GET_CURRENT;
                DBCDBUS_writeRegWithCallback(DBC_MOPC_ADDR, regVal, DBCDBUS_irqCbFinishIrqHandling);
                return;
            default: break;
        }
        powerModeChangeState = POWER_MODE_GET_CURRENT;
    }
    DBCDBUS_irqCbFinishIrqHandling(obj, flags, eventResponse);
}

static void DBCDBUS_irqCbFinishIrqHandling(void *obj, uint32_t flags, const struct MCAL_EventResponse *eventResponse)
{
    (void)*(uint8_t*)obj; (void)flags; (void)*eventResponse;
    (void)MSPI_disableEvent(&DBCDRV_mspiHandle, &DBCDBUS_mspiCallback, MCAL_EVENT_TRANSFER_COMPLETE);
    DBCDBUS_isSpiLock = false;
    DBCDBUS_isIrqHandlingFinished = true;
#ifdef APP_VARIANT
    DBCDBUS_handleIrqExt(DBCDBUS_irqBuffer);
#endif
}

#if defined (__CC_ARM)
    #pragma pop /* restore setting of anon_unions */
#endif
