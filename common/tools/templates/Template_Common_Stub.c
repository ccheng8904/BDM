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
#include "CommonStub_dcl.h"

<$UnitName_Stub_header_include>

/******************************************************************************/
/* LOCAL DEFINITIONS                                                          */
/******************************************************************************/
#define STUBVAR_ARRAY_SIZE  10U

/******************************************************************************/
/* PUBLIC DATA DEFINITIONS to be used by TEng                                 */
/******************************************************************************/
<$ExtStubDataDefinition>


/******************************************************************************/
/* FUNCTION POINTER LIST to reference functions under test and                */
/* pointer argument variables use function to prevent linker optimization     */
/******************************************************************************/
static int COMMONSTUB_use_of_pointer_argument_vars(void);

<$FunctionPtrList>

<$UseOfPtrArgumentFunction>



/******************************************************************************/
/* PUBLIC STUB FUNCTION DEFINITIONS                                           */
/******************************************************************************/
<$ExtStubDefinition>
