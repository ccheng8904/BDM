/*******************************************************************************
 *  Copyright (c) 2017 BSH Hausgeraete GmbH,
 *  Carl-Wery-Str. 34, 81739 Munich, Germany, www.bsh-group.de
 *
 *  All rights reserved. This program and the accompanying materials
 *  are protected by international copyright laws.
 *  Please contact copyright holder for licensing information.
 *
 *******************************************************************************
 *  PROJECT          programmer
 *  COMP_ABBREV      MEMDRV
 ******************************************************************************/


/******************************************************************************/
/* DOCUMENTATION                                                              */
/******************************************************************************/
/** \file
*
*   \brief    User defined stub implementation of private functions
*             of mem_drv for dynamic SW unit test.
*
*   \note     This is not a stand alone translation unit.
*             It is included by automatically generated file
*             mem_drv_IntStub.c into mem_drv.c file
*/


/******************************************************************************/
/* PRIVATE DEFINITIONS                                                        */
/******************************************************************************/
// the following defines are used to enable or disable the user defined stub functions:
// if the according identifier is defined (= not commented out)
// then the user defined stub function is defined (here in mem_drv_UserIntStub.c),
// otherwise the default stub function is defined (in automatically generated file mem_drv_IntStub.c)


/******************************************************************************/
/* PUBLIC DATA DEFINITIONS to be used by TEng                                 */
/******************************************************************************/



/******************************************************************************/
/* PRIVATE STUB FUNCTION DEFINITIONS                                          */
/******************************************************************************/

/* instruction for creation of a user defined stub function:                                           */

/* Copy the automatically generated stub function in mem_drv_IntStub.c including                       */
/* the encircling #ifndef <FUNCTIONNAME>_STUB … #endif and paste them into this file.                  */
/* Change the #ifndef into a #ifdef.                                                                   */
/* Replace the function body with your code.                                                           */ 
/* Where required add further variables (see block 'PUBLIC DATA DEFINITIONS to be used by TEng' above; */
/* these variable definitions have to have the modifier volatile.                                      */
/* Add a #define < FUNCTIONNAME >_STUB above (see block 'PRIVATE DEFINITIONS'):                        */
/* if its commented in then your user defined stub function will be linked;                            */
/* if its commented out then the automatically generated default stub function will be linked.         */

/* Add every test point used in the definition of the stub functions in this file.                     */
/* to the define MEM_DRV_USERINTSTUB_TPS in the file <UnitName>_UserIntStub_TPs.h                      */

// just take original implementation te prevent adaptation of framed unit
#define MEMDRV_MIN_STUB
static uint32_t MEMDRV_min_Stub(uint32_t val1, uint32_t val2)
{
    return ((val1>val2) ? val2 : val1);
}


#define MEMDRV_ERASEATOMIC_STUB
static enum MAL_Status MEMDRV_eraseAtomic_Stub(uint32_t * addr, uint32_t * size)
{
    *size = MEMDRV_eraseAtomic_size_argument;
    MEMDRV_eraseAtomic_addr = (volatile uint32_t *)addr;
    MEMDRV_eraseAtomic_size = (volatile uint32_t *)size;
    return (enum MAL_Status) MEMDRV_eraseAtomic_RET;
}

#define MEMDRV_WRITEATOMIC_STUB
static enum MAL_Status MEMDRV_writeAtomic_Stub(uint32_t * addr, uint32_t * size, const  uint8_t *  * data)
{
    *size = MEMDRV_writeAtomic_size_argument;
    MEMDRV_writeAtomic_addr = (volatile uint32_t *)addr;
    MEMDRV_writeAtomic_size = (volatile uint32_t *)size;
    MEMDRV_writeAtomic_data = (volatile  uint8_t *volatile   *volatile ) data;
    return (enum MAL_Status) MEMDRV_writeAtomic_RET;
}


#define MEMDRV_WRITEBUFFERED_STUB
static enum MAL_Status MEMDRV_writeBuffered_Stub(uint32_t * size, const  uint8_t *  * data)
{
    *size = MEMDRV_writeBuffered_size_argument;
    MEMDRV_writeBuffered_size = (volatile uint32_t *)size;
    MEMDRV_writeBuffered_data = (volatile  uint8_t *volatile   *volatile ) data;
    return (enum MAL_Status) MEMDRV_writeBuffered_RET;
}



#define MEMDRV_CALCCRCATOMIC_STUB
static enum MAL_Status MEMDRV_calcCrcAtomic_Stub(uint32_t * addr, uint32_t * size, CRC_crc_t * crc)
{
    *size = MEMDRV_calcCrcAtomic_size_argument;
    MEMDRV_calcCrcAtomic_addr = (volatile uint32_t *)addr;
    MEMDRV_calcCrcAtomic_size = (volatile uint32_t *)size;
    MEMDRV_calcCrcAtomic_crc = (volatile CRC_crc_t *)crc;
    return (enum MAL_Status) MEMDRV_calcCrcAtomic_RET;
}
