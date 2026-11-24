/*******************************************************************************
*   Copyright (c) 2017 BSH Hausgeraete GmbH,
*   Carl-Wery-Str. 34, 81739 Munich, Germany, www.bsh-group.de
*
*   All rights reserved. This program and the accompanying materials
*   are protected by international copyright laws.
*   Please contact copyright holder for licensing information.
*
********************************************************************************
*   PROJECT          <$PROJECT_NAME>
*   COMP_ABBREV      <$UNIT_PREFIX>
*******************************************************************************/


/******************************************************************************/
/* DOCUMENTATION                                                              */
/******************************************************************************/
/** \file
*
*   \brief    Default wrapper function implementation for dynamic SW unit test.
*             for all functions defined in <$UnitName>.
*             
*   \note     This file is not a stand alone translation unit.
*             For the dynamic SW unit test of the unit <$UnitName> this file
*             is included by <$UnitName>_IntStub.c.
*
*/


/******************************************************************************/
/* PUBLIC WRAPPER FUNCTION DECLARATIONS                                       */
/******************************************************************************/
#include "<$UnitName>_ut_Wrapper.h"


/******************************************************************************/
/* PUBLIC WRAPPER Argument Defines                                               */
/******************************************************************************/
<$WrapperArgumentDefines>


/******************************************************************************/
/* INCLDUE USER DEFINED wrapper IMPLEMENTATIONS                               */
/* incl. switches between default and user defined wrapper implementation     */
/******************************************************************************/
#include "..\AutoUser\<$UnitName>_ut_UserWrapper.c"


/******************************************************************************/
/* FUNCTION POINTER LIST to reference functions under test and function       */
/* to reference pointer argument variables to prevent linker optimization     */
/******************************************************************************/
<$WrapperFunctionPtrLists>


/******************************************************************************/
/* PUBLIC WRAPPER FUNCTION DEFINITIONS                                        */
/******************************************************************************/
<$WrapperDefinition>




