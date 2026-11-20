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

#ifndef MAP_CMD_H_
#define MAP_CMD_H_

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
#include <stddef.h>
#include <stdbool.h>
#include "map_cmd_types.h"

/******************************************************************************/
/* PUBLIC DEFINITIONS                                                         */
/******************************************************************************/



/******************************************************************************/
/* PUBLIC TYPE DEFINITIONS                                                    */
/******************************************************************************/

/**
*   \brief Type of function to handle received MAP command
*/
typedef void (*MAP_CMD_Func_t)(struct MAP_Arguments args);

/******************************************************************************/
/* PUBLIC FUNCTION DECLARATIONS                                               */
/******************************************************************************/

/**
*   \brief Analyses for MAP_ProcessMessage, which MAP command has been received.
*   \param[in] Identifier
*   \return Function of type MAP_CMD_Func_t
*   \retval MAP_CMD_GetDeviceInfo to handle the Get Device Info Reuqest
*   \retval MAP_CMD_SetUpModule to handle the Setup Module Request
*   \retval MAP_CMD_Read to handle the Read request
*   \retval MAP_CMD_WriteStart to handle the Write Start Request
*   \retval MAP_CMD_WriteAppend to handle the Write Append Request
*   \retval MAP_CMD_WritePrematurelyEnd to handle the Prematurely Write End Request
*   \retval MAP_CMD_HandleUnknownCmd to handle unknown request identifiers and also received MAP messages with response identifiers.
*/
MAP_CMD_Func_t MAP_CMD_GetCmdExecFunc(uint8_t id);

#ifdef __cplusplus
}
#endif

#endif
