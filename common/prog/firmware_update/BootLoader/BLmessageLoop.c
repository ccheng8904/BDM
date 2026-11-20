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
 *
 *  \ingroup  firmware_update
 *
 *  \brief    Implementation Boot-Loader Message Loop.
 *
 *  \details  Handles the message loop for Boot-Loader and RAM-Loader
 *            and calls the according service functions.
 */


/******************************************************************************/
/* INCLUDES                                                                   */
/******************************************************************************/
#include "bsh_stdinc.h"
#include <stdint.h>
#include <stdbool.h>
#include "BLmessageLoop.h"
#include "BLcom.h"
#include "BLcommands.h"
#include "BLservices.h"
#if defined(VARIANT_ROMLOADER) || defined(VARIANT_LROMLOADER)
  #ifdef MCAL_INCLUDED
    #include "BLcomMcalMap.h"
  #else
    #include "BLcomMap.h"
  #endif
#endif

/******************************************************************************/
/* PRIVATE DEFINITIONS                                                        */
/******************************************************************************/
/** message buffer size for ROM-Loader (space for a single frame) */
#define BLMLP_FRAME_LENGTH  4u


/******************************************************************************/
/* PRIVATE FUNCTION DECLARATIONS                                              */
/******************************************************************************/
static void BLMLP_init(void);


/******************************************************************************/
/* PRIVATE DATA DEFINITIONS                                                   */
/******************************************************************************/
/*lint -esym(9003,BLMLP_*) could define variable at block scope
  *                 [MISRA 2012 Rule 8.9, advisory]
  */
static const uint8_t  BLMLP_frameLength = BLMLP_FRAME_LENGTH;
#ifdef VARIANT_ROMLOADER
    static const uint16_t BLMLP_msgTimeout  = 500u;  // 500ms message time-out
#endif
static uint8_t  BLMLP_msgBuffer[BLMLP_FRAME_LENGTH]; // message buffer


/******************************************************************************/
/* PUBLIC FUNCTION DEFINITIONS                                                */
/******************************************************************************/
void BLMLP_handleMessageLoop(uint16_t bootTimeOut)
{
    uint16_t timeOut = bootTimeOut;

    BLMLP_init();
  #ifndef VARIANT_RAMLOADER
    BLCOM_sendByte(BL_HAND_SHAKE);
  #endif
    for(;;)
    {
        enum BLCOM_rxResult rxResult;
        
        rxResult = BLCOM_receiveFrame(BLMLP_msgBuffer, BLMLP_frameLength, timeOut);
        if (rxResult == BLCOM_rxSuccess)
        {
            rxResult = BLSRV_doCommandService(BLMLP_msgBuffer);
            #ifdef VARIANT_ROMLOADER
            timeOut  = BLMLP_msgTimeout;
            #endif
        }
        if (rxResult == BLCOM_rxError)
        {
            BLCOM_sendAck(BL_TRANSMISSION_ERROR);
        }
        if (rxResult == BLCOM_rxTimeOut)
        {
            BLCOM_sendAck(BL_RECEIVER_TIMEOUT);
          #if defined(VARIANT_ROMLOADER) || defined(VARIANT_LROMLOADER)
            BLCOM_generateReset(); // leave ROM-Loader and enter application
          #endif
        }
      #ifdef UNIT_UNDER_TEST
        BLMLP_handleMessageLoop_timeOut = timeOut;
        return; // no endless loop for unit test
      #endif
    }
}



/******************************************************************************/
/* PRIVATE FUNCTION DEFINITIONS                                               */
/******************************************************************************/
static void BLMLP_init(void)
{
    BLCOM_init();
    BLSRV_init();
}

