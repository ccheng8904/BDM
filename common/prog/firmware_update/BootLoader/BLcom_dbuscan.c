/*******************************************************************************
 *  Copyright (c) 2022 BSH Hausgeraete GmbH,
 *  Carl-Wery-Str. 34, 81739 Munich, Germany, www.bsh-group.de
 *
 *  All rights reserved. This program and the accompanying materials
 *  are protected by international copyright laws.
 *  Please contact copyright holder for licensing information.
 *
 *******************************************************************************
 *  PROJECT          Generic SW / Boot-Loader
 ******************************************************************************/


/******************************************************************************/
/* DOCUMENTATION                                                              */
/******************************************************************************/
/** \file
 *  \ingroup  firmware_update
 *
 *  \brief    Implementation of Boot-Loader Communication
 *
 *  \details  This unit is responsible for the Boot-Loader communication via the DBusCAN chip.
 *            It provides interfaces for configuration, sending and receiving.
 */


/******************************************************************************/
/* INCLUDES                                                                   */
/******************************************************************************/
#include "bsh_stdinc.h"
#include <stdint.h>
#include "LibDefines.h"
#include "utility.h"
#include "BLcom.h"
#include "BLcomMcalMap.h"
#include "timer/timerlib.h"
#include "dbuscan_drv.h"
#include "dbuscan_bl.h"


/******************************************************************************/
/* PRIVATE DEFINITIONS                                                        */
/******************************************************************************/
#define BLCOM_BL_CMD_SIZE             (4u) ///< Byte size of BootLoader command frame (not including checksum byte)
#define BLCOM_TIMEOUT_US         (300000u) ///< Timeout for communication in µs


/******************************************************************************/
/* PRIVATE FUNCTION DECLARATIONS                                              */
/******************************************************************************/
static void BLCOM_updateChecksum(uint8_t data);
static void BLCOM_initTransmission(void);
static enum BLCOM_rxResult BLCOM_checkRxResult(uint8_t receivedCheckSum);
static bool BLCOM_isTimeOut(void);
static void BLCOM_setTimeOut(uint16_t timeOut);
static void BLCOM_reloadTimeOut(void);
static void BLCOM_handleTimer(void);
static bool BLCOM_isReceiveError(void);
static uint16_t BLCOM_getAvailableDataSize(void);
static void BLCOM_waitForTxFinished(uint32_t timeout);


/******************************************************************************/
/* PRIVATE DATA DEFINITIONS                                                   */
/******************************************************************************/
static bool     BLCOM_rxErrorFlag;        ///< holds a receive error
static bool     BLCOM_firstFrameReceived; ///< true after first frame received successfully
static uint8_t  BLCOM_checksum;           ///< holds the checksum for rx/tx
static uint16_t BLCOM_timer;              ///< the timer used for receive time out
static uint16_t BLCOM_reloadTime;         ///< this is used to reload inter-byte timer
static uint16_t BLCOM_timeStamp;          ///< holds last time stamp


/******************************************************************************/
/* PUBLIC FUNCTION DEFINITIONS                                                */
/******************************************************************************/
 /*lint -save -e9049 increment/decrement operation combined with other operation
  *                  with side-effects [MISRA 2012 Rule 13.3 advisory]
  */
/****************** BLCOM_receiveFrame ****************************************/
enum BLCOM_rxResult BLCOM_receiveFrame(uint8_t *buffer, uint16_t length, uint16_t timeout)
{
    uint16_t index = 0u;

    /* Set timeout for first time, but in case of invalid received command do not reset timeout.
        This will ensure jump from Bootloader to user app even in case when
        unexpected DBus traffic appears on the communication line.*/
    if ((BLCOM_rxErrorFlag == false) || (BLCOM_reloadTime == 0u)) { BLCOM_setTimeOut(timeout); }
    BLCOM_initTransmission();
    while (!TESTUB(BLCOM_isTimeOut)())
    {
        BLCOM_triggerWdg();
        uint16_t dataLenToRx = (length + 1u) - index; // length+1 because of checksum byte
        uint16_t rxDataLen = BLCOM_getAvailableDataSize();
        if ((rxDataLen == dataLenToRx)  ||   // Read data from chip if: - complete (rest of) frame was received or
            ((rxDataLen >= WORD_SIZE)   &&   // - received whole word but
            (length != BLCOM_BL_CMD_SIZE)))  // - wait if command frame is coming
        {
            if ((rxDataLen != dataLenToRx) && ((rxDataLen % WORD_SIZE) != 0u))
            {
                rxDataLen = (uint16_t)UTI_WORD_FLOOR((uint32_t)rxDataLen); // read only whole words received
            }
            const uint8_t *rxDataPtr = DBCBL_readData(rxDataLen);
            if (NULL == rxDataPtr)
            {
                return BLCOM_rxError;
            }
            for (uint16_t i = 0u; i < rxDataLen; i++)
            {
                if (index < length)
                {   /* last byte (checksum) not reached */
                    buffer[index++] = *rxDataPtr;
                    BLCOM_updateChecksum(*rxDataPtr);
                    rxDataPtr++;
                }
                else
                {   /* return whether rx was successful or not*/
                    return BLCOM_checkRxResult(*rxDataPtr);
                }
            }
        }
    }
    DBCBL_clearRxFifo(); // ensure Rx FIFO is empty before receiving new data
    return BLCOM_rxTimeOut;
}

