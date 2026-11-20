/*******************************************************************************
 *  Copyright (c) 2017 BSH Hausgeraete GmbH,
 *  Carl-Wery-Str. 34, 81739 Munich, Germany, www.bsh-group.de
 *
 *  All rights reserved. This program and the accompanying materials
 *  are protected by international copyright laws.
 *  Please contact copyright holder for licensing information.
 *
 *******************************************************************************
 *  PROJECT          SW_LIB
 *  COMP_ABBREV      VEEPROM
 ******************************************************************************/
#ifndef VEEPROMXS_H
#define VEEPROMXS_H

#ifdef __cplusplus
extern "C" {
#endif

 /*****************************************************************************/
/* DOCUMENTATION                                                              */
/******************************************************************************/
/** \file VEEPROMXS.h
  published application specific definitions and declarations for Volatile EEPROM
*/
/******************************************************************************/
/* INCLUDES                                                                   */
/******************************************************************************/
#ifndef VECTORCAST
#include "VEEPROMmcXS.h"
#endif //VECTORCAST

/******************************************************************************/
/* PUBLIC FUNCTION DEFINITIONS                                                */
/******************************************************************************/
#if defined(VEEPROM_LARGE)
/**
 * \brief get size in bytes of drawer with given ID
 * 
 * \return size of payload in bytes ignoring cellsize
 */
extern uint8_t VEEPROM_ucGetDrawerSize( tDrawerID tDrawer );
/**
 * \brief get number of drawers 1
 */
extern tDrawerID VEEPROM_tGetNumberOfDrawers(void);

/**
 * \brief get cache address of drawer with given ID
 */
extern uint8_t* VEEPROM_pucGetDrawerAddress( tDrawerID tDrawer );

#else //if not defined(VEEPROM_LARGE)
/**
 * \brief get size in bytes of drawer with given ID
 * 
 * \return size of payload in bytes ignoring cellsize
 */
extern uint8_t VEEPROM_ucGetDrawerSize( uint8_t ucDrawerId );
/**
 * \brief get number of drawers 1
 */
extern uint8_t VEEPROM_ucGetNumberOfDrawers ( void );

/**
 * \brief get cache address of drawer with given ID
 */
extern uint8_t* VEEPROM_pucGetDrawerAddress( uint8_t ucDrawerId );
#endif //defined(VEEPROM_LARGE)

/**
 * \brief Give permission for process of defragmentation
 * 
 * \note While Process of defragmentation is taking place, all interrupts are locked.
 *
 * This function is called as callback from Application to get PERMISSION to 
 * start Process of defragmentation. If VEEPROM_bApprobate returns false, process
 * of defragmentation is not performed. This leads to inconsistency of data in
 * cached values to data in FLASH until the next Write-operation was performed
 * properly (with allowed defragmentation).
 *
 * The process of defragmentation is started when trying to write data to full flash-page. If
 * Flash-page in use has too less cells free, it has to defrag the page first by
 * swapping the latest data to the other free page. This produces new free space on new page.
 * BUT IT ALSO TAKES some 100æs.
 */
extern bool VEEPROM_bApprobateDefrag( void );
/**
 * \brief function is called when defragmentation was performed.
 *
 * \note Application can be informed with this function that defragmentation has been finished.
 */
extern void  VEEPROM_vDefragEnded( void );
/** \brief function to provide watchdog timer reset
*/
extern void _VEEPROM_vResetWatchdogTimer( void );
/** \brief callback function called from VEEPROM_bDoDefragNow()
 *  \note Application can adapt this function for its needs.\n
 *  The function is called each time when current data were copied into new page but erase process was not performed yet.
 */
extern void VEEPROM_vBeforePageErase(void);
#if defined(VEEPROM_LARGE)
/**
 * \brief function is called when function VEEPROM_bReadWholeActivePage() sets valid value from flash page into cache variable.
 *
 * \note Application can be informed with this function that a cache variable was set.
 */
void VEEPROM_vAfterCacheVariableSet( tDrawerID tDrawer);
#else
/**
 * \brief function is called when function VEEPROM_bReadWholeActivePage() sets valid value from flash page into cache variable.
 *
 * \note Application can be informed with this function that a cache variable was set.
 */
void VEEPROM_vAfterCacheVariableSet( uint8_t ucDrawerIdent);
#endif //defined(VEEPROM_LARGE)

#ifdef __cplusplus
}
#endif

#endif //VEEPROMXS_H
