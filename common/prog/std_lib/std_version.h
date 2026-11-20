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
 *  COMP_ABBREV      STDV
 ******************************************************************************/


#ifndef STANDARD_VERSION_H
#define STANDARD_VERSION_H

#ifdef __cplusplus
extern "C" {
#endif


/******************************************************************************/
/* DOCUMENTATION                                                              */
/******************************************************************************/
/** \file
 *
 *  \ingroup  standardVersion
 *
 *  \brief    Public definitions and declarations unified versioning accross
 *            different software modules.
 */


/******************************************************************************/
/* INCLUDES                                                                   */
/******************************************************************************/
#include <stdint.h>
#include <stdbool.h>
#include "std_symbol.h"


/******************************************************************************/
/* PUBLIC DEFINITIONS                                                         */
/******************************************************************************/

/** \brief    Definition for length of HW and SW ID array.
 *
 *  \details  Length of the ID string is fixed to 8 characters, do not change 
 *            it's value!
 */
#define STDV_ID_LENGTH      8U
/* Note 9026: Function-like macro, '...', defined [MISRA 2012 Directive 4.9, advisory] */
/*lint -save -e9026  */
/*lint -emacro(572,STDV_Parse*) "Warning 572: Excessive shift value (precision 0 shifted right by xx)" */
#define STDV_ParseVersion(version)       {(uint8_t)((uint16_t)(version) >>  8u), (uint8_t)(version)}
#define STDV_ParseVersionBuild(build)    {(uint8_t)((uint32_t)(build)   >> 24u),\
                                          (uint8_t)((uint32_t)(build)   >> 16u),\
                                          (uint8_t)((uint32_t)(build)   >>  8u),\
                                          (uint8_t)(build)}

#define STDV_ParseIdentifier(ID)          {(uint8_t)((uint64_t)(ID)   >> 56ULL),\
                                           (uint8_t)((uint64_t)(ID)   >> 48ULL),\
                                           (uint8_t)((uint64_t)(ID)   >> 40ULL),\
                                           (uint8_t)((uint64_t)(ID)   >> 32ULL),\
                                           (uint8_t)((uint64_t)(ID)   >> 24ULL),\
                                           (uint8_t)((uint64_t)(ID)   >> 16ULL),\
                                           (uint8_t)((uint64_t)(ID)   >>  8ULL),\
                                           (uint8_t)(ID)}
/*lint -restore -e9026 */

/******************************************************************************/
/* PUBLIC TYPE DEFINITIONS                                                    */
/******************************************************************************/

/** \brief    Standard software and hardware identifier structure.
 */
struct STDV_idArray
{
    uint8_t  ID[STDV_ID_LENGTH]; ///< 8 character identifier
};


/** \brief    Standard version structure.
 *
 *  \details  Structure for software and hardware version.
 */
struct STDV_version
{
    uint8_t ID[STDV_ID_LENGTH];         ///< 8 character identifier
    uint8_t major[sizeof(uint16_t)];    ///< major changes e.g. new functionality, new interfaces
    uint8_t minor[sizeof(uint16_t)];    ///< minor changes, backwards compatible firmware
    uint8_t revision[sizeof(uint16_t)]; ///< revision (e.g. a fix)
    uint8_t build[sizeof(uint32_t)];    ///< software build counter or id from vcs
}SYMBOL_PACKED;


/******************************************************************************/
/* PUBLIC FUNCTION DECLARATIONS                                               */
/******************************************************************************/

/** \brief    Function for comparing two version IDs.
 *
 *  \details  This function compares ID from version structure with ID aray 
 *            structure.
 *
 *  \param    version: pointer to version structure.
 *  \param    idArray: pointer to ID array structure.
 *
 *  \return   True, if ID strings are equal.
 */
bool STDV_isIdEqual(const struct STDV_version *version, const struct STDV_idArray *idArray);


#ifdef __cplusplus
}
#endif

#endif // STANDARD_VERSION_H
