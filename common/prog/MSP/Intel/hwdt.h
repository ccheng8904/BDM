/*****************************************************************************************************************
* Copyright by BSH Bosch und Siemens Hausgeraete GmbH - EDS                                                       
*                                                                                                                 
*-----------------------------------------------------------------------------------------------------------------
*                                                                                                                 
* Use, reproduction and dissemination of this software or parts of it, is not permitted without express           
* written authority of BSH EDS. The user is allowed to use this software exclusively for the development          
* of electronic boards for BSH. Usage for other purposes is strictly prohibited (e.g. development of              
* electronic boards for third parties). All rights, including copyright, rights created by patent grant           
* or registration, and rights by protection of utility patents, are reserved. Violations will be                  
* prosecuted by civil and criminal law.                                                                           
*                                                                                                                 
* The software was created and approved by acknowledged rules of technology. Because of the complexity            
* of embedded controller software the user of this software has to perform his own tests to ensure                
* proper functionality in his environment. The software was developed for usage as a software library.            
* The user is sole responsible for every other usage. The user may modify the software for adaptations            
* needed for other microcontrollers, as well as to other compilers at own risk. BSH will not support              
* any adaptations or changes. BSH is authorised to use all adaptations for own purposes free of charge.           
* BSH assumes no liability for the functionality or reliability of the software even in concrete                  
* applications. BSH assumes no liability for consequential damages, except in case of intention or gross          
* negligence. In any case the liability is limited to the typical and predictable damage.                         
*                                                                                                                 
* Technical changes are reserved.                                                                                 
*                                                                                                                 
******************************************************************************************************************
*   PROJECT          Generic Intel HAL
*   MODULE-PREFIX    HWDT
*   FILE             %PM%
*   ARCHIVE          %PP%:%PI%
*   PROCESSOR        independent of processor
*   COMPILER         independent of compiler 
*****************************************************************************************************************
*   AUTHOR           Thorsten Klein
*                                                                                                                
*   LAST CHANGE      %PRT%
*   PRESENT REVISION %PR%
*   STATUS           %PS%
*****************************************************************************************************************
*   DESCRIPTION
*   Public definitions and declarations for unit test of HWDT.
*****************************************************************************************************************
*   CHANGES
*   %PL%
* 
*****************************************************************************************************************/
/*! \file
     Public definitions and declarations for unit test of HWDT.\n
*/

#ifndef __HWDT_STUB_H
#define __HWDT_STUB_H

#ifdef __cplusplus
extern "C" {
#endif

#include "LibTypes.h" 


// The Intel implementations of all functions set the accordant test point named <func_name>_TP (see also hwdt_Stub_TPs.h).
// The parameters of all functions are stored by the Intel implementation of that function in a variable named <func_name>_<param_name> of the accordant type that can be read out by the test engine;
// for all  functions that have a return value the Intel implementation of that functions returns the value of a variable named <func_name>__RET that can be set by the test engine.

// test variable declaration
extern volatile uchar  HWDT_vInit_index;

// Intel-HAL function declaration

//#ifdef TIMERLIGHT_MODULE

extern void HWDT_vInit(uchar index);
static inline void HWDT_vTrigger(void) {}

//#endif

#ifdef __cplusplus
}
#endif

#endif


























