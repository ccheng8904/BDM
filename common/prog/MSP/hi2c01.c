/*******************************************************************************
 *  Copyright (c) 2019 BSH Hausgeraete GmbH,
 *  Carl-Wery-Str. 34, 81739 Munich, Germany, www.bsh-group.de
 *
 *  All rights reserved. This program and the accompanying materials
 *  are protected by international copyright laws.
 *  Please contact copyright holder for licensing information.
 *
 *******************************************************************************
 *  PROJECT          HAL PED Framework
 *  COMP_ABBREV      HI2C
 ******************************************************************************/


/******************************************************************************/
/* DOCUMENTATION                                                              */
/******************************************************************************/
/** \file
 *
 *  \brief    This module contains the implementation of HAL I2C package 01
 *
 */


/******************************************************************************/
/* INCLUDES                                                                   */
/******************************************************************************/
#include "bsh_stdinc.h"
#include <stdint.h>
#include <stdbool.h>
#include "processor.h"
#include "hdio.h"
#include "hi2c.h"
#include "hi2c01xs.h"
#include "hwdt.h"           /* Include watchdog HAL */


/******************************************************************************/
/* VARIABLES                                                                  */
/******************************************************************************/
uint8_t HI2C0_ucError;
static uint8_t HI2C0_ucLastRx = 0;
bool HI2C0_bEventEnabled;

/******************************************************************************/
/* FUNCTIONS                                                                  */
/******************************************************************************/
static void HI2C0_vWaitForSlave(void);
static void HI2C0_vMakeStopCondition(void);

/******************************************************************************/
/* CONSTANTS                                                                  */
/******************************************************************************/
#define SDA_TEST_COUNT    10u

/******************************************************************************/
/* DEFINITIONS                                                                */
/******************************************************************************/
/* HI2C0_vWaitForSlave() */
/* set SCL to a high state and wait until the slave releases it too */
static void HI2C0_vWaitForSlave(void)
{
   uint8_t ucWait = HI2C0_ucMaxWaitState;

   HI2C0_vInputSCL();
   HI2C0_vSetSCL();

   while (!HI2C0_bGetSCL())
   {
      ucWait--;

      if (ucWait == 0u)
      {
         /* SCL stuck? ... */
         HI2C0_ucError |= HI2C_TIMEOUT;
         return;
      }
   }
   HI2C0_vOutputSCL();
   HI2C0_vBitDelayH();

}

/* HI2C0_vMakeStopCondition() */
static void HI2C0_vMakeStopCondition(void)
{
   /* generate stop condition */
   HI2C0_vOutputSDA();
   HI2C0_vClrSDA();
   HI2C0_vBitDelayL();
   HI2C0_vWaitForSlave();
   HI2C0_vSetSDA();
   HI2C0_vBitDelayH();

   /* release bus */
   HI2C0_vInputSCL();
   HI2C0_vInputSDA();
}

/* HI2C0_vInit() */
/*lint -save -efunc(715,HI2C0_vInit)    disable lint args not used informational*/
void HI2C0_vInit(uint8_t ucIndex)
{
#ifdef HI2C_SPECIAL_INIT 
   HI2C0_vInitPort(); /* Initialize the special features of the periphery. */

#endif
   /* preset for port latch */
   HI2C0_vInputSCL();
   HI2C0_vInputSDA();
}

/* HI2C0_bSetAddr() */
bool HI2C0_bSetAddr(uint8_t ucAddress)
{
   /* no errors so far */
   HI2C0_ucError = 0u;

   HI2C0_vSetSDA();
   HI2C0_vSetSCL();

   /* generate start condition */
   HI2C0_vOutputSDA();
   HI2C0_vOutputSCL();
   HI2C0_vBitDelayH();
   HI2C0_vClrSDA()   ;
   HI2C0_vBitDelayH();
   HI2C0_vClrSCL()   ;
   HI2C0_vBitDelayL();

   /* transmit address */
   return HI2C0_bSetTxData(ucAddress, false);
}

