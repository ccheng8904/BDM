/*******************************************************************************
*   Copyright (c) 2017 BSH Hausgeraete GmbH,
*   Carl-Wery-Str. 34, 81739 Munich, Germany, www.bsh-group.de
*
*   All rights reserved. This program and the accompanying materials
*   are protected by international copyright laws.
*   Please contact copyright holder for licensing information.
*
********************************************************************************
*   PROJECT          mem_utility
*   COMP_ABBREV      RBUF
*******************************************************************************/


/******************************************************************************/
/* DOCUMENTATION                                                              */
/******************************************************************************/

/** \file
*
*   \brief    User defined stub implementation of public functions
*             of ring_buffer for dynamic SW unit test.
*
*   \note     This is not a stand alone translation unit
*             It is included by automatically generated file
*             ring_buffer_Stub.c into ring_buffer.c file
*/


/******************************************************************************/
/* PRIVATE DEFINITIONS                                                        */
/******************************************************************************/
// the following defines are used to enable or disable the user defined stub functions:
// if the according identifier is defined (= not commented out)
// then the user defined stub function is defined (here in ring_buffer_UserStub.c),
// otherwise the default stub function is defined (in automatically generated file ring_buffer_Stub.c)



/******************************************************************************/
/* PUBLIC DATA DEFINITIONS to be used by TEng                                 */
/******************************************************************************/
struct RBUF_ringBuffer RBUF_myRingBuf;
uint8_t RBUF_buffer[16];
uint8_t RBUF_outBuffer[16];
const char myText[] = "Hello World";
const uint8_t myTextBuf[] = "Hello World";

/******************************************************************************/
/* PUBLIC STUB FUNCTION DEFINITIONS                                           */
/******************************************************************************/

/* instruction for creation of a user defined stub function:                                              */

/* Copy the automatically generated stub function in ring_buffer_Stub.c including                         */
/* the encircling #ifndef <FUNCTIONNAME>_STUB … #endif and paste them into this file.                     */
/* Change the #ifndef into a #ifdef.                                                                      */
/* Replace the function body with your code.                                                              */
/* Where required add further variables (see block 'PUBLIC DATA DEFINITIONS to be used by TEng' above;    */
/* these variable definitions have to have the modifier volatile.                                         */
/* Add a #define < FUNCTIONNAME >_STUB above (see block 'PRIVATE DEFINITIONS'):                           */
/* if its commented in then your user defined stub function will be linked;                               */
/* if its commented out then the automatically generated default stub function will be linked.            */

/* Add every test point used in the definition of the stub functions in this file.                        */
/* to the define RING_BUFFER_USERSTUB_TPS in the file <UnitName>_UserStub_TPs.h                           */



/******************************************************************************/
/* PUBLIC TEST DRIVER FUCNTIONS                                               */
/******************************************************************************/

/* instruction for creation of a user defined test driver  :                  */

/* if you want to implement and use test drivers                              */
/* - declare your test drivers here                                           */
/* - add the test driver functions                                            */
/*  to the following RING_BUFFER_atUserTestDriverFuncPt array                */
/*   and place it after the test driver declarations                          */
/* - define your test drivers                                                 */
/* - add the define RING_BUFFER_UT_TEST_DRIVER to the dmake macro            */
/*   DEFINES or DEFINES_CL in your Project.inc file                           */
/*   (not here because used by GenericStub.c)                                 */
/*   to have your drivers linked into the image file                          */
/*   (the array is not allowed to be empty in this case)                      */


// declare your drivers before the following array
// (separate the array elements with an comma - no comma after the last element)
#if defined RING_BUFFER_UT_TEST_DRIVER
const T_TM_Anchor RING_BUFFER_atUserTestDriverFuncPtr[] =
{
    (T_TM_Anchor) &RBUF_myRingBuf,
    (T_TM_Anchor) RBUF_buffer,
    (T_TM_Anchor) RBUF_outBuffer,
    (T_TM_Anchor) myText,
    (T_TM_Anchor) myTextBuf

};
#endif

