/*******************************************************************************
 *  Copyright (c) 2017 BSH Hausgeraete GmbH,
 *  Carl-Wery-Str. 34, 81739 Munich, Germany, www.bsh-group.de
 *
 *  All rights reserved. This program and the accompanying materials
 *  are protected by international copyright laws.
 *  Please contact copyright holder for licensing information.
 *
 *******************************************************************************
 * PROJECT          Generic SW
 * COMP_ABBREV      PHYP
 ******************************************************************************/


/******************************************************************************/
/* DOCUMENTATION                                                              */
/******************************************************************************/
/** \file
 *
 *  \ingroup  Physpan
 *
 *  \brief    Implementation for configuration of digital panel.
 *
 */


/******************************************************************************/
/* INCLUDES                                                                   */
/******************************************************************************/
#include "bsh_stdinc.h"
#include <stdint.h>
#include <stdbool.h>
#include "physpan.h"
#include "phyppriv.h"


/******************************************************************************/
/* PRIVATE DEFINITIONS                                                        */
/******************************************************************************/


/******************************************************************************/
/* PRIVATE TYPE DEFINITIONS                                                   */
/******************************************************************************/



/******************************************************************************/
/* PRIVATE FUNCTION DECLARATIONS                                              */
/******************************************************************************/
static Tpanel PHYP_getKeyStatus(void);

/******************************************************************************/
/* PUBLIC DATA DEFINITIONS                                                   */
/******************************************************************************/
/** Number of Samples */
const uint8_t  PHYP_ucDigiSampleCount = 6u;

/** Time between two samples */
const uint16_t PHYP_uiDigiSampleTime  = 2000u;


/******************************************************************************/
/* PRIVATE DATA DEFINITIONS                                                   */
/******************************************************************************/

/** \brief Saves the keys to be cleared
 *
 *   After a complete sample period (number of samples done),
 *   each bit of key to be cleared is reset.\n
 *   Starting a sample period, it is initialised with all bits cleared.
 *
 */
static Tpanel PHYP_tKeyToClear;



/** \brief Saves the keys to be set
 *
 *  After a complete sample period (number of samples done),
 *  each bit of key to be set is set.\n
 *  Starting a sample period, it is initialised with all bits set.
 */
static Tpanel PHYP_tKeyToSet;


/******************************************************************************/
/* PUBLIC FUNCTION DEFINITIONS                                                */
/******************************************************************************/



//------ Application specific implementation of function called by library ----------------
/* function is called from library task handler after debouncing time */

void _PHYP_vGetDigiKeyState(void)
{
    Tpanel Keys = PHYP_getKeyStatus(); // reads current (undebounced) key-state form multiplexer

    PHYP_tKeyToClear |= Keys;     // if key is set, set the bit
    PHYP_tKeyToSet   &= Keys;     // if key is released, clear the bit
}


void _PHYP_vInitDigiSampleSequence(void)
{
    PHYP_tKeyState |= PHYP_tKeyToSet;    // set keys, which were set all samples
    PHYP_tKeyState &= PHYP_tKeyToClear;  // clear keys, which were reset all samples

    //prepare next sample period
    PHYP_tKeyToClear = 0u;
    PHYP_tKeyToSet   =(Tpanel)(~(Tpanel)0u);
}


bool _PHYP_bDigiSetNextChannel(void)
{
    return true; //multiplexing not needed in this application
}


bool _PHYP_bIsDigiConfigurationReady(void) 
{
    return true;//nothing to do for configuration
}


/******************************************************************************/
/* PRIVATE FUNCTION DEFINITIONS                                               */
/******************************************************************************/
static Tpanel PHYP_getKeyStatus(void)
{
    /* Need do be implemented by application
     * The undebounced key-state shall be bit-coded in the return value
     */
    return 0u;
}
