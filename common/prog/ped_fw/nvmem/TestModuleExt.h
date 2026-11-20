/*****************************************************************************************************************
* Copyright by BSH Bosch und Siemens Hausgeraete GmbH - EDS                                                       
*                                                                                                                 
*-----------------------------------------------------------------------------------------------------------------
*                                                                                                                 
* Die Nutzung, die Vervielfaeltigung und die Weitergabe dieser Software oder ihrer Teile ist nur mit              
* ausdruecklicher schriftlicher Zustimmung der BSH EDS gestattet. Die Software darf vom Verwender                 
* ausschliesslich fuer die Entwicklung von Elektroniken fuer BSH genutzt wer-den, eine Nutzung fuer               
* sonstige Zwecke (z.B. Entwicklung von Elektroniken fuer Dritte) ist untersagt. Saemtliche Rechte,               
* einschliesslich aller Urheber-, Patent- und Gebrauchsmuster-rechte, an der Software bleiben BSH                 
* vorbehalten. Zuwiderhandlungen werden straf- und zivilrechtlich verfolgt.                                       
*                                                                                                                 
* Die Software wurde nach dem Stand der Technik erstellt und geprueft. Aufgrund der Kom-plexitaet der             
* Implementierung von Embedded Controller Software sind  vom Verwender fuer jede Implementierung eigene           
* Tests durchzufuehren, um die Funktionssicherheit in seiner Entwicklung sicherzustellen. Die Software            
* wurde fuer den Einsatz als Bibliothek entwickelt. Jede anderweitige Verwendung erfolgt in alleiniger            
* Verantwortung des Nutzers. Der Nutzer ist berechtigt, die Software zur Anpassung an andere                      
* Microcontroller (als Motorola HC08) und andere Compiler (als Metrowerks) in eigener Verantwortung zu            
* veraendern  und anzu-passen. BSH leistet keinen Support fuer derartige Anpassungen, ist jedoch                  
* berechtigt, von Verwender vorgenommene Anpassungen fuer eigene Zwecke kostenfrei zu nutzen.                     
*                                                                                                                 
* Die Haftung der BSH fuer die Funktionsfaehigkeit und -sicherheit der Software, auch beim  Einsatz in            
* einer konkreten Anwendung, und moegliche, durch sie verursachte Folgeschae-den ist, ausser im Falle von         
* Vorsatz oder grober Fahrlaessigkeit, ausgeschlossen. In jedem Fall ist die Haftung der BSH jedoch auf           
* den typischerweise auftretenden, vorhersehbaren Schaden begrenzt.                                               
*                                                                                                                 
* Technische AEnderungen sind vorbehalten.                                                                        
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
* The user is sole responsible for every other usage.  The user may modify the software for adaptations           
* needed for other microcontrollers than the Motorola HC08, as well as to another compilers than                  
* Metrowerks at own risk. BSH will not support any adaptations or changes. BSH is authorised to use all           
* adaptations for own purposes free of charge.                                                                    
*                                                                                                                 
* BSH assumes no liability for the functionality or reliability of the software even in concrete                  
* applications. BSH assumes no liability for consequential damages, except in case of intention or gross          
* negligence. In any case the liability is limited to the typical and predictable damage.                         
*                                                                                                                 
* Technical changes are reserved.                                                                                 
*                                                                                                                 
******************************************************************************************************************
 *   PROJECT          TEST_MODULE
 *   MODULE-PREFIX    TM_
 *   FILE             %PM%
 *   ARCHIVE          %PP%:%PI%
 *   PROCESSOR        independent of processor
 *   COMPILER         independent of compiler
 *****************************************************************************************************************
 *   AUTHOR           emka
 *   CREATED          2006-21-09
 *
 *   LAST CHANGE      %PRT%
 *   LAST REVISION    %PR%
 *   STATUS           %PS%
 *****************************************************************************************************************
   DESCRIPTION                                                                                                   */
/** \file TestModuleExt.h
   Test Module Extensions to be used by projects and libraries. Optional Test Features for code instrumentation
   are supported in this file.
*/
 /*****************************************************************************************************************
 *   CHANGES
 *   %PL%
 *
 *****************************************************************************************************************/

/************************************************************
*****               IMPORTANT NOTE                     ******
*************************************************************
*                                                           *
*    This module contains macros for test support.          *
*    All tests should be make under debug conditions.       *
*    Debug conditions are switched via test defines.        *
*    These switches must be undefined in release software.  *
*    Than all test macros contains empty do while loops.    *
*/

