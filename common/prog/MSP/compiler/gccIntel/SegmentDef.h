
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
 *  Compiler         GCC / Intel
 ******************************************************************************/

/**************************************************************************************************/
/* DOCUMENTATION                                                                                  */
/**************************************************************************************************/
/*! \file
     This header defines platform specific settings for segmentation
     Hint: Segmentation is ignored on PC
 
*/
#ifndef SEGMENTDEF_H
#define SEGMENTDEF_H

#ifdef __cplusplus
extern "C" {
#endif

#define SDEF_SetSegmentRW(segment)      __attribute__ ((section(LD_STRINGIZE(.segment))))
#define SDEF_SetSegmentZI(segment)      __attribute__ ((section(LD_STRINGIZE(.segment))))
#define SDEF_SetSegmentConst(segment)   __attribute__ ((section(LD_STRINGIZE(.segment))))
#define SDEF_SetSegmentCode(segment)    __attribute__ ((section(LD_STRINGIZE(.segment))))

// not relevant for GCC but must be used for portability reasons
#define SDEF_SetSegmentRW_Default()
#define SDEF_SetSegmentZI_Default()
#define SDEF_SetSegmentConst_Default()
#define SDEF_SetSegmentCode_Default()

#define SDEF_NO_INIT

#ifdef __cplusplus
}
#endif

#endif
