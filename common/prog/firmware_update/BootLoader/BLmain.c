/*******************************************************************************
 *  Copyright (c) 2017 BSH Hausgeraete GmbH,
 *  Carl-Wery-Str. 34, 81739 Munich, Germany, www.bsh-group.de
 *
 *  All rights reserved. This program and the accompanying materials
 *  are protected by international copyright laws.
 *  Please contact copyright holder for licensing information.
 *
 *******************************************************************************
 *  PROJECT          Generic SW / Boot-Loader
 ******************************************************************************/


/******************************************************************************/
/* DOCUMENTATION                                                              */
/******************************************************************************/
/** \file
 *
 *  \ingroup  firmware_update
 *
 *  \brief    Implementation Boot-Loader-Main.
 *
 *  \details  The Boot-Loader-Main handles the start-up behaviour for
 *            the Boot-Loader application.
 */

/*lint -esym(9046,?ain) Note 9046: 'main' is typographically ambiguous with
    respect to 'Main' when ignoring case [MISRA 2012 Directive 4.5, advisory] */

/******************************************************************************/
/* INCLUDES                                                                   */
/******************************************************************************/
#include "bsh_stdinc.h"
#include <stdint.h>
#include "firmware_update/BootManager/BootManagerSharedData.h"
#include "firmware_update/BootLoader/BLmessageLoop.h"
#include "basic/basic.h"

#ifdef VARIANT_ROMLOADER
#include "processor.h"
#include "IntTbl.h"


/******************************************************************************/
/* PRIVATE FUNCTION DECLARATIONS                                              */
/******************************************************************************/
static void BLMAIN_startApplication(void);


/******************************************************************************/
/* PRIVATE DATA DEFINITIONS                                                   */
/******************************************************************************/

/******************************************************************************/
/* PUBLIC FUNCTION DEFINITIONS                                                */
/******************************************************************************/
#if defined(NO_RAM_INIT) && !defined(IARRX)
void Main(void)
#else
int main(void)
#endif //defined(NO_RAM_INIT)
{
    const uint16_t BLMAIN_longBootTimeOut  = 1000u;
    const uint16_t BLMAIN_shortBootTimeOut = 60u;
    uint16_t bootTmo = BLMAIN_shortBootTimeOut;
    
    if (!BMDAT_isPowerOnReset())
    {
        if (BMDAT_getMagicPattern() == BMDAT_startAppAfterReset)
        {
            BLMAIN_startApplication(); // never come back
        }
        if (BMDAT_getMagicPattern() == BMDAT_longBootTimeOut)
        {
            bootTmo = BLMAIN_longBootTimeOut;
        }
    }
    BASIC_vInitPlatform(); // initialise system clock
    BMDAT_setMagicPattern(BMDAT_startAppAfterReset);
    BLMLP_handleMessageLoop(bootTmo); // never come back
#if !defined(NO_RAM_INIT) || defined(IARRX)
    return 0;
#endif
}    


#else // other build then ROMLOADER
#if (defined(NO_RAM_INIT) && !defined(IARRX)) || defined(BTM_FIRMWARE_UPDATE)
void Main(void)
#else
int main(void)
#endif //defined(NO_RAM_INIT)
{
    const uint16_t BLMAIN_TimeOut = 500u;
  #if defined (VARIANT_GECKO_BL) || (defined (VARIANT_LROMLOADER) && (defined (RA4) || defined (RA2))) 
    BASIC_vInitPlatform(); // initialise system clock
  #endif
    BLMLP_handleMessageLoop(BLMAIN_TimeOut); // never come back
#if (!defined(NO_RAM_INIT) || defined(IARRX)) && !defined(BTM_FIRMWARE_UPDATE)
    return 0;
#endif
}
#endif

/******************************************************************************/
/* PRIVATE FUNCTION DEFINITIONS                                               */
/******************************************************************************/

#ifdef VARIANT_ROMLOADER


/** \brief    Start the main application 
 *            that has to be terminated by an empty line.
 *
 *  \note this function will not return, it will leave current application
 */
static void BLMAIN_startApplication(void)
{
    BMDAT_setMagicPattern(BMDAT_startBlAfterReset);
    ITBL_StartApplication((size_t)BL_APPLICATION_ROM_START);//lint !e9048 lint problem using UINT32_C
}
#endif

