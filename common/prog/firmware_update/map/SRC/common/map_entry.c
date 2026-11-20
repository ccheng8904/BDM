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


/******************************************************************************/
/* DOCUMENTATION                                                              */
/******************************************************************************/



/******************************************************************************/
/* INCLUDES                                                                   */
/******************************************************************************/
#include "bsh_stdinc.h"
#include "map_entry.h"
#include "map_cmd.h"

/******************************************************************************/
/* PRIVATE DEFINITIONS                                                        */
/******************************************************************************/



/******************************************************************************/
/* PRIVATE TYPE DEFINITIONS                                                   */
/******************************************************************************/



/******************************************************************************/
/* PRIVATE FUNCTION DECLARATIONS AND PRIVATE MACRO FUNCTION DEFINITIONS       */
/******************************************************************************/



/******************************************************************************/
/* PRIVATE DATA DEFINITIONS                                                   */
/******************************************************************************/



/******************************************************************************/
/* PUBLIC FUNCTION DEFINITIONS                                                */
/******************************************************************************/

void MAP_ProcessMessage(const void *const rxData, uint16_t maxRxLen, uint16_t rxLen, void *const txData, uint16_t maxTxLen, uint16_t *const txLen)
{
     if(rxLen<(uint8_t)sizeof(struct MAP_ReqHeader))
    {
        *txLen=0U;/*Not enough data, to form shortest possibble MAP frame (only header).*/
    }
    else if(maxTxLen<(uint8_t)sizeof(struct MAP_RespHeader))
    {
        *txLen=0U;/*Not enough tx buffer, to send shortest possible response (only header).*/
    }
    else
    {   /*Axivion Next Line MisraC2012-11.5: Tested to work correctly on all platforms */
        const struct MAP_ReqHeader *req=(const struct MAP_ReqHeader*)rxData;
        MAP_CMD_Func_t MAP_CMD_callbackFunction=MAP_CMD_GetCmdExecFunc(req->id);
        struct MAP_Arguments arguments=
        {
            .rxData=rxData,
            .rxLen=rxLen,
            .txData=txData,
            .txLen=txLen,
            .maxRxPayload=maxRxLen,
            .maxTxPayload=maxTxLen
        };
        MAP_CMD_callbackFunction(arguments);
    }
}

/******************************************************************************/
/* PRIVATE FUNCTION DEFINITIONS                                               */
/******************************************************************************/

