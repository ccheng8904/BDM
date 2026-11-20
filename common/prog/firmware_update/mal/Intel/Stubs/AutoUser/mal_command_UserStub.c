/*******************************************************************************
*   Copyright (c) 2017 BSH Hausgeraete GmbH,
*   Carl-Wery-Str. 34, 81739 Munich, Germany, www.bsh-group.de
*
*   All rights reserved. This program and the accompanying materials
*   are protected by international copyright laws.
*   Please contact copyright holder for licensing information.
*
********************************************************************************
*   PROJECT          firmware_update__mal
*   COMP_ABBREV      MAL
*******************************************************************************/


/******************************************************************************/
/* DOCUMENTATION                                                              */
/******************************************************************************/

/** \file
*
*   \brief    User defined stub implementation of public functions
*             of mal_command for dynamic SW unit test.
*
*   \note     This is not a stand alone translation unit
*             It is included by automatically generated file
*             mal_command_Stub.c into mal_command.c file
*/


/******************************************************************************/
/* PRIVATE DEFINITIONS                                                        */
/******************************************************************************/
// the following defines are used to enable or disable the user defined stub functions:
// if the according identifier is defined (= not commented out)
// then the user defined stub function is defined (here in mal_command_UserStub.c),
// otherwise the default stub function is defined (in automatically generated file mal_command_Stub.c)



/******************************************************************************/
/* PUBLIC DATA DEFINITIONS to be used by TEng                                 */
/******************************************************************************/



/******************************************************************************/
/* PUBLIC STUB FUNCTION DEFINITIONS                                           */
/******************************************************************************/

/* instruction for creation of a user defined stub function:                                              */

/* Copy the automatically generated stub function in mal_command_Stub.c including                         */
/* the encircling #ifndef <FUNCTIONNAME>_STUB … #endif and paste them into this file.                     */
/* Change the #ifndef into a #ifdef.                                                                      */
/* Replace the function body with your code.                                                              */
/* Where required add further variables (see block 'PUBLIC DATA DEFINITIONS to be used by TEng' above;    */
/* these variable definitions have to have the modifier volatile.                                         */
/* Add a #define < FUNCTIONNAME >_STUB above (see block 'PRIVATE DEFINITIONS'):                           */
/* if its commented in then your user defined stub function will be linked;                               */
/* if its commented out then the automatically generated default stub function will be linked.            */

/* Add every test point used in the definition of the stub functions in this file.                        */
/* to the define MAL_COMMAND_USERSTUB_TPS in the file <UnitName>_UserStub_TPs.h                           */



/******************************************************************************/
/* PUBLIC TEST DRIVER FUCNTIONS                                               */
/******************************************************************************/

/* instruction for creation of a user defined test driver  :                  */

/* if you want to implement and use test drivers                              */
/* - declare your test drivers here                                           */
/* - add the test driver functions                                            */
/*  to the following MAL_COMMAND_atUserTestDriverFuncPt array                */
/*   and place it after the test driver declarations                          */
/* - define your test drivers                                                 */
/* - add the define MAL_COMMAND_UT_TEST_DRIVER to the dmake macro            */
/*   DEFINES or DEFINES_CL in your Project.inc file                           */
/*   (not here because used by GenericStub.c)                                 */
/*   to have your drivers linked into the image file                          */
/*   (the array is not allowed to be empty in this case)                      */


// declare your drivers before the following array
// (separate the array elements with an comma - no comma after the last element)
#if defined MAL_COMMAND_UT_TEST_DRIVER
const T_TM_Anchor MAL_COMMAND_atUserTestDriverFuncPtr[] =
{
    (T_TM_Anchor) <DriverFuncName1>,
    (T_TM_Anchor) <DriverFuncName2>,
    ...
    (T_TM_Anchor) <DriverFuncNameN>

};
#endif