#ifndef __TESTMODULEEXT_H
#define __TESTMODULEEXT_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes */
#include "LibTypes.h"
#include "timer/timerlib.h"
#include "testmodulexs.h"

/* Definitions and Declarations */

/* DEFINITIONS ****************************************************************************************/

/* MACROS ****************************************************************************************/

/* Macros */
/* Test Stub Routing (TESTUB) */
#ifdef UNIT_TEST 
/**
   Macro to route a local function to its stub.

\param function - name of the original function
*/
#define  TESTUB(function)  function ## _Stub
#else
#define  TESTUB(function)  function
#endif

/* Test Points(TP) */
/* TM_SET_TESPOINT(tpId) */
#ifdef TM_TESTPOINTS_INCLUDED 
/**
   Macro to set a test point.

\param tpId - test point id as defined by user
*/
#define TM_SET_TESPOINT(tpId)   do {     \
   if ((uint)(tpId) < (uint)TM_MAX_TESTPOINTS) {     \
      TM_aucTestPoints[(tpId)] = (uchar)(tpId); \
   }}while (0) 
#else
#define TM_SET_TESPOINT(tpId)   do {} while(0)
#endif

/* TM_CLEAR_TESPOINT(tpId) */
#ifdef TM_TESTPOINTS_INCLUDED
/**    
   Macro to clear a test point.

\param tpId - test point id as defined by user
*/
#define TM_CLEAR_TESPOINT(tpId) do { \
   if ((tpId) < TM_MAX_TESTPOINTS) { \
      TM_aucTestPoints[(tpId)] = 0;  \
   }}while(0) 
#else
#define TM_CLEAR_TESPOINT(tpId) do {} while(0)
#endif

/* TM_CHECK_TESPOINT(tpId) */
#ifdef TM_TESTPOINTS_INCLUDED
/**    
   Macro to check a test point.

\param tpId - test point id as defined by user
\return 
   TRUE  - test point set
   FALSE - test point not set
*/
#define TM_CHECK_TESPOINT(tpId)          \
   (((tpId) < TM_MAX_TESTPOINTS) &&      \
    (TM_aucTestPoints[(tpId)] == (tpId)))
#else
#define TM_CHECK_TESPOINT(tpId) FALSE
#endif

/* TM_SET_VALUE_TESPOINT(tpId, value) */
#ifdef TM_TESTPOINTS_INCLUDED
/**    
   Macro to set a test point to a value.

\param tpId - test point id as defined by user
\param value - value of test point
*/
#define TM_SET_VALUE_TESPOINT(tpId, value) do { \
   if ((tpId) < TM_MAX_TESTPOINTS) {            \
      TM_aucTestPoints[(tpId)] = (value);       \
   }}while (0) 
#else
#define TM_SET_VALUE_TESPOINT(tpId, value)   do {} while(0)
#endif

/* TM_CHECK_VALUE_TESPOINT(tpId,value) */
#ifdef TM_TESTPOINTS_INCLUDED
/**    
   Macro to check the value of a test point.

\param tpId - test point id as defined by user
\param value - value of test point
\return 
   TRUE  - value of test point okay
   FALSE - value of test point not okay
*/
#define TM_CHECK_VALUE_TESPOINT(tpId,value) \
   (((tpId) < TM_MAX_TESTPOINTS) &&         \
    (TM_aucTestPoints[(tpId)] == (value)))

#else
#define TM_CHECK_VALUE_TESPOINT(tpId, value) FALSE
#endif

/* TM_GET_VALUE_TESPOINT(tpId) */
#ifdef TM_TESTPOINTS_INCLUDED
/**    
   Macro to get the value of a test point.

\param tpId - test point id as defined by user
\return 
   value of test point
*/
#define TM_GET_VALUE_TESPOINT(tpId) \
    (TM_aucTestPoints[(tpId)])
#else
#define TM_GET_VALUE_TESPOINT(tpId)          FALSE
#endif

/* TM_INC_VAL_TESPOINT(tpId) */
#ifdef TM_TESTPOINTS_INCLUDED
/**    
   Macro to increment a test point in the array.

\param tpId - test point id as defined by user
*/
#define TM_INC_VAL_TESPOINT(tpId) do { \
   if ((tpId) < TM_MAX_TESTPOINTS) {            \
      TM_aucTestPoints[(tpId)] = TM_aucTestPoints[(tpId)] + 1;       \
   }}while (0) 
#else
#define TM_INC_VAL_TESPOINT(tpId)   do {} while(0)
#endif

