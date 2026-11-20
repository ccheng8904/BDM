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
 *  COMP_ABBREV      MUART
 *  PROCESSOR        SH30F9X71
 ******************************************************************************/


#ifndef MUART_MC_H
#define MUART_MC_H

#ifdef __cplusplus
extern "C" {
#endif

/******************************************************************************/
/* DOCUMENTATION                                                              */
/******************************************************************************/
/** \file     muart_mc.h
 *
 *  \ingroup  MCAL
 *
 *  \brief    Public definitions and declarations for unit MUART.
 *
 */


/******************************************************************************/
/* INCLUDES                                                                   */
/******************************************************************************/

#include "BSH_stdinc.h"
#include "processor.h"
#include "mcal/muart_types.h"
#include "mcal/mdio.h"
#include "mcal/msup.h"

/******************************************************************************/
/* PUBLIC DEFINITIONS                                                         */
/******************************************************************************/

/******************************************************************************/
/* PUBLIC TYPE DEFINITIONS                                                    */
/******************************************************************************/

enum MUART_IRQ_Index
{
    MUART0_IRQ_INDEX,
    MUART1_IRQ_INDEX,
    MUART2_IRQ_INDEX,
    MUART3_IRQ_INDEX,
    MUART_IRQ_CNT
};


struct MUART_Clock
{
    struct MSUP_RegBitField32   RcuEnr;
};


struct MUART_Periph
{
    UART_TypeDef            *USARTx;
    enum MUART_IRQ_Index    index;
    IRQn_Type               IRQn;
    struct MUART_Clock      clockCfg;
};


struct MUART_Channel
{
    const struct MUART_Periph        *muart;
    const struct MDIO_Channel        *rxPin;
    const struct MDIO_Channel        *txPin;
};


/******************************************************************************/
/* PUBLIC DATA DECLARATIONS                                                   */
/******************************************************************************/

extern const struct MUART_Periph MUART0;

extern const struct MUART_Periph MUART1;

extern const struct MUART_Periph MUART2;

extern const struct MUART_Periph MUART3;


/******************************************************************************/
/* PUBLIC DEFINITIONS                                                         */
/******************************************************************************/


/******************************************************************************/
/* PUBLIC FUNCTION DECLARATIONS                                               */
/******************************************************************************/

extern void MUART_configurePeriphDataBits(const struct MUART_Periph *muart, enum MUART_DataBits dataBits);
extern void MUART_configurePeriphStopBits(const struct MUART_Periph *muart, enum MUART_StopBits stopBits);
extern void MUART_configurePeriphParity(const struct MUART_Periph *muart, enum MUART_Parity parity);
extern void MUART_configurePeriphBaudrate(const struct MUART_Periph *muart, uint32_t baudrate);

extern void MUART_enableClock(const struct MUART_Periph *muart);
extern void MUART_disableClock(const struct MUART_Periph *muart);


#ifdef __cplusplus
}
#endif

#endif // MUART_MC_H
