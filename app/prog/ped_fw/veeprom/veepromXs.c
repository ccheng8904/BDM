/*******************************************************************************
 *  Copyright (c) 2017 BSH Hausgeraete GmbH,
 *  Carl-Wery-Str. 34, 81739 Munich, Germany, www.bsh-group.de
 *
 *  All rights reserved. This program and the accompanying materials
 *  are protected by international copyright laws.
 *  Please contact copyright holder for licensing information.
 *
 *******************************************************************************
 *  PROJECT          VEEPROM
 ******************************************************************************/

/******************************************************************************/
/* DOCUMENTATION                                                              */
/******************************************************************************/

/** \file  VEEPROMXS.c
 * 
 *  \brief Application specific implementation / configuration for Volatile EEPROM library
 */

/******************************************************************************/
/* INCLUDES                                                                   */
/******************************************************************************/
#include "bsh_stdinc.h"
#include "LibTypes.h"
#include "VEEPROM.h"
#include "VEEPROMpriv.h"
#include "nvmem/cache.h"
#ifdef TRIGGER_WDO
#include "hwdt.h"
#endif

/******************************************************************************/
/* PRIVATE DEFINITIONS                                                        */
/******************************************************************************/

/******************************************************************************/
/* PRIVATE TYPE DEFINITIONS                                                   */
/******************************************************************************/

/******************************************************************************/
/* PRIVATE FUNCTION DECLARATIONS AND PRIVATE MACRO FUNCTION DEFINITIONS       */
/******************************************************************************/

/******************************************************************************/
/* PRIVATE DATA DEFINITIONS                                                   */
/******************************************************************************/

/******************************************************************************/
/* PUBLIC FUNCTION DEFINITIONS                                                */
/******************************************************************************/
/* observer functions */
#if defined(VEEPROM_LARGE)
/** \brief Gets size of drawer with given Id
 *
 * \note if VEEPROM is used without upper libraries NVMEM and no caching is used for application,
 *  then:
 *  - define a table containing DrawerSizes, e.g. VEEPROM_aucDrawerSizes[]
 *  - adapt VEEPROM_ucGetDrawerSize(...) that it returns corresponding values from table VEEPROM_aucDrawerSizes
 * \param ucDrawerId   - drawer number
 * \return
 * drawer size
 */
uint8_t VEEPROM_ucGetDrawerSize( tDrawerID tDrawer )
{
    return CACHE_ucGetDrawerSizeVEEPROM(tDrawer);
}
/** \brief Gets number of drawers
 *
 * \note  if VEEPROM is used without upper libraries NVMEM and no caching is used for application,
 * then:
 * - adapt VEEPROM_ucGetNumberOfDrawers() that it returns number of elements of array VEEPROM_aucDrawerSizes
 * \return
 * number of drawers
 */
tDrawerID VEEPROM_tGetNumberOfDrawers(void)
{
    return CACHE_tGetNumberOfDrawersVEEPROM();
}

/** \brief Gets address of drawer cache variable with given Id
 *
 * \note  if VEEPROM is used without upper libraries NVMEM and no caching is used for application,
 * then:
 * - adapt VEEPROM_pucGetDrawerAddress() that it returns corresponding address of cache variable for that drawer
 *
 * \param ucDrawerId   - drawer number
 * \return
 *  pointer to cache variable
 *
 */
uint8_t* VEEPROM_pucGetDrawerAddress( tDrawerID tDrawer )
{
    return( CACHE_pucGetCacheAddressVEEPROM( tDrawer ) );
}

#else //if not defined(VEEPROM_LARGE)
/** \brief Gets size of drawer with given Id
 *
 * \note if VEEPROM is used without upper libraries NVMEM and no caching is used for application,
 *  then:
 *  - define a table containing DrawerSizes, e.g. VEEPROM_aucDrawerSizes[]
 *  - adapt VEEPROM_ucGetDrawerSize(...) that it returns corresponding values from table VEEPROM_aucDrawerSizes
 * \param ucDrawerId   - drawer number
 * \return
 * drawer size
 */
uint8_t VEEPROM_ucGetDrawerSize( uint8_t ucDrawerId )
{
    return CACHE_ucGetDrawerSizeVEEPROM( ucDrawerId );
}
/** \brief Gets number of drawers
 *
 * \note  if VEEPROM is used without upper libraries NVMEM and no caching is used for application,
 * then:
 * - adapt VEEPROM_ucGetNumberOfDrawers() that it returns number of elements of array VEEPROM_aucDrawerSizes
 * \return
 * number of drawers
 */
uint8_t VEEPROM_ucGetNumberOfDrawers(void)
{
    return CACHE_ucGetNumberOfDrawersVEEPROM();
}

/** \brief Gets address of drawer cache variable with given Id
 *
 * \note  if VEEPROM is used without upper libraries NVMEM and no caching is used for application,
 * then:
 * - adapt VEEPROM_pucGetDrawerAddress() that it returns corresponding address of cache variable for that drawer
 *
 * \param ucDrawerId   - drawer number
 * \return
 *  pointer to cache variable
 *
 */
uint8_t* VEEPROM_pucGetDrawerAddress( uint8_t ucDrawerId )
{
    return( CACHE_pucGetCacheAddressVEEPROM( ucDrawerId ) );
}
#endif //defined(VEEPROM_LARGE)

/* Application MUST implement this function. 
 * It will be called from function VEEPROM_bWrite if process of defragmentation must be started
 * to get new free page for writing data. This process is only performed, if
 * the function VEEPROM_bApprobateDefrag() returns true. Otherwise it is not 
 * allowed for NVMEM-library to start process of defragmentation.
 * In this case only the cached value would be updated. 
 * If a later Write-function-call gets the permission to defrag data,
 * it will "save" the previously not permanently stored value from the cache one.
 */
/* VEEPROM_bApprobateDefrag()*/
bool VEEPROM_bApprobateDefrag(void)
{
    return (true);
}

/* VEEPROM_vBeforePageErase()*/
/** \brief callback function called from VEEPROM_bDoDefragNow()
 *  \note Application can adapt this function for its needs.\n
 *  The function is called each time when current data were copied into new page but erase process was not performed yet.
 */
void VEEPROM_vBeforePageErase(void)
{
    return;
}

/* VEEPROM_vDefragEnded()*/
/** \brief function is called when complete defragmentation was performed
 *  \note Application can adapt this function for its needs.\n
 *  The function is called each time when process of defragmentation has been finished.
 */
void  VEEPROM_vDefragEnded( void )
{
    return;
}

/* VEEPROM_vResetWatchdogTimer()*/
/** \brief function to provide watchdog timer reset
*/
void _VEEPROM_vResetWatchdogTimer( void )
{
#ifdef TRIGGER_WDO
    HWDT_vTrigger();
#endif
}

/* Callback function is called by VEEPROM library when function VEEPROM_bReadWholeActivePage() 
 * sets valid value from flash page into cache variable.
 * If NVMEM.lib is used, this function is already called and therefore should be here only as a note !
 * If only VEEPROM.lib without NVMEM.lib is used, this function should be defined !
 */
#if 0
#if defined(VEEPROM_LARGE)
/* VEEPROM_vAfterCacheVariableSet() */
void VEEPROM_vAfterCacheVariableSet( tDrawerID tDrawer )
{
    return;
}

#else
/* VEEPROM_vAfterCacheVariableSet() */
void VEEPROM_vAfterCacheVariableSet( uint8_t ucDrawerIdent)
{
    return;
}

#endif //defined(VEEPROM_LARGE)
#endif //0
