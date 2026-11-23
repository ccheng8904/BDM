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
 *  PROCESSOR        CMS32M67xx
 ******************************************************************************/


/******************************************************************************/
/* DOCUMENTATION                                                              */
/******************************************************************************/
/** \file     muart_mc.c
 *
 *  \ingroup  MCAL
 *
 *  \brief    Implementation of MUART.
 *
 *  \details  CMS32M67xx UART mapping:
 *
 *             - the UART divider register DLR holds fMAIN / 16 / baudrate
 *             - LCR selects word length, parity, stop bits and the break
 *               control (BCON = break transmission)
 *             - LSR holds the receive data ready (RDR), overrun (OE), parity
 *               error (PE), framing error (FE) and transmit empty (THRE) flags
 *             - IER enables the receive (RBREIE), transmit (THREIE) and
 *               receive line status (RLSIE) interrupts
 *             - the TXD pin is selected by the UART0RXDCFG / UART1RXDCFG
 *               register for the RXD pin, TXD pins are selected by PxyCFG
 *
 */

/******************************************************************************/
/* INCLUDES                                                                   */
/******************************************************************************/

#include "math/mathDiv.h"
#include "muart_mc.h"
#include "muart_priv.h"
#include "std_lib/std_symbol.h"

#ifdef MCAL_MPCM_INCLUDED
    #include "mpcm.h"
#endif

/******************************************************************************/
/* PRIVATE DEFINITIONS                                                        */
/******************************************************************************/

#define MUART_ERROR_ANY_READ_MASK   (UART_LSR_FE_Msk | UART_LSR_PE_Msk)

#define MUART_LSR_RX_ERROR_MASK     (UART_LSR_FE_Msk | UART_LSR_PE_Msk)

/* the overrun flag (LSR bit 1) is not exposed by the vendor header, it is
 * cleared by reading the RBR together with the other line status flags */
#define MUART_LSR_OE_MASK           0x00000002U

#define MUART_LCR_BREAK_MASK        (UART_LCR_BCON_Msk)

/******************************************************************************/
/* PRIVATE FUNCTION DECLARATIONS AND PRIVATE MACRO FUNCTION DEFINITIONS       */
/******************************************************************************/

#ifndef MCAL_MPCM_INCLUDED
static uint32_t MUART_getPeripheralClock(void);
#endif

/******************************************************************************/
/* PUBLIC DATA DEFINITIONS                                                    */
/******************************************************************************/
/* "Side effect in initializer list, volatile access": false positive, as volatile register is not accessed */
/*Axivion Disable Style MisraC2012-13.1  : false positive, no volatile access, just pointer used */

/*lint -save -e923 Note 923: Cast from unsigned int to pointer*/

const struct MUART_Periph MUART0 =
{
    UART0,
    MUART0_IRQ_INDEX,
    UART0_IRQn,
    {
        CGC_PER12Periph_UART0
    }
};

const struct MUART_Periph MUART1 =
{
    UART1,
    MUART1_IRQ_INDEX,
    UART1_IRQn,
    {
        CGC_PER12Periph_UART1
    }
};

/*Axivion Enable Style MisraC2012-13.1 */
/******************************************************************************/
/* PRIVATE DATA DEFINITIONS                                                   */
/******************************************************************************/



/******************************************************************************/
/* PUBLIC FUNCTION DEFINITIONS                                                */
/******************************************************************************/


void MUART_configurePeriphDataBits(const struct MUART_Periph *muart, enum MUART_DataBits dataBits)
{
    uint32_t tmp = muart->UARTx->LCR & ~UART_LCR_WLS_Msk;

    if((dataBits == MUART_DATA_BITS_8) || (dataBits == MUART_DATA_BITS_9))
    {
        /* 8 data bits, a parity bit (if enabled) is handled separately */
        tmp |= UART_WLS_8;
    }
    else
    {
        /* 7 data bits */
        tmp |= UART_WLS_7;
    }

    muart->UARTx->LCR = tmp;
}


void MUART_configurePeriphStopBits(const struct MUART_Periph *muart, enum MUART_StopBits stopBits)
{
    uint32_t tmp = muart->UARTx->LCR & ~UART_LCR_SBS_Msk;

    if(stopBits == MUART_STOP_BITS_2)
    {
        tmp |= UART_STOP_BIT_2;
    }
    else
    {
        /* NOTE: 0.5 and 1.5 stop bits are not available, 1 stop bit is used */
        tmp |= UART_STOP_BIT_1;
    }

    muart->UARTx->LCR = tmp;
}


