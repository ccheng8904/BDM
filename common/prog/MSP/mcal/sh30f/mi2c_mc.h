/*******************************************************************************
 *  Copyright (c) 2022 BSH Hausgeraete GmbH,
 *  Carl-Wery-Str. 34, 81739 Munich, Germany, www.bsh-group.de
 *
 *  All rights reserved. This program and the accompanying materials
 *  are protected by international copyright laws.
 *  Please contact copyright holder for licensing information.
 *
 *******************************************************************************
 *  PROJECT          MCAL
 *  COMP_ABBREV      MI2C
 *  PROCESSOR        SH30F
 ******************************************************************************/


#ifndef MI2C_MC_H
#define MI2C_MC_H

#ifdef __cplusplus
extern "C" {
#endif

/******************************************************************************/
/* DOCUMENTATION                                                              */
/******************************************************************************/
/** \file     mi2c_mc.h
 *
 *  \ingroup  MCAL
 *
 *  \brief    Public definitions and declarations for unit MI2C.
 *
 */


/******************************************************************************/
/* INCLUDES                                                                   */
/******************************************************************************/

#include <assert.h>
#include "BSH_stdinc.h"
#include "processor.h"
// #include "mcal.h"
#include "mcal/mi2c_types.h"
#include "mcal/mdio.h"
#include "mcal/msup.h"

/******************************************************************************/
/* PUBLIC DEFINITIONS                                                         */
/******************************************************************************/

// Multiple I2C units share the same IRQ
//#define MI2C_MULTIPLE_HANDLES_PER_IRQ


/*lint -save -e9026 "Function-like macro defined..." */

#define MI2C_DataSetupTimeDelay() do{HSUP_vNop();HSUP_vNop();}while(0)

// Default static configuration definition
#define MI2C_SCFG_DEFAULT     {.clkSrc = MI2C_CLOCK_SOURCE_PCLK}


/*lint -restore -e9026 */

// Replace vendor macro definition with own name
#define I2C0                    TWI0
#define I2C_TypeDef             TWI_TypeDef

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
    struct MSUP_RegBitField32      clockCfg;
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
/* PUBLIC DEFINITIONS                                                         */
/******************************************************************************/


/******************************************************************************/
/* PUBLIC FUNCTION DECLARATIONS                                               */
/******************************************************************************/
//extern int32_t MI2C_handleEEPROMevents(void *obj, uint32_t flags, int32_t data);
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


#ifdef __cplusplus
}
#endif

#endif // MI2C_MC_H
