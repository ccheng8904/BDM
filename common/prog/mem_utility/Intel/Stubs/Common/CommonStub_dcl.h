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


#ifndef COMMONSTUB_DCL_STUB_H
#define COMMONSTUB_DCL_STUB_H

#ifdef __cplusplus
extern "C" {
#endif


/******************************************************************************/
/* DOCUMENTATION                                                              */
/******************************************************************************/
/** \file
*
*   \brief    Common Stub function declaration for dynamic SW unit test.
*
*/


/******************************************************************************/
/* INCLUDES                                                                   */
/* (necessary  if this file is provided as stand alone translation unit       */
/* in an application project)                                                 */
/******************************************************************************/
#include "hsup.h"
#include "hsupMc.h"
#include "LibTypes.h"
#include "ring_buffer.h"

/* PUBLIC STUB FUNCTION DECLARATIONS                                          */
/******************************************************************************/
extern void HSUP_vDisableInt_Stub(void);
extern void HSUP_vEnableInt_Stub(void);
extern size_t strlen_Stub(const  char * str);



#ifdef __cplusplus
}
#endif

#endif // COMMONSTUB_DCL_STUB_H
