/*******************************************************************************
 *  Copyright (c) 2023 BSH Hausgeraete GmbH,
 *  Carl-Wery-Str. 34, 81739 Munich, Germany, www.bsh-group.de
 *
 *  All rights reserved. This program and the accompanying materials
 *  are protected by international copyright laws.
 *  Please contact copyright holder for licensing information.
 *
 *******************************************************************************
 *  PROJECT          MCAL
 *  COMP_ABBREV      MI2C
 *  PROCESSOR        CMS32M67xx
 ******************************************************************************/


/******************************************************************************/
/* DOCUMENTATION                                                              */
/******************************************************************************/
/** \file     mi2c_mc.h
 *
 *  \ingroup  MCAL
 *
 *  \brief    Public definitions and declarations for unit MI2C.
 *
 *  \details  The CMS32M67xx provides one I2C unit (IICA0). Because the MCAL
 *            MI2C implementation expects a register level interface, the
 *            interface of the (not existing) hardware I2C support is defined
 *            here and implemented on top of the vendor driver functions.
 *
 */

#ifndef MI2C_MC_H
#define MI2C_MC_H

#ifdef __cplusplus
extern "C" {
#endif

/******************************************************************************/
/* INCLUDES                                                                   */
/******************************************************************************/

#include <assert.h>
#include "BSH_stdinc.h"
#include "processor.h"
#include "mcal/mi2c_types.h"
#include "mcal/mdio.h"
#include "mcal/msup.h"

/******************************************************************************/
/* PUBLIC DEFINITIONS                                                         */
/******************************************************************************/

/*lint -save -e9026 "Function-like macro defined..." */

#define MI2C_DataSetupTimeDelay() do{HSUP_vNop();HSUP_vNop();}while(0)

// Default static configuration definition
#define MI2C_SCFG_DEFAULT     {.clkSrc = MI2C_CLOCK_SOURCE_PCLK}

/*lint -restore -e9026 */

// Replace vendor macro definition with own name
#define I2C0                    I2C
#define I2C_TypeDef             I2C0_Type

/* status/event bits reported by MI2C_getStatusReg() */
#define MI2C_STATUS_ACK         0x00000001U
#define MI2C_STATUS_NACK        0x00000002U
#define MI2C_STATUS_STOP        0x00000004U
#define MI2C_STATUS_RX          0x00000008U
#define MI2C_STATUS_ERROR       0x00000010U
#define MI2C_STATUS_START       0x00000020U

/******************************************************************************/
/* PUBLIC TYPE DEFINITIONS                                                    */
/******************************************************************************/

enum MI2C_unitIndex
{
    MI2C0_IRQ_INDEX,
    MI2C_IRQ_CNT
};


struct MI2C_Periph
{
    I2C_TypeDef                    *I2Cx;
    enum MI2C_unitIndex            index;
    IRQn_Type                      IRQn;
};


enum MI2C_ClockSource
{
    MI2C_CLOCK_SOURCE_PCLK     = 0u
};


struct MI2C_StaticConfig
{
    enum MI2C_ClockSource          clkSrc;
};


struct MI2C_Channel
{
    const struct MI2C_Periph       *mi2c;
    const struct MDIO_Channel      *sdaPin;
    const struct MDIO_Channel      *sclPin;
    const struct MI2C_StaticConfig scfg;
};


/******************************************************************************/
/* PUBLIC DATA DECLARATIONS                                                   */
/******************************************************************************/
#if defined(MI2C0_PRESENT)
extern const struct MI2C_Periph MI2C0;
#endif

/******************************************************************************/
/* PUBLIC FUNCTION DECLARATIONS                                               */
/******************************************************************************/
extern void MI2C_clearAckFlag(const struct MI2C_Channel *channel);
extern void MI2C_clearNackFlag(const struct MI2C_Channel *channel);
extern void MI2C_clearStopFlag(const struct MI2C_Channel *channel);
extern void MI2C_clearRxFlag(const struct MI2C_Channel *channel);
extern void MI2C_sendAckReg(const struct MI2C_Channel *channel);
extern void MI2C_sendNackReg(const struct MI2C_Channel *channel);
extern void MI2C_sendStopReg(const struct MI2C_Channel *channel);
extern void MI2C_setBaudrate(const struct MI2C_Channel *channel, enum MI2C_Baudrate baudRate, enum MI2C_OperationMode mode);

extern void MI2C_output(const struct MDIO_Channel *channel);
extern void MI2C_input(const struct MDIO_Channel *channel);
extern void MI2C_initPort(const struct MI2C_Channel *channel, bool internalPullUps);

/* register level interface used by the generic MI2C implementation */
extern void MI2C_configure(const struct MI2C_Channel *channel, const struct MI2C_Config *cfg);
/* MI2C_configurePins is implemented in mi2c.c */
extern void MI2C_enableChannel(const struct MI2C_Channel *channel);
extern void MI2C_disableChannel(const struct MI2C_Channel *channel);
extern enum MCAL_Error MI2C_makeStartCondition(const struct MI2C_Channel *channel);
extern enum MCAL_Error MI2C_sendStop(const struct MI2C_Channel *channel);
extern void MI2C_beforeCommunication(const struct MI2C_Channel *channel, uint16_t baseAddress, bool isReading,
                                     enum MI2C_BaseAddressLength length, uint16_t dataLength);
extern void MI2C_afterCommunication(const struct MI2C_Channel *channel);
extern enum MCAL_Error MI2C_getChannelError(const struct MI2C_Channel *channel);
extern void MI2C_clearErrorReg(const struct MI2C_Channel *channel);
extern void MI2C_setReceiveMode(const struct MI2C_Channel *channel);
extern enum MCAL_Error MI2C_sendAddress(const struct MI2C_Channel *channel, uint16_t baseAddress, enum MI2C_BaseAddressLength addressLength);
extern void MI2C_beforeReadByte(const struct MI2C_Channel *channel);
extern uint8_t MI2C_readDataReg(const struct MI2C_Channel *channel);
extern void MI2C_writeDataReg(const struct MI2C_Channel *channel, uint32_t data);
extern uint32_t MI2C_getStatusReg(const struct MI2C_Channel *channel);
extern uint32_t MI2C_getStatusRegMaskForEvent(const struct MI2C_Channel *channel, enum MCAL_Event event);
extern enum MI2C_unitIndex MI2C_IRQ_getIndex(const struct MI2C_Channel *channel);
extern void MI2C_IRQ_enable(const struct MI2C_Channel *channel);
extern void MI2C_IRQ_enableEvent(const struct MI2C_Channel *channel, enum MCAL_Event event);
extern void MI2C_IRQ_disableEvent(const struct MI2C_Channel *channel, enum MCAL_Event event);


#ifdef __cplusplus
}
#endif

#endif // MI2C_MC_H
