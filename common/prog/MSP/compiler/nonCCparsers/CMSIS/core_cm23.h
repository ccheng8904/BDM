/*******************************************************************************
 *  Copyright (c) 2017 BSH Hausgeraete GmbH,
 *  Carl-Wery-Str. 34, 81739 Munich, Germany, www.bsh-group.de
 *
 *  All rights reserved. This program and the accompanying materials
 *  are protected by international copyright laws.
 *  Please contact copyright holder for licensing information.
 *
 *******************************************************************************
 *  PROJECT          MCU Framework
 *  Purpose          Wrapper for original CMSIS core_cm23.h
 ******************************************************************************/


#ifndef CORE_CM23_NON_CC_H
#define CORE_CM23_NON_CC_H

/* Note 923: explicit cast from 'unsigned long' to 'SCB_Type *' [MISRA 2012 Rule 11.1, required], [MISRA 2012 Rule 11.4, required], [MISRA 2012 Rule 11.6, required]
 * Note 9023: multiple use of stringize/pasting operators in definition of macro _VAL2FLD [MISRA 2012 Rule 1.3, required]
 * Note 9029: an unsigned value and a signed value cannot be used together as operands to - [MISRA 2012 Rule 10.4, required]
 * Note 9031: cannot assign a composite expression of type 'unsigned8' to an object of wider type 'unsigned32' [MISRA 2012 Rule 10.6, required]
 * Note 9018: union 'R_TFU_Type::(anonymous)' declared [MISRA 2012 Rule 19.2, advisory]
 * Note 954: local variable 'vectors' could be pointer to const [MISRA 2012 Rule 8.13, advisory]
 * Note 9032: left operand to & is a composite expression of type 'unsigned8' which is smaller than the right operand of type 'unsigned32' [MISRA 2012 Rule 10.7, required]
 * Note 931: both sides have side effects
*/
//lint -save -e923 -e954 -e9018 -e9023 -e9029 -e9031 -e9032 -e931
//lint -emacro(923,SysTick*)
#include "cmsis_compiler.h"
#include "MSP/CMSIS/core_cm23.h"
//lint -restore -e923 -e954 -e9018 -e9023 -e9029 -e9031 -e9032 -e931

#endif // CORE_CM23_NON_CC_H