void MUART_configurePeriphParity(const struct MUART_Periph *muart, enum MUART_Parity parity)
{
    uint32_t tmp = muart->UARTx->LCR & ~(UART_LCR_PSEL_Msk | UART_LCR_PEN_Msk);

    if(parity == MUART_PARITY_EVEN)
    {
        tmp |= UART_PARITY_EVEN;
    }
    else if(parity == MUART_PARITY_ODD)
    {
        tmp |= UART_PARITY_ODD;
    }
    else
    {
        /* no parity */
    }

    muart->UARTx->LCR = tmp;
}


void MUART_configurePeriphBaudrate(const struct MUART_Periph *muart, uint32_t baudrate)
{
    uint32_t clkHz;
    uint32_t divider;

  #ifdef MCAL_MPCM_INCLUDED
    clkHz = (uint32_t)MPCM_getClockFreq(MPCM_CLOCK_PCLK0);
  #else
    clkHz = MUART_getPeripheralClock();
  #endif

    if((baudrate > 0U) && (clkHz > 0U))
    {
        divider = clkHz / (16U * baudrate);
        muart->UARTx->DLR = divider;
    }
    else
    {
        /* assert */
    }
}


void MUART_enableClock(const struct MUART_Periph *muart)
{
    CGC_PER12PeriphClockCmd(muart->clockCfg.periphClockMask, ENABLE);
}


void MUART_disableClock(const struct MUART_Periph *muart)
{
    CGC_PER12PeriphClockCmd(muart->clockCfg.periphClockMask, DISABLE);
}


void MUART_configureTxPin(const struct MUART_Periph *muart, const struct MDIO_Channel *txPin)
{
    struct MDIO_Config cfg = MDIO_CFG_ALTERNATE_FUNCTION;

    (void)muart;

    if(txPin == NULL)
    {
        return;
    }

    /* CMS32M67xx: the TXD0 signal is alternate function 1 of the port pin */
    cfg.outputState = MDIO_OUTPUT_STATE_HIGH;

    (void)MDIO_init(txPin, &cfg);
}


void MUART_configureRxPin(const struct MUART_Periph *muart, const struct MDIO_Channel *rxPin)
{
    const uint32_t portIndex = (uint32_t)MDIO_getPort(rxPin)->portIndex;
    const uint32_t pinIndex  = (uint32_t)MDIO_getPin(rxPin);
    struct MDIO_Config cfg   = MDIO_CFG_ALTERNATE_FUNCTION_PULL_UP;

    /* the RXD pin is selected by UART0RXDCFG / UART1RXDCFG, the register value
     * is built from the port and the pin number, see the vendor header */
    if(muart->index == MUART0_IRQ_INDEX)
    {
        PORT->UART0RXDCFG = (uint8_t)((portIndex << 4) | pinIndex);
    }
    else
    {
        PORT->UART1RXDCFG = (uint8_t)((portIndex << 4) | pinIndex);
    }

    (void)MDIO_init(rxPin, &cfg);
}


enum MCAL_Error MUART_configure(const struct MUART_Channel *channel, const struct MUART_Config *cfg)
{
    const struct MUART_Periph *muart = channel->muart;

    MUART_enableClock(muart);

    /* disable the transmitter and receiver during configuration */
    muart->UARTx->IER = 0U;
    muart->UARTx->LCR = 0U;

    MUART_configurePeriphBaudrate(muart, cfg->baudrate);
    MUART_configurePeriphParity(muart, cfg->parity);
    MUART_configurePeriphDataBits(muart, cfg->dataBits);
    MUART_configurePeriphStopBits(muart, cfg->stopBits);

    /* select the pins of the channel */
    if(channel->txPin != NULL)
    {
        MUART_configureTxPin(muart, channel->txPin);
    }
    if(channel->rxPin != NULL)
    {
        MUART_configureRxPin(muart, channel->rxPin);
    }

    return MCAL_OK;
}


void MUART_configureBaudrate(const struct MUART_Channel *channel, uint32_t baudrate)
{
    MUART_configurePeriphBaudrate(channel->muart, baudrate);
}