/* HI2C0_bSetTxData() */
bool HI2C0_bSetTxData(uint8_t ucDataByte, bool bStop)
{
   uint8_t ucCounter;

   /* transmit all 8 data bits */
   ucCounter = 8u;
   do
   {
      /* send each bit, MSB first */

      if ((ucDataByte & 0x80u) != 0u)
      {
         HI2C0_vSetSDA();
      }
      else
      {
         HI2C0_vClrSDA();
      }
      ucDataByte = (uint8_t)(ucDataByte << 1u);

      HI2C0_DataSetupTimeDelay();
      /* generate clock */
      HI2C0_vSetSCL();
      HI2C0_vBitDelayH();
      HI2C0_vClrSCL();
      HI2C0_vBitDelayL();
      --ucCounter;
   }
   while (ucCounter > 0u);

   /* listen for ACK */
   HI2C0_vSetSDA();
   HI2C0_vInputSDA();
   HI2C0_vWaitForSlave();

   if (HI2C0_bGetSDA())
   {
      /* ack didn't happen, may be nothing out there */
      HI2C0_ucError |= HI2C_NACK;
   }
   HI2C0_vClrSCL();
   HI2C0_vBitDelayL();
   HI2C0_vSetSDA();
   HI2C0_vOutputSDA();

   if (bStop || (HI2C0_ucError != 0u))
   {
      /* generate stop condition */
      HI2C0_vMakeStopCondition();
   }
   /* call event handler */

   if (HI2C0_bEventEnabled)
   {
      HI2C0_vHandleEvent();
   }
   return ((HI2C0_ucError == 0u) ? true : false);

}


uint8_t HI2C0_ucGetRxData(void)
{
    return HI2C0_ucLastRx;
}

/* HI2C0_vTriggerReceive() */
void HI2C0_vTriggerReceive(bool bStop)
{
   uint8_t ucCounter;

   /* switch to input since we want to receive data */
   HI2C0_vInputSDA();

   /* receive the bits -- starting with the MSB */
   ucCounter = 8u;

   do
   {
      HI2C0_vSetSCL();
      HI2C0_vBitDelayH();

      HI2C0_ucLastRx <<= 1u;

      if (HI2C0_bGetSDA())
      {
         HI2C0_ucLastRx |= 1u;
      }
      HI2C0_vClrSCL();
      HI2C0_vBitDelayL();
      --ucCounter;

   }
   while (ucCounter > 0u);

   /* send ACK according to the stop flag */
   HI2C0_vOutputSDA();

   if (bStop)
   {
      /* no acknowledge */
      HI2C0_vSetSDA();
   }
   else
   {
      /* acknowledge */
      HI2C0_vClrSDA();
   }
   HI2C0_vWaitForSlave();
   HI2C0_vClrSCL();
   HI2C0_vSetSDA();
   HI2C0_vBitDelayL();

   if (bStop)
   {
      HI2C0_vMakeStopCondition();
   }
   /* call event handler */

   if (HI2C0_bEventEnabled)
   {
      HI2C0_vHandleEvent();
   }
}

/* HI2C_vSendStop() */
void HI2C0_vSendStop(void)
{
   /* Send stop-condition.  Changing of SDA during SCL = OUTPUT_HIGH is */
   /* only allowed while sending start- or stop-condition.  For generating */
   /* stop-condition both SDA and SCL must be LOW */
   HI2C0_vClrSCL();
   HI2C0_vClrSDA();
   HI2C0_vOutputSCL();
   HI2C0_vOutputSDA();

   HI2C0_vBitDelayL();              /* clock low period */
   HI2C0_vSetSCL();              /* change SCL edge to OUTPUT_HIGH level */
   HI2C0_vBitDelayH();              /* Stop condition setup time */
   HI2C0_vSetSDA();;             /* change SDA edge to OUTPUT_HIGH level */

   HI2C0_vInputSCL();
   HI2C0_vInputSDA();
}

/* HI2C0_bForceBusRelease() */
bool HI2C0_bForceBusRelease(void)
{
   uint16_t uiCounter;
   bool bSdaStatus;

   HI2C0_vOutputSCL();
   HI2C0_vInputSDA();

   for (uiCounter = 0u; uiCounter < 15u; uiCounter++)
   {
      HI2C0_vClrSCL();          /* security: send some clocks to free the bus-lines */
      HI2C0_vBitDelayL();       /* clock low period */
      HI2C0_vSetSCL();
      HI2C0_vBitDelayH();
   }
   HI2C0_vSendStop();

   /* check BUS-lines */
   for (uiCounter = 0u; uiCounter < SDA_TEST_COUNT; uiCounter++)
   {
      bSdaStatus = HI2C0_bGetSDA();

      if ( bSdaStatus )
      {
         break;
      }
   }
   return (bSdaStatus);
}

/* HI2C0_vEnableEvent() */
void HI2C0_vEnableEvent(void)
{ 
    HI2C0_bEventEnabled = true;
}

/* HI2C0_vDisableEvent() */
void HI2C0_vDisableEvent(void)
{ 
    HI2C0_bEventEnabled = false;
}
