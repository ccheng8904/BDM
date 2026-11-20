/*******************************************************************************
*   Copyright (c) 2016 BSH Hausgeraete GmbH,
*   Carl-Wery-Str. 34, 81739 Munich, Germany, www.bsh-group.de
*
*   All rights reserved. This program and the accompanying materials
*   are protected by international copyright laws.
*   Please contact copyright holder for licensing information.
*
********************************************************************************
*   PROJECT          Generic SW
*   COMP_ABBREV      INTP    
*******************************************************************************/


/******************************************************************************/
/* DOCUMENTATION                                                              */
/******************************************************************************/
/** \file
*
*   \brief    Default initialization of Interrupt Levels
*
*/

#include "bsh_stdinc.h" 
#include "int_prio.h"
#include <stdint.h>


/******************************************************************************/
/* PRIVATE DEFINITIONS                                                        */
/******************************************************************************/

//lint -emacro(923,NVIC_IPR_, SHPR*)
#define NVIC_IPR_      ((uint32_t*)0xE000E400U)   /*!< Base address Interrupt Priority Registers                */
#if !defined(CORTEX_M0) && !defined(CORTEX_M0_PLUS)
    #define SHPR1_         (*(uint32_t*)0xE000ED18U)   /*!< System Handler Priority Register 1                   */
#endif
#define SHPR2_         (*(uint32_t*)0xE000ED1CU)   /*!< System Handler Priority Register 2                       */
#define SHPR3_         (*(uint32_t*)0xE000ED20U)   /*!< System Handler Priority Register 3                       */


#if defined(CORTEX_M0) || defined(CORTEX_M0_PLUS)
#define IPR_MAX   8u  /*!< CM0:                     0xE000E400-0xE000E41C NVIC_IPR0-NVIC_IPR07  */
#elif defined(CORTEX_M33)
#define IPR_MAX 120u  /*!< CM33:                    0xE000E400-0xE000E5DF NVIC_IPR0-NVIC_IPR119 */
#else
#define IPR_MAX  60u  /*!< CM3, CM4, CM4F CM7 CM23: 0xE000E400-0xE000E4EF NVIC_IPR0-NVIC_IPR59  */
#endif


/******************************************************************************/
/* PUBLIC FUNCTION DEFINITIONS                                                */
/******************************************************************************/

void INTP_setDefaultInterruptPriority(void)
{   /* Note 923: explicit cast from 'unsigned int' to 'uint32_t *'
     * Necessary here and not a problem on CortexM */
    /*lint --e{923} */
    for (uint8_t i=0; i<IPR_MAX; i++)
    {
        NVIC_IPR_[i] = DFLT_INT_PRIO;
    }

#if !defined(CORTEX_M0) && !defined(CORTEX_M0_PLUS)
    SHPR1_ = DFLT_INT_PRIO;      /*!< System Handler Priority Register 1 */
#endif
    SHPR2_ = DFLT_INT_PRIO;      /*!< System Handler Priority Register 2 */
    SHPR3_ = DFLT_INT_PRIO;      /*!< System Handler Priority Register 3 */
}
