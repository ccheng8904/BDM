/*******************************************************************************
 *  Copyright (c) 2017 BSH Hausgeraete GmbH,
 *  Carl-Wery-Str. 34, 81739 Munich, Germany, www.bsh-group.de
 *
 *  All rights reserved. This program and the accompanying materials
 *  are protected by international copyright laws.
 *  Please contact copyright holder for licensing information.
 *
 *******************************************************************************
 *  PROJECT          firmware_update__mal
 *  COMP_ABBREV      MAL
 ******************************************************************************/


/******************************************************************************/
/* DOCUMENTATION                                                              */
/******************************************************************************/
/** \file
*
*   \brief    User defined stub implementation of private functions
*             of mal_command for dynamic SW unit test.
*
*   \note     This is not a stand alone translation unit.
*             It is included by automatically generated file
*             mal_command_IntStub.c into mal_command.c file
*/


/******************************************************************************/
/* PRIVATE DEFINITIONS                                                        */
/******************************************************************************/
// the following defines are used to enable or disable the user defined stub functions:
// if the according identifier is defined (= not commented out)
// then the user defined stub function is defined (here in mal_command_UserIntStub.c),
// otherwise the default stub function is defined (in automatically generated file mal_command_IntStub.c)


/******************************************************************************/
/* PUBLIC DATA DEFINITIONS to be used by TEng                                 */
/******************************************************************************/



/******************************************************************************/
/* PRIVATE STUB FUNCTION DEFINITIONS                                          */
/******************************************************************************/

/* instruction for creation of a user defined stub function:                                           */

/* Copy the automatically generated stub function in mal_command_IntStub.c  including                  */
/* the encircling #ifndef <FUNCTIONNAME>_STUB … #endif and paste them into this file.                  */
/* Change the #ifndef into a #ifdef.                                                                   */
/* Replace the function body with your code.                                                           */ 
/* Where required add further variables (see block 'PUBLIC DATA DEFINITIONS to be used by TEng' above  */
/* these variable definitions have to have the modifier volatile.                                      */
/* Add a #define < FUNCTIONNAME >_STUB above (see block 'PRIVATE DEFINITIONS'):                        */
/* if its commented in then your user defined stub function will be linked;                            */
/* if its commented out then the automatically generated default stub function will be linked.         */

/* Add every test point used in the definition of the stub functions in this file.                     */
/* to the define MAL_COMMAND_USERINTSTUB_TPS in the file <UnitName>_UserIntStub_TPs.h                  */

#define MAL_COPYDEVINFO_STUB
static void MAL_copyDevInfo_Stub(uint8_t index, uint16_t * const modNum, uint32_t * const size, uint8_t * const name)
{
    MAL_copyDevInfo_index = index;
    MAL_copyDevInfo_modNum = (volatile uint16_t *volatile ) modNum;
    MAL_copyDevInfo_size = (volatile uint32_t *volatile ) size;
    MAL_copyDevInfo_name = (volatile uint8_t *volatile ) name;
    *modNum = MAL_copyDevInfo_modNum_argument;
    *size   = MAL_copyDevInfo_size_argument;
    *name   = MAL_copyDevInfo_name_argument;
    
}

#define MAL_PROCESSNEXTINDEX_STUB
static void MAL_processNextIndex_Stub(uint8_t * const index)
{
    MAL_processNextIndex_index = (volatile uint8_t *volatile ) index;
    *index = MAL_processNextIndex_index_argument;
}

#define MAL_ISOK_STUB
static bool MAL_IsOk_Stub(enum MAL_Status result)
{
    MAL_result = result;
    return result == MAL_ST_OK;
}

#define MAL_HANDLEWRITEBLOCK_STUB
volatile uint32_t MAL_handleWriteBlock_MAL_sizeLeft;
static enum MAL_Status MAL_handleWriteBlock_Stub(uint16_t dataLen, const  uint8_t * const data)
{
    MAL_handleWriteBlock_dataLen = dataLen;
    MAL_handleWriteBlock_data = (volatile  uint8_t *volatile ) data;
    MAL_sizeLeft = MAL_handleWriteBlock_MAL_sizeLeft;
    return (enum MAL_Status) MAL_handleWriteBlock_RET;
}
