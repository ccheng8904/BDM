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


#ifndef DBUSCAN_DBUS_H
#define DBUSCAN_DBUS_H

#ifdef __cplusplus
extern "C" {
#endif


/******************************************************************************/
/* DOCUMENTATION                                                              */
/******************************************************************************/
/** \file
 *
 *  \ingroup dbuscan
 *
 *  \brief   Public definitions and declarations related to DBus part of the DBusCAN chip
 *
 */


/******************************************************************************/
/* INCLUDES                                                                   */
/******************************************************************************/
#include "bsh_stdinc.h"
#include <stdint.h>
#include "dbuscan_types.h"
#include "mcal/mcal_includes.h"
#include "bustypes.h"


/******************************************************************************/
/* PUBLIC DEFINITIONS                                                         */
/******************************************************************************/


/******************************************************************************/
/* PUBLIC DATA DEFINITIONS                                                    */
/******************************************************************************/
extern DBC_WriteBuf_t DBCDBUS_txBuf;        ///< Buffer for writing data to the DBusCAN chip over SPI


/******************************************************************************/
/* PUBLIC TYPE DEFINITIONS                                                    */
/******************************************************************************/


/******************************************************************************/
/* PUBLIC FUNCTION DECLARATIONS                                               */
/******************************************************************************/

/** \brief   Configurable routine to allow additional handling of interrupts 
 *           from the DBusCAN chip according to project needs.
 *
 *  \param   irqBuf: union structure of all interrupt flags of the DBusCAN chip
 */
extern void DBCDBUS_handleIrqExt(union DBC_IrqBuf irqBuf);


/** \brief   Handles interrupts from the DBusCAN chip.
 *
 *  \details All interrupt flags from the DBusCAN chip
 *           are read, cleared and handled if needed.
 *
 *  \return  true if handling of interrupt flags is complete, otherwise false
 */
extern bool DBCDBUS_handleIrq(void);


/** \brief   Sends DBus frame via the DBusCAN chip (over DBus line).
 *
 *  \return  false if the request to send the DBus message is being processed, true if it is complete
 */
extern bool DBCDBUS_sendFrame(void);


/** \brief   Reads received DBus frame from the DBusCAN chip.
 *
 *  \return  pointer to received DBus frame
 */
extern TbusMessage* DBCDBUS_readFrame(void);


/** \brief   Sends wake-up pulse via the DBusCAN chip (over DBus line).
 */
extern enum DBC_Error DBCDBUS_sendWakeUpPulse(void);


/** \brief   Sets DBus node filter in the DBusCAN chip.
 *
 *  \param   filterAddr:  address of node filter to be set. Possible values: DBC_DBUS_NF0_ADDR - DBC_DBUS_NF13_ADDR
 *  \param   ackEn:       enable of sending acknowledge for messages received by specified node filter
 *  \param   node:        DBus node address to be filtered. Possible values: 0-15
 *  \param   subsystMask: mask of DBus subsystems to be filtered. Possible values: 0x0000-0xFFFF
 *
 *  \return  enum DBC_Error
 */
extern enum DBC_Error DBCDBUS_setNodeFilter(enum DBC_RegAddr filterAddr, bool ackEn, uint8_t node, uint16_t subsystMask);


/** \brief   Resets DBus node filter of the DBusCAN chip.
 *
 *  \param   filterAddr: address of node filter to be reset. Possible values: DBC_DBUS_NF0_ADDR - DBC_DBUS_NF13_ADDR
 *
 *  \return  enum DBC_Error
 */
extern enum DBC_Error DBCDBUS_resetNodeFilter(enum DBC_RegAddr filterAddr);


/** \brief   Resets all free DBus node filters (except reserved DBC_DBUS_NF14_ADDR) of the DBusCAN chip.
 *
 *  \return  enum DBC_Error
 */
extern enum DBC_Error DBCDBUS_resetAllNodeFilters(void);


#ifndef RTOS_DBUS_EVENTDRIVEN
/** \brief   Returns information whether transmission of DBus message from the DBusCAN chip is completed.
 *
 *  \note    This interface is only for internal use, not to be called in application SW.
 *
 *  \return  true or false
 */
extern bool DBCDBUS_isMsgTransmitted(void);

#else
/** \brief   Returns number of reveived messages.
 *
 *  \return  Number of received messages, saved in DBCDBUS_newMsgCount
 */
extern uint8_t DBCDBUS_getNewMsgCount(void);


/** \brief   Returns pointer DBCDBUS_rxFramePtr.
 *
 *  \return  Pointer DBCDBUS_rxFramePtr.
 */
extern const TbusMessage* DBCDBUS_getRxFramePtr(void);

#endif //RTOS_DBUS_EVENTDRIVEN


/** \brief   Returns acknowledge value received by the DBusCAN chip.
 *
 *  \return  received acknowledge value
 */
extern uint8_t DBCDBUS_getReceivedAck(void);


/** \brief   Enables acknowledge for received message on DBus node (and any subnode) address
 *
 *  \return  enum DBC_Error
 */
extern enum DBC_Error DBCDBUS_enableAck(void);


/** \brief   Disables acknowledge for received message on DBus node (and any subnode) address
 *
 *  \return  enum DBC_Error
 */
extern enum DBC_Error DBCDBUS_disableAck(void);


#ifdef __cplusplus
}
#endif

#endif // DBUSCAN_DBUS_H
