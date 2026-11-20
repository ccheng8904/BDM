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

#include "LibTypes.h"
#include "utility.h"
#include "mpxlib.h"
#include "mpxpriv.h"
#include "utility/utility.h"


/******************************************************************************/
/* LOCAL DEFINITIONS                                                          */
/******************************************************************************/
#define STUBVAR_ARRAY_SIZE  10U

/******************************************************************************/
/* PUBLIC DATA DEFINITIONS to be used by TEng                                 */
/******************************************************************************/
volatile  uint8_t *volatile UTI_vMemCopy_pucSource;
volatile  uint8_t   UTI_vMemCopy_pucSource_argument;
volatile uint8_t *volatile UTI_vMemCopy_pucDestination;
volatile uint8_t   UTI_vMemCopy_pucDestination_argument;
volatile uint16_t UTI_vMemCopy_uiLength;



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
help += (int) &UTI_vMemCopy_pucSource_argument;
	help += (int) &UTI_vMemCopy_pucDestination_argument;

return help;
}




/******************************************************************************/
/* PUBLIC STUB FUNCTION DEFINITIONS                                           */
/******************************************************************************/
#ifndef UTI_VMEMCOPY_STUB
void UTI_vMemCopy_Stub(const  uint8_t * const pucSource, uint8_t * const pucDestination, uint16_t uiLength)
{
    UTI_vMemCopy_pucSource = (volatile  uint8_t *volatile ) pucSource;
    UTI_vMemCopy_pucDestination = (volatile uint8_t *volatile ) pucDestination;
    UTI_vMemCopy_uiLength = uiLength;
}
#endif



