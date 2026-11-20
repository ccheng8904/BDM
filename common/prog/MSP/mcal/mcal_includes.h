
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
 *  COMP_ABBREV      MCAL
 ******************************************************************************/


#ifndef MCAL_INCLUDES_H
#define MCAL_INCLUDES_H


/******************************************************************************/
/* DOCUMENTATION                                                              */
/******************************************************************************/
/** \file
 *
 *  \ingroup  MCAL
 *
 *  \brief    Include header file for MCAL.
 *
 */


/******************************************************************************/
/* INCLUDES                                                                   */
/******************************************************************************/
#include "BSH_stdinc.h"

#ifdef MCAL_MPCM_INCLUDED
    #include "mpcm.h"
#endif

#ifdef MCAL_MDIO_INCLUDED
    #include "mdio.h"
#endif

#ifdef MCAL_MUART_INCLUDED
    #include "muart.h"
#endif

#ifdef MCAL_MEXTI_INCLUDED
    #include "mexti.h"
#endif

#ifdef MCAL_MADC_INCLUDED
    #include "madc.h"
#endif

#ifdef MCAL_MTIM_INCLUDED
    #include "mtim.h"
#endif

#ifdef MCAL_MSUP_INCLUDED
    #include "msup.h"
#endif

#ifdef MCAL_MSPI_INCLUDED
    #include "mspi.h"
#endif

#ifdef MCAL_MWDT_INCLUDED
    #include "mwdt.h"
#endif

#ifdef MCAL_MI2C_INCLUDED
    #include "mi2c.h"
#endif

#ifdef MCAL_MDMA_INCLUDED
    #include "mdma.h"
#endif

#ifdef MCAL_MDAC_INCLUDED
    #if defined(MDAC_PRESENT)
        #include "mdac.h"
    #endif
#endif

#endif // MCAL_INCLUDES_H
