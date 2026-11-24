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
*   \brief    Default stub implementation of private functions
*             of <$UnitName> for unit test.
*
*   \note     This is not a stand alone translation unit.
*             For the unit test of the <$UnitName> itself
*             this file is included by <$UnitName>.c 
*/



/******************************************************************************/
/* PUBLIC DATA DEFINITIONS to be used by TEng                                 */
/******************************************************************************/
<$IntStubDataDefinition>


/******************************************************************************/
/* INCLDUE USER DEFINED STUB IMPLEMENTATIONS                                  */
/* incl. switches between default and user defined stub implementation        */
/******************************************************************************/
#include "../AutoUser/<$UnitName>_UserIntStub.c"


/******************************************************************************/
/* pointer argument variables use function to prevent linker optimization     */
/******************************************************************************/
#if (defined (UNIT_UNDER_TEST))
static int <$UNIT_NAME>_use_of_pointer_argument_vars_local(void);
#if (! (defined (WORK_WITH_WRAPPER)))
<$FunctionPtrListLocal>
#endif
<$UseOfPtrArgumentFunctionLocal>
#endif


/******************************************************************************/
/* PRIVATE STUB FUNCTION DEFINITIONS FOR PRIVATE FUNCTIONS                    */
/******************************************************************************/

<$IntStubDefinition>


#if defined (WORK_WITH_WRAPPER)
#include "<$UnitName>_ut_Wrapper.c"
#endif