void MUART_enableChannel(const struct MUART_Channel *channel)
{
    /* enable the UART by enabling the receive and the transmit interrupt
     * sources only, the CMS32M67xx UART has no separate enable bit */
    channel->muart->UARTx->IER |= UART_IER_RLSIE_Msk;
}


void MUART_disableChannel(const struct MUART_Channel *channel)
{
    channel->muart->UARTx->IER = 0U;
}


void MUART_writeDataReg(const struct MUART_Channel *channel, uint32_t data)
{
    channel->muart->UARTx->THR = data;
}


uint16_t MUART_readDataReg(const struct MUART_Channel *channel)
{
    return (uint16_t)channel->muart->UARTx->RBR;
}


uint32_t MUART_getStatusReg(const struct MUART_Channel *channel)
{
    return channel->muart->UARTx->LSR;
}


void MUART_clearErrorReg(const struct MUART_Channel *channel)
{
    /* reading the LSR and the RBR clears the line status error flags */
    (void)channel->muart->UARTx->LSR;
    (void)channel->muart->UARTx->RBR;
}


uint32_t MUART_getStatusRegMaskForEvent(const struct MUART_Channel *channel, enum MCAL_Event event)
{
    (void)*channel;

    if(event == MCAL_EVENT_READ_BYTE)
    {
        return UART_LSR_RDR_Msk;
    }
    else if(event == MCAL_EVENT_WRITE_BYTE)
    {
        return UART_LSR_THRE_Msk;
    }
    else if(event == MCAL_EVENT_ERROR)
    {
        return MUART_ERROR_ANY_READ_MASK;
    }
    else
    {
        return 0u;
    }
}


enum MCAL_Error MUART_getChannelError(const struct MUART_Channel *channel)
{
    enum MCAL_Error error = MCAL_OK;
    uint32_t status = MUART_getStatusReg(channel);

    if((status & UART_LSR_FE_Msk) != 0U)
    {
        error = MCAL_ERROR_FRAMING;
    }
    else if((status & UART_LSR_PE_Msk) != 0U)
    {
        error = MCAL_ERROR_PARITY;
    }
    else if((status & MUART_LSR_OE_MASK) != 0U)
    {
        error = MCAL_ERROR_OVERFLOW;
    }
    else
    {
        /* nothing to do */
    }

    return error;
}


enum MUART_IRQ_Index MUART_IRQ_getIndex(const struct MUART_Channel *channel)
{
    return channel->muart->index;
}


void MUART_IRQ_enable(const struct MUART_Channel *channel)
{
    NVIC_EnableIRQ(channel->muart->IRQn);
}


void MUART_IRQ_enableEvent(const struct MUART_Channel *channel, enum MCAL_Event event)
{
    if(event == MCAL_EVENT_READ_BYTE)
    {
        channel->muart->UARTx->IER |= UART_IER_RBREIE_Msk;
    }
    else if(event == MCAL_EVENT_WRITE_BYTE)
    {
        channel->muart->UARTx->IER |= UART_IER_THREIE_Msk;
    }
    else
    {
        /* Nothing to do */
    }
}


void MUART_IRQ_disableEvent(const struct MUART_Channel *channel, enum MCAL_Event event)
{
    if(event == MCAL_EVENT_READ_BYTE)
    {
        channel->muart->UARTx->IER &= ~UART_IER_RBREIE_Msk;
    }
    else if(event == MCAL_EVENT_WRITE_BYTE)
    {
        channel->muart->UARTx->IER &= ~UART_IER_THREIE_Msk;
    }
    else
    {
        /* Nothing to do */
    }
}


void MUART_setBreak(const struct MUART_Channel *channel, bool enable)
{
    if(enable)
    {
        channel->muart->UARTx->LCR |= MUART_LCR_BREAK_MASK;
    }
    else
    {
        channel->muart->UARTx->LCR &= ~MUART_LCR_BREAK_MASK;
    }
}


bool MUART_isRxOngoing(const struct MUART_Channel *channel)
{
    /* the CMS32M67xx UART does not provide an idle line detection, therefore
     * the DBus idle detection is done by the ring buffer state */
    (void)*channel;

    return false;
}


/******************************************************************************/
/* PRIVATE FUNCTION DEFINITIONS                                               */
/******************************************************************************/
#ifndef MCAL_MPCM_INCLUDED

static uint32_t MUART_getPeripheralClock(void)
{
    /* the CMS32M67xx UART is clocked by the main system clock */
    return (uint32_t)SystemCoreClock;
}
#endif