/****************** BLCOM_sendByte ********************************************/
void BLCOM_sendByte(uint8_t data)
{
    (void)DBCBL_sendData(&data, (uint16_t)sizeof(data));
    BLCOM_waitForTxFinished(BLCOM_TIMEOUT_US);
    DBCBL_readTxStatus();
}

/****************** BLCOM_sendBuffer ******************************************/
void BLCOM_sendBuffer(const uint8_t *buffer, uint16_t length)
{
    BLCOM_initTransmission();
    (void)DBCBL_sendData(buffer, length);
    BLCOM_waitForTxFinished(BLCOM_TIMEOUT_US);
    DBCBL_readTxStatus();
    while (length-- > 0u)
    {
        BLCOM_updateChecksum(*buffer++);
    }
    BLCOM_sendByte(BLCOM_checksum);
}

/****************** BLCOM_sendAck *********************************************/
void BLCOM_sendAck(uint8_t ack)
{
    if (BLCOM_firstFrameReceived == true)
    {
        BLCOM_sendByte(ack);
    }
}

/****************** BLCOM_setBaudrate *****************************************/
void BLCOM_setBaudrate(uint16_t baudrate)
{
    (void)DBCDRV_setDbusBaudrate(baudrate);
}

/****************** BLCOM_init ************************************************/
void BLCOM_init(void)
{
  #ifndef VARIANT_RAMLOADER
    struct MSUP_TimeoutHandle timeoutHandle;
    (void)_TIM_bGetInitConfig();
    MSUP_setTimeOutExt(&timeoutHandle, BLCOM_TIMEOUT_US);
    while ((DBC_ERROR == DBCDRV_init(NULL)) && !MSUP_isTimeOutExt(&timeoutHandle)) {}
    BLCOM_firstFrameReceived = false;
  #else
    // frame already received in ROM-Loader
    BLCOM_firstFrameReceived = true;
    (void)DBCDRV_initComChannels(NULL);
  #endif
    BLCOM_initWdgCfgStruct();
    BLCOM_reloadTime = 0u;
}

/******************************************************************************/
/* PRIVATE FUNCTION DEFINITIONS                                               */
/******************************************************************************/

/** Updates the checksum for transmission
 *
 *  \param data: data byte to update
 */
static void BLCOM_updateChecksum(uint8_t data)
{
    BLCOM_checksum += data;
    if (BLCOM_checksum < data)
    {
        BLCOM_checksum++;
    }
}

/** Initialize a transmission
 *
 *  This interface resets checksum and error flag
 */
static void BLCOM_initTransmission(void)
{
    BLCOM_checksum = 0u;
    BLCOM_rxErrorFlag = false;
}

/** returns the result of finished reception */
static enum BLCOM_rxResult BLCOM_checkRxResult(uint8_t receivedCheckSum)
{
    if (BLCOM_isReceiveError() || (BLCOM_checksum != receivedCheckSum))
    {
        DBCBL_clearRxFifo(); // ensure clearing (corrupted data from) Rx FIFO if it's not read already
        BLCOM_rxErrorFlag = true;
    }
    if (true == BLCOM_rxErrorFlag)
    {
        return BLCOM_rxError;
    }
    else
    {
        if (true == BLCOM_firstFrameReceived)
        {
            BLCOM_reloadTimeOut();
        }
        BLCOM_firstFrameReceived = true;
        return BLCOM_rxSuccess;
    }
}

/** \brief returns true if a time-out happened
 *
 *  \return true if timer is down, false else
 */
static bool BLCOM_isTimeOut(void)
{
    BLCOM_handleTimer();
    return ((BLCOM_timer == 0u) ? true : false);
}

/** \brief set the value for time-out
 *
 *  \param time time-out in milliseconds
 */
static void BLCOM_setTimeOut(uint16_t timeOut)
{
    BLCOM_timer      = timeOut;
    BLCOM_reloadTime = timeOut;
    BLCOM_timeStamp  = TIM_uiGetCircleMicroSeconds();
}

/** \brief set the value for time-out
 *
 *  \param time time-out in milliseconds
 */
static void BLCOM_reloadTimeOut(void)
{
    BLCOM_timer = BLCOM_reloadTime;
}

/** \brief handles the timer */
static void BLCOM_handleTimer(void)
{
    const uint16_t BLCOM_milliSecond = 1000u; // 1ms in µs ticks
    if (BLCOM_timer > 0u)
    {
        if ((uint16_t)(TIM_uiGetCircleMicroSeconds() - BLCOM_timeStamp) >= BLCOM_milliSecond)
        {
            BLCOM_timer--;
            BLCOM_timeStamp += BLCOM_milliSecond;
        }
    }
}

static bool BLCOM_isReceiveError(void)
{
    return DBCBL_clearIrq();
}

static uint16_t BLCOM_getAvailableDataSize(void)
{
    if (DBCDRV_isIrqEvent())
    {
        return DBCBL_getRxDataSize();
    }
    return 0u;
}

static void BLCOM_waitForTxFinished(uint32_t timeout)
{
    struct MSUP_TimeoutHandle timeoutHandle;
    MSUP_setTimeOutExt(&timeoutHandle, timeout);
    while (DBCBL_isTxOngoing() && !MSUP_isTimeOutExt(&timeoutHandle))
    {
        BLCOM_triggerWdg();
    }
}

/*lint -restore -e9049 */
