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
 *  COMP_ABBREV      TM
 ******************************************************************************/


/******************************************************************************/
/* DOCUMENTATION                                                              */
/******************************************************************************/
/** \file
*
*   \brief    This file defines the so called 'test point enumeration'
*             which is used in the implementation of the code coverage support
*             features provided by the TestSupport library
*             which are used by the default stub function implementation.
*
*   \details  The test point enumeration defines an enumeration type
*             that defines an enumeration constant (with unique value)
*             for every function that is defined in any unit under test
*             i.e. for every function that is potentially going to be tested
*             in the dynamic SW unit test.
*             It there  are user defined test points -
*             i.e. if the according defines have been set in
*             the according ..._UserStub_TPs.h or _UserIntStub_TPs.h file -
*             then also for every user defined test points
*             an enumeration constant (with unique value) is defined.
*             These enumeration constants are used in the implementation of the
*             code coverage support features provided by the TestSupport library
*             which are used by the default stub function implementation.
* 
**/


#ifndef TESTPOINTS_STUB_H
#define TESTPOINTS_STUB_H

#ifdef __cplusplus
extern "C" {
#endif


/******************************************************************************/
/* INCLUDE of test points of generic HAL for PC testing                       */
/******************************************************************************/



/******************************************************************************/
/* INCLUDES of test point definitions for stubbed project units               */
/******************************************************************************/
<$AllUnitTPDefinitionIncludes>


/******************************************************************************/
/* Global test point definitions                                              */
/******************************************************************************/
typedef enum T_TM_TestPointsEnum { /* Struct should have same name than type */

    TM_NoTestPoint, // reserved internal usage

    // project test points
    
<$TestPointEnumeration>



    TM_NumberOfTestPoints

}T_TM_TestPointsEnum;


#ifdef __cplusplus
}
#endif

#endif
