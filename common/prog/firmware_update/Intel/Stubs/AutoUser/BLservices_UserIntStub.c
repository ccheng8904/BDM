/*******************************************************************************
 *  Copyright (c) 2017 BSH Hausgeraete GmbH,
 *  Carl-Wery-Str. 34, 81739 Munich, Germany, www.bsh-group.de
 *
 *  All rights reserved. This program and the accompanying materials
 *  are protected by international copyright laws.
 *  Please contact copyright holder for licensing information.
 *
 *******************************************************************************
 *  PROJECT          ramLoader
 *  COMP_ABBREV      BLSRV
 ******************************************************************************/


/******************************************************************************/
/* DOCUMENTATION                                                              */
/******************************************************************************/
/** \file
*
*   \brief    User defined stub implementation of private functions
*             of BLservices for dynamic SW unit test.
*
*   \note     This is not a stand alone translation unit.
*             It is included by automatically generated file
*             BLservices_IntStub.c into BLservices.c file
*/


/******************************************************************************/
/* PRIVATE DEFINITIONS                                                        */
/******************************************************************************/
// the following defines are used to enable or disable the user defined stub functions:
// if the according identifier is defined (= not commented out)
// then the user defined stub function is defined (here in BLservices_UserIntStub.c),
// otherwise the default stub function is defined (in automatically generated file BLservices_IntStub.c)


/******************************************************************************/
/* PUBLIC DATA DEFINITIONS to be used by TEng                                 */
/******************************************************************************/



/******************************************************************************/
/* PRIVATE STUB FUNCTION DEFINITIONS                                          */
/******************************************************************************/

/* instruction for creation of a user defined stub function:                                           */

/* Copy the automatically generated stub function in BLservices_IntStub.c  including                   */
/* the encircling #ifndef <FUNCTIONNAME>_STUB … #endif and paste them into this file.                  */
/* Change the #ifndef into a #ifdef.                                                                   */
/* Replace the function body with your code.                                                           */ 
/* Where required add further variables (see block 'PUBLIC DATA DEFINITIONS to be used by TEng' above; */
/* these variable definitions have to have the modifier volatile.                                      */
/* Add a #define < FUNCTIONNAME >_STUB above (see block 'PRIVATE DEFINITIONS'):                        */
/* if its commented in then your user defined stub function will be linked;                            */
/* if its commented out then the automatically generated default stub function will be linked.         */

/* Add every test point used in the definition of the stub functions in this file.                     */
/* to the define BLSERVICES_USERINTSTUB_TPS in the file <UnitName>_UserIntStub_TPs.h                   */

#define BLSRV_DOCRCCALCULATION_STUB
static bool BLSRV_doCrcCalculation_Stub(uint32_t address, uint32_t length, uint32_t * crc)
{
    BLSRV_doCrcCalculation_address = address;
    BLSRV_doCrcCalculation_length = length;
    BLSRV_doCrcCalculation_crc = (volatile uint32_t *)crc;
    *crc = BLSRV_doCrcCalculation_crc_argument;
    return (bool) BLSRV_doCrcCalculation_RET;
}
