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
 *  PROCESSOR        CMS32M67xx
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
 *  \details  The CMS32M67xx provides one SPI unit (SSP0). The configuration
 *            and the data transfer are done by the vendor driver functions
 *            (SSP_...).
 *
 */

/******************************************************************************/
/* INCLUDES                                                                   */
/******************************************************************************/

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

/* SSP status register bits of the CMS32M67xx */
#define MSPI_STAT_RNE_Msk        0x00000004U
#define MSPI_STAT_TNF_Msk        0x00000002U
#define MSPI_STAT_BSY_Msk        0x00000010U

/******************************************************************************/
/* PRIVATE FUNCTION DECLARATIONS AND PRIVATE MACRO FUNCTION DEFINITIONS       */
/******************************************************************************/

static bool MSPI_isBusy(const struct MSPI_Periph *mspi);
static uint32_t MSPI_getPeripheralClock(const struct MSPI_Channel *channel);

/******************************************************************************/
/* PUBLIC DATA DEFINITIONS                                                    */
/******************************************************************************/

#ifdef MCAL_MDMA_INCLUDED
#if defined(MSPI0_PRESENT)
/* The SSP data register is the DMA endpoint for both directions. The
 * CMS32M67xx MDMA configures channel addresses at runtime, so the link is
 * described here instead of a pre-defined link table. */
static const struct MDMA_Link MSPI0_MDMA_LinkTX =
{
    (uint32_t)&SSP->DAT,
    MDMA_WIDTH_8_BIT,
    false
};
static const struct MDMA_Link MSPI0_MDMA_LinkRX =
{
    (uint32_t)&SSP->DAT,
    MDMA_WIDTH_8_BIT,
    false
};
#endif
#endif

#if defined(MSPI0_PRESENT)
const struct MSPI_Periph MSPI0 = 
{
    SSP,
    MSPI0_IRQ_INDEX,
    SSP0_IRQn,
    CGC_PER12Periph_SPI
#ifdef MCAL_MDMA_INCLUDED
    ,&MSPI0_MDMA_LinkTX
    ,&MSPI0_MDMA_LinkRX
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
    uint32_t frameMode;
    uint32_t clockPolarity;
    uint32_t clockPhase;
    uint32_t dataLength;

    /* enable the SPI clock and reset the unit to its default value */
    CGC_PER12PeriphClockCmd(channel->mspi->clockMask, ENABLE);
    SSP_Stop();

    frameMode     = SSP_FRAME_SPI;
    clockPolarity = (cfg->clockPolarity == MSPI_CLOCK_POLARITY_1) ? SSP_CPO_1 : SSP_CPO_0;
    clockPhase    = (cfg->clockPhase == MSPI_CLOCK_PHASE_1) ? SSP_CPHA_1 : SSP_CPHA_0;
    dataLength    = (cfg->dataBits == MSPI_DATA_BITS_16) ? SSP_DAT_LENGTH_16 : SSP_DAT_LENGTH_8;

    SSP_ConfigRunMode(frameMode, clockPolarity, clockPhase, dataLength);
    SSP_EnableMasterMode();
    SSP_DisableMasterAutoControlCS();

    /* the configuration is applied on the transition to the enabled state */
    MCAL_RETURN_ON_ERROR(MSPI_configureBaudrate(channel, cfg->clockFreq));

    /* select the SPI pins of the channel */
    if(channel->sclk != NULL)
    {
        (void)MDIO_init(channel->sclk, &MDIO_ALTERNATE_FUNCTION);
    }
    if(channel->miso != NULL)
    {
        (void)MDIO_init(channel->miso, &MDIO_ALTERNATE_FUNCTION);
    }
    if(channel->mosi != NULL)
    {
        (void)MDIO_init(channel->mosi, &MDIO_ALTERNATE_FUNCTION);
    }
    if(channel->cs != NULL)
    {
        (void)MDIO_init(channel->cs, &MDIO_OUTPUT);
    }

    return MCAL_OK;
}


enum MCAL_Error MSPI_configureBaudrate(const struct MSPI_Channel *channel, uint32_t baudrate)
{
    uint32_t clkHz = MSPI_getPeripheralClock(channel);
    uint32_t divider;
    uint32_t clkM;
    uint32_t clkN;

    if((baudrate == 0u) || (clkHz == 0u))
    {
        return MCAL_ERROR_PARAM;
    }

    /* SSP clock = fPCLK / (CPSDVSR * (1 + SCR)), use CPSDVSR as divider and
     * SCR = 0 */
    divider = clkHz / baudrate;
    if(divider < 2u)
    {
        divider = 2u;
    }
    divider &= (uint32_t)(~1u);     /* CPSDVSR has to be even */
    if(divider > 254u)
    {
        divider = 254u;
    }

    clkM = divider;
    clkN = 0u;

    SSP_ConfigClk(clkM, clkN);

    return MCAL_OK;
}


void MSPI_enableChannel(const struct MSPI_Channel *channel)
{
    (void)*channel;

    SSP_Start();
}


void MSPI_disableChannel(const struct MSPI_Channel *channel)
{
    while(MSPI_isBusy(channel->mspi)) { }

    SSP_Stop();
}


void MSPI_writeDataReg(const struct MSPI_Channel *channel, uint32_t data)
{
    SSP_SendData(data);
}


