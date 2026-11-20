/*******************************************************************************
*   Copyright (c) 2016 BSH Hausgeraete GmbH,
*   Carl-Wery-Str. 34, 81739 Munich, Germany, www.bsh-group.de
*
*   All rights reserved. This program and the accompanying materials
*   are protected by international copyright laws.
*   Please contact copyright holder for licensing information.
*
********************************************************************************
*   PROJECT          state
*   UNIT-PREFIX      STE
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
*   \brief    User defined stub implementation of private functions
*             of stateeng for dynamic SW unit test.
*
*   \note     This is not a stand alone translation unit.
*             It is included by automatically generated file
*             stateeng_IntStub.c into stateeng.c file
*/


/******************************************************************************/
/* PRIVATE DEFINITIONS                                                        */
/******************************************************************************/
// the following defines are used to enable or disable the user defined stub functions:
// if the according identifier is defined (= not commented out)
// then the user defined stub function is defined (here in stateeng_UserIntStub.c),
// otherwise the default stub function is defined (in automatically generated file stateeng_IntStub.c)
#define BISCONDITIONTRUE_STUB
#define BDOESEVENTFIT_STUB
/******************************************************************************/
/* PUBLIC DATA DEFINITIONS to be used by TEng                                 */
/******************************************************************************/

typedef struct structCondition_test
{
    uint8_t     TrueCondition :1;   //TRUE if result TRUE fit, else FALSE
    uint8_t     ConditionID   :7;
} ConditionType_test;



typedef union test_structCoditionType
{

	uint8_t i;
	ConditionType_test bitfields;
}T_unionConditionType;

volatile ConditionType_test bIsConditionTrue_Condition_test;
volatile uchar bIsConditionTrue_test_RET;
volatile StDefault dummy_Stdefault;
/******************************************************************************/
/* PRIVATE STUB FUNCTION DEFINITIONS                                          */
/******************************************************************************/

/* instruction for creation of a user defined stub function:                                           */

/* Copy the automatically generated stub function in stateeng_IntStub.c  including                     */
/* the encircling #ifndef <FUNCTIONNAME>_STUB … #endif and paste them into this file.                  */
/* Change the #ifndef into a #ifdef.                                                                   */
/* Replace the function body with your code.                                                           */ 
/* Where required add further variables (see block 'PUBLIC DATA DEFINITIONS to be used by TEng' above; */
/* these variable definitions have to have the modifier volatile.                                      */
/* Add a #define < FUNCTIONNAME >_STUB above (see block 'PRIVATE DEFINITIONS'):                        */
/* if its commented in then your user defined stub function will be linked;                            */
/* if its commented out then the automatically generated default stub function will be linked.         */
#ifdef BISCONDITIONTRUE_STUB
static uchar bIsConditionTrue_Stub(ConditionType Condition)
{
    bIsConditionTrue_Condition_test.TrueCondition = Condition.TrueCondition;
    bIsConditionTrue_Condition_test.ConditionID = Condition.ConditionID;
    return (uchar) bIsConditionTrue_test_RET;
}
#endif
/* Add every test point used in the definition of the stub functions in this file.                     */
/* to the define STATEENG_USERINTSTUB_TPS in the file <UnitName>_UserIntStub_TPs.h                     */
#ifdef BDOESEVENTFIT_STUB
static uchar bDoesEventFit_Stub(EventType EventId, EventType FittingEvent)
{
    dummy_struct_Stateclient.LastCompoundEvent = 0;
    bDoesEventFit_EventId = EventId;
    bDoesEventFit_FittingEvent = FittingEvent;
    return (uchar) bDoesEventFit_RET;
}
#endif
