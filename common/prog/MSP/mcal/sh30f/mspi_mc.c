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
 *  COMP_ABBREV      MSPI
 *  PROCESSOR        SH30F9X71
 ******************************************************************************/
 

/******************************************************************************/
/* DOCUMENTATION                                                              */
/******************************************************************************/
/** \file     mspi_mc.c
 *
 *  \ingroup  MCAL
 *
 *  \brief    Implementation of MSPI.
 *
 */


/******************************************************************************/
/* INCLUDES                                                                   */
/******************************************************************************/

#include "math/mathDiv.h"
#include "mspi.h"
#include "mspi_mc.h"
#include "mspi_priv.h"
#include "mcal_assert.h"

#ifdef MCAL_MPCM_INCLUDED
    #include "mpcm.h"
#endif

/******************************************************************************/
/* PRIVATE DEFINITIONS                                                        */
/******************************************************************************/

// SPI ERROR FLAG
#define MSPI_ERROR_ANY_MASK           (SPI_FR_WCOL_Msk | SPI_FR_RXOV_Msk |SPI_FR_MODF_Msk)


/******************************************************************************/
/* PRIVATE FUNCTION DECLARATIONS AND PRIVATE MACRO FUNCTION DEFINITIONS       */
/******************************************************************************/
static bool MSPI_isBusy(const struct MSPI_Periph *mspi);
static enum MCAL_Error MSPI_setPresc(const struct MSPI_Channel *channel, uint32_t requiredSclkClock, uint32_t *returnValue);
#ifdef CURRENTLY_UNUSED_INTERFACE
static enum MCAL_Error MSPI_getChannelError(const struct MSPI_Channel *channel);
#endif
/******************************************************************************/
/* PUBLIC DATA DEFINITIONS                                                    */
/******************************************************************************/

#if defined(MSPI0_PRESENT)
const struct MSPI_Periph MSPI0 = 
{
    SPI0,
    MSPI0_IRQ_INDEX,
    SPI0_IRQn,
    {
        &RCC->APB1ENR.V32,
        RCC_APB1ENR_SPI0EN_Msk
    }
#ifdef MCAL_MDMA_INCLUDED
    ,&MDMA_MSPI0_Link_TX
    ,&MDMA_MSPI0_Link_RX
#endif 
};
#endif

#if defined(MSPI1_PRESENT)
const struct MSPI_Periph MSPI1 = 
{
    SPI1,
    MSPI1_IRQ_INDEX,
    SPI1_LED_IRQn,
    {
        &RCC->APB1ENR.V32,
        RCC_APB1ENR_SPI1EN_Msk
    }
#ifdef MCAL_MDMA_INCLUDED
    ,&MDMA_MSPI1_Link_TX
    ,&MDMA_MSPI1_Link_RX
#endif 
};
#endif

/******************************************************************************/
/* PRIVATE DATA DEFINITIONS                                                   */
/******************************************************************************/


/******************************************************************************/
/* PUBLIC FUNCTION DEFINITIONS                                                */
/******************************************************************************/
enum MCAL_Error MSPI_configure(const struct MSPI_Channel *channel, const struct MSPI_Config *cfg)
{
    uint32_t configReg;
    SPI_TypeDef *spiUnit = channel->mspi->SPIx;

    // unlock RCC registers
    RCC_REGS_UNLOCK();
    // enable the spi
    MSUP_setRegBitField32(&channel->mspi->clockCfg);
    // lock RCC registers
    RCC_REGS_LOCK();
    //set to default value
    spiUnit->CR.V32 = 0u;
    configReg = 0u;
    MCAL_RETURN_ON_ERROR(MSPI_setPresc(channel,cfg->clockFreq, &configReg));
    configReg |= (cfg->frameFormat   != MSPI_FRAME_FORMAT_MSB_FIRST) ? SPI_CR_DIR_Msk : (uint32_t) 0u;
    configReg |= (cfg->clockPolarity == MSPI_CLOCK_POLARITY_1      ) ? SPI_CR_CPOL_Msk     : (uint32_t) 0u;
    configReg |= (cfg->clockPhase    == MSPI_CLOCK_PHASE_1         ) ? SPI_CR_CPHA_Msk     : (uint32_t) 0u;
    // disable NSS pin
    configReg |= ((uint32_t)SPI_CR_SSDIS_Msk);
    // if 16 bit set FF16 1, if not set 0 use 8 bit
    configReg |= (cfg->dataBits == MSPI_DATA_BITS_16) ? ((uint32_t) 1u << SPI_CR_SPDATL_Pos) : (uint32_t) 0u;
    spiUnit->CR.V32 = configReg;

