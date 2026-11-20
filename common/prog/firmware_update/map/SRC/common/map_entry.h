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

#ifndef MAP_ENTRY_H_
#define MAP_ENTRY_H_

#ifdef __cplusplus
extern "C" {
#endif

/******************************************************************************/
/* DOCUMENTATION                                                              */
/******************************************************************************/



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



/******************************************************************************/
/* PUBLIC FUNCTION DECLARATIONS                                               */
/******************************************************************************/

/**
*   Processes data, which have been received as MAP frame.
*   Checks, whether received data is long enough to be a map message.
*   Checks, whether tx buffer is big enough to sent map messages with the minimum length (only header).
*   If one of the named conditions is not fulfilled, the message is ignored.
*   Otherwise the message will be processed.
*   \param[in] rxData Received data
*   \param[in] maxRxLen Maximum length of data that can be received
*   \param[in] rxLen Length of data that have been received
*   \param[out] txData Data to be sent
*   \param[in] Maximum length of data to send
*   \param[out] txLen Length of data to be sent
*/
void MAP_ProcessMessage(const void *const rxData, uint16_t maxRxLen, uint16_t rxLen, void *const txData, uint16_t maxTxLen, uint16_t *const txLen);

#ifdef __cplusplus
}
#endif

#endif

