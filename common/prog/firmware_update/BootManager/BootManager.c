/*******************************************************************************
 *  Copyright (c) 2017 BSH Hausgeraete GmbH,
 *  Carl-Wery-Str. 34, 81739 Munich, Germany, www.bsh-group.de
 *
 *  All rights reserved. This program and the accompanying materials
 *  are protected by international copyright laws.
 *  Please contact copyright holder for licensing information.
 *
 *******************************************************************************
 *  PROJECT          Firmware Update Boot-Manager
 ******************************************************************************/


/******************************************************************************/
/* DOCUMENTATION                                                              */
/******************************************************************************/
/** \file
 *
 *  \ingroup  BootManager
 *
 *  \brief    Implementation of Boot-Manager-Main 
 *
 */


/******************************************************************************/
/* INCLUDES                                                                   */
/******************************************************************************/
#include "bsh_stdinc.h"
#include "firmware_update/fwu_config.h"
#include "BootManagerSharedData.h"
#include "BootManagerModule.h"
#if defined(MCAL_MSUP_INCLUDED)
#include "mcal/msup.h" //Needed for reset execution
#else
#include "hsup.h" 
#endif //MCAL_MSUP_INCLUDED
#include "ped_fw/basic/basic.h"
#include "firmware_update/mal/fwu_opt_bytes.h"

/******************************************************************************/
/* PRIVATE FUNCTION DECLARATIONS                                              */
/******************************************************************************/
static void BMAPP_switchBootModule(void);
static bool BMAPP_isModuleValidToStart(enum MAL_ModuleEnum bootModule);


/******************************************************************************/
/* PUBLIC FUNCTION DEFINITIONS                                                */
/******************************************************************************/

/*lint -esym(9046,?ain) Note 9046: 'main' is typographically ambiguous with
    respect to 'Main' when ignoring case [MISRA 2012 Directive 4.5, advisory] */
#if defined(NO_RAM_INIT) && !defined(IARRX)
void Main(void)/*lint !e970 that's ansi C*/
#else
int main(void)/*lint !e970 that's ansi C*/
#endif
{
    BASIC_vInitPlatform(); // initialise system clock
    BMDAT_initSharedData();

    if (BMAPP_isModuleValidToStart( BMDAT_getBootModule()) )
    {
       BMMOD_startModule();   // never come back
    }
    /* something wrong selected module could not be started */
    BMDAT_incrementStartAttempts();
    
    if (BMDAT_getStartAttempts() >= FWU_MAX_RETRIES_TO_SWITCH_MODULE)
    {   // try another boot module after some attempts
        BMAPP_switchBootModule();
    }
    // Exit!!
#if defined(MCAL_MSUP_INCLUDED)
    MSUP_generateReset(); // try again after reset
#else
    HSUP_vGenerateReset(); // try again after reset
#endif 
#if !defined(NO_RAM_INIT)
    return 1; // line cannot be reached, to prevent SCA and compiler warning
#endif
}

/******************************************************************************/
/* PRIVATE FUNCTION DEFINITIONS                                               */
/******************************************************************************/

/** \brief    Switches to the next possible boot module.
 *
 *  \details  If any error occurs during module start this function is called
 *            to switch to the next possible boot module
 */
static void BMAPP_switchBootModule(void)
{
    if (BMDAT_getBootModule() != MAL_PROGRAMMER_ID)
    {   // try first the programmer
        BMDAT_setBootModule(MAL_PROGRAMMER_ID);
    }
    else
    {   // when programmer not works try application
        BMDAT_setBootModule(MAL_PRODUCT_APP1);
    }
    /* note when Programmer and application does not work the
       the BP2 loader may be started.
       This is not done because of security
     */
}


/** \brief    Checks if the selected boot module is valid to start.
 *
 *  \param    bootModule: sw module to be started
 *  \return   true if the given module is valid and start-able
 */
static bool BMAPP_isModuleValidToStart(enum MAL_ModuleEnum bootModule)
{
    if (!BMMOD_bInitMemoryModule(bootModule))
    {
        BMDAT_setErrorFlag(BMDAT_ER_BOOT_MODULE);
        return false;
    }
    if (!BMMOD_isModuleStartable())
    {
        BMDAT_setErrorFlag(BMDAT_ER_UNSTARTABLE);
        return false;
    }
    if (!BMMOD_isModuleIntegrityOk()) // Watch dog is triggered in this function 
    {
        BMDAT_setErrorFlag(BMDAT_ER_INTEGRITY);
        if(!FWU_isIntegCheckDisabledCalc() && !FWU_isIntegCheckErrorIgnored())
        {
            return false;
        }
    }
    if (!BMMOD_isModuleFirmwareIdValid())
    {
        BMDAT_setErrorFlag(BMDAT_ER_FW_ID);
        if(!FWU_isCompatibilityCheckErrorIgnored())
        {
            return false;
        }
    }
    return true; // all required checks passed
   
}

