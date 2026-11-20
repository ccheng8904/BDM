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
*   \brief    User defined wrapper function implementation
*             for dynamic SW unit test
*             for ring_buffer.c.             .
*             
*   \note     This is not a stand alone translation unit. It is included
*             by automatically generated file  ring_buffer_ut_Wrapper.c
*/


/******************************************************************************/
/* PRIVATE DEFINITIONS                                                        */
/******************************************************************************/
// the following defines are used to enable or disable the user defined wrapper functions:
// if the according identifier is defined (= not commented out)
// then the user defined wrapper function is defined (here in ring_buffer_User_ut_Wrapper.c),
// otherwise the default wrapper function is defined (in automatically generated file ring_buffer_ut_Wrapper.c)


/******************************************************************************/
/* PUBLIC DATA DEFINITIONS to be used by TEng                                 */
/******************************************************************************/



/******************************************************************************/
/* PUBLIC WRAPPER FUNCTION DEFINITIONS                                        */
/******************************************************************************/

#define RBUF_WRITEDATA_U64_UT_WRAPPER
void RBUF_writeData_u64_ut_wrapper (void)
{
    RBUF_writeData_u64_return_argument = (bool)RBUF_writeData_u64(RBUF_writeData_u64_rb_argument_p, (uint64_t)0x0123456789abcdef);
}


volatile uint32_t RBUF_readData_u64_data_arg_hi;
volatile uint32_t RBUF_readData_u64_data_arg_lo;


#define RBUF_READDATA_U64_UT_WRAPPER
void RBUF_readData_u64_ut_wrapper (void)
{
    uint64_t RBUF_readData_u64_data_arg;
    RBUF_readData_u64_return_argument = (bool)RBUF_readData_u64(RBUF_readData_u64_rb_argument_p, &RBUF_readData_u64_data_arg);
    if(RBUF_readData_u64_return_argument)
    {
        RBUF_readData_u64_data_arg_hi = (uint32_t)(RBUF_readData_u64_data_arg >> 32u);
        RBUF_readData_u64_data_arg_lo = (uint32_t)RBUF_readData_u64_data_arg;
    }
}
