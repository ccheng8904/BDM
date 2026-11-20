/*******************************************************************************
*   Copyright (c) 2016 BSH Hausgeraete GmbH,
*   Carl-Wery-Str. 34, 81739 Munich, Germany, www.bsh-group.de
*
*   All rights reserved. This program and the accompanying materials
*   are protected by international copyright laws.
*   Please contact copyright holder for licensing information.
*
********************************************************************************
*   PROJECT          PTEng
*   UNIT-PREFIX      Common
********************************************************************************
*   %PCMS_HEADER_SUBSTITUTION_START:%
*   ARCHIVE          %PP:%:%PI:%
*   PRESENT REVISION %PR:%
*   STATUS           %PS:%
*   %PCMS_HEADER_SUBSTITUTION_END:%
*******************************************************************************/


/******************************************************************************/
/* DOCUMENTATION                                                              */
/******************************************************************************/
/** \file
*
*   \brief    Default stub implementation of undefined functions.
*
*/



/******************************************************************************/
/* INCLUDES                                                                   */
/* (necessary  if this file is provided as stand alone translation unit       */
/* in an application project)                                                 */
/******************************************************************************/
#include "bsh_stdinc.h"
#include <stdint.h>
#include "segXs.h"
#include "seg.h"
#include "segpriv.h"
#include "utility/utility.h"
#include "LibTypes.h"
#include "SegmentDef.h"


/******************************************************************************/
/* LOCAL DEFINITIONS                                                          */
/******************************************************************************/
#define STUBVAR_ARRAY_SIZE  10U

/******************************************************************************/
/* PUBLIC DATA DEFINITIONS to be used by TEng                                 */
/******************************************************************************/
//const uint8_t SEG_ucNumberOfBlinkBytes = 40u;
//const uint16_t SEG_uiNumberOfSegments = 320u;
//const uint8_t SEG_ucNumberOfDigits = 12u;
const uint8_t SEG_ucNumberOfBlinkRhythm;
//uint8_t SEG_ucTextIndex;
//uint8_t SEG_ucSegmentCountForDigits;
uint8_t SEG_pucBlinkField[STUBVAR_ARRAY_SIZE]={};
//uint8_t SEG_pucTextBuffer[STUBVAR_ARRAY_SIZE]={};
//uint8_t SEG_pucTextBlinkBuffer[STUBVAR_ARRAY_SIZE]={};
uchar BEAT_ucBeatTimer;



/******************************************************************************/
/* FUNCTION POINTER LIST to reference functions under test and                */
/* pointer argument variables use function to prevent linker optimization     */
/******************************************************************************/
static int COMMONSTUB_use_of_pointer_argument_vars(void);

const T_TM_Anchor COMMONSTUB_atTestFuncPtr[] = 
{
    (T_TM_Anchor) COMMONSTUB_use_of_pointer_argument_vars
};


static int COMMONSTUB_use_of_pointer_argument_vars (void)
{
int help = 0;

return help;
}




/******************************************************************************/
/* PUBLIC STUB FUNCTION DEFINITIONS                                           */
/******************************************************************************/