    return MCAL_OK;
}


enum MCAL_Error MSPI_configureBaudrate(const struct MSPI_Channel *channel, uint32_t baudrate)
{
    const bool channelEnabled = ((channel->mspi->SPIx->CR.BIT.SPIEN) != 0u);
    uint32_t configReg;
    MSPI_disableChannel(channel);
    configReg = channel->mspi->SPIx->CR.V32;
    MCAL_RETURN_ON_ERROR(MSPI_setPresc(channel,baudrate, &configReg));
    channel->mspi->SPIx->CR.V32 = configReg;
    if(channelEnabled)
    {
        MSPI_enableChannel(channel);
    }
    return MCAL_OK;
}


void MSPI_enableChannel(const struct MSPI_Channel *channel)
{
    uint32_t configReg = channel->mspi->SPIx->CR.V32;
    configReg |= (SPI_CR_SPIEN_Msk | SPI_CR_MSTR_Msk);
    channel->mspi->SPIx->CR.V32 = configReg;
}


void MSPI_disableChannel(const struct MSPI_Channel *channel)
{
    uint32_t configReg;
    SPI_TypeDef* spiUnit = channel->mspi->SPIx;
    while((spiUnit->FR.BIT.BUSY) != 0u) { }
    configReg = spiUnit->CR.V32;
    configReg &= ~((uint32_t)SPI_CR_SPIEN_Msk | SPI_CR_MSTR_Msk);
    spiUnit->CR.V32 = configReg;
    while((spiUnit->CR.V32 & ((uint32_t)SPI_CR_SPIEN_Msk | SPI_CR_MSTR_Msk)) != 0u) { }
}

void MSPI_writeDataReg(const struct MSPI_Channel *channel, uint32_t data)
{
    if(channel->mspi->SPIx->CR.V32 & SPI_CR_SPDATL_Msk)
    {
        channel->mspi->SPIx->TDR = (uint16_t)data;
    } else
    {   
        // 928:  explicit cast from 'volatile uint32_t *'to 'volatile uint8_t *'(misra advisory)
        *((volatile uint8_t *)&channel->mspi->SPIx->TDR) = (uint8_t)data;/*lint !e928 */
    }
}


uint16_t MSPI_readDataReg(const struct MSPI_Channel *channel)
{
    uint16_t data = (uint16_t)channel->mspi->SPIx->RDR;
    return data;
}


uint32_t MSPI_getStatusReg(const struct MSPI_Channel *channel)
{
    return channel->mspi->SPIx->FR.V32;
}


void MSPI_clearErrorReg(const struct MSPI_Channel *channel)
{
    channel->mspi->SPIx->FR.V32 |= SPI_FR_MODFC_Msk | SPI_FR_RXOVC_Msk | SPI_FR_WCOLC_Msk;
}


uint32_t MSPI_getStatusRegMaskForEvent(enum MCAL_Event event)
{
    if(event == MCAL_EVENT_READ_BYTE)
    {
        return SPI_FR_SPRI_Msk;
    }
    else if(event == MCAL_EVENT_WRITE_BYTE)
    {
        return SPI_FR_SPTI_Msk;
    }
    else if(event == MCAL_EVENT_ERROR)
    {
        return MSPI_ERROR_ANY_MASK;
    }
    else
    {
        return 0u;
    }
}


enum MSPI_IRQ_Index MSPI_IRQ_getIndex(const struct MSPI_Channel *channel)
{
    return channel->mspi->index;
}


void MSPI_IRQ_enable(const struct MSPI_Channel *channel)
{
    NVIC_EnableIRQ(channel->mspi->SPI_IRQn);
}


void MSPI_IRQ_enableEvent(const struct MSPI_Channel *channel, enum MCAL_Event event)
{
    SPI_TypeDef *spiPeriph = channel->mspi->SPIx;
    uint32_t configReg = spiPeriph->CR.V32;
    // Note: error event always enable
    if(MCAL_EVENT_READ_BYTE == event)
    {
        configReg |= SPI_CR_SPRIE_Msk;
    }
    else if(MCAL_EVENT_WRITE_BYTE == event)
    {
        configReg |= SPI_CR_SPTIE_Msk;
    }
    else {/* No valid event */}
    spiPeriph->CR.V32 = configReg;
}


