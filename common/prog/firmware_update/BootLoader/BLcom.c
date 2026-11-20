/*******************************************************************************
 *  Copyright (c) 2017 BSH Hausgeraete GmbH,
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
 *  \details  This unit is responsible for the UART communication.
 *            It provides interfaces for configure, sending and receiving
 */


/******************************************************************************/
/* INCLUDES                                                                   */
/******************************************************************************/
#include "bsh_stdinc.h"
#include <stdint.h>
#include "BLcom.h"

#ifdef MCAL_INCLUDED
  #include "BLcomMcalMap.h"
#else
  #include "BLcomMap.h"
#endif

#include "hwdt.h"
#ifdef EFR32BG21
    #include "hsup.h"
#endif
#include "timer/timerlib.h"

/******************************************************************************/
/* PRIVATE FUNCTION DECLARATIONS                                              */
/******************************************************************************/
static uint8_t BLCOM_receiveData(void);
static void    BLCOM_updateChecksum(uint8_t data);
static void    BLCOM_initTransmission(void);
static enum    BLCOM_rxResult BLCOM_checkRxResult(uint8_t receivedCheckSum);
static bool    BLCOM_isTimeOut(void);
static void    BLCOM_setTimeOut(uint16_t timeOut);
static void    BLCOM_reloadTimeOut(void);
static void    BLCOM_handleTimer(void);



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
/* PUBLIC DATA DEFINITIONS                                                    */
/******************************************************************************/
#ifdef MCAL_INCLUDED
const struct MUART_Config MuartHandleConfig =
{
    MUART_DATA_BITS_8,
    MUART_PARITY_EVEN,
    MUART_STOP_BITS_1,
    (DBUS_DEFAULT_BAUDRATE*100uL)
};
#endif


/******************************************************************************/
/* PUBLIC FUNCTION DEFINITIONS                                                */
/******************************************************************************/
 /*lint -save -e9049 increment/decrement operation combined with other operation
  *                  with side-effects [MISRA 2012 Rule 13.3 advisory]
  */
/****************** BLCOM_receiveFrame ****************************************/
enum BLCOM_rxResult BLCOM_receiveFrame(uint8_t *buffer, uint16_t length, uint16_t timeout)
{
    uint16_t index  = 0u;
    /* Set timeout for first time, but in case of invalid received command do not reset timeout.
        This will ensure jump from Bootloader to user app even in case when
        unexpected DBus traffic appears on the communication line.*/
    if ((BLCOM_rxErrorFlag == false) || (BLCOM_reloadTime == 0u)) { BLCOM_setTimeOut(timeout);}
    BLCOM_initTransmission();
    while (!TESTUB(BLCOM_isTimeOut)())
    {
        BLCOM_triggerWdg();
        if (BLCOM_isDataAvailable())
        {   /* next data byte available */
            uint8_t rxData = BLCOM_receiveData();
            if (index < length)
            {   /* last byte (checksum) not reached */
                buffer[index++] = rxData;
                BLCOM_updateChecksum(rxData);
            }
            else
            {   /* return whether rx was successful or not*/
                return BLCOM_checkRxResult(rxData);
            }
        }
    }
    return BLCOM_rxTimeOut;
}


/****************** BLCOM_sendByte ********************************************/
void BLCOM_sendByte(uint8_t data)
{
#if (defined(MSPM0_SERIES_G) && (CORE_CLOCK==80U))
    HSUP_vDelay(10); // for time critical reason this delay is required for TI MSPM0 platform
#elif defined(MSPM0_SERIES_G) && (CORE_CLOCK!=80U) && (CORE_CLOCK!=32U)
    #pragma error "HSUP_vDelay adjustment might be necessary"
#endif
    BLCOM_setTxData(data);
    while (BLCOM_isTxOnGoing())
    {
        BLCOM_triggerWdg();
    }
#ifndef BLCOM_RxNotDelayed
/* Hint, if it is for sure, that TX event happens before RX event, this is not required 
 * It is important, that the sent byte will be received and ignored, but it is not guaranteed,
 * that the the receive is finished directly after transmit finished.
 * To be sure the following code is switched on by default.
 */
    {
        static const uint16_t BLCOM_rxTmo = 3000u; ///< Receive Time-out in us
        uint16_t timeStamp = TIM_uiGetCircleMicroSeconds();
        while (!BLCOM_isDataAvailable() &&
              ( (uint16_t)(TIM_uiGetCircleMicroSeconds() - timeStamp ) < BLCOM_rxTmo ))
        {BLCOM_triggerWdg();}
    }
#endif
    (void)BLCOM_receiveData(); // read back transmitted byte
#ifdef EFR32BG21
    HSUP_vDelay(5); // for some reason this delay required for EFR platform
#endif
}


/****************** BLCOM_sendBuffer ******************************************/
void BLCOM_sendBuffer(const uint8_t *buffer, uint16_t length)
{
    BLCOM_initTransmission();
    while (length-- > 0u)
    {
        BLCOM_sendByte(*buffer);
        BLCOM_updateChecksum(*buffer++);
    }
    BLCOM_sendByte(BLCOM_checksum);
}


void BLCOM_sendAck(uint8_t ack)
{
    if (BLCOM_firstFrameReceived)
    {
        BLCOM_sendByte(ack);
    }
}

/****************** BLCOM_setBaudrate *****************************************/
void BLCOM_setBaudrate(uint16_t baudrate)
{
    BLCOM_Configure(baudrate);
}

void BLCOM_init(void)
{
  #ifndef VARIANT_RAMLOADER
    BLCOM_initUart();
    BLCOM_enableUart();
    (void)_TIM_bGetInitConfig();
    BLCOM_firstFrameReceived = false;
  #else
    // frame already received in ROM-Loader
    BLCOM_firstFrameReceived = true;
  #endif
  #ifdef MCAL_INCLUDED
    BLCOM_initWdgCfgStruct();
  #endif
    BLCOM_reloadTime = 0u;
}


/******************************************************************************/
/* PRIVATE FUNCTION DEFINITIONS                                               */
/******************************************************************************/

/** \brief returns received data
 *
 *  This interface returns the latest received data byte
 *  It also checks possible receive errors and sets the error flag
 *  \sa BLCOM_rxErrorFlag when any receive error happened.
 *
 *  \return latest received data byte
 */
static uint8_t BLCOM_receiveData(void)
{
    if (BLCOM_isReceiveError())
    {
        BLCOM_rxErrorFlag = true;
    }
    else
    {   // successful receive
        if (BLCOM_firstFrameReceived){BLCOM_reloadTimeOut();}
    }
    return BLCOM_getRxData();
}


/** Updates the checksum for transmission 
 *
 *  \param data: data byte to update
 */
static void BLCOM_updateChecksum(uint8_t data)
{
    BLCOM_checksum += data;
    if (BLCOM_checksum < data){BLCOM_checksum++;}
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
    if (BLCOM_checksum != receivedCheckSum)
    {
        BLCOM_rxErrorFlag = true;
    }
    if (true == BLCOM_rxErrorFlag)
    {
        return BLCOM_rxError;
    }
    else
    {
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
    const uint16_t BLCOM_milliSecond = 1000u; // 1ms in us ticks
    if (BLCOM_timer > 0u)
    {
        if ( (uint16_t)(TIM_uiGetCircleMicroSeconds() - BLCOM_timeStamp ) >= BLCOM_milliSecond )
        {
            BLCOM_timer--;
            BLCOM_timeStamp += BLCOM_milliSecond;
        }
    }
}
/*lint -restore -e9049 */