uint16_t MSPI_readDataReg(const struct MSPI_Channel *channel)
{
    (void)*channel;

    return (uint16_t)SSP_GetData();
}


uint32_t MSPI_getStatusReg(const struct MSPI_Channel *channel)
{
    return channel->mspi->SSPx->STAT;
}


void MSPI_clearErrorReg(const struct MSPI_Channel *channel)
{
    (void)*channel;
}


uint32_t MSPI_getStatusRegMaskForEvent(enum MCAL_Event event)
{
    if(event == MCAL_EVENT_READ_BYTE)
    {
        return MSPI_STAT_RNE_Msk;
    }
    else if(event == MCAL_EVENT_WRITE_BYTE)
    {
        return MSPI_STAT_TNF_Msk;
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
    NVIC_EnableIRQ(channel->mspi->SSP_IRQn);
}


void MSPI_IRQ_enableEvent(const struct MSPI_Channel *channel, enum MCAL_Event event)
{
    if(event == MCAL_EVENT_READ_BYTE)
    {
        SSP_EnableReciveHalfInt();
    }
    else if(event == MCAL_EVENT_WRITE_BYTE)
    {
        SSP_EnableSendHalfInt();
    }
    else
    {
        /* nothing to do */
    }
}


void MSPI_IRQ_disableEvent(const struct MSPI_Channel *channel, enum MCAL_Event event)
{
    (void)*channel;

    if(event == MCAL_EVENT_READ_BYTE)
    {
        SSP_DisableReciveHalfInt();
    }
    else if(event == MCAL_EVENT_WRITE_BYTE)
    {
        SSP_DisableSendHalfInt();
    }
    else
    {
        /* nothing to do */
    }
}


bool MSPI_isRxOngoing(const struct MSPI_Channel *channel)
{
    return (MSPI_isBusy(channel->mspi)) ? true : false;
}


bool MSPI_isTxOngoing(const struct MSPI_Channel *channel)
{
    return ((channel->mspi->SSPx->STAT & MSPI_STAT_BSY_Msk) != 0u) ? true : false;
}


void MSPI_clearTxFlag(const struct MSPI_Channel *channel)
{
    (void)*channel;

    SSP_ClearSendHalfIntFlag();
}


void MSPI_clearRxFlag(const struct MSPI_Channel *channel)
{
    (void)*channel;

    SSP_ClearReceiveHalfIntFlag();
}


void MSPI_enableDMA(const struct MSPI_Periph *periph, enum MSPI_DmaDirection dmaDir)
{
    (void)periph;
    (void)dmaDir;

    CGC_PER10PeriphClockCmd(CGC_PER10Periph_DMA, ENABLE);
}


void MSPI_disableDMA(const struct MSPI_Periph *periph)
{
    (void)periph;
}

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


/******************************************************************************/
/* PRIVATE FUNCTION DEFINITIONS                                               */
/******************************************************************************/
static bool MSPI_isBusy(const struct MSPI_Periph *mspi)
{
    return ((mspi->SSPx->STAT & MSPI_STAT_BSY_Msk) != 0u) ? true : false;
}


static uint32_t MSPI_getPeripheralClock(const struct MSPI_Channel *channel)
{
    (void)*channel;

  #ifdef MCAL_MPCM_INCLUDED
    return (uint32_t)MPCM_getClockFreq(MPCM_CLOCK_PCLK0);
  #else
    return (uint32_t)SystemCoreClock;
  #endif
}

/******************************************************************************/
/* VENDOR DRIVER EXTENSIONS                                                   */
/******************************************************************************/
/* The interrupt mask of the SSP unit is programmed by the IMSC register, the
 * vendor driver provides no functions for it. */
void SSP_EnableSendHalfInt(void)
{
    SSP->IMSC |= 0x00000008U;     /* TXIM  */
}


void SSP_DisableSendHalfInt(void)
{
    SSP->IMSC &= ~0x00000008U;    /* TXIM  */
}


void SSP_EnableReciveHalfInt(void)
{
    SSP->IMSC |= 0x00000004U;     /* RXIM  */
}


void SSP_DisableReciveHalfInt(void)
{
    SSP->IMSC &= ~0x00000004U;    /* RXIM  */
}


void SSP_EnableReciveTimerOverflowInt(void)
{
    SSP->IMSC |= 0x00000040U;     /* RTIM  */
}


void SSP_DisableReciveTimerOverflowInt(void)
{
    SSP->IMSC &= ~0x00000040U;    /* RTIM  */
}


uint32_t SSP_GetSendHalfIntFlag(void)
{
    return ((SSP->MIS & 0x00000008U) != 0u) ? 1u : 0u;
}


void SSP_ClearSendHalfIntFlag(void)
{
    SSP->ICLR = 0x00000008U;
}


uint32_t SSP_GetReceiveHalfIntFlag(void)
{
    return ((SSP->MIS & 0x00000004U) != 0u) ? 1u : 0u;
}


void SSP_ClearReceiveHalfIntFlag(void)
{
    SSP->ICLR = 0x00000004U;
}


uint32_t SSP_GetReciveTimerOverflowIntFlag(void)
{
    return ((SSP->MIS & 0x00000040U) != 0u) ? 1u : 0u;
}


void SSP_ClearReciveTimerOverflowIntFlag(void)
{
    SSP->ICLR = 0x00000040U;
}