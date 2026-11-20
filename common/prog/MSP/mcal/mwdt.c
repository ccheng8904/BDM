/*******************************************************************************
 *  Copyright (c) 2017 BSH Hausgeraete GmbH,
 *  Carl-Wery-Str. 34, 81739 Munich, Germany, www.bsh-group.de
 *
 *  All rights reserved. This program and the accompanying materials
 *  are protected by international copyright laws.
 *  Please contact copyright holder for licensing information.
 *
 *******************************************************************************
 *  PROJECT          MCAL
 *  COMP_ABBREV      MWDT
 ******************************************************************************/


/******************************************************************************/
/* DOCUMENTATION                                                              */
/******************************************************************************/
/** \file
 *
 *  \ingroup  MCAL
 *
 *  \brief    Implementation of MWDT
 */


/******************************************************************************/
/* INCLUDES                                                                   */
/******************************************************************************/
#include "BSH_stdinc.h"
#include "mwdt.h"
#include "math/mathDiv.h"
#include "mwdt_mc.h"
#include "mcal/mwdt_priv.h"

#ifdef MCAL_MPCM_INCLUDED
    #include "mpcm.h"
#endif

//lint -esym(9045,*MWDT_Config)    "non-hidden definition of type struct MWDT_Config"
//lint -esym(9045,*MCAL_EventData) "non-hidden definition of type struct MCAL_EventData"


/******************************************************************************/
/* PRIVATE DEFINITIONS                                                        */
/******************************************************************************/
#define MWDT_MS_TO_US                   1000U


/******************************************************************************/
/* PRIVATE FUNCTION DECLARATIONS                                              */
/******************************************************************************/



/******************************************************************************/
/* PUBLIC FUNCTION DEFINITIONS                                                */
/******************************************************************************/

enum MCAL_Error MWDT_init(const struct MWDT_Config *config)
{
    // Unlock Watchdog peripheral
    if(MWDT_unlock() == MCAL_OK)
    {
        // Configure/enable Watchdog
        return (MWDT_initWDT(config));
    }
    else
    {
        // Watchdog unlock failed
        return MCAL_ERROR_CONFIG;
    }
}


void MWDT_trigger(void)
{
    // trigger watchdog peripheral
    MWDT_triggerWDT();
}


void MWDT_initConfigStruct(void)
{
    MWDT_initConfigStructMWDT();
}


uint16_t MWDT_getPrecisionInPercent(uint32_t actualTimeUs, uint16_t requiredTimeMs)
{
    // percent in % = ((actualTimeUs / (requiredTimeMs * 1000)) * 100) the same as
    // percent in % = (actualTimeUs / (requiredTimeMs * 10))
    uint32_t percent;
    percent = MTDIV_div_32_32(actualTimeUs, (uint32_t)requiredTimeMs * (MWDT_MS_TO_US / 100u));
    if(percent < 100u)
    {
        percent = 100u - percent;
    } else
    {
        percent = percent - 100u;
    }
    return (uint16_t)percent;
}
