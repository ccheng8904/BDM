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
 *  COMP_ABBREV      DBGP
 ******************************************************************************/

#ifndef DEBUG_EXTENDED_PROTOCOL_H
#define DEBUG_EXTENDED_PROTOCOL_H

/******************************************************************************/
/* DOCUMENTATION                                                              */
/******************************************************************************/
/** \file
 *
 *  \ingroup  debug_extended
 *
 *  \brief    Public definitions and declarations of generic part of the
 *            debug_extended component: Protocol layer
 *
 *  \details  
 */

/******************************************************************************/
/* INCLUDES                                                                   */
/******************************************************************************/
#include <stdint.h>
#include "debug_extended/interface.h"
#include "dbgx_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/******************************************************************************/
/* PUBLIC PREPROCESSOR DEFINITIONS                                            */
/******************************************************************************/

/* Return stati: */
#define DBGP_STAT_NO_ERROR             DBGI_STAT_NO_ERROR
#define DBGP_STAT_NOTIFY_PROBLEM       ((int32_t)-1)
#define DBGP_STAT_ARRAY_LEN_PROBLEM    ((int32_t)-2)

/* Protocols: */
#define DBGP_PROTOCOL_NONE             ((uint8_t)0x00)
#define DBGP_PROTOCOL_TEXT             ((uint8_t)0x01)
#define DBGP_PROTOCOL_TCH_DER          ((uint8_t)0x02)

/* Length of the protocol frame array: */
#define DBGP_LEN_OF_PROT_FRAME_ARRAY   ((uint8_t)96)

/* Length of the string of level and scenario (string has no zero-termination): */
#define DBGP_LEN_ATTRIBUTES_STRING          ((uint8_t)15)  // Length does not include the postfix ": "
#define DBGP_LEN_ATTRIBUTES_STRING_POSTFIX  ((uint8_t)2)   // Length for the postfix ": "


/******************************************************************************/
/* PUBLIC TYPE DEFINITIONS                                                    */
/******************************************************************************/

/******************************************************************************/
/* PUBLIC FUNCTION DECLARATIONS                                               */
/******************************************************************************/

#if defined STS_SIM_TEST_CI
    extern DBGP_ReceiveDataFunctionOfApi_t   DBGP_ReceiveDataFunctionOfProt_CB;
#endif

/** \brief  Sends the data of the application protocol (e.g. touch data)
 *          towards the interface layer of debug_extended component
 *  \note   Do not use this function in the application
 *
 *  \param  DbgAttributes  Pointer to the string of indexes of level, scenario
 *                         module ID and other attributes needed for putting out
 *                         onto the debug_extended interface besides the debug_extended data
 *  \param  Module_ID  ID of the module (e.g touch), needed for the PC GUI tool
 *  \param  Function   Function of the debug_extended output data, needed for the PC GUI tool
 *  \param  Data       Pointer to payload (user data) of debug_extended output data for the PC GUI tool
 *  \param  DataLen    Number of bytes of payload (user data) of debug_extended output data for the PC GUI tool
 *  \return Error status (==0: no error; < 0: error)
 */
extern int32_t DBGP_sendProtData(const char *DbgAttributes, uint16_t Module_ID,
                                 uint16_t Function, const uint8_t *Data, uint16_t DataLen);

/** \brief  Notifies the receive data function for application protocol (e.g. touch data)
 *  \note   Do not use this function in the application
 *
 *  \param  ReceiveDataFunctionOfProt_CB  Function pointer to the API layer
 *                                        internal callback function for Rx-data
 *  \return Error status (==0: no error; < 0: error)
 */
extern int32_t DBGP_notifyProtReceiveData_CB(DBGP_ReceiveDataFunctionOfApi_t ReceiveDataFunctionOfProt_CB);

/** \brief  Sends an array of characters towards the interface layer of debug_extended component
 *  \note   Do not use this function in the application
 *
 *  \param  DbgAttributes  Pointer to the string of indexes of level, scenario
 *                         module ID and other attributes needed for putting out
 *                         onto the debug_extended interface besides the debug_extended data
 *  \param  CharArray              Pointer to the character array
 *  \param  NumberOfArrayElements  Number of elements in the array
 *  \return Error status (==0: no error; < 0: error)
 */
extern int32_t DBGP_writeProtCharArray(const char *DbgAttributes, const char *CharArray, uint16_t NumberOfArrayElements);

/** \brief  Gets a NULL-terminated string from the protocol layer of debug_extended component
 *  \note   Do not use this function in the application
 *
 *  \param  String  Pointer to the string
 *  \param  MaxLen  Maximum number of bytes (characters) of the string,
 *                  NULL termination character included
 *  \return >= 0: Number of bytes of the string, NULL included;
 *          -1  : Error
 *          -2  : Callback function will be triggered, when read process is finished
 */
extern int32_t DBGP_getProtString(char *String, uint16_t MaxLen);

/** \brief  Gets a NULL-terminated string from the interface layer of debug_extended component
 *  \note   Do not use this function in the application
 *
 *  \param  Data     Pointer to the allocated memory
 *  \param  DataLen  Maximum number of bytes (characters) of the string, NULL termination character included
 *  \return >= 0: Number of bytes of the string, NULL included; -1: Error
 */
extern void DBGP_getProtString_CB(uint8_t *Data, uint16_t DataLen);

/** \brief  Sets the protocols of debug_extended data
 *  \note   One protocol per debug_extended interface. More than one interface is
 *          allowed (e.g. for EMC purposes; interface connections are antennas!).
 *  \note   Do not use this function in the application
 *
 *  \param  Protocols          Pointer to array of the protocol IDs
 *  \param  NumberOfProtocols  Number of elements in the array
 */
extern void DBGP_setProtocolConfiguration(const uint8_t *Protocols, uint8_t NumberOfProtocols, bool DbgAttributesActive);


#ifdef __cplusplus
}
#endif

#endif   /* From: ifndef DEBUG_EXTENDED_PROTOCOL_H */

