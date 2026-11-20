/*******************************************************************************
*   Copyright (c) 2017 BSH Hausgeraete GmbH,
*   Carl-Wery-Str. 34, 81739 Munich, Germany, www.bsh-group.de
*
*   All rights reserved. This program and the accompanying materials
*   are protected by international copyright laws.
*   Please contact copyright holder for licensing information.
*
********************************************************************************
*   PROJECT          state
*   COMP_ABBREV      STE
*******************************************************************************/


/******************************************************************************/
/* DOCUMENTATION                                                              */
/******************************************************************************/

/** \file
*
*   \brief    User defined stub implementation of public functions
*             of stateeng for dynamic SW unit test.
*
*   \note     This is not a stand alone translation unit
*             It is included by automatically generated file
*             stateeng_Stub.c into stateeng.c file
*/


/******************************************************************************/
/* PRIVATE DEFINITIONS                                                        */
/******************************************************************************/
// the following defines are used to enable or disable the user defined stub functions:
// if the according identifier is defined (= not commented out)
// then the user defined stub function is defined (here in stateeng_UserStub.c),
// otherwise the default stub function is defined (in automatically generated file stateeng_Stub.c)


void test_function(void);
uint8_t STE_testdummy(void);
/******************************************************************************/
/* PUBLIC DATA DEFINITIONS to be used by TEng                                 */
/******************************************************************************/

typedef struct StructStateClient test_StStateClient_struct;

test_StStateClient_struct dummy_struct_Stateclient;

const ConditionFunc  STE_pointer_to_conditionfuncpointer[2] = {STE_testdummy, STE_testdummy};


typedef const struct structState test_structState_struct;

test_structState_struct demo_struct;

StStateClient * test_psClient ;

EventType  test_EventId = 1;

EventType*  test_CompoundEventTable[10] = {1,2,3,4,5,6,7,8,9,0};

EventType* test_EventP;
EventType test_Event_arg;
EventType test_Event_array[3u];

StateType  test_ActiveState;

StateType  test_LastState;



/******************************************************************************/
/* PUBLIC STUB FUNCTION DEFINITIONS                                           */
/******************************************************************************/

/* instruction for creation of a user defined stub function:                                              */

/* Copy the automatically generated stub function in stateeng_Stub.c including                            */
/* the encircling #ifndef <FUNCTIONNAME>_STUB … #endif and paste them into this file.                     */
/* Change the #ifndef into a #ifdef.                                                                      */
/* Replace the function body with your code.                                                              */
/* Where required add further variables (see block 'PUBLIC DATA DEFINITIONS to be used by TEng' above;    */
/* these variable definitions have to have the modifier volatile.                                         */
/* Add a #define < FUNCTIONNAME >_STUB above (see block 'PRIVATE DEFINITIONS'):                           */
/* if its commented in then your user defined stub function will be linked;                               */
/* if its commented out then the automatically generated default stub function will be linked.            */

/* Add every test point used in the definition of the stub functions in this file.                        */
/* to the define STATEENG_USERSTUB_TPS in the file <UnitName>_UserStub_TPs.h                              */



/******************************************************************************/
/* PUBLIC TEST DRIVER FUCNTIONS                                               */
/******************************************************************************/

/* instruction for creation of a user defined test driver  :                  */

/* if you want to implement and use test drivers                              */
/* - declare your test drivers here                                           */
/* - add the test driver functions                                            */
/*  to the following STATEENG_atUserTestDriverFuncPt array                */
/*   and place it after the test driver declarations                          */
/* - define your test drivers                                                 */
/* - add the define STATEENG_UT_TEST_DRIVER to the dmake macro            */
/*   DEFINES or DEFINES_CL in your Project.inc file                           */
/*   (not here because used by GenericStub.c)                                 */
/*   to have your drivers linked into the image file                          */
/*   (the array is not allowed to be empty in this case)                      */


// declare your drivers before the following array
// (separate the array elements with an comma - no comma after the last element)
#if defined STATEENG_UT_TEST_DRIVER
const T_TM_Anchor STATEENG_atUserTestDriverFuncPtr[] =
{
    (T_TM_Anchor) test_function,
    (T_TM_Anchor) STE_testdummy

};
#endif

uint8_t STE_testdummy_RET = 0;

uint8_t STE_testdummy() {
    return STE_testdummy_RET;
}

void test_function(void)
{   
    (void)STE_pointer_to_conditionfuncpointer;
    uint8_t i;
    test_psClient =  &dummy_struct_Stateclient;
    test_LastState=  &demo_struct;
    test_ActiveState = &demo_struct;
    test_EventP = &test_Event_arg;
    test_CompoundEventTable[2] = test_EventP;
    test_Event_arg = 2;
    test_Event_array[0] = 0;

}
