/*******************************************************************************
*   Copyright (c) 2017 BSH Hausgeraete GmbH,
*   Carl-Wery-Str. 34, 81739 Munich, Germany, www.bsh-group.de
*
*   All rights reserved. This program and the accompanying materials
*   are protected by international copyright laws.
*   Please contact copyright holder for licensing information.
*
********************************************************************************
*   PROJECT          PTEng
*   COMP_ABBREV      Common
*******************************************************************************/


#ifndef COMMON_RD_STUB_H
#define COMMON_RD_STUB_H

#ifdef __cplusplus
extern "C" {
#endif


/******************************************************************************/
/* DOCUMENTATION                                                              */
/******************************************************************************/
/** \file
*
*   \brief    Function call redirection for dynamic SW unit test
*             for all undefined functions
*
*/


/******************************************************************************/
/* PUBLIC STUB FUNCTION DECLARATIONS resp.                                    */
/* REDIRECTION OF FUNCTION CALLS TO STUB CALLS                                */
/******************************************************************************/


// Isolate units that are under test, i.e.
//  1)  if any other (='foreign') translation is under test and if this unit shall be stubbed: prevent the other (='foreign') unit from using (=calling) this unit by stubbing this unit's interface
#if (defined (UNIT_UNDER_TEST)) && (defined (FOR_UNIT_UNDER_TEST_STUB_COMMONSTUB))
    #define APPLY_STUBBING_COMMONSTUB_TEST
#endif

#if defined (APPLY_STUBBING_COMMONSTUB_TEST)
    		#define BMDAT_setErrorFlag BMDAT_setErrorFlag_Stub
		#define BMDAT_setErrorModule BMDAT_setErrorModule_Stub
		#define CRC_Calc CRC_Calc_Stub
		#define CRC_Finish CRC_Finish_Stub
		#define CRC_Init CRC_Init_Stub
		#define CRC_Update CRC_Update_Stub
		#define MEMDRV_calcCrc MEMDRV_calcCrc_Stub
		#define MEMDRV_close MEMDRV_close_Stub
		#define MEMDRV_eraseRange MEMDRV_eraseRange_Stub
		#define MEMDRV_flush MEMDRV_flush_Stub
		#define MEMDRV_getTimeout MEMDRV_getTimeout_Stub
		#define MEMDRV_open MEMDRV_open_Stub
		#define MEMDRV_read MEMDRV_read_Stub
		#define MEMDRV_write MEMDRV_write_Stub
		#define memcpy memcpy_Stub
		#define strlen strlen_Stub

#endif




#ifdef __cplusplus
}
#endif

#endif // COMMON_RD_STUB_H
