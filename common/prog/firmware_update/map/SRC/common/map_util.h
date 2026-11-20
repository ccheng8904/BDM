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

#ifndef MAP_UTIL_H_
#define MAP_UTIL_H_

#ifdef __cplusplus
extern "C" {
#endif

/******************************************************************************/
/* DOCUMENTATION                                                              */
/******************************************************************************/
/** \file
 *  definitions for map utilities.
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
*   \brief Big endian byte structure of sixteen bit value
*/
struct BIGE_Short
{
    uint8_t hi;
    uint8_t lo;
};/*lint -e9045 "non-hidden definition of type..." not hidden by intention */

/**
*   \brief Big endian byte structure of thirty two bit value
*/
struct BIGE_Long
{
    uint8_t hh;
    uint8_t hl;
    uint8_t lh;
    uint8_t ll;
};/*lint -e9045 "non-hidden definition of type..." not hidden by intention */

/******************************************************************************/
/* PUBLIC FUNCTION DECLARATIONS                                               */
/******************************************************************************/

/**
*   \brief Transforms big endian byte structure into sixteen bit value.
*   \param[in] Byte structure
*   \return Sixteen bit value
*/
static inline uint16_t BIGE_ToShort(const struct BIGE_Short *const bytes);
static inline uint16_t BIGE_ToShort(const struct BIGE_Short *const bytes)
{
    return ((uint16_t)bytes->hi<<8)|((uint16_t)bytes->lo);
}

/**
*   \brief Transforms big endian byte structure into thirty two bit value.
*   \param[in] Byte structure
*   \return Thirty two bit value
*/
static inline uint32_t BIGE_ToLong(const struct BIGE_Long *const bytes);
static inline uint32_t BIGE_ToLong(const struct BIGE_Long *const bytes)
{
    return ((uint32_t)bytes->hh<<24)|((uint32_t)bytes->hl<<16)|((uint32_t)bytes->lh<<8)|((uint32_t)bytes->ll);
}

/**
*   \brief Transforms sixteen bit value into big endian byte structure
*   \param[out] Byte structure to be filled
*   \param[in] Value to be transformed into bytes
*/
static inline void BIGE_SetShortBytes(struct BIGE_Short *const bytes, uint16_t value);
static inline void BIGE_SetShortBytes(struct BIGE_Short *const bytes, uint16_t value)
{
    bytes->hi=(uint8_t)(value>>8);
    bytes->lo=(uint8_t)value;
}

/**
* \brief Transforms thirty two bit value into big endian byte structure
* \param[out] Byte structure to be filled
* \param[in] Value to be transformed into bytes
*/
static inline void BIGE_SetLongBytes(struct BIGE_Long *const bytes, uint32_t value);
static inline void BIGE_SetLongBytes(struct BIGE_Long *const bytes, uint32_t value)
{
    bytes->hh=(uint8_t)(value>>24);
    bytes->hl=(uint8_t)(value>>16);
    bytes->lh=(uint8_t)(value>>8);
    bytes->ll=(uint8_t)value;
}

#ifdef __cplusplus
}
#endif

#endif

