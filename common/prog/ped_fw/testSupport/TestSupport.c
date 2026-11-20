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
 *  COMP_ABBREV      TM
 ******************************************************************************/


/******************************************************************************/
/* DOCUMENTATION                                                              */
/******************************************************************************/
/** \file
 *
 *  \brief      Implementation of module TestSupport library.
 */



/******************************************************************************/
/* INCLUDES                                                                   */
/******************************************************************************/
#define TESTSUPPORT_MODULE // required for TestSupport.h
#include "TestSupport.h"
#include "bsh_stdinc.h" //lint -esym( 766,bsh_stdinc.h) "header not used.."
#include <stdint.h>



/******************************************************************************/
/* LOCAL DEFINITIONS                                                          */
/******************************************************************************/
                                                                              
                                                                              
/******************************************************************************/
/* LOCAL TYPE DEFINITIONS                                                     */
/******************************************************************************/
                                                                              
                                                                              
/******************************************************************************/
/* LOCAL DECLARATIONS                                                         */
/******************************************************************************/
static uint16_t TM_uiFindWatchIndex(uint16_t uiTestPoint);                        
                                                                              
/******************************************************************************/
/* LOCAL VARIABLES                                                            */
/******************************************************************************/
static uint16_t TM_uiStackIndex; // stack index                                 
volatile uint8_t TM_ucTpSelect;  // script selected type of test point          
volatile void* TM_pvDummyPtr;  // dummy pointer                               
/******************************************************************************/
/* LOCAL FUNCTIONS DEFINITION                                                 */
/******************************************************************************/
static uint16_t TM_uiFindWatchIndex(uint16_t uiTestPoint)
{
    uint16_t uiIndex;
    for (uiIndex = 0u; uiIndex < TM_uiTpWatchArraySize; uiIndex++)
    {
        if (TM_atWatchPointDef[uiIndex] == (uint16_t)TM_WATCH_POINT_END)
        {
            break; // no watch point found
        }
        if (TM_atWatchPointDef[uiIndex] == uiTestPoint)
        {
            return uiIndex;
        }
    }
    return (uint16_t)TM_NO_WATCH_POINT; // test point 'uiTestPoint' is not a watch point

}

/******************************************************************************/
/* GLOBAL FUNCTIONS DEFINITION                                                */
/******************************************************************************/
// Set the script defined test point in case of module test
void TM_vSetFlexPoint(uint16_t uiTestPoint)
{
    if (TM_ucTpSelect >= TM_ucCountOfTpHandlers)
    {
        TM_ucTpSelect = 0; // prevent using a out of bound pointer
    }

    TM_atTestPointHandler[TM_ucTpSelect](uiTestPoint); // call the test point handler

}

// Set a stack test point in case of module test
void TM_vSetStackPoint(uint16_t uiTestPoint)
{
    uint16_t uiWatchIndex;

    uiWatchIndex = TM_uiFindWatchIndex(uiTestPoint); // search if uiTestPoint is a watch point

    if (uiWatchIndex == (uint16_t)TM_NO_WATCH_POINT)          // not a watch point?
    {
        TM_atTestPointStack[TM_uiStackIndex] = uiTestPoint; // then put test point on stack
        TM_uiStackIndex++;
        if (TM_uiStackIndex >= TM_uiTpStackSize)
        {
            TM_uiStackIndex = 0;
        }
    }
    else
    {
        TM_atWatchPoints[uiWatchIndex]++; // increment watch point
    }
}


// Increment a watch point
void TM_vSetWatchPoint(uint16_t uiTestPoint)
{
    uint16_t uiWatchIndex;

    uiWatchIndex = TM_uiFindWatchIndex(uiTestPoint); // search if uiTestPoint is a watch point
    TM_atWatchPoints[uiWatchIndex]++; // increment watch point
}

void TM_vResetTestPoints(void)
{
    uint16_t uiIndex;
    for (uiIndex = 0; uiIndex < TM_uiTpArraySize; uiIndex++)
    {
        TM_atTestPoints[uiIndex] = 0;
    }
    for (uiIndex = 0; uiIndex < TM_uiTpStackSize; uiIndex++)
    {
        TM_atTestPointStack[uiIndex] = 0;
    }
    for (uiIndex = 0; uiIndex < TM_uiTpWatchArraySize; uiIndex++)
    {
        TM_atWatchPoints[uiIndex] = 0;
    }

    TM_uiStackIndex = 0;



}

void TM_vInit(void)
{
    uint16_t uiIndex;

    TM_ucTpSelect   = 0;

    TM_vResetTestPoints();
    for (uiIndex = 0; uiIndex < TM_uiTpWatchArraySize; uiIndex++)
    {
        TM_atWatchPointDef[uiIndex] = 0;
    }
    /*lint -e9025 More than two pointer indirection levels, [MISRA 2012 Rule 18.5, advisory] */
    TM_pvDummyPtr = (void*)TM_aptAnchor;// reference anchor to prevent optimisation

}
