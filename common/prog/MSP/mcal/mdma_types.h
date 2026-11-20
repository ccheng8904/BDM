/*******************************************************************************
 *  Copyright (c) 2017 BSH Hausgeraete GmbH,
 *  Carl-Wery-Str. 34, 81739 Munich, Germany, www.bsh-group.de
 *
 *  All rights reserved. This program and the accompanying materials
 *  are protected by international copyright laws.
 *  Please contact copyright holder for licensing information.
 *
 *******************************************************************************
 *  PROJECT          MCAL
 *  COMP_ABBREV      MDMA
 ******************************************************************************/


#ifndef MDMA_TYPES_H
#define MDMA_TYPES_H

#ifdef __cplusplus
extern "C" {
#endif

/******************************************************************************/
/* DOCUMENTATION                                                              */
/******************************************************************************/
/** \file
 *
 *  \ingroup  MCAL
 *
 *  \brief    Public definitions and type definitions for unit MDMA.
 *
 */


/******************************************************************************/
/* INCLUDES                                                                   */
/******************************************************************************/
#include "BSH_stdinc.h"
#include "mcal_types.h"

/******************************************************************************/
/* PUBLIC DEFINITIONS                                                         */
/******************************************************************************/


/******************************************************************************/
/* PUBLIC TYPE DEFINITIONS                                                    */
/******************************************************************************/

/** DMA direction */
enum MDMA_Direction
{
    MDMA_DIRECTION_MEM_TO_MEM,
    MDMA_DIRECTION_MEM_TO_PERIPH,
    MDMA_DIRECTION_PERIPH_TO_MEM,
    MDMA_DIRECTION_PERIPH_TO_PERIPH
};


/** DMA transfer mode */
enum MDMA_Mode
{
    MDMA_MODE_SINGLE,               ///< Single shot DMA transfer
    MDMA_MODE_CIRCULAR              ///< Circular DMA transfer
};


/** Data width enumeration */
enum MDMA_DataWidth
{
    MDMA_WIDTH_8_BIT,
    MDMA_WIDTH_16_BIT,
    MDMA_WIDTH_32_BIT
};


/** MDMA source/destination link structure */
struct MDMA_Link
{
    uint32_t                addr;       ///< Address of source/destination
    enum MDMA_DataWidth     dataWidth;  ///< Width of data (8/16/32-bit)
    bool                    increment;  ///< Enable increment of address
};


/** MDMA generic configuration structure */
struct MDMA_ChannelConfig
{
    enum MDMA_Direction     direction;
    enum MDMA_Mode          mode;
    uint16_t                dataLen;
    
    struct MDMA_Link        src;
    struct MDMA_Link        dst;
};


#ifdef __cplusplus
}
#endif

#endif // MDMA_TYPES_H
