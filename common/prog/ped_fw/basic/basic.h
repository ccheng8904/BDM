/*******************************************************************************
 *  Copyright (c) 2017 BSH Hausgeraete GmbH,
 *  Carl-Wery-Str. 34, 81739 Munich, Germany, www.bsh-group.de
 *
 *  All rights reserved. This program and the accompanying materials
 *  are protected by international copyright laws.
 *  Please contact copyright holder for licensing information.
 *
 *******************************************************************************
 *  PROJECT          PP PED Framework
 *  COMP_ABBREV      BASIC
 ******************************************************************************/


#ifndef BASIC_H
#define BASIC_H

#ifdef __cplusplus
extern "C" {
#endif

/******************************************************************************/
/* DOCUMENTATION                                                              */
/******************************************************************************/
/** \file
 *
 *  \ingroup  PED-Framework
 *
 *  \brief    Public definitions and declarations for basic function unit.
 *
 *  \details  Provides interfaces to initialise controller, platform and application
 *             as well as scheduling interface.
 */

/******************************************************************************/
/* INCLUDES                                                                   */
/******************************************************************************/
#include "bsh_stdinc.h"
#ifdef __IAR_SYSTEMS_ICC__
#if !defined(__PCGCC__) && !defined(__NOT_CC__)
#include <intrinsics.h>
#endif
#elif  defined(__MWERKS__) || defined(__HIWARE__)
#include <hidef.h>
#endif
#if defined(__ARMCC_VERSION) && (__ARMCC_VERSION >= 6010050)
//#include <arm_compat.h>
#include "cmsis_version.h"
#endif

/**
   This interface performs the low level platform initialization :
   Clock, Oscillator, PLL, reset etc.

\param
   none

\return
   none

\remarks
   - default implementation is provided for each platform. 
   - user can optionally implement this function and not use the default
   - this function will be called by the boot routine where appropriate

*/


extern void BASIC_vInitPlatform (void);




/**
   This interface configures the platform as needed :
   IO ports, A/D converter, UART etc.

\param
   none

\return
   none

\remarks
   - default implementation is provided for each platform. 
   - user can optionally implement this function and not use the default
   - this function will be called at runtime by the generic main routine

*/


extern void BASIC_vConfigPlatform (void);




/**
   This interface enables all interrupts

\param
   none

\return
   none

\remarks
   - header basic.h contains macro implementation for Metrowerks and IAR
     for other compilers this function must be implemented in basicMcXs.c
*/
#ifdef __NOT_CC__
  extern void BASIC_vEnableAllInterrupts(void);
#elif defined(__IAR_SYSTEMS_ICC__)
  static inline void BASIC_vEnableAllInterrupts(void) {__enable_interrupt();}
#elif  defined(__arm__)
  #ifdef __GNUC__
    #include "processor.h"
  #endif
  static inline void BASIC_vEnableAllInterrupts(void) {__enable_irq();}
#else
  void BASIC_vEnableAllInterrupts(void);
#endif



/**
   This interface serves as wrapper for user application initialization.
   (e.g. global interrupt enabling, scheduler registration or other
    user specific applications)

\param
   none

\return
   none

\remarks
   - example implementation is provided for basic software. 
   - user must implement this function suibably for his needs
   - this function will be called at runtime by the generic main routine


*/
extern void BASIC_vUsrInitAppl (void);




/**
   This interface serves as wrapper for user tasks management.
   (e.g. using scheduler or a main loop or what ever)

\param 
   none

\return 
   none

\remarks
   - example implementation is provided for basic software. 
   - user must implement this function suitably for his needs
   - this function will be called at runtime by the generic main routine

*/
extern void BASIC_vUsrSchedTasks (void);

#if defined(NO_RAM_INIT) || defined(BTM_FIRMWARE_UPDATE)
/** Needed instead of main() to prevent usage of startup,
 *  when ram initialisation not needed.
 */
extern void Main(void);
#elif !defined(__GNUC__) || defined(__ARMCC_VERSION)
/** Compiler startup for armcclang and keilarm
 *  when ram initialisation not needed.
 */
extern void __main(void); /*lint !e683 reserved word MisraC2012 21.2 */
#endif



/**
   This function configures ECC and parity check for RAM1.

\remarks
   - Enable 1-bit ECC error update. 
   - Enable NMI interrupt on parity and ECC error
   - This function is called in ITBL_vResetHandler()

*/
#if defined(RA4)
   void BASIC_vEccInit(void);
#endif

#ifdef __cplusplus
}
#endif

#endif
