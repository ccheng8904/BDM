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
 *  COMPILER         armclang
 ******************************************************************************/

/******************************************************************************/
/* DOCUMENTATION                                                              */
/******************************************************************************/
/** \file 
 *  \brief This header defines platform specific settings for segmentation
 *   
 *   \b Example: (place a variable in segment "my_data_segment")
 *   \code
 *   SDEF_SetSegmentRW(my_data_segment)
 *   static uint32_t EXMPL_myPlacedVariable;
 *   SDEF_SetSegmentRW_Default()
 *   \endcode
 *  
 *   \note Some platforms are not able to initialize variable data inside
 *         user defined segments! So initialized variables are not portable.
 */


#ifndef SEGMENTDEF_H
#define SEGMENTDEF_H

#ifdef __cplusplus
extern "C" {
#endif

/*lint -esym(9026, SDEF_*) function-like macro*/

#ifdef __NOT_CC__

/** Next variable will be placed into specified RW-Segment.
 *
 *  Use SDEF_SetSegmentRW_Default() to switch back to default segment.
 *  \note Initialization of variables placed in segments is not portable, variables
 *        should be supposed uninitialized.
 *  \note Software timer (timerlib.h) will be initialized by zero during initialization
 */
#define SDEF_SetSegmentRW(segment)

/** Next variable will be placed into specified ZI-Segment.
 *
 *  Use SDEF_SetSegmentZI_Default() to switch back to default segment:
 *  \note Obsolete definition, do not use
 *  \note zero initialization of variables in user defined segments is not portable
 */
#define SDEF_SetSegmentZI(segment)

#define SDEF_SetSegmentConst(segment)  ///< Next const will be placed into specified ROM-Segment
#define SDEF_SetSegmentCode(segment)   ///< Next function will be placed into specified code segment

#define SDEF_SetSegmentRW_Default()    ///< switch back to default RW segment
#define SDEF_SetSegmentZI_Default()    ///< switch back to default ZI segment
#define SDEF_SetSegmentConst_Default() ///< switch back to default const segment
#define SDEF_SetSegmentCode_Default()  ///< switch back to default code segment

#else
    
#define SDEF_SetSegmentRW(segment)      PRAGMA(clang section bss=#segment)
#define SDEF_SetSegmentZI(segment)      SDEF_SetSegmentRW(segment)
#define SDEF_SetSegmentConst(segment)   PRAGMA(clang section rodata=#segment)
#define SDEF_SetSegmentCode(segment)    PRAGMA(clang section text=#segment)

#define SDEF_SetSegmentRW_Default()     PRAGMA(clang section bss="")
#define SDEF_SetSegmentZI_Default()     SDEF_SetSegmentRW_Default()
#define SDEF_SetSegmentConst_Default()  PRAGMA(clang section rodata="")
#define SDEF_SetSegmentCode_Default()   PRAGMA(clang section text="")

#define PRAGMA(X) _Pragma(#X) // internal use only to force macro expanding

#endif

#define SDEF_NO_INIT

#ifdef __cplusplus
}
#endif

#endif // SEGMENTDEF_H
