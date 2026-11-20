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


/******************************************************************************/
/* DOCUMENTATION                                                              */
/******************************************************************************/
/** \file     muart_mc.c
 *
 *  \ingroup  MCAL
 *
 *  \brief    Implementation of MUART.
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
#define MUART_ERROR_ANY_READ_MASK    (UART_FR_PE_Msk | UART_FR_FE_Msk | \
                                      UART_FR_RXOV_Msk)

#define MUART_ERROR_ANY_CLEAR_MASK    (UART_FR_PEC_Msk | UART_FR_FEC_Msk | \
                                       UART_FR_RXOVC_Msk)

#define UART_CR_STOP_2bits      0x01u

#define UART_CR_PARITY_ENABLED      (0x2UL << UART_CR_MULTIE_Pos)

/******************************************************************************/
/* PRIVATE FUNCTION DECLARATIONS AND PRIVATE MACRO FUNCTION DEFINITIONS       */
/******************************************************************************/

static bool MUART_isParityEnabled(const struct MUART_Periph *muart)
{
     return ((muart->USARTx->CR.V32 & UART_CR_PARITY_ENABLED) != 0U);
}

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
        {
            &RCC->APB1ENR.V32,        // source clock for UART0-3 >> APB1 
            RCC_APB1ENR_UART0EN_Msk
        }
    }
};

const struct MUART_Periph MUART1 =
{
    UART1,
    MUART1_IRQ_INDEX,
    UART1_IRQn,
    {
        {
            &RCC->APB1ENR.V32,        // source clock for UART0-3 >> APB1 
            RCC_APB1ENR_UART1EN_Msk
        }
    }
};

const struct MUART_Periph MUART2 =
{
    UART2,
    MUART2_IRQ_INDEX,
    UART2_IRQn,
    {
        {
            &RCC->APB1ENR.V32,        // source clock for UART0-3 >> APB1 
            RCC_APB1ENR_UART2EN_Msk
        }
    }
};

