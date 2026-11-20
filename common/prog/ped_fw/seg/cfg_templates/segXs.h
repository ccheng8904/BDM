/*******************************************************************************
 *  Copyright (c) 2017 BSH Hausgeraete GmbH,
 *  Carl-Wery-Str. 34, 81739 Munich, Germany, www.bsh-group.de
 *
 *  All rights reserved. This program and the accompanying materials
 *  are protected by international copyright laws.
 *  Please contact copyright holder for licensing information.
 *
 *******************************************************************************
 *  PROJECT          PP PED-Framework
 *  COMP_ABBREV      SEG
 ******************************************************************************/

#ifndef SEGXS_H
#define SEGXS_H

#ifdef __cplusplus
extern "C" {
#endif


/******************************************************************************/
/* DOCUMENTATION                                                              */
/******************************************************************************/
/** \file
 *
 *  \ingroup  PED-Framework
 *
 *  \brief    published application specific definitions and declarations
 *            for segment library.
 * 
 *  \details  This just provides an empty implementation when seg is unused.
 *            Please use the example template when it will be used.
 *
 */



/** Dependencies of library after configuration
 */
#define DEP_SEG     DEPLIB_SEG,

#define SEG_NUMBER_OF_SEGMENTS     320U  ///<Number of segments, defined by application


#define SEG_NUMBER_OF_DIGITS 12U


#ifdef __cplusplus
}
#endif

#endif
