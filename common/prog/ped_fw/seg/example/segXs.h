/*******************************************************************************
*   Copyright (c) 2016 BSH Hausgeraete GmbH,
*   Carl-Wery-Str. 34, 81739 Munich, Germany, www.bsh-group.de
*
*   All rights reserved. This program and the accompanying materials
*   are protected by international copyright laws.
*   Please contact copyright holder for licensing information.
*
********************************************************************************
*   PROJECT          SW_LIB
*   COMP_ABBREV      SEG
*******************************************************************************/
#ifndef __SEGXS_H
#define __SEGXS_H

#ifdef __cplusplus
extern "C" {
#endif

/******************************************************************************/
/* DOCUMENTATION                                                              */
/******************************************************************************/
/******************************************************************************/
/* INCLUDES                                                                   */
/******************************************************************************/
#include "LibTypes.h"

/******************************************************************************/
/*   DEPENDENCIES                                                             */
/******************************************************************************/


/** Dependencies of library after configuration

If the configuration changes the dependencies of segment library, this shall adapted here.\n
The following example add a dependency to module EEP:\n
\code
#define DEP_SEG     {DEPLIB_SEG  MOD_EEP, SCH_END_MARK}
\endcode
\note There is no comma after DEPLIB_SEG
*/
#define DEP_SEG     DEPLIB_SEG  MOD_BEAT,MOD_VAR,


/******************************************************************************/
/* PUBLIC DEFINITIONS                                                         */
/******************************************************************************/
#define SEG_NUMBER_OF_SEGMENTS     320  ///<Number of segments, defined by application
//#define SEG_16BIT //comment in if any ID (logical or physical) exceeds 254 (not 255 which is used as terminator!)

#define SEG_NUMBER_OF_DIGITS 12  //todo: might be removed

/******************************************************************************/
/* PUBLIC TYPE DEFINITIONS                                                    */
/******************************************************************************/


/******************************************************************************/
/* PUBLIC TYPE DEFINITIONS                                                    */
/******************************************************************************/
///Enumeration of all logical segments and segment groups
enum
{
  SEG_POS_0,                           ///<0:  Position 0 inside
  SEG_POS_1,                           ///<1:  Position 1 inside
  SEG_POS_2,                           ///<2:  Position 2 inside
  SEG_POS_3,                           ///<3:  Position 3 inside
  SEG_POS_4,                           ///<4:  Position 4 inside
  SEG_POS_5,                           ///<5:  Position 5 inside
  SEG_POS_6,                           ///<6:  Position 6 inside
  SEG_POS_7,                           ///<7:  Position 7 inside
  SEG_POS_8,                           ///<8:  Position 8 inside
  SEG_POS_9,                           ///<9:  Position 9 inside
  SEG_POS_10,                          ///<10: Position 10 inside
  SEG_POS_11,                          ///<11: Position 11 inside
  SEG_POS_12,                          ///<12: Position 12 inside
  SEG_POS_13,                          ///<13: Position 13 inside

  SEG_JOG,                             ///<14: Jogdial
  SEG_ALL_PROGRAM_POSITIONS,           ///<15: Program positions

  SEG_ECO,                             ///<16: Eco Perfect
  SEG_SPEED                            ///<17: Speed Perfect
};
/*const uint8_t SEG_pucBasicBlinkRhythm[]*/
//Beatfrequenz is 4 Hz
//SEG_BLINK and SEG_BLINK2: 0,25s on, 0,25s off ==> 0101 0101 = 0x55
//SEG_BLINK2: 0,5s on, 0,5s off ==> 0011 0011 = 0x33
//SEG_BLINK3: 1s on, 1s off ==> 0000 1111 = 0x0F
static const uint8_t SEG_pucBasicBlinkRhythm[] =
{
  0x55,
  0x33,
  0x0F
};


/******************************************************************************/
/* PUBLIC FUNCTION DECLARATIONS                                               */
/******************************************************************************/


#ifdef __cplusplus
}
#endif

#endif
