/*******************************************************************************
*   Copyright (c) 2023 BSH Hausgeraete GmbH,
*   Carl-Wery-Str. 34, 81739 Munich, Germany, www.bsh-group.de
*
*   All rights reserved. This program and the accompanying materials
*   are protected by international copyright laws.
*   Please contact copyright holder for licensing information.
*
********************************************************************************
*   PROJECT          Generic SW
*   AUTHOR           Wu Xiuwei
*   CREATED          10.05.2023
*******************************************************************************/


/******************************************************************************/
/* DOCUMENTATION                                                              */
/******************************************************************************/
/** \file    Processor.h
*
*   \brief    Global processor include for SH30F9X71.
*/
#ifndef __PROCESSOR_H
#define __PROCESSOR_H

#ifdef __cplusplus
extern "C" {
#endif

#include "bsh_stdinc.h"

#ifdef MCAL_MWDT_INCLUDED
    // MCAL Watchdog mapping
    #define HWDT_vTrigger       MWDT_trigger
#endif 

/* Suppress vendor lint issues.
    Note 9021: use of '#undef' is discouraged
    Note 9027: an enum value is not an appropriate operand to !
    Note 9027: a signed value is not an appropriate left operand to <<
    Note 9053: the shift value is at least the precision of the essential type of the left hand side
    Note 923: explicit cast from 'uint32_t' (aka 'unsigned int') to 'RCC_TypeDef *'
    Supplemental 893: expanded from macro 'RCC'
    Warning 1564: converting integer constant expression, which evaluates to 0 but is not an integer literal equal to zero or one, to bool
    Info 849: enumerator 'xx' reuses the constant value 'n' previously used by enumerator 'yy'
    Warning 488: enumerator 'xx' reuses the constant value 'n' previously used implicitly by enumerator 'yy'
    
*/
//lint -save -e9021 -e9027 -e9053 -e923 -e893 -e1564 -e849 -e488
#if defined(SH30F9071) || defined(SH30F9871)
#include "sh30f9xx_sa0.h"
#include "system_sh30f9xx_sa0.h"
#include "sh30f9xx_sa0_libcfg.h"

#elif  defined(UNIT_TEST)
#warning "Processor derivative not defined, consider possibly required register-stubs."

#elif !defined(LIBRARY_BUILD)
#error "Processor derivative not defined, please check compiler options!"
#endif


#include "features.h"

#include "sh30f_ext_cfg.h"
#include "sh30f_ext.h"
//lint -restore -e9021 -e9027 -e9053 -e923 -e893 -e1564 -e849 -e488

#ifdef __cplusplus
}
#endif

#endif // __PROCESSOR_H