/* TM_ADD_TESPOINT(tpId) */
#ifdef TM_TESTPOINTS_INCLUDED
/**    
   Macro to add a test point to a history array.

\param tpId - test point id as defined by user
*/
#define TM_ADD_TESPOINT(tpId) do {              \
if (TM_ucTpIndex < TM_MAX_TESTPOINTS) {         \
      TM_aucTestPoints[TM_ucTpIndex] = (tpId);  \
      TM_ucTpIndex++;                           \
   }}while (0) 
#else
#define TM_ADD_TESPOINT(tpId)   do {} while(0)
#endif

/* SHORT MACROS TM_TPx */
#define TM_TPSet(tpId)       TM_SET_TESPOINT(tpId)
#define TM_TPC(tpId)         TM_CLEAR_TESPOINT(tpId)
#define TM_TPCK(tpId)        TM_CHECK_TESPOINT(tpId)
#define TM_TPV(tpId, value)  TM_SET_VALUE_TESPOINT(tpId, value)
#define TM_TPCV(tpId, value) TM_CHECK_VALUE_TESPOINT(tpId,value)
#define TM_TPG(tpId)         TM_GET_VALUE_TESPOINT(tpId)
#define TM_TPI(tpId)         TM_INC_VAL_TESPOINT(tpId)
#define TM_TPA(tpId)         TM_ADD_TESPOINT(tpId)

/* 16 Bit Time Measurement */
/* TM_RUNTIME16_START(rtId) */
#ifdef TM_RTIME16_INCLUDED
/**    Macro to start a runtime measurement.

   \param    rtId - runtime measurement id as defined by user
   \remarks  Use this macros for times below 65 ms.
*/
#define TM_RUNTIME16_START(rtId)     do { \
   if ((rtId) < TM_MAX_RTIME_ENTRIES)   { \
      TM_auiRuntimeValues[(rtId)] =       \
         TIM_uiGetCircleMicroSeconds();   \
   }}while(0)    
#else
#define TM_RUNTIME16_START(rtId)    do {} while(0)
#endif

/* TM_RUNTIME16_STOP(rtId) */
#ifdef TM_RTIME16_INCLUDED
/**    Macro to stop runtime measurement.

   \param    rtId - runtime measurement id as defined by user
   \remarks  Use this macros for times below 65 ms.
*/
#define TM_RUNTIME16_STOP(rtId)       do { \
   if ((rtId) < TM_MAX_RTIME_ENTRIES)    { \
      TM_auiRuntimeValues[(rtId)] =        \
         (TIM_uiGetCircleMicroSeconds() -  \
          TM_auiRuntimeValues[(rtId)]);    \
   }} while(0)
#else
#define TM_RUNTIME16_STOP(rtId)     do {} while(0)
#endif

/* TM_RUNTIME16_GET(rtId,uiRt) */
#ifdef TM_RTIME16_INCLUDED
/**    Macro to get measured runtime.

   \param    rtId  - runtime measurement id as defined by user
   \param    uiRt - variable in which time is written
   \remarks  Use this macros for times below 65 ms.
*/
#define TM_RUNTIME16_GET(rtId,uiRt) do {             \
        (uiRt) = (((rtId) < TM_MAX_RTIME_ENTRIES) ?  \
                   TM_auiRuntimeValues[(rtId)] : 0); \
        } while(0)
#else
#define TM_RUNTIME16_GET(rtId,ulRt) do {ulRt = 0;} while(0)  
#endif

/* TM_RUNTIME16_CLEAR(rtId) */
#ifdef TM_RTIME16_INCLUDED
/**    Macro to clear an entry in runtime array.

   \param    rtId  - runtime measurement id as defined by user
   \remarks  Use this macros for times below 65 ms.
*/
#define TM_RUNTIME16_CLEAR(rtId) do {   \
   if ((rtId) < TM_MAX_RTIME_ENTRIES) { \
      TM_auiRuntimeValues[(rtId)] = 0;  \
   }} while(0)
#else
#define TM_RUNTIME16_CLEAR(rtId)    do {} while(0) 
#endif

/* 32 Bit Time Measurement */
/* TM_RUNTIME32_START(rtId) */
#ifdef TM_RTIME32_INCLUDED
/**    Macro to start a runtime measurement.

   \param    rtId - runtime measurement id as defined by user
   \remarks  Use this macros for times over 65 ms.
*/
#define TM_RUNTIME32_START(rtId)     do { \
   if ((rtId) < TM_MAX_RTIME_ENTRIES)   { \
      TM_au32RuntimeValues[(rtId)] =      \
         TIM_u32GetCircleMicroSeconds();  \
   }}while(0)    
#else
#define TM_RUNTIME32_START(rtId)    do {} while(0)
#endif

