/*******************************************************************************
 *  Copyright (c) 2017 BSH Hausgeraete GmbH,
 *  Carl-Wery-Str. 34, 81739 Munich, Germany, www.bsh-group.de
 *
 *  All rights reserved. This program and the accompanying materials
 *  are protected by international copyright laws.
 *  Please contact copyright holder for licensing information.
 *
 *******************************************************************************
 *  PROJECT          <$PROJECT_NAME>
 *  COMP_ABBREV      <$UNIT_PREFIX>
 ******************************************************************************/


/******************************************************************************/
/* DOCUMENTATION                                                              */
/******************************************************************************/
/** \file
*
*   \brief    Default stub implementation of public functions
*             of <$UnitName> for dynamic SW unit test.
*
*   \note     For the unit test of the <$UnitName> itself
*             this file is included by <$UnitName>.c 
*             i.e. it is not a stand alone translation unit.
*             For the unit test of an application project 
*             that is using <$UnitName> as library
*             this file is provided as stand alone translation unit.
*/



/******************************************************************************/
/* INCLUDES                                                                   */
/* (necessary  if this file is provided as stand alone translation unit       */
/* in an application project)                                                 */
/******************************************************************************/
#include "bsh_stdinc.h"
#include <stdint.h>
<$UnitName_header_include>


/******************************************************************************/
/* PUBLIC STUB FUNCTION DECLARATIONS                                          */
/******************************************************************************/
//solved by rd and dcl
//<$UnitName_Stub_header_include>

/******************************************************************************/
/* PUBLIC COVERAGE VARIABLE DEFINITIONS                                       */
/******************************************************************************/
<$COVERAGE_VARS>

/******************************************************************************/
/* PUBLIC DATA DEFINITIONS to be used by TEng                                 */
/******************************************************************************/
<$ExtStubDataDefinition>


/******************************************************************************/
/* INCLDUE USER DEFINED STUB IMPLEMENTATIONS                                  */
/* incl. switches between default and user defined stub implementation        */
/******************************************************************************/
#include "../AutoUser/<$UnitName>_UserStub.c"



/******************************************************************************/
/* FUNCTION POINTER LIST to reference functions under test and                */
/* pointer argument variables use function to prevent linker optimization     */
/******************************************************************************/
#if (defined (UNIT_UNDER_TEST))
static int <$UNIT_NAME>_use_of_pointer_argument_vars(void);
#if(! (defined (WORK_WITH_WRAPPER)))
<$FunctionPtrList>
#endif
<$UseOfPtrArgumentFunction>
#endif


/******************************************************************************/
/* PUBLIC STUB FUNCTION DEFINITIONS                                           */
/******************************************************************************/
<$ExtStubDefinition>
