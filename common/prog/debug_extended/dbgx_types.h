/*******************************************************************************
 *  Copyright (c) 2017 BSH Hausgeraete GmbH,
 *  Carl-Wery-Str. 34, 81739 Munich, Germany, www.bsh-group.de
 *
 *  All rights reserved. This program and the accompanying materials
 *  are protected by international copyright laws.
 *  Please contact copyright holder for licensing information.
 *
 *******************************************************************************
 *  PROJECT          Generic SW
 *  COMP_ABBREV      DBGX
 ******************************************************************************/

#ifndef DEBUG_EXTENDED_TYPES_H
#define DEBUG_EXTENDED_TYPES_H

/******************************************************************************/
/* DOCUMENTATION                                                              */
/******************************************************************************/
/** \file
 *
 *  \ingroup  debug_extended
 *
 *  \brief    Public types of generic part of the debug_extended component
 *
 *  \details
 */


/******************************************************************************/
/* INCLUDES                                                                   */
/******************************************************************************/
#include "bsh_stdinc.h"
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/******************************************************************************/
/* GLOBAL DEFINITIONS                                                         */
/******************************************************************************/


/******************************************************************************/
/* GLOBAL TYPE DEFINITIONS                                                    */
/******************************************************************************/

/** For callback functions of the debug_extended user input: */
typedef void (*DBGX_InputCallbackFunctions_t)(uint8_t *Data, uint16_t DataLen);

/** For API callback function of debug_extended rx-data of application protocol (e.g. touch): */
typedef void (*DBGX_ReceiveDataFunctionOfApi_t)(uint16_t Function, uint8_t *Data, uint16_t DataLen);

/** For callback function of debug_extended rx-data of application protocol (e.g. touch) */
typedef void (*DBGP_ReceiveDataFunctionOfApi_t)(uint16_t Module_ID, uint16_t Function, uint8_t *Data, uint16_t DataLen);

/** For solving of detected exceptional errors: */
typedef void (*DBGE_ErrorSolvingFunction_t)(uint32_t ErrorCode, uint32_t ErrorParameter);




/** structure of the callback elements (pointer to arrays in the config-area) */
struct DBGX_ApiCallbacksGen_s
{
    uint16_t                         MaxNumberModules_Rx;
    DBGX_ReceiveDataFunctionOfApi_t *ReceiveDataFunctionsOfApi_CB;
    uint16_t                        *ModuleIDs;
};

/** structure of the filters of the debug_extended Tx-data (pointer to arrays in the config-area) */
struct DBGX_FilterGen_s
{
    uint16_t NumberFilterFlagWords;
    uint32_t *FilterFlagWords;
};

/** structure of the saved exceptional errors */
struct DBGE_SavedErrors_s
{
    uint32_t FirstError;
    uint32_t LastError;
    uint32_t FirstErrorParameter;
    uint32_t LastErrorParameter;
};

/** structure of the protocol layer configuration */
struct DBGP_ProtocolConfig_s
{
    const uint8_t *Protocols;
    uint8_t NumberOfProtocols;
    bool DbgAttributesActive;
};

#ifdef __cplusplus
}
#endif


#endif /* DEBUG_EXTENDED_TYPES_H */