/* TM_RUNTIME32_STOP(rtId) */
#ifdef TM_RTIME32_INCLUDED
/**    Macro to stop runtime measurement.

   \param rtId - runtime measurement id as defined by user
   \remarks  Use this macros for times over 65 ms.
*/
#define TM_RUNTIME32_STOP(rtId)       do { \
   if ((rtId) < TM_MAX_RTIME_ENTRIES)    { \
      TM_au32RuntimeValues[(rtId)] =       \
         (TIM_u32GetCircleMicroSeconds() - \
          TM_au32RuntimeValues[(rtId)]);   \
   }} while(0)
#else
#define TM_RUNTIME32_STOP(rtId)     do {} while(0)
#endif

/* TM_RUNTIME32_GET(rtId,u32Rt) */
#ifdef TM_RTIME32_INCLUDED
/**    Macro to get measured runtime.

   \param rtId  - runtime measurement id as defined by user
   \param u32Rt - variable in which time is written
   \remarks  Use this macros for times over 65 ms.
*/
#define TM_RUNTIME32_GET(rtId,u32Rt) do {             \
        (u32Rt) = (((rtId) < TM_MAX_RTIME_ENTRIES) ?  \
                   TM_au32RuntimeValues[(rtId)] : 0); \
        } while(0)
#else
#define TM_RUNTIME32_GET(rtId,ulRt) do {ulRt = 0;} while(0)  
#endif

/* TM_RUNTIME32_CLEAR(rtId) */
#ifdef TM_RTIME32_INCLUDED
/**    Macro to clear an entry in runtime array.

   \param rtId  - runtime measurement id as defined by user
   \remarks  Use this macros for times over 65 ms.
*/
#define TM_RUNTIME32_CLEAR(rtId) do {   \
   if ((rtId) < TM_MAX_RTIME_ENTRIES) { \
      TM_au32RuntimeValues[(rtId)] = 0; \
   }} while(0)
#else
#define TM_RUNTIME32_CLEAR(rtId)    do {} while(0) 
#endif

/* Trigger Setting */
#ifdef TM_TRIGGER_INCLUDED 
/**    Macro to set a trigger value.
\param ucTrgPin  - Trigger pin in range 1 to 255
\param bTrgOn    - TRUE = Set Trigger / FALSE = Clear Trigger
\remarks
   - Setting trgPin=0 clears all trigger prins
   - Trigger is set by setting a pin to high or low.
   - It is therefore assumed that trigger was initialized and that appropriate
     functions are available to set the trigger 
*/
#define TM_SET_TRIGGER(ucTrgPin, bTrgOn)  do {TM_vSetTrg((ucTrgPin), (bTrgOn));} while (0)
#else
#define TM_SET_TRIGGER(ucTrgPin, bTrgOn)  do {} while (0)
#endif

/* Sample Values */
/* TM_TRIGGER_SAMPLING */
#ifdef TM_SAMPLEVALUES_INCLUDED
/**    
   Macro to reset sample value array.

\param none 
*/
#define TM_TRIGGER_SAMPLING do {            \
   TM_uiSampleIndex = 0;                    \
   }while (0) 
#else
#define TM_TRIGGER_SAMPLING   do {} while(0)
#endif

/* TM_ADD_SAMPLEVALUE(val) */
#ifdef TM_SAMPLEVALUES_INCLUDED
/**    
   Macro to sample values into the sample value array.

\param val - value to sample 
*/
#define TM_ADD_SAMPLEVALUE(term,val) do {            \
if (TM_uiSampleIndex < TM_MAX_SAMPLEVALUES && term == TM_ucTermIndex) {   \
      TM_auiSampleValues[TM_uiSampleIndex] = (uint)(val);  \
      TM_uiSampleIndex++;                           \
   }}while (0) 
#else
#define TM_ADD_SAMPLEVALUE(term,val)   do {} while(0)
#endif

/* TM_SET_SAMPLE_TERM(term) */
#ifdef TM_SAMPLEVALUES_INCLUDED
/**    
   Macro to set the term, that should be sampled.

\param term - term to sample 
*/
#define TM_SET_SAMPLE_TERM(term)   do {\
   TM_ucTermIndex = term;              \
} while(0)
#else
#define TM_SET_SAMPLE_TERM(term)   do {} while(0)
#endif

/* Trace Address */
/* TM_TRIGGER_TRACE_ADR */
#ifdef TM_ADDRESSTRACE_INCLUDED
/**    
   Macro to reset address trace array.

\param none 
*/
#define TM_TRIGGER_TRACE_ADR do {            \
   TM_ucFncIndx = 0;                         \
   }while (0) 
