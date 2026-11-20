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
/** \file
 *  Implementation of MP-MEM access interface.
 */
/******************************************************************************/
/* INCLUDES                                                                   */
/******************************************************************************/
#include "bsh_stdinc.h"
#include "map_mem_access_interface.h"
#include "firmware_update/mal/mal_command.h"

/******************************************************************************/
/* PRIVATE DEFINITIONS                                                        */
/******************************************************************************/

struct MAP_MemAccessInterface MAP_MemAccess;

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

void MAP_MemAccess_Init(void)
{
    MAP_MemAccess.devInfo=MAL_CMD_DevInfo;
    MAP_MemAccess.open=MAL_CMD_Open;
    MAP_MemAccess.close=MAL_CMD_Close;
    MAP_MemAccess.read=MAL_CMD_Read;
    MAP_MemAccess.writeStart=MAL_CMD_WriteStart;
    MAP_MemAccess.writeAppend=MAL_CMD_WriteAppend;
    MAP_MemAccess.writePrematurelyEnd = MAL_CMD_WritePrematurelyEnd;
}

/******************************************************************************/
/* PRIVATE FUNCTION DEFINITIONS                                               */
/******************************************************************************/

