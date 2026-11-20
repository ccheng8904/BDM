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
*             of moduleAdministration for dynamic SW unit test.
*
*   \note     This is not a stand alone translation unit.
*             It is included by automatically generated file
*             moduleAdministration_IntStub.c into moduleAdministration.c file
*/


/******************************************************************************/
/* PRIVATE DEFINITIONS                                                        */
/******************************************************************************/
// the following defines are used to enable or disable the user defined stub functions:
// if the according identifier is defined (= not commented out)
// then the user defined stub function is defined (here in moduleAdministration_UserIntStub.c),
// otherwise the default stub function is defined (in automatically generated file moduleAdministration_IntStub.c)


/******************************************************************************/
/* PUBLIC DATA DEFINITIONS to be used by TEng                                 */
/******************************************************************************/



/******************************************************************************/
/* PRIVATE STUB FUNCTION DEFINITIONS                                          */
/******************************************************************************/

/* instruction for creation of a user defined stub function:                                           */

/* Copy the automatically generated stub function in moduleAdministration_IntStub.c  including                  */
/* the encircling #ifndef <FUNCTIONNAME>_STUB … #endif and paste them into this file.                  */
/* Change the #ifndef into a #ifdef.                                                                   */
/* Replace the function body with your code.                                                           */ 
/* Where required add further variables (see block 'PUBLIC DATA DEFINITIONS to be used by TEng' above; */
/* these variable definitions have to have the modifier volatile.                                      */
/* Add a #define < FUNCTIONNAME >_STUB above (see block 'PRIVATE DEFINITIONS'):                        */
/* if its commented in then your user defined stub function will be linked;                            */
/* if its commented out then the automatically generated default stub function will be linked.         */

/* Add every test point used in the definition of the stub functions in this file.                     */
/* to the define MODULEADMINISTRATION_USERINTSTUB_TPS in the file <UnitName>_UserIntStub_TPs.h                      */

volatile  struct MAL_partition_s *volatile MAL_searchMemModule_partitionTbl_flex;
volatile uint8_t MAL_searchMemModule_maxTableLen_flex;
volatile  struct MAL_partition_s *volatile MAL_searchMemModule_flex_RET;


#define MAL_SEARCHMEMMODULE_STUB
static const  struct MAL_partition_s *  MAL_searchMemModule_Stub(enum MAL_ModuleEnum module, const  struct MAL_partition_s * partitionTbl, uint8_t maxTableLen)
{
    MAL_searchMemModule_module = module;
    if(partitionTbl == MAL_partitionFlex)
    {
        MAL_searchMemModule_partitionTbl_flex = (volatile  struct MAL_partition_s *volatile ) partitionTbl;
        MAL_searchMemModule_maxTableLen_flex = maxTableLen;
        return (const  struct MAL_partition_s * ) MAL_searchMemModule_flex_RET;
    }
    else
    {
        MAL_searchMemModule_partitionTbl = (volatile  struct MAL_partition_s *volatile ) partitionTbl;
        MAL_searchMemModule_maxTableLen = maxTableLen;
        return (const  struct MAL_partition_s * ) MAL_searchMemModule_RET;
    }
}


volatile uint32_t MAL_getBuildId_App2_RET;
#define MAL_GETBUILDID_STUB
static uint32_t MAL_getBuildId_Stub(const  struct MAL_partition_s * partition)
{   
    MAL_getBuildId_partition = (volatile  struct MAL_partition_s *volatile ) partition;
    if(partition->mod_number == MAL_PRODUCT_APP2)
    {
        return (uint32_t) MAL_getBuildId_App2_RET;
    }
    else
    {
        return (uint32_t) MAL_getBuildId_RET;
    }
    return (uint32_t) MAL_getBuildId_RET;
}


