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


/******************************************************************************/
/* DOCUMENTATION                                                              */
/******************************************************************************/
/** \file
 *
 *  \ingroup  standardVersion
 *
 *  \brief    Public definitions and declarations unified versioning across
 *            different software modules.
 */
 
/******************************************************************************/
/* INCLUDES                                                                   */
/******************************************************************************/
#include "std_version.h"
//#include <assert.h>

/******************************************************************************/
/* PUBLIC FUNCTION DEFINITIONS                                                */
/******************************************************************************/

bool STDV_isIdEqual(const struct STDV_version *version, const struct STDV_idArray *idArray)
{
    //assert(version == NULL);
    //assert(idArray == NULL);
    
    for(uint8_t i = 0; i < STDV_ID_LENGTH; i++)
    {
        if(version->ID[i] != idArray->ID[i])
        {
            return false;
        }
    }
    return true;
}