#else
#define TM_TRIGGER_TRACE_ADR   do {} while(0)
#endif

/* TM_TRACE_FUNCTION_ADR(val) */
#ifdef TM_ADDRESSTRACE_INCLUDED
/**    
   Macro to sample values into the sample value array.

\param val - value to sample 
*/
#define TM_TRACE_FUNCTION_ADR(adr) do    {                   \
if   (TM_ucFncIndx < TM_MAX_FUNCADDRESS) {                   \
      TM_aulFuncAddress[TM_ucFncIndx] = (ulong)(adr);        \
      TM_ucFncIndx     ++;                                   \
     }                                                       \
else {TM_aulFuncAddress[0] = (ulong)(adr);                   \
      TM_ucFncIndx         = 1;                              \
}}while (0) 
#else
#define TM_TRACE_FUNCTION_ADR(adr)   do {} while(0)
#endif

/* Function Trace */
#ifdef TM_TRACEFUNC_INCLUDED 
#define TM_vTrace(funcaddr,tp) do {                            \
   if (TM_ucTraceIndx >= TM_MAX_TRACES) {                      \
      TM_ucTraceIndx = 0;                                      \
   }                                                           \
   TM_aulTraceFuncAddress[TM_ucTraceIndx] = (ulong)(funcaddr); \
   TM_aucTraceFuncTestPoint[TM_ucTraceIndx] = (tp);            \
   TM_auiTraceFuncTimeStamp [TM_ucTraceIndx] =                 \
                                 TIM_uiGetCircleMicroSeconds();\
   TM_ucTraceIndx ++;                                          \
   } while (0)
#else
#define TM_vTrace(funcaddr,tp) do {} while (0)
#endif

/* TYPE DEFINITIONS ***********************************************************************************/

/* VARIABLES ******************************************************************************************/
/* Variables */
/* Test Points */
#ifdef TM_TESTPOINTS_INCLUDED
/** 
   Global buffer containing test points.
\remarks
   -# User must define this variables in his code to use this feature (XS-File).
   -# User can access this buffer and read data by script from host.
*/
extern uchar TM_aucTestPoints [TM_MAX_TESTPOINTS];
extern uchar TM_ucTpIndex;
#endif

/* 16-Bit Time Measurement */
#ifdef TM_RTIME16_INCLUDED 
/** 
   Global buffer containing 16-bit time measurement units.
\remarks
   -# 16-Bit timer is more accurate for durations below 65 ms.
   -# User must define this variable in his code to use this feature (XS-File).
   -# User can access this buffer and read data by script from host.
*/
extern uint TM_auiRuntimeValues [TM_MAX_RTIME_ENTRIES];
#endif

/* 32-Bit Time Measurement */
#ifdef TM_RTIME32_INCLUDED 
/** 
   Global buffer containing 32-bit time measurement units.
\remarks
   -# 32-Bit timer is suitable for durations above 65 ms.
   -# User must define this variable in his code to use this feature (XS-File).
   -# User can access this buffer and read data by script from host.
*/
extern uint32 TM_au32RuntimeValues [TM_MAX_RTIME_ENTRIES];
#endif

/* Sample Values */
#ifdef TM_SAMPLEVALUES_INCLUDED
/** 
   Global buffer containing sample values.
\remarks
   -# User must define this variables in his code to use this feature (XS-File).
   -# User can access this buffer and read data by script from host.
*/
extern uint  TM_auiSampleValues [TM_MAX_SAMPLEVALUES];
extern uint  TM_uiSampleIndex;
extern uchar TM_ucTermIndex;
#endif

/** Sample terms for sample values */

/* Address Trace */
#ifdef TM_ADDRESSTRACE_INCLUDED
/** Definition of function address trace array */
extern ulong TM_aulFuncAddress [TM_MAX_FUNCADDRESS];
extern uchar TM_ucFncIndx;
#endif

/* Function Trace */
#ifdef TM_TRACEFUNC_INCLUDED 
/** Buffer for Trace function adresses */
extern ulong TM_aulTraceFuncAddress[TM_MAX_TRACES];
/** Buffer for Trace function test points,
    \remark : instead of an ulong [][], a char variable is ised to save memory
*/
extern uchar TM_aucTraceFuncTestPoint[TM_MAX_TRACES];
/** Buffer for Trace function timestamp */
extern uint TM_auiTraceFuncTimeStamp[TM_MAX_TRACES];

/** Trace index */
extern uchar TM_ucTraceIndx;
#endif

#ifdef __cplusplus
}
#endif

#endif //#ifndef __TESTMODULEEXT_H
