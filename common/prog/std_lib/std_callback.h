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


#ifndef STD_CALLBACK_H
#define STD_CALLBACK_H

#if defined(__cplusplus) && !defined(MOCK_IT)
extern "C" {
#endif


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
#include <stdint.h>


/******************************************************************************/
/* PUBLIC TYPE DEFINITIONS                                                    */
/******************************************************************************/

/** \brief    Type definition for standard callback functions
 *
 *  \param    objPtr:       pointer to user defined object/instance
 *  \param    eventFlags:   event flags specifying which event is pending
 *  \param    eventData:    event data to be processed by callback function
 *
 *  \return   standard return value (0-OK / <0-ERROR)
 */
typedef int32_t (*STDCB_CallbackFunction_t) (void* objPtr, uint32_t eventFlags, int32_t eventData);

/*lint -esym(9045,*STDCB_Callback)  "non-hidden definition of type struct STDCB_Callback" */
/** \brief    Standard callback structure for one callback instance
 *
 *  \details  Callback instances are arranged in linked lists, which supports 
 *            adding and removing callbacks during runtime.
 *            All callbacks has to be allocated in static memory!
 *            Callback instance can not be const!
 */
struct STDCB_Callback
{
    STDCB_CallbackFunction_t    functionPtr;    ///< Pointer to user callback function
    void                        *objectPtr;     ///< Pointer to user object
    uint32_t                    eventMask;      ///< User defined mask for masking event flags
    struct STDCB_Callback       *_nextCallback; ///< Pointer to next callback structure (set automatically)
};


/******************************************************************************/
/* PUBLIC FUNCTION DECLARATIONS                                               */
/******************************************************************************/

/** \brief    Function for initializing callbacks structure.
 *
 *  \details  This function initializes callback structure based on input
 *            parameters. This function is not needed, if the structure
 *            is statically initialized.
 *
 *  \param    cbPtr:    pointer to statically defined callback structure
 *  \param    funcPtr:  pointer to callback function
 *  \param    objPtr:   pointer to user object(instance), use NULL if not needed
 *  \param    eventMask: user defined event mask for enabling event callbacks
 *
 *  \return   standard return value (0-OK / <0-ERROR)
 */
extern int32_t STDCB_InitCallback(struct STDCB_Callback *cbPtr, STDCB_CallbackFunction_t funcPtr, void *objPtr, uint32_t eventMask);


/** \brief    Function for adding callback to linked list.
 *
 *  \details  This function inserts callback object to the end of linked list.
 *            If callback instance is already present in linked list, callback
 *            is ignored.
 *
 *  \param    cbList:   pointer to pointer to callback linked list
 *  \param    cbPtr:    pointer to callback instance
 *
 *  \return   standard return value (0-OK / <0-ERROR)
 */
extern int32_t STDCB_AttachCallback(struct STDCB_Callback **cbList, struct STDCB_Callback *cbPtr);


/** \brief    Function for removing callback from linked list.
 *
 *  \details  This function removes callback object from linked list and keeps
 *            its consistency. This function should be used only in special
 *            cases (e.g when dynamic allocation of callback object is used).
 *            In the vast majority of cases, only attaching of callback is used.
 *
 *  \param    cbList:   pointer to pointer to callback linked list
 *  \param    cbPtr:    pointer to callback instance
 *
 *  \return   standard return value (0-OK / <0-ERROR)
 */
extern int32_t STDCB_DetachCallback(struct STDCB_Callback **cbList, const struct STDCB_Callback *cbPtr);


/** \brief    Function for executing callbacks.
 *
 *  \details  This function executes all callback functions specified by linked 
 *            list that meets the condition, that the pending event masked by
 *            enabled events for callback is not zero.
 *
 *  \param    cbPtr:      pointer to first element in callback linked list
 *  \param    eventFlags: event flags specifying which event is pending
 *  \param    eventData:  event data passed to all callback functions
 *
 *  \return   return value is logical OR of all callback return values
 */
extern int32_t STDCB_ExecuteCallbacks(const struct STDCB_Callback *cbPtr, uint32_t eventFlags, int32_t eventData);


static inline void STDCB_EnableEvents(struct STDCB_Callback *cbPtr, uint32_t eventMaskToEnable)
{
    cbPtr->eventMask |= eventMaskToEnable;
}


static inline void STDCB_DisableEvents(struct STDCB_Callback *cbPtr, uint32_t eventMaskToDisable)
{
    cbPtr->eventMask &= ~eventMaskToDisable;
}


#if defined(__cplusplus) && !defined(MOCK_IT)
}
#endif


/******************************************************************************/
/* PUBLIC CLASS DECLARATIONS                                                  */
/******************************************************************************/
#ifdef __cplusplus

namespace STDCB
{
    class Callback
    {
        private:
            struct STDCB_Callback callback;

        public:
            Callback(STDCB_CallbackFunction_t funcPtr, void *objPtr = NULL, uint32_t eventMask = 0u)
            {
                (void)STDCB_InitCallback(&this->callback, funcPtr, objPtr, eventMask);
            }

            void enableEvents(uint32_t eventMaskToEnable)
            {
                STDCB_EnableEvents(&this->callback, eventMaskToEnable);
            }

            void disableEvents(uint32_t eventMaskToDisable)
            {
                STDCB_DisableEvents(&this->callback, eventMaskToDisable);
            }

        protected:
            struct STDCB_Callback *getCallback()
            {
                return &this->callback;
            }
    };
}

#endif // __cplusplus


#endif // STD_CALLBACK_H
