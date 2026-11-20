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

#ifndef HI2C_H
#define HI2C_H

#ifdef __cplusplus
extern "C" {
#endif

/******************************************************************************/
/* DOCUMENTATION                                                              */
/******************************************************************************/
/** \file
 *  \brief    This file contains the exported definitions and interfaces for the implementation of the HAL I2C.
 *

\section CCTifc1 Interface Package 1
<TABLE>
<TR><TD><b>Interface</TD>  <TD>Description</b></TD></TR>
<TR><TD>HI2C\<unit\>_vInit(uint8_t index)</TD><TD>Initializes the operation of the HI2C.</TD></TR>
<TR><TD>HI2C\<unit\>_bSetAddr(uint address)</TD><TD>Sets the address of the slave to communicate to.</TD></TR>
<TR><TD>HI2C\<unit\>_bSetTxData(uint8_t dataByte, bool stop)</TD><TD>Sends a single data byte over the I2C bus.</TD></TR>
<TR><TD>HI2C\<unit\>_ucGetRxData(void)</TD><TD>Returns the most recent received data byte from the addressed slave.</TD></TR>
<TR><TD>HI2C\<unit\>_vTriggerReceive(bool stop)</TD><TD>Trigger a reception.</TD></TR>
<TR><TD>HI2C\<unit\>_vHandleEvent(void)</TD><TD>A handler called, if the transmission/reception process has been finished.</TD></TR>
<TR><TD>HI2C\<unit\>_vEnableEvent(void)</TD><TD>Enables the call of the event handler.</TD></TR>
<TR><TD>HI2C\<unit\>_vDisableEvent(void)</TD><TD>Disables the call of the event handler.</TD></TR>
<TR><TD>HI2C<unit>_ucGetError(void)</TD><TD>Returns the collected error information.</TD></TR>
</TABLE>
\n
\n
\n
The following picture visualises the Interface of HI2C:\n
\n
\image html hi2c.png "Picture 1: Overview HAL I2C"


================== Example of HI2C library usage ==================
How to set/use HI2C library to communicate with I2C slave devices:

1. Choose two pins that will be used by I2C library and adjust functions in
    file hi2c01xs.c to correspond with chosen pins.
    Optionally you can enable internal pull-up resistors for I2C line in HI2C0_vInitPort() in file hi2c01xs.c

2. Include header files hi2c.h and hi2c01xs.h into related project files.

3. to initialize I2C interface, following functions have to be called:
    HI2C0_vInit(0);                 - initializes GPIO pins
    HI2C0_vEnableEvent();           - enables handler, that is called after successful transfer

4. example of transmit to I2C slave device:
    ucstatus = HI2C0_bSetAddr(<slave_address> | HI2C_WRITE);       - addressing I2C slave
    ucstatus = HI2C0_bSetTxData(0x07, true);                            - transmit data into I2C slave
    ucstatus = HI2C0_ucGetError();                                      - get status of transmission

5. example of reading data from I2C slave device:
    ucstatus = HI2C0_bSetAddr(<slave_address> | HI2C_READ);        - addressing I2C device
    HI2C0_vTriggerReceive(false);                                       - receive data from I2C device
    uidata = HI2C0_ucGetRxData() <<8;                                   - save data from I2C read buffer(upper byte)
    HI2C0_vTriggerReceive(true);                                        - receive data from I2C device
    uidata |= HI2C0_ucGetRxData();                                      - save data from I2C read buffer(lower byte)
=========================  Example end ===========================
*/

/******************************************************************************/
/* INCLUDES                                                                   */
/******************************************************************************/
#include <stdint.h>
#include <stdbool.h>
#include "bsh_stdinc.h"
#include "processor.h"

/******************************************************************************/
/* PUBLIC DEFINITIONS                                                         */
/******************************************************************************/
/* Definitions for HI2C<unit>_ucGetError */
/**
\b description: \n
 This is the define that represents the error indication of a message not acknowledged by the slave.
 The error is  returned by HI2C<unit>_ucGetError().
*/
#define HI2C_NACK    (1u)

/**
\b description: \n
 This is the define that represents the error indication of a message timeout by the slave.
 The error is  returned by HI2C<unit>_ucGetError().
*/
#define HI2C_TIMEOUT (2u)

/**
\b description: \n
 flags used when addressing I2C slave device
*/
#define HI2C_READ      1u
#define HI2C_WRITE     0u

/******************************************************************************/
/* TYPE DEFINITIONS                                                           */
/******************************************************************************/

/******************************************************************************/
/* DECLARATIONS                                                               */
/******************************************************************************/
/**

\b \<unit\>: 0 \n

\b description: \n
 This is the external reference to the maximum waitstate delay. This delay controls
 the number of cycles the master shall wait for a slave acknowledge.

<B>HC08 implementation:</B>
 cycles on the HC08 implementation is a loop with takes at least 20 processor cycles
*/
extern const uint8_t HI2C0_ucMaxWaitState;

/* private data */
extern uint8_t HI2C0_ucError;
extern bool HI2C0_bEventEnabled;


/******************************************************************************/
/* GLOBAL FUNCTIONS                                                           */
/******************************************************************************/

/* HI2C<unit>_vInit*/
/**
 * \brief    Initializes the operation of the HI2C.
 *
 *           This function initializes the operation of the HI2C<unit>. 
 *           It uses the respective configuration parameters defined in HI2C<unit>_Config. 
 *           The call of the handler function HI2C<unit>_vHandleEvent is disabled 
 *           after initialization. After initialization the HI2C<unit> is ready to accept 
 *           further requests.
 *
 * \param    ucIndex: Indexes the configuration set to be used for initialization.
 *
 * \return   none.
 *
 * \note     This function is not reentrant. A configuration HI2C<unit>_Config is not 
 *           required for initialization.
 */
extern void HI2C0_vInit(uint8_t ucIndex);

/* HI2C<unit>_bSetAddr*/
/**
 * \brief    Sets the address of the slave to communicate to.
 *
 *           This interface sets the address of the slave to communicate to. The read/write information is
 *           contained in the address information. After the call of this interface the transmission of the
 *           address starts. Depending on the read/write bit of the address further calls of HI2C<unit>_bSetTxData
 *           or HI2C<unit>_ucGetRxData are possible until the stop information is passed to the interface.
 *           HI2CPackage1:
 *             This function completes the transmission and returns if
 *             the address is sent.
 *           HI2CPackage2:
 *             Once the transmission is completed, the respective handler
 *             HI2C<unit>_vHandleEvent is called.
 *
 * \param    uiAddress: This parameter is the address (including direction) to communicate to.
 *
 * \return   true  - if the setting of the address was successful.
 *           false - otherwise.
 *
 * \note     HI2CPackage1:
 *             The interface returns false, if the partner that shall be addressed is
 *             not responding (no acknowledge) or the partner inserts too many
 *             waitstates (by holding SCL low).
 *             If the event handler is enabled, the event handler is immediately
 *             called by this function once the reception is completed.
 *           HI2CPackage2: 
 *             If the bus is currently busy due to another transmission/reception, this
 *             interface returns false.
 */
extern bool HI2C0_bSetAddr(uint8_t ucAddress);

/* HI2C<unit>_bSetTxData*/
/**
 * \brief    Sends a single data byte over the I2C bus.
 *
 *           This interface sends a single data byte over the I2C bus. After the call of this
 *           interface the transmission process starts.
 *           HI2CPackage1:
 *             This function completes the transmission and returns, if the date has been
 *             sent. If the event handler is enabled the event handler is called immediately
 *             once the reception is completed.
 *           HI2CPackage2:
 *             Once the transmission has been finished the respective handler HI2C<unit>_vHandleEvent
 *             is be called.
 *
 * \param    ucDataByte: This parameter contains the data byte to be transmitted.
 *           bStop: This parameter defines if the stop condition has to be
 *                  transmitted after the data byte.
 *
 * \return   true  - if the setting of the data was successful
 *           false - otherwise.
 *
 * \note     HI2CPackage1:
 *             The interface returns false if the partner is not responding (no
 *             acknowledge) or the partner inserts too many waitstates (by holding SCL low).
 *           HI2CPackage2:
 *             This interface shall return false if any previous transmissions
 *             (addressing) has been failed or a previous transmissions (addressing) is
 *             still in progress.\n
 */
extern bool HI2C0_bSetTxData(uint8_t ucDataByte, bool bStop);

/* HI2C<unit>_ucGetRxData*/
/**
 * \brief    Returns the most recent received data byte.
 *
 *           This interface returns the most recent received data byte from the addressed
 *           slave.
 *
 * \param    none
 *
 * \return   received data byte.
 *
 * \note     HI2CPackage1:
 *             The interface returns false if the partner is not responding (no
 *             acknowledge) or the partner inserts too many waitstates (by holding SCL low).
 *           HI2CPackage2:
 *             This interface shall return false if any previous transmissions
 *             (addressing) has been failed or a previous transmissions (addressing) is
 *             still in progress.\n
 */
extern uint8_t HI2C0_ucGetRxData(void);

/* HI2C<unit>_vTriggerReceive*/
/**
 * \brief    Triggers a reception.
 *
 *           HI2CPackage1:
 *             The interface triggers a reception and saves the received data byte once
 *             the reception has been finished. The stop information passed via the interface
 *             is used to generate the stop condition if requested.
 *             If the event handler is enabled the event handler will be immediately called by
 *             this function once the reception is completed.
 *           HI2CPackage2:
 *             This interface triggers the reception of the next data byte. The stop
 *             information passed via the interface shall is used to generate the stop
 *             condition if requested.
 *
 * \param    bStop: This information is used to signal the slave whether the received byte
 *                  shall be the last one.
 *                  - true, generate STOP condition
 *                  - false, generate no STOP condition
 *
 * \return   none
 */
extern void HI2C0_vTriggerReceive(bool bStop);

/* HI2C<unit>_vHandleEvent*/
/**
 * \brief    A handler called, if the transmission/reception process to/from the slave has been finished.
 *
 *           This handler is called, if the transmission/reception process to/from the slave
 *           has been finished or upon occurrence of an error condition.
 *           HI2CPackage1:
 *             The implementation calls this function on the same interrupt level as the
 *             caller of the HI2C<unit>_vTriggerReceive or HI2C<unit>_bSetTxData function.
 *
 * \param    none
 *
 * \return   none
 *
 * \note     This function must be implemented by the user.
 */
extern void HI2C0_vHandleEvent(void);

/* HI2C<unit>_vEnableEvent*/
/**
 * \brief    Enables the call of the event handler.
 *
 *           This function enables the call of the event handler HI2C<unit>_vHandleEvent.
 *
 * \param    none
 *
 * \return   none
 *
 * \note     This function must be implemented by the user.
 */
extern void HI2C0_vEnableEvent(void);

/* HI2C<unit>_vDisableEvent*/
/**
 * \brief    Disables the call of the event handler.
 *
 *           This function disables the call of the event handler HI2C<unit>_vHandleEvent.
 *
 * \param    none
 *
 * \return   none
 */
extern void HI2C0_vDisableEvent(void);

/* HI2C<unit>_ucGetError*/
/**
 * \brief    Returns the collected error information.
 *
 *           This interface returns the collected error information. The error information
 *           is cleared on read. In case an error is detected that has terminated the
 *           communication, the handler HI2C<unit>_vHandleEvent is be called.
 *
 * \param    none
 *
 * \return   HI2C_NACK: This information is set whenever the acknowledge of the addressed partner was
 *                      not detected.
 *           HI2C_TIMEOUT: This information is set whenever the partner has inserted to many waitstates
 *                         (stuck SCL).
 */
static inline uint8_t HI2C0_ucGetError(void)  {return HI2C0_ucError;}

/* HI2C<unit>_vSendStop*/
/**
 * \brief    Generates a stop condition on the I2C bus.
 *
 *           This interface generates a stop condition on the I2C bus.
 *
 * \param    none
 *
 * \return   none
 */
extern void HI2C0_vSendStop(void);

/* HI2C<unit>_bForceBusRelease*/
/**
 * \brief    Generates clock pulses and a stop condition to force a slave to release the bus.
 *
 *           This interface generates clock pulses and a stop condition to force a slave to release the
 *           bus.
 *
 * \param    none
 *
 * \return   true: bus release was successful
 *           false: bus release has been failed
 */
extern bool HI2C0_bForceBusRelease(void);

/* HI2C<unit>_vBitDelayH*/
/**
 * \brief    Performs the delay for H level of signal.
 *
 *           User defined function that perform the delay for high phase of SCL
 *           (5 탎ec for 100 kHz mode or 1.3 탎ec for 400 kHz mode)
 *           To achieve the best performance the call/return overhead can be taken
 *           into account.
 *
 * \param    none
 *
 * \return   none
 */
extern void HI2C0_vBitDelayH(void);

/* HI2C<unit>_vBitDelayL*/
/**
 * \brief    Performs the delay for L level of signal.
 *
 *           User defined function that perform the delay for low phase of SCL
 *           (5 탎ec for 100 kHz mode or 1.3 탎ec for 400 kHz mode)
 *           To achieve the best performance the call/return overhead can be taken
 *           into account.
 *
 * \param    none
 *
 * \return   none
 */
extern void HI2C0_vBitDelayL(void);


#ifdef __cplusplus
}
#endif

#endif // HI2C_H

