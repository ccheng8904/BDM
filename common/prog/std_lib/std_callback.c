/*******************************************************************************
 *  Copyright (c) 2017 BSH Hausgeraete GmbH,
 *  Carl-Wery-Str. 34, 81739 Munich, Germany, www.bsh-group.de
 *
 *  All rights reserved. This program and the accompanying materials
 *  are protected by international copyright laws.
 *  Please contact copyright holder for licensing information.
 *
 *******************************************************************************
 *  PROJECT          Generic SW
 *  COMP_ABBREV      STDCB
 ******************************************************************************/


/******************************************************************************/
/* DOCUMENTATION                                                              */
/******************************************************************************/
/** \file
 *
 *  \ingroup  standardLibrary
 *
 *  \brief    Public definitions and declarations for unit STDCB.
 *
 *  \details  Standard callback library for adding, removing and executing of
 *            callback functions. All callbacks are defined by STDCB_Callback
 *            structure and arranged in linked list. When executing callbacks,
 *            only those callback functions are executed, that have enabled
 *            specific event by event mask in callback structure. This allows 
 *            attaching multiple callbacks to one callbacks list, where each 
 *            callbacks responds to different event (e.g. received / transmitted
 *            data or error event).
 */


/******************************************************************************/
/* INCLUDES                                                                   */
/******************************************************************************/
#include "bsh_stdinc.h"
#include "std_callback.h"
//#include <assert.h>       // Assert is currently disabled, due to lint issues

/******************************************************************************/
/* PUBLIC FUNCTION DEFINITIONS                                                */
/******************************************************************************/


int32_t STDCB_InitCallback(struct STDCB_Callback *cbPtr, STDCB_CallbackFunction_t funcPtr, void *objPtr, uint32_t eventMask)
{
    //assert(cbPtr == NULL);
    
    cbPtr->functionPtr = funcPtr;
    cbPtr->objectPtr = objPtr;
    cbPtr->eventMask = eventMask;
    cbPtr->_nextCallback = NULL;
    
    return 0;
}


int32_t STDCB_AttachCallback(struct STDCB_Callback **cbList, struct STDCB_Callback *cbPtr)
{
    //assert(cbList == NULL);
    //assert(cbPtr == NULL);
    //assert(!IS_ROM_AREA(cbPtr));
    //assert(!IS_STACK_AREA(cbPtr));
    
    struct STDCB_Callback *cb = NULL;
    
    if(cbPtr != NULL)
    {
        // Chceck if cbList is NULL - when asserts are disabled
        if(cbList == NULL)
        {
            return -1;
        }
        // Check first member of array
        else if(*cbList == NULL)
        {
            *cbList = cbPtr;
            cbPtr->_nextCallback = NULL;
        }
        else
        {
            // Get firs callback pointer from list
            cb = *cbList;
            while(cb->_nextCallback != NULL)
            {
                // Check for same callback pointer
                if(cb == cbPtr)
                {
                    return 0;
                }
                // Set next callback pointer
                cb = cb->_nextCallback;
            }
            
            cb->_nextCallback = cbPtr;
            cbPtr->_nextCallback = NULL;
        }
    }
    
    return 0;
}


int32_t STDCB_DetachCallback(struct STDCB_Callback **cbList, const struct STDCB_Callback *cbPtr)
{
    //assert(cbList);
    //assert(cbPtr);
    
    struct STDCB_Callback *cb = NULL;
    struct STDCB_Callback *cbToRm = NULL;
    
    // Chceck if cbList is NULL - when asserts are disabled
    if(cbList == NULL)
    {
        return -1;
    }
    // Check first member of array
    else if(*cbList == cbPtr)
    {
        *cbList = NULL;
    }
    else
    {
        // Get firs callback pointer from list
        cb = *cbList;
        while(cb->_nextCallback != NULL)
        {
            // Check if it is callback to detach
            if(cb->_nextCallback == cbPtr)
            {
                cbToRm = cb->_nextCallback;
                cb->_nextCallback = cbToRm->_nextCallback;
                cbToRm->_nextCallback = NULL;
                break;
            }
        }
    }
    
    return 0;
}


int32_t STDCB_ExecuteCallbacks(const struct STDCB_Callback *cbPtr, uint32_t eventFlags, int32_t eventData)
{
    //assert(cbPtr);
    
    int32_t retVal = 0;
    uint32_t ef;
    
    if(eventFlags != 0U)
    {
        while(cbPtr != NULL)
        {
            ef = eventFlags & cbPtr->eventMask;
            if((ef != 0U) && (cbPtr->functionPtr != NULL))
            {
                /*lint -e{9027} Tested to work correctly on Cortex-M devices */
                retVal |= (int32_t)cbPtr->functionPtr(cbPtr->objectPtr, ef, eventData);
            }
            cbPtr = cbPtr->_nextCallback;
        }
    }
    
    return retVal;
}