const struct MUART_Periph MUART3 =
{
    UART3,
    MUART3_IRQ_INDEX,
    UART3_IRQn,
    {
        {
            &RCC->APB1ENR.V32,        // source clock for UART0-3 >> APB1 
            RCC_APB1ENR_UART3EN_Msk
        }
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
    uint32_t tmp =  muart->USARTx->CR.V32 & ~UART_CR_SM_Msk;

    if(dataBits == MUART_DATA_BITS_8)
    {
        if(MUART_isParityEnabled(muart))
        {
            // set 9-bit mode (8-bit data + 1-bit parity)
            tmp |= UART_Mode_3; 
        }
        else 
        {
            // standart 8-bit mode without parity
            tmp |= UART_Mode_1; 
        }
    }
    else if(dataBits == MUART_DATA_BITS_9)
    {
        // standart 9-bit mode
        tmp |= UART_Mode_3; 
    }
    else
    {
        // assert
    }

    muart->USARTx->CR.V32 =  tmp;
}


void MUART_configurePeriphStopBits(const struct MUART_Periph *muart, enum MUART_StopBits stopBits)
{
    uint32_t tmp = muart->USARTx->CR.V32;

    if(stopBits == MUART_STOP_BITS_1)
    {
        // clean flag for one bit stop
        tmp &= ~UART_CR_STOP_Msk;
    }
    else if(stopBits == MUART_STOP_BITS_2)
    {
        // set flag for two bits stop
        tmp |= UART_CR_STOP_2bits;
    }
    else
    {
        // assert, invalid option(MUART_STOP_BITS_0_5,MUART_STOP_BITS_1_5)
    }

    muart->USARTx->CR.V32 = tmp;
}


void MUART_configurePeriphParity(const struct MUART_Periph *muart, enum MUART_Parity parity)
{
    // Clear Parity bits
    uint32_t tmp = muart->USARTx->CR.V32 & ~(UART_CR_PS_Msk | UART_CR_MULTIE_Msk);
    
    /* NOTE: Parity can be enabled only for 9-bit mode (8bit data + 1 bit parity)
    */

    if(parity == MUART_PARITY_NONE)
    {
        // Nothing to do for this settings
    }
    else if(parity == MUART_PARITY_EVEN)
    {
        // NOTE: available only in 9-bit mode!
        tmp |= UART_Parity_Even;
    }
    else if(parity == MUART_PARITY_ODD)
    {
        // NOTE: available only in 9-bit mode!
        tmp |= UART_Parity_Odd;
    }
    else
    {
        // assert
    }

    muart->USARTx->CR.V32 = tmp;
}


void MUART_configurePeriphBaudrate(const struct MUART_Periph *muart, uint32_t baudrate)
{
    uint32_t clkHz;
    uint32_t intdiv;
    uint32_t fradiv;
    
    muart->USARTx->CR.V32 |= UART_CR_SBRTEN_Msk;        // enable UART baudrate generator

  #ifdef MCAL_MPCM_INCLUDED
    enum MPCM_Clock clockSource = MPCM_CLOCK_PCLK1;     //UART clock source = APB1 

    clkHz = (uint32_t)MPCM_getClockFreq(clockSource);
  #else
    //USART[0-3] is using APB1 which is PCLOCK1
    clkHz = PCLOCK1 * 1000000u;
  #endif
    /* Info 774: boolean condition for 'if' always evaluates to 'true' (involving variable 'clkHz') [MISRA 2012 Rule 14.3, advisory] */
    if(clkHz > 0U)//lint !e774 : intended to prevent additional compiler switches
    {
        
        intdiv = ((clkHz / 16U) / baudrate) - 1U;
        fradiv = (clkHz / baudrate) - (16U * (intdiv + 1U));
        muart->USARTx->BRT.V32 = ((UART_BRT_SBRT_Msk | UART_BRT_BFINE_Msk) & (intdiv | (fradiv << 16)));
    }
    else
    {
        // assert
    }
}


void MUART_enableClock(const struct MUART_Periph *muart)
{
    // unlock RCC registers
    RCC_REGS_UNLOCK();
    
    MSUP_setRegBitField32(&muart->clockCfg.RcuEnr);
    
    // Lock RCC registers
    RCC_REGS_LOCK();
}


void MUART_disableClock(const struct MUART_Periph *muart)
{
    // unlock RCC registers
    RCC_REGS_UNLOCK();
    
    MSUP_clearRegBitField32(&muart->clockCfg.RcuEnr);
    
    // Lock RCC registers
    RCC_REGS_LOCK();
}


enum MCAL_Error MUART_configure(const struct MUART_Channel *channel, const struct MUART_Config *cfg)
{
    const struct MUART_Periph *muart = channel->muart;

    MUART_enableClock(muart);

    MUART_configurePeriphBaudrate(muart, cfg->baudrate);
    MUART_configurePeriphParity(muart, cfg->parity);
    MUART_configurePeriphDataBits(muart, cfg->dataBits);
    MUART_configurePeriphStopBits(muart, cfg->stopBits);

    return MCAL_OK;
}


void MUART_configureBaudrate(const struct MUART_Channel *channel, uint32_t baudrate)
{
    MUART_disableChannel(channel);
    MUART_configurePeriphBaudrate(channel->muart, baudrate);
    MUART_enableChannel(channel);
}


void MUART_enableChannel(const struct MUART_Channel *channel)
{
    channel->muart->USARTx->CR.V32 |= (UART_CR_TEN_Msk | UART_CR_REN_Msk);
}


void MUART_disableChannel(const struct MUART_Channel *channel)
{
    channel->muart->USARTx->CR.V32 &= ~(UART_CR_TEN_Msk | UART_CR_REN_Msk);
}


void MUART_writeDataReg(const struct MUART_Channel *channel, uint32_t data)
{
    channel->muart->USARTx->TDR.V32 = data;
}


uint16_t MUART_readDataReg(const struct MUART_Channel *channel)
{
    return (uint16_t)channel->muart->USARTx->RDR.V32;
}


uint32_t MUART_getStatusReg(const struct MUART_Channel *channel)
{
    return channel->muart->USARTx->FR.V32;
}


void MUART_clearErrorReg(const struct MUART_Channel *channel)
{
    channel->muart->USARTx->FR.V32 = MUART_ERROR_ANY_CLEAR_MASK;
}


uint32_t MUART_getStatusRegMaskForEvent(const struct MUART_Channel *channel, enum MCAL_Event event)
{
    (void)*channel;

    if(event == MCAL_EVENT_READ_BYTE)
    {
        return UART_FR_RI_Msk;
    }
    else if(event == MCAL_EVENT_WRITE_BYTE)
    {
        return UART_FR_TI_Msk;
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

    if(status & UART_FR_FE_Msk)
    {
        error = MCAL_ERROR_FRAMING;
    }
    else if(status & UART_FR_PE_Msk)
    {
        error = MCAL_ERROR_PARITY;
    }
    else if(status & UART_FR_RXOV_Msk)
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
        channel->muart->USARTx->CR.V32 |= UART_CR_RIE_Msk;
    }
    else if(event == MCAL_EVENT_WRITE_BYTE)
    {
        channel->muart->USARTx->CR.V32 |= UART_CR_TIE_Msk;
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
        channel->muart->USARTx->CR.V32 &= ~UART_CR_RIE_Msk;
    }
    else if(event == MCAL_EVENT_WRITE_BYTE)
    {
        channel->muart->USARTx->CR.V32 &= ~UART_CR_TIE_Msk;
    }
    else
    {
        /* Nothing to do */
    }
}
