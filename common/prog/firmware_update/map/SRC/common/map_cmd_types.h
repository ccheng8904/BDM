/*******************************************************************************
*   Copyright (c) 2017 BSH Hausgeraete GmbH,
*   Carl-Wery-Str. 34, 81739 Munich, Germany, www.bsh-group.de
*
*   All rights reserved. This program and the accompanying materials
*   are protected by international copyright laws.
*   Please contact copyright holder for licensing information.
*
********************************************************************************
*   PROJECT          FWU
*   COMP_ABBREV      MAP
*******************************************************************************/

#ifndef MAP_CMD_TYPES_H_
#define MAP_CMD_TYPES_H_

#ifdef __cplusplus
extern "C" {
#endif

/******************************************************************************/
/* DOCUMENTATION                                                              */
/******************************************************************************/
/** \file
 *  MAP command type definitions.
 */
/******************************************************************************/
/* INCLUDES                                                                   */
/******************************************************************************/
#include "bsh_stdinc.h"
#include <stdint.h>

/******************************************************************************/
/* PUBLIC DEFINITIONS                                                         */
/******************************************************************************/



/******************************************************************************/
/* PUBLIC TYPE DEFINITIONS                                                    */
/******************************************************************************/


/**
*   \brief Header of MAP request
*/
struct MAP_ReqHeader
{
    uint8_t id;
    uint8_t seqCnt;
};


/**
*   \brief Header of MAP response
*/
struct MAP_RespHeader
{
    uint8_t id;
    uint8_t seqCnt;
    uint8_t status;
};


/**
*   \brief Received and transmitted data with actual and maximum length
*/
struct MAP_Arguments
{
    const void *const rxData;
    uint16_t rxLen;
    void *const txData;
    uint16_t *const txLen;
    uint16_t maxRxPayload;
    uint16_t maxTxPayload;
};

/******************************************************************************/
/* PUBLIC FUNCTION DECLARATIONS                                               */
/******************************************************************************/

#ifdef __cplusplus
}
#endif

#endif