void MSPI_IRQ_disableEvent(const struct MSPI_Channel *channel, enum MCAL_Event event)
{
    SPI_TypeDef *spiPeriph = channel->mspi->SPIx;
    uint32_t configReg = spiPeriph->CR.V32;
    // Note: error event cannot disable
    if(MCAL_EVENT_READ_BYTE == event)
    {
        configReg &= ~SPI_CR_SPRIE_Msk;
    }
    else if(MCAL_EVENT_WRITE_BYTE == event)
    {
        configReg &= ~SPI_CR_SPTIE_Msk;
    }
    else {/* No valid event */}
    spiPeriph->CR.V32 = configReg;
}


bool MSPI_isRxOngoing(const struct MSPI_Channel *channel)
{
    return (MSPI_isBusy(channel->mspi) || (MDIO_read(channel->miso) == false)) ? true : false;
}


bool MSPI_isTxOngoing(const struct MSPI_Channel *channel)
{
    return ((channel->mspi->SPIx->FR.BIT.BUSY) != 0u);
}


void MSPI_clearTxFlag(const struct MSPI_Channel *channel)
{
    /* TXE bit is cleared and set automatically */
    (void) *channel;
}


void MSPI_clearRxFlag(const struct MSPI_Channel *channel)
{
    /* RXNE bit is cleared automatically after read all received data */
    (void) *channel;
}


void MSPI_enableDMA(const struct MSPI_Periph *periph, enum MSPI_DmaDirection dmaDir)
{
    if((dmaDir == MSPI_DMA_DIRECTION_RX) || (dmaDir == MSPI_DMA_DIRECTION_TX_RX))
    {
        periph->SPIx->CR.V32 |= SPI_CR_SPDMAR_Msk;
    }

    if((dmaDir == MSPI_DMA_DIRECTION_TX) || (dmaDir == MSPI_DMA_DIRECTION_TX_RX))
    {
        periph->SPIx->CR.V32 |= SPI_CR_SPDMAT_Msk;
    }
}


void MSPI_disableDMA(const struct MSPI_Periph *periph)
{
    periph->SPIx->CR.V32 &= ~((uint32_t)SPI_CR_SPDMAR_Msk | SPI_CR_SPDMAT_Msk);
}


/******************************************************************************/
/* PRIVATE FUNCTION DEFINITIONS                                               */
/******************************************************************************/
static bool MSPI_isBusy(const struct MSPI_Periph *mspi)
{
    return ((mspi->SPIx->FR.BIT.BUSY) != 0u);
}


static enum MCAL_Error MSPI_setPresc(const struct MSPI_Channel *channel, uint32_t requiredSclkClock, uint32_t *returnValue)
{
    (void)*channel;
    uint16_t possibleDivider;
    uint32_t clkHz;
    uint32_t neededPresc;
    uint32_t presc = 0u;;
  #ifdef MCAL_MPCM_INCLUDED
    //APB1 -> MPCM_CLOCK_PCLK1
    clkHz = (uint32_t)MPCM_getClockFreq(MPCM_CLOCK_PCLK1);
  #else
    clkHz = PCLOCK1 * 1000000u;

  #endif
    if(clkHz > 0u)
    {
        // sh30f has these dividers 2u, 4u, 8u, 16u, 32u, 64u, 128u, 256u, 512u, 1024u
        neededPresc = MTDIV_div_32_32(clkHz, requiredSclkClock);
        if(((clkHz) % requiredSclkClock) != 0u)
        {
            neededPresc++;
        }
        possibleDivider = 2u;
        const uint16_t prescMaxValue = 9u;
        while((possibleDivider < neededPresc) && (presc < prescMaxValue))
        {
            presc++;
            possibleDivider <<= 1u; // possibleDivider *= 2
        }
        *returnValue &= ~SPI_CR_SPR_Msk;           // clean prescaler bits
        *returnValue |= presc << SPI_CR_SPR_Pos;    // set prescaler bits
        return MCAL_OK;
    }
    return MCAL_ERROR;  // HINT: Please check if MPCM is initialized
}


#ifdef CURRENTLY_UNUSED_INTERFACE
static enum MCAL_Error MSPI_getChannelError(const struct MSPI_Channel *channel)
{
    (void)*channel;
    /* MiKi TODO */
    return MCAL_OK;
}
#endif


#ifdef MCAL_MDMA_INCLUDED
const struct MDMA_Link* MSPI_MDMA_getPeriphLink_TX(const struct MSPI_Periph *periph)
{
    return periph->mdmaLinkTXCfg;
}
const struct MDMA_Link* MSPI_MDMA_getPeriphLink_RX(const struct MSPI_Periph *periph)
{
    return periph->mdmaLinkRXCfg;
}
#endif 
