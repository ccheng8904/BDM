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
/** \file     mspi_mc.h
 *
 *  \ingroup  MCAL
 *
 *  \brief    Public definitions and declarations for unit MSPI.
 *
 */

#ifndef MSPI_MC_H
#define MSPI_MC_H

#ifdef __cplusplus
extern "C" {
#endif

/******************************************************************************/
/* INCLUDES                                                                   */
/******************************************************************************/

#include <assert.h>
#include "BSH_stdinc.h"
#include "processor.h"
#include "mcal/mspi_types.h"
#include "mcal/mdio.h"
#include "mcal/msup.h"
#ifdef MCAL_MDMA_INCLUDED
#include "mcal/mdma_types.h"
#endif 

/******************************************************************************/
/* PUBLIC DEFINITIONS                                                         */
/******************************************************************************/

/******************************************************************************/
/* PUBLIC TYPE DEFINITIONS                                                    */
/******************************************************************************/

enum MSPI_IRQ_Index
{
    MSPI0_IRQ_INDEX,
    MSPI_IRQ_CNT
};


struct MSPI_Periph
{
    SSP0_Type                    *SSPx;
    enum   MSPI_IRQ_Index        index;
    const  IRQn_Type             SSP_IRQn;
    uint32_t                     clockMask;
#ifdef MCAL_MDMA_INCLUDED
    const struct MDMA_Link       *mdmaLinkTXCfg;
    const struct MDMA_Link       *mdmaLinkRXCfg;
#endif
};


struct MSPI_Channel
{
    const struct MSPI_Periph         *mspi;
    const struct MDIO_Channel        *sclk;
    const struct MDIO_Channel        *miso;
    const struct MDIO_Channel        *mosi;
    const struct MDIO_Channel        *cs;
};


/******************************************************************************/
/* PUBLIC DATA DECLARATIONS                                                   */
/******************************************************************************/

#if defined(MSPI0_PRESENT)
extern const struct MSPI_Periph MSPI0;
#endif


/******************************************************************************/
/* PUBLIC FUNCTION DECLARATIONS                                               */
/******************************************************************************/
extern void MSPI_clearTxFlag(const struct MSPI_Channel *channel);
extern void MSPI_clearRxFlag(const struct MSPI_Channel *channel);

/** Function enabling of DMA event generation.
 *  \param periph: pointer to peripheral instance
 *  \param dmaType: enum element describing DMA direction
 */
extern void MSPI_enableDMA(const struct MSPI_Periph *periph, enum MSPI_DmaDirection dmaDir);

/** Function disabling of DMA event generation.
 *  \param periph: pointer to peripheral instance
 */
extern void MSPI_disableDMA(const struct MSPI_Periph *periph);

#ifdef MCAL_MDMA_INCLUDED
/** Function for getting MDMA_Link when using MDMA for writing data to SPI.
 *  \param periph: pointer to peripheral instance
 */
extern const struct MDMA_Link* MSPI_MDMA_getPeriphLink_TX(const struct MSPI_Periph *periph);
/** Function for getting MDMA_Link when using MDMA for reading data from SPI.
 *  \param periph: pointer to peripheral instance
 */
extern const struct MDMA_Link* MSPI_MDMA_getPeriphLink_RX(const struct MSPI_Periph *periph);
#endif

#ifdef __cplusplus
}
#endif

#endif // MSPI_MC_H
