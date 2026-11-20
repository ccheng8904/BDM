/*******************************************************************************
 *  Copyright (c) 2019 BSH Hausgeraete GmbH,
 *  Carl-Wery-Str. 34, 81739 Munich, Germany, www.bsh-group.de
 *
 *  All rights reserved. This program and the accompanying materials
 *  are protected by international copyright laws.
 *  Please contact copyright holder for licensing information.
 *
 *******************************************************************************
 *  PROJECT          SW_LIB
 *  COMP_ABBREV      FSF
 ******************************************************************************/

/******************************************************************************/
/* DOCUMENTATION                                                              */
/******************************************************************************/
/*lint -save -e9023 -e9024 Multiple use of '#/##' operators in definition of macro*/
/*lint -save -e9026 Function-like macro, '', defined*/
/**  \file fsramcon.h
   This file contains the exports of the Functional Safety Module RAM Consistency Check - FSF RAM CON.
    
    \brief This file contains the exported interfaces of the FSF RAM Consistency Module.


   The detailed description can be found in the section \ref FSF_RAM_CON_DESCR.
   \n
*/

/**
    \defgroup FSF_RAM_CON_DESCR FSF RAM CON - RAM consistency check for functional safety modules

    \section Introduction

      The RAM consistency check provides a service to test the consistency of a RAM data block that is 
      used by a functional safety module.

    \section Description

      \subsection Abbreviations


        <b>'consistent RAM function'</b>
        The background task and the interrupt service routine, that require the RAM consistency check are called as
        'consistent RAM function' in the further description.

        <b>'consistent memory block'</b>
        A 'consistent memory block' is a memory block, that is check for inconsistency.  A 'consistent memory block' belongs to a
        'subsystem'. A 'subsystem' contains 'consistent RAM functions'.

        <b>'check method'</b>
        A 'check method' is a method to detect a RAM inconsistency of a 'consistent memory block'. The implementation of FSF RAM CON
        contains different selectable methods.

        <b>'check value'</b>
        A 'check value' contains the information, against which the 'consistent memory block' is checked, to detect a RAM inconsistency.
        The layout of the 'check value' depends on the used 'check method'.

        <b>'subsystem'</b>
        A 'subsystem' is a set of modules containing background task and/or interrupt service routines. The subsystem is a enclosed entity.

      \subsection FSF_RAM_CON_PRINCIPLE Principle of Operation

        The FSF RAM CON provides different 'check methods' to check the consistency of a RAM block.

        There exist 2 methods:
        -# \ref FSF_RAM_CON_DESCR_CHECK_VALUE
        -# \ref FSF_RAM_CON_DESCR_MIRROR


        In general the FSF RAM CON operates as follows.
        -# The RAM consistency check is performed in 'consistent RAM functions' of a 'subsystem' that is declared as functional safety relevant.
        -# Before any read access in a 'consistent RAM function' to a variable in a 'consistent memory block' is performed, the memory block is checked.
        -# If a RAM inconsistency is discovered, the _FSF_vApplicationErrorNotify() of the channel \ref FSF_RAM_CONSISTENCY_CHECK is called directly, using the given channel identifier
           of a module that encountered an inconsistent RAM state.
        -# At the time point, when a 'consistent RAM function' does not modify any variable in the 'consistent memory block' any more. The 'check value' is updated.
        -# Exported interfaces of the 'subsystem', which modify variables in a 'consistent memory block', update the 'check value' at the modification time point directly.

      
      \subsection FSF_RAM_CON_DESCR_MEMORY Definition of consistent memory blocks
        
        Variables of a 'subsystem', that shall be checked against illegal write accesses, are gathered in a 'consistent memory block'.
        The following has to be applied:
        -# The definition of the variables of a 'subsystem' takes place in a structure.
        -# The structure definition is in this context a 'consistent memory block'.
        -# All elements of the structure have to be sorted by size, to minimize memory consumption.
        -# For the definition of the structure, the macro \ref FSF_RAM_CON_DEFINE_VAR has to be used.
        -# Each 'consistent memory block' has an unique name.
        
        The variables of a 'subsystem' shall be gathered in separate 'consistent memory blocks' using the following rules:
        -# variables, that are only used in a background task,
        -# variables, that are only used by an interrupt service routine.
        -# variables, that are used by an interrupt service routine and a background task.

        The number of 'consistent memory blocks' shall also be defined in a manner,
        -# that run time consumption is minimized,
        -# that memory consumption is minimized,
        -# that an inconsistent memory can be detected, at the beginning of a 'consistent RAM function', before any write or read access
           of a variable in the 'consistent memory block' is performed.

        It is important, that the 'consistent memory blocks' are not initialized at the definition.
        The initialization of elements in the 'consistent memory block' shall be performed in the background routine of the 'subsystem'.

     \subsection  FSF_RAM_CON_DESCR_CHECK_VALUE  'check method' - RAM consistency check based on a 8 bit checksum

        The FSF RAM CON uses an 8 bit sum of the 'consistent memory block' to be checked. The 8 bit sum is called 'check value' in the
        further description.

        Each 'consistent memory block' has it's own 'check value'.
        If the 'check value' does not match the current 8 bit sum of the 'consistent memory block', a RAM inconsistency is detected.

     \subsection FSF_RAM_CON_DESCR_MIRROR 'check method' - RAM consistency check based on a memory mirror

        Each 'consistent memory block' is mirrored in RAM. The content of the mirrored memory block is equal to the
        'consistent memory block', the 'subsystem operates' on.

        If the 'consistent memory block' and the mirrored memory block have a different contents, a RAM inconsistency is detected.

      \subsection FSF_RAM_CON_DESCR_USAGE Usage

        The 'check method' is defined during compile time using a preprocessor directive.
        The preprocessor directive determines the provided macros and functions of the subsystem FSF RAM CON through
        the header file fsframcon.h.
        The preprocessor directive must be set before the header file \ref fsframcon.h is included in a module, that 
        uses the RAM consistency service.

        The following preprocessor directives are available:
        -# \c #define \c FSF_RAM_CON_BY_CHECK_VALUE - \ref FSF_RAM_CON_DESCR_CHECK_VALUE
        -# \c #define \c FSF_RAM_CON_BY_MIRROR - \ref FSF_RAM_CON_DESCR_MIRROR
        -# \c #undef  \c FSF_RAM_CON_BY_CHECK_VALUE and \c #undef \c FSF_RAM_CON_BY_MIRROR - the RAM consistency is disabled


        The 'consistent RAM function' 
        -# has to check <b>each</b> memory block before any <b>read</b> access is 
           performed to a variable of the 'consistent memory block' using the function \ref FSF_RAM_CON_vCheck.
        -# has to update the 'check value' of <b>each</b> memory block after no <b>write</b> access is performed to a variable of the 'consistent memory block'. 
           The update is performed using the function \ref FSF_RAM_CON_vUpdateCheck .
        The above is valid for 'consistent memory block', which contain variables, that are read or written within the 'consistent RAM function'.

        A 'consistent RAM function' uses the following code structure to add the RAM consistency check.
        The example is made for a subsystem named 'Example', the 'consistent memory block' is named 'task'.

        \code

          #define FSF_RAM_CON_BY_MIRROR
          #include "fsframcon.h"

          typedef struct _TExample_task {
             uint8_t ucTest;
             struct{
               uint8_t bExample:1;
               ...
             }ucMyBitField; 
          }TExample_task;
          FSF_RAM_CON_DEFINE_VAR(Example, task);


         void Example_MyConsistenRamFunc(void) {
           FSF_RAM_CON_vCheck(Example, task, FSF_RAM_CON_MY_MY_FUNC_TASK);

           Example_task.ucTest++;
           Example_task.ucMyBitField.bExample = 0;

           FSF_RAM_CON_vUpdateCheck(Example, task);
         }

         void Example_SetActive(void) {
           FSF_RAM_CON_vSetBit(Example, task, ucMyBitField, bExample, 1);
         }

      \endcode

        It is recommended to have only one termination point within the 'consistent RAM function', since each exit requires the update
        of the 'check value'.

      \subsection FSF_RAM_CON_DESCR_DIRECT_UPDATE Direct update of the check value


        The following circumstances require a direct update of the 'check value' at the assignment of a value to a variable:
        -# all variables of a 'consistent memory block', that are modified in exported interfaces of the subsystem, 
           must update the check value in the interface function at the assignment of the value
        -# all variables of a 'consistent memory block', that are written by a background task handler and in addition by an interrupt service routine, must update the
           check value in the background task handler at the assignment of the value

        The assignment of a value to a variable and the direct update of the check value is performed by the following functions provided by FSF RAM CON:
        - \ref FSF_RAM_CON_vSetUchar
        - \ref FSF_RAM_CON_vSetBoolean
        - \ref FSF_RAM_CON_vSetUint
        - \ref FSF_RAM_CON_vSetBit
        - \ref FSF_RAM_CON_vSetBitUchar
        - \ref FSF_RAM_CON_vResBitUchar
        - \ref FSF_RAM_CON_vSetBitUint
        - \ref FSF_RAM_CON_vResBitUint

        \code
         void Example_SetActive(void) {
           FSF_RAM_CON_vSetBit(Example, task, ucMyBitField, bExample, 1);
         }
        \endcode

    \section Implementation

     The implementation of the FSF RAM CON is found in the modules
       -# \ref fsramcon.c
       -# \ref fsramcon.h

    \section Configuration

        none

    \section FSF_RAM_CON_DESCR_SERVICES Provided Services

      \subsection Service_FSF_RAM_CON_DEFINE_VAR FSF_RAM_CON_DEFINE_VAR(module, blockName) 

        This service performs the definition of a 'consistent memory block'.

        <p>
        parameter <i>module</i> The mnemonic of the 'subsystem' e.g. ADOR. \n
        parameter <i>blockName</i> The name of the 'consistent memory block'.\n
        </p>

      \subsection Service_FSF_RAM_CON_vCheck FSF_RAM_CON_vCheck(module, blockName, ucId)

        This service performs the consistency check of a 'consistent memory block' against the related 'check value'.
        If an inconsistency is detected the function \ref _FSF_vApplicationErrorNotify is called with the parameters
        -# the functional safety channel identifier \ref FSF_RAM_CONSISTENCY_CHECK  defined in \ref fSafe.h,
        -# the error code \ref FSF_ERR_CHANNEL_ERROR 
        -# and the user identifier denoting the 'consistent memory block'. 

        <p>
        parameter <i>module</i> The mnemonic of the 'subsystem' e.g. ADOR. \n
        parameter <i>blockName</i> The name of the 'consistent memory block'.\n
        parameter <i>ucId</i> The identifier of the 'consistent memory block' passed to _FSF_vApplicationErrorNotify.\n
        </p>


      \subsection Service_FSF_RAM_CON_vUpdateCheck FSF_RAM_CON_vUpdateCheck(module, blockName)

        This service updates the 'check value' of a 'consistent memory block'.

        <p>
        parameter <i>module</i> The mnemonic of the 'subsystem' e.g. ADOR. \n
        parameter <i>blockName</i> The name of the 'consistent memory block' to update the 'check value'.\n
        </p>

      \subsection Service_FSF_RAM_CON_vSetUchar FSF_RAM_CON_vSetUchar(module, blockName, elementName, ucNewVal)

        This function sets the value of a variable of type \ref uint8_t and updates the check value related to a 'consistent 
        memory block. 

        <p>
        parameter <i>module</i> The mnemonic of the 'subsystem' e.g. ADOR. \n
        parameter <i>blockName</i> The name of the 'consistent memory block' to update the 'check value'.\n
        parameter <i>elementName</i> The name of the variable (structure element).\n
        parameter <i>ucNewVal</i> The value to set.\n
        </p>

      \subsection Service_FSF_RAM_CON_vSetBoolean FSF_RAM_CON_vSetBoolean(module, blockName, elementName, bNewVal)

        This function sets the value of a variable of type \ref boolean and updates the check value related to a 'consistent 
        memory block. 

        <p>
        parameter <i>module</i> The mnemonic of the 'subsystem' e.g. ADOR. \n
        parameter <i>blockName</i> The name of the 'consistent memory block' to update the 'check value'.\n
        parameter <i>elementName</i> The name of the variable (structure element).\n
        parameter <i>bNewVal</i> The value to set.\n
        </p>

      \subsection Service_FSF_RAM_CON_vSetUint FSF_RAM_CON_vSetUint(module, blockName, elementName, uiNewVal)

        This function sets the value of a variable of type \ref uint16_t and updates the check value related to a 'consistent 
        memory block. 

        <p>
        parameter <i>module</i> The mnemonic of the 'subsystem' e.g. ADOR. \n
        parameter <i>blockName</i> The name of the 'consistent memory block' to update the 'check value'.\n
        parameter <i>elementName</i> The name of the variable (structure element).\n
        parameter <i>uiNewVal</i> The value to set.\n
        </p>

      \subsection Service_FSF_RAM_CON_vSetBit FSF_RAM_CON_vSetBit(module, blockName, bitFieldName, bitName, ucNewVal)

        This function sets a bit in a bitfield of size \ref uint8_t and updates the check value related to a 'consistent 
        memory block. 

        \warning The size of the bitfield must not be greater than 8 bit.

        <p>
        parameter <i>module</i> The mnemonic of the 'subsystem' e.g. ADOR. \n
        parameter <i>blockName</i> The name of the 'consistent memory block' to update the 'check value'.\n
        parameter <i>bitFieldName</i> The name of the bitfield (structure element).\n
        parameter <i>bitName</i> The name of the bit in the bitfield (structure element).\n
        parameter <i>ucNewVal</i> The value to set.\n
        </p>

      \subsection Service_FSF_RAM_CON_vSetBitUchar FSF_RAM_CON_vSetBitUchar(module, blockName, bitFieldVar, bitMask)

        This function sets bits in a variable of type \ref uint8_t using a bitmask and updates the check value related to a 'consistent 
        memory block. 

        <p>
        parameter <i>module</i> The mnemonic of the 'subsystem' e.g. ADOR. \n
        parameter <i>blockName</i> The name of the 'consistent memory block' to update the 'check value'.\n
        parameter <i>bitFieldVar</i> The name of the bitfield variable (structure element).\n
        parameter <i>bitMask</i> The mask denoting the bits to set of type \ref uint8_t.\n
        </p>


      \subsection Service_FSF_RAM_CON_vResBitUchar FSF_RAM_CON_vResBitUchar(module, blockName, bitFieldVar, bitMask)

        This function resets bits in a variable of type \ref uint8_t using a bitmask and updates the check value related to a 'consistent 
        memory block. 

        <p>
        parameter <i>module</i> The mnemonic of the 'subsystem' e.g. ADOR. \n
        parameter <i>blockName</i> The name of the 'consistent memory block' to update the 'check value'.\n
        parameter <i>bitFieldVar</i> The name of the bitfield variable (structure element).\n
        parameter <i>bitMask</i> The mask denoting the bits to reset of type \ref uint8_t.\n
        </p>
 
      \subsection Service_FSF_RAM_CON_vSetBitUint FSF_RAM_CON_vSetBitUint(module, blockName, bitFieldVar, bitMask)

        This function sets bits in a variable of size \type uint16_t using a bitmask and updates the check value related to a 'consistent 
        memory block. 

        <p>
        parameter <i>module</i> The mnemonic of the 'subsystem' e.g. ADOR. \n
        parameter <i>blockName</i> The name of the 'consistent memory block' to update the 'check value'.\n
        parameter <i>bitFieldVar</i> The name of the bitfield variable (structure element).\n
        parameter <i>bitMask</i> The mask denoting the bits to set of type \ref uint.\n
        </p>


      \subsection Service_FSF_RAM_CON_vResBitUint FSF_RAM_CON_vResBitUint(module, blockName, bitFieldVar, bitMask)

        This function resets bits in a variable of size \type uint16_t using a bitmask and updates the check value related to a 'consistent 
        memory block. 

        <p>
        parameter <i>module</i> The mnemonic of the 'subsystem' e.g. ADOR. \n
        parameter <i>blockName</i> The name of the 'consistent memory block' to update the 'check value'.\n
        parameter <i>bitFieldVar</i> The name of the bitfield variable (structure element).\n
        parameter <i>bitMask</i> The mask denoting the bits to reset of type \ref uint.\n
        </p>
*/
/*Axivion Enable MisraC2012-3.1 */

#ifndef __FSRAMCON_H

#define __FSRAMCON_H

#ifdef __cplusplus
extern "C" {
#endif

/******************************************************************************/
/* INCLUDES                                                                   */
/******************************************************************************/
#include "bsh_stdinc.h"
#include "hsup.h"
#include "LibTypes.h"
#include "fspriv.h" // needed for _FSF_vApplicationErrorNotify()
#include "fSafe.h"
#include "FuncSafe/fSafeXS.h"
#include "processor.h"

/*~+:Definitions and Declarations */

/*~+:FSF_RAM_CON_DEFINE_VAR(module, blockName)*/
/* Structures are aligned because of fast execution of RAM_CHECH_MIRROR */
/* Checking 4 bytes instead of 1 byte */
/* FSF_RAM_CON_getMirrorBlock - created because alignment of 
   mirror structure doesn't work correctly in armClang v6.18 (PEDF-4600) */
#ifdef FSF_RAM_CON_BY_CHECK_VALUE
    #define FSF_RAM_CON_DEFINE_VAR(module, blockName)                               \
        __ALIGNED(4) static T##module##_##blockName module##_##blockName;           \
        __ALIGNED(4) static uint8_t                 module##_ucCheckValue##blockName
#else
    #ifdef FSF_RAM_CON_BY_MIRROR
        #define FSF_RAM_CON_DEFINE_VAR(module, blockName)                           \
        __ALIGNED(4) static T##module##_##blockName module##_##blockName;           \
        __ALIGNED(4) static T##module##_##blockName module##_##blockName##Mirror;    \
        T##module##_##blockName* FSF_RAM_CON_get##module##_##blockName##Mirror(void)\
        { return &module##_##blockName##Mirror;}
    #else
        #define FSF_RAM_CON_DEFINE_VAR(module, blockName)                           \
        __ALIGNED(4) static T##module##_##blockName module##_##blockName /**< For a detailed description of the macro see \ref Service_FSF_RAM_CON_DEFINE_VAR . */
    #endif
#endif

/*~+:TYPE DEFINITIONS*/

/*~+:PUBLIC FUNCTIONS*/
#ifdef __QAC_RELEASE__
/* Switch off QAC-Warning 310:
"Msg(4:0310) Casting to different object pointer type."
*/
#pragma PRQA_MACRO_MESSAGES_OFF "FSF_RAM_CON_vCheck", 310
/* Switch off QAC-Warning 2004:
"Msg(4:2004) No concluding 'else' exists in this 'if'-'else'-'if' statement."
*/
#pragma PRQA_MACRO_MESSAGES_OFF "FSF_RAM_CON_vCheck", 2004
/* Switch off QAC-Warning 310:
"Msg(4:4105) Right hand operand of arithmetic or bitwise operator is a "boolean" expression."
*/
#pragma PRQA_MACRO_MESSAGES_OFF "FSF_RAM_CON_vCheck", 4105

/* Switch off QAC-Warning 310:
"Msg(4:0310) Casting to different object pointer type."
*/
#pragma PRQA_MACRO_MESSAGES_OFF "FSF_RAM_CON_vUpdateCheck", 310
/* Switch off QAC-Warning 2004:
"Msg(4:2004) No concluding 'else' exists in this 'if'-'else'-'if' statement."
*/
#pragma PRQA_MACRO_MESSAGES_OFF "FSF_RAM_CON_vUpdateCheck", 2004

/* Switch off QAC-Warning 310:
"Msg(4:0310) Casting to different object pointer type."
*/
#pragma PRQA_MACRO_MESSAGES_OFF "FSF_RAM_CON_vSetBit", 310

#endif

#ifdef VECTORCAST_TESTING
    extern void FSF_DUMMY_RAM_CON_vUpdateCheckByMirror(void);
    extern void FSF_DUMMY_RAM_CON_vCheckByMirror(void);
#endif //VECTORCAST_TESTING

/*~+:LINT*/
/* supress lint warning 506 and lint info 774 */

/*lint -emacro( 506, FSF_RAM_CON_vCheck ) */
/*lint -emacro( 506, FSF_RAM_CON_vUpdateCheck ) */

/*lint -emacro( 774, FSF_RAM_CON_vCheck ) */
/*lint -emacro( 774, FSF_RAM_CON_vUpdateCheck ) */

/*~+:FSF_RAM_CON_vCheck(module, blockName, ucId)*/
/**
 * \brief This function performs the RAM consistency check using the method \ref FSF_RAM_CON_DESCR_CHECK_VALUE.
 * If an inconsistency is detected the function \ref _FSF_vApplicationErrorNotify is called with the parameters
 * \ref FSF_RAM_CONSISTENCY_CHECK as the functional safety channel identifier defined in \ref fSafe.h,
 * the error code \ref FSF_ERR_CHANNEL_ERROR and the user identifier denoting the 'consistent memory block'. 
 *
 * @warning This function should not be called directly from other submodules than FSF RAM CON.
 *
 * @param ucCheckValue The current 'check value' - here the 8 bit sum of the 'consistent memory block'.
 * @param ucSizeInBytes The size of the 'consistent memory block' in bytes.
 * @param pucStartAddress The start address of 'consistent memory block' to be checked.
 * @param ucId The functional safety channel identifier.
 */
extern void _FSF_RAM_CON_vCheckBySum(uint8_t ucCheckValue, uint8_t ucSizeInBytes, const uint8_t* pucStartAddress, uint8_t ucId);

/**
 * \brief This function performs the RAM consistency check using the method \ref FSF_RAM_CON_DESCR_MIRROR.
 * If an inconsistency is detected the function \ref _FSF_vApplicationErrorNotify is called with the parameters
 * \ref FSF_RAM_CONSISTENCY_CHECK as the functional safety channel identifier defined in \ref fSafe.h,
 * the error code \ref FSF_ERR_CHANNEL_ERROR and the user identifier denoting the 'consistent memory block'.
 *
 * @warning This function should not be called directly from other submodules than FSF RAM CON.
 *
 * @param ucSizeInBytes The size of the 'consistent memory block' in bytes.
 * @param pucStartAddress The start address of 'consistent memory block' to be checked.
 * @param pucMIrrorStartAddress The start address of mirrored 'consistent memory block' to check against.
 * @param ucId The functional safety channel identifier.
 */
extern void _FSF_RAM_CON_vCheckByMirror(uint8_t ucSizeInBytes, const uint8_t* pucStartAddress, const uint8_t* pucMIrrorStartAddress, uint8_t ucId);
#ifdef FSF_RAM_CON_BY_CHECK_VALUE
    #define FSF_RAM_CON_vCheck(module, blockName, ucId)                                                 \
        if((int16_t)sizeof(module##_##blockName) >= 8)                                                  \
        {                                                                                               \
            HSUP_vDisableInt();                                                                         \
            _FSF_RAM_CON_vCheckBySum(module##_ucCheckValue##blockName,                                  \
                (uint8_t)sizeof(module##_##blockName),                                                  \
                (const uint8_t*)((void*)&module##_##blockName),                                         \
                (uint8_t)(ucId));                                                                       \
            HSUP_vEnableInt();                                                                          \
        }                                                                                               \
        else if((int16_t)sizeof(module##_##blockName) >= 1)                                             \
        {                                                                                               \
            uint8_t ucNewCheckValue;                                                                    \
            HSUP_vDisableInt();                                                                         \
            ucNewCheckValue = ((const uint8_t*)(void*)&(module##_##blockName))[0];                      \
            if((int16_t)sizeof(module##_##blockName) >= 2)                                              \
            {                                                                                           \
                ucNewCheckValue += ((const uint8_t*)(void*)&(module##_##blockName))[1];                 \
                if((int16_t)sizeof(module##_##blockName) >= 3)                                          \
                {                                                                                       \
                    ucNewCheckValue += ((const uint8_t*)(void*)&(module##_##blockName))[2];             \
                    if((int16_t)sizeof(module##_##blockName) >= 4)                                      \
                    {                                                                                   \
                        ucNewCheckValue += ((const uint8_t*)(void*)&(module##_##blockName))[3];         \
                        if((int16_t)sizeof(module##_##blockName) >= 5)                                  \
                        {                                                                               \
                            ucNewCheckValue += ((const uint8_t*)(void*)&(module##_##blockName))[4];     \
                            if((int16_t)sizeof(module##_##blockName) >= 6)                              \
                            {                                                                           \
                                ucNewCheckValue += ((const uint8_t*)(void*)&(module##_##blockName))[5]; \
                                if((int16_t)sizeof(module##_##blockName) >= 7)                          \
                                {                                                                       \
                                    ucNewCheckValue += ((const uint8_t*)(void*)                         \
                                        &(module##_##blockName))[6];                                    \
                                }                                                                       \
                            }                                                                           \
                        }                                                                               \
                    }                                                                                   \
                }                                                                                       \
            }                                                                                           \
            if((module##_ucCheckValue##blockName) != ucNewCheckValue)                                   \
            {                                                                                           \
                _FSF_vApplicationErrorNotify((uint8_t)FSF_ERR_CHANNEL_ERROR,                            \
                    (uint8_t)FSF_RAM_CONSISTENCY_CHECK, (ucId));                                        \
            }                                                                                           \
            HSUP_vEnableInt();                                                                          \
        }                                                                                               \
        else                                                                                            \
        {                                                                                               \
                                                                                                        \
        }
#else
    #ifdef FSF_RAM_CON_BY_MIRROR
    #ifdef VECTORCAST_TESTING
        #warning " VECTOR CAST RAM CON CHECK DISABLING is active !!! "
        #define FSF_RAM_CON_vCheck(module, blockName, ucId)     FSF_DUMMY_RAM_CON_vCheckByMirror();
    #else // regular usage
        #define FSF_RAM_CON_vCheck(module, blockName, ucId)                                             \
            if( (int16_t)sizeof(module##_##blockName) >= 8)                                             \
            {                                                                                           \
                _FSF_RAM_CON_vCheckByMirror((uint8_t)sizeof(module##_##blockName),                      \
                    (const uint8_t*)((void*)&(module##_##blockName)),                                   \
                    (const uint8_t*)((void*)&(module##_##blockName##Mirror)),                           \
                    (uint8_t)(ucId));                                                                   \
            }                                                                                           \
            else if( (int16_t)sizeof(module##_##blockName) >= 1)                                        \
            {                                                                                           \
                bool isFailed = false;                                                                  \
                HSUP_vDisableInt();                                                                     \
                isFailed = isFailed || ((((const uint8_t*)(void*)                                       \
                    &(module##_##blockName##Mirror))[0]^0xFFu) !=                                       \
                    ((const uint8_t*)(void*)&(module##_##blockName))[0]);                               \
                if( (int16_t)sizeof(module##_##blockName) >= 2)                                         \
                {                                                                                       \
                    isFailed = isFailed || ((((const uint8_t*)(void*)                                   \
                        &(module##_##blockName##Mirror))[1]^0xFFu) !=                                   \
                        ((const uint8_t*)(void*)&(module##_##blockName))[1]);                           \
                    if( (int16_t)sizeof(module##_##blockName) >= 3)                                     \
                    {                                                                                   \
                        isFailed = isFailed || ((((const uint8_t*)(void*)                               \
                            &(module##_##blockName##Mirror))[2]^0xFFu) !=                               \
                            ((const uint8_t*)(void*)&(module##_##blockName))[2]);                       \
                        if( (int16_t)sizeof(module##_##blockName) >= 4)                                 \
                        {                                                                               \
                            isFailed = isFailed || ((((const uint8_t*)(void*)                           \
                                &(module##_##blockName##Mirror))[3]^0xFFu) !=                           \
                                ((const uint8_t*)(void*)&(module##_##blockName))[3]);                   \
                            if( (int16_t)sizeof(module##_##blockName) >= 5)                             \
                            {                                                                           \
                                isFailed = isFailed || ((((const uint8_t*)(void*)                       \
                                    &(module##_##blockName##Mirror))[4]^0xFFu) !=                       \
                                    ((const uint8_t*)(void*)&(module##_##blockName))[4]);               \
                                if( (int16_t)sizeof(module##_##blockName) >= 6)                         \
                                {                                                                       \
                                    isFailed = isFailed || ((((const uint8_t*)(void*)                   \
                                        &(module##_##blockName##Mirror))[5]^0xFFu) !=                   \
                                        ((const uint8_t*)(void*)&(module##_##blockName))[5]);           \
                                    if( (int16_t)sizeof(module##_##blockName) >= 7)                     \
                                    {                                                                   \
                                        isFailed = isFailed || ((((const uint8_t*)(void*)               \
                                            &(module##_##blockName##Mirror))[6]^0xFFu) !=               \
                                            ((const uint8_t*)(void*)&(module##_##blockName))[6]);       \
                                    }                                                                   \
                                }                                                                       \
                            }                                                                           \
                        }                                                                               \
                    }                                                                                   \
                }                                                                                       \
                if(isFailed)                                                                            \
                {                                                                                       \
                    _FSF_vApplicationErrorNotify((uint8_t)FSF_ERR_CHANNEL_ERROR,                        \
                        (uint8_t)FSF_RAM_CONSISTENCY_CHECK, (ucId));                                    \
                }                                                                                       \
                HSUP_vEnableInt();                                                                      \
            }                                                                                           \
            else                                                                                        \
            {                                                                                           \
                                                                                                        \
            }
    #endif //VECTORCAST_TESTING
    #else
        /**< For a detailed description of the macro see \ref Service_FSF_RAM_CON_vCheck . */           \
        #define FSF_RAM_CON_vCheck(module, blockName, ucId)
    #endif
#endif

/*~+:FSF_RAM_CON_vUpdateCheck(module, blockName)*/
/**
 * \brief This function updates the check value of the 'consistent memory block using the method \ref FSF_RAM_CON_DESCR_CHECK_VALUE.
 *
 * @warning This function should not be called directly from other submodules than FSF RAM CON.
 *
 * @param pucCheckValue The address of the 'check value' - here the 8 bit sum of the 'consistent memory block', for update purpose.
 * @param ucSizeInBytes The size of the 'consistent memory block' in bytes.
 * @param pucStartAddress The start address of 'consistent memory block' to be checked.
 */
extern void _FSF_RAM_CON_vUpdateCheckBySum(uint8_t *pucCheckValue, uint8_t ucSizeInBytes, const uint8_t *pucStartAddress);

/**
 * \brief This function updates the check value of the 'consistent memory block using the method \ref FSF_RAM_CON_DESCR_MIRROR.
 *
 * @warning This function should not be called directly from other submodules than FSF RAM CON.
 *
 * @param ucSizeInBytes The size of the 'consistent memory block' in bytes.
 * @param pucStartAddress The start address of 'consistent memory block' to be checked.
 * @param pucStartAddress The start address of mirrored 'consistent memory block' to update.
 */
extern void _FSF_RAM_CON_vUpdateCheckByMirror(uint8_t ucSizeInBytes, const uint8_t *pucStartAddress, uint8_t *pucMirrorStartAddress);
#ifdef FSF_RAM_CON_BY_CHECK_VALUE
    #define FSF_RAM_CON_vUpdateCheck(module, blockName)                                                 \
        if((int16_t)sizeof(module##_##blockName) >= 8)                                                  \
        {                                                                                               \
            _FSF_RAM_CON_vUpdateCheckBySum(                                                             \
                (uint8_t*)((void*)&(module##_ucCheckValue##blockName)),                                 \
                (uint8_t)sizeof(module##_##blockName),                                                  \
                (const uint8_t*)((void*)&(module##_##blockName)));                                      \
        }                                                                                               \
        else if((int16_t)sizeof(module##_##blockName) >= 1)                                             \
        {                                                                                               \
            uint8_t ucNewCheckValue;                                                                    \
            HSUP_vDisableInt();                                                                         \
            ucNewCheckValue = ((const uint8_t*)(void*)&(module##_##blockName))[0];                      \
            if( (int16_t)sizeof(module##_##blockName) >= 2)                                             \
            {                                                                                           \
                ucNewCheckValue += ((const uint8_t*)(void*)&(module##_##blockName))[1];                 \
                if( (int16_t)sizeof(module##_##blockName) >= 3)                                         \
                {                                                                                       \
                    ucNewCheckValue += ((const uint8_t*)(void*)&(module##_##blockName))[2];             \
                    if( (int16_t)sizeof(module##_##blockName) >= 4)                                     \
                    {                                                                                   \
                        ucNewCheckValue += ((const uint8_t*)(void*)&(module##_##blockName))[3];         \
                        if( (int16_t)sizeof(module##_##blockName) >= 5)                                 \
                        {                                                                               \
                            ucNewCheckValue += ((const uint8_t*)(void*)&(module##_##blockName))[4];     \
                            if( (int16_t)sizeof(module##_##blockName) >= 6)                             \
                            {                                                                           \
                                ucNewCheckValue += ((const uint8_t*)(void*)&(module##_##blockName))[5]; \
                                if( (int16_t)sizeof(module##_##blockName) >= 7)                         \
                                {                                                                       \
                                    ucNewCheckValue +=                                                  \
                                        ((const uint8_t*)(void*)&(module##_##blockName))[6];            \
                                }                                                                       \
                            }                                                                           \
                        }                                                                               \
                    }                                                                                   \
                }                                                                                       \
            }                                                                                           \
            module##_ucCheckValue##blockName = ucNewCheckValue;                                         \
            HSUP_vEnableInt();                                                                          \
        }                                                                                               \
        else                                                                                            \
        {                                                                                               \
                                                                                                        \
        }
#else
    #ifdef FSF_RAM_CON_BY_MIRROR
    #ifdef VECTORCAST_TESTING
        #warning " VECTOR CAST RAM CON CHECK DISABLING is active !!! "
        #define FSF_RAM_CON_vUpdateCheck(module, blockName)     FSF_DUMMY_RAM_CON_vUpdateCheckByMirror();
    #else // regular usage
        #define FSF_RAM_CON_vUpdateCheck(module, blockName)                                             \
            if( (int16_t)sizeof(module##_##blockName) >= 8)                                             \
            {                                                                                           \
                _FSF_RAM_CON_vUpdateCheckByMirror(                                                      \
                    (uint8_t)sizeof(module##_##blockName),                                              \
                    (const uint8_t*)((void*)&(module##_##blockName)),                                   \
                    (uint8_t*)((void*)&(module##_##blockName##Mirror)));                                \
            }                                                                                           \
            else if( (int16_t)sizeof(module##_##blockName) >= 1)                                        \
            {                                                                                           \
                HSUP_vDisableInt();                                                                     \
                ((uint8_t*)(void*)&(module##_##blockName##Mirror))[0] =                                 \
                ((((const uint8_t*)(void*)&(module##_##blockName))[0])^0xFFu);                          \
                if( (int16_t)sizeof(module##_##blockName) >= 2)                                         \
                {                                                                                       \
                    ((uint8_t*)(void*)&(module##_##blockName##Mirror))[1] =                             \
                    ((((const uint8_t*)(void*)&(module##_##blockName))[1])^0xFFu);                      \
                    if( (int16_t)sizeof(module##_##blockName) >= 3)                                     \
                    {                                                                                   \
                        ((uint8_t*)(void*)&(module##_##blockName##Mirror))[2] =                         \
                        ((((const uint8_t*)(void*)&(module##_##blockName))[2])^0xFFu);                  \
                        if( (int16_t)sizeof(module##_##blockName) >= 4)                                 \
                        {                                                                               \
                            ((uint8_t*)(void*)&(module##_##blockName##Mirror))[3] =                     \
                            ((((const uint8_t*)(void*)&(module##_##blockName))[3])^0xFFu);              \
                            if( (int16_t)sizeof(module##_##blockName) >= 5)                             \
                            {                                                                           \
                                ((uint8_t*)(void*)&(module##_##blockName##Mirror))[4] =                 \
                                ((((const uint8_t*)(void*)&(module##_##blockName))[4])^0xFFu);          \
                                if( (int16_t)sizeof(module##_##blockName) >= 6)                         \
                                {                                                                       \
                                    ((uint8_t*)(void*)&(module##_##blockName##Mirror))[5] =             \
                                    ((((const uint8_t*)(void*)&(module##_##blockName))[5])^0xFFu);      \
                                    if( (int16_t)sizeof(module##_##blockName) >= 7)                     \
                                    {                                                                   \
                                        ((uint8_t*)(void*)&(module##_##blockName##Mirror))[6] =         \
                                        ((((const uint8_t*)(void*)&(module##_##blockName))[6])^0xFFu);  \
                                    }                                                                   \
                                }                                                                       \
                            }                                                                           \
                        }                                                                               \
                    }                                                                                   \
                }                                                                                       \
                HSUP_vEnableInt();                                                                      \
            }                                                                                           \
            else                                                                                        \
            {                                                                                           \
                                                                                                        \
            }
    #endif //VECTORCAST_TESTING
    #else
        /**< For a detailed description of the macro see \ref Service_FSF_RAM_CON_vUpdateCheck . */     \
        #define FSF_RAM_CON_vUpdateCheck(module, blockName)
    #endif
#endif

/*~+:FSF_RAM_CON_vSetUchar(module, blockName, elementName, value)*/
/**
 * \brief This function sets the value of a variable  of type \ref uint8_t and updates the check value related to a 'consistent memory block using the method \ref FSF_RAM_CON_DESCR_CHECK_VALUE.
 *
 * @warning This function should not be called directly from other submodules than FSF RAM CON.
 *
 * @param pucVariable The address of the variable to set.
 * @param pucCheckValue The address of the 'check value' - here the 8 bit sum of the 'consistent memory block' to update.
 * @param ucNewVal The value to be set.
 */
extern void _FSF_RAM_CON_vSetUcharBySum(uint8_t *pucVariable, uint8_t *pucCheckValue, uint8_t ucNewVal);
#ifdef FSF_RAM_CON_BY_CHECK_VALUE
    #define FSF_RAM_CON_vSetUchar(module, blockName, elementName, ucNewVal)     \
    {                                                                           \
        _FSF_RAM_CON_vSetUcharBySum(                                            \
            (uint8_t*)((void*)&(module##_##blockName.elementName)),           \
            (uint8_t*)((void*)&(module##_ucCheckValue##blockName)),             \
            (ucNewVal));                                                        \
    }
#else
    #ifdef FSF_RAM_CON_BY_MIRROR
        #define FSF_RAM_CON_vSetUchar(module, blockName, elementName, ucNewVal) \
        {                                                                       \
            HSUP_vDisableInt();                                                 \
            module##_##blockName.elementName = (ucNewVal);                      \
            module##_##blockName##Mirror.elementName   = (ucNewVal)^0xFFu;      \
            HSUP_vEnableInt();                                                  \
        }
    #else
        #define FSF_RAM_CON_vSetUchar(module, blockName, elementName, ucNewVal) \
        {                                                                       \
            /**< For a detailed description of the macro see
             * \ref Service_FSF_RAM_CON_vSetUchar . */                          \
            module##_##blockName.elementName = (ucNewVal);                      \
        }
    #endif
#endif

/*~+:FSF_RAM_CON_vSetBoolean(module, blockName, elementName, value)*/
#ifdef FSF_RAM_CON_BY_CHECK_VALUE
    #define FSF_RAM_CON_vSetBoolean(module, blockName, elementName, bNewVal)    \
    {                                                                           \
        _FSF_RAM_CON_vSetUcharBySum(                                            \
            (uint8_t*)((void*)&(module##_##blockName.elementName)),             \
            (uint8_t*)((void*)&(module##_ucCheckValue##blockName)),             \
            (uint8_t)(bNewVal));                                                \
    }
#else
    #ifdef FSF_RAM_CON_BY_MIRROR
        #define FSF_RAM_CON_vSetBoolean(module, blockName, elementName, bNewVal)\
        {                                                                       \
            HSUP_vDisableInt();                                                 \
            *(uint8_t*)((void*)&(module##_##blockName.elementName)) =           \
                (uint8_t)(bNewVal);                                             \
            *(uint8_t*)((void*)&(module##_##blockName##Mirror.elementName)) =   \
                (uint8_t)((uint8_t)(bNewVal)^0xFFu);                            \
            HSUP_vEnableInt();                                                  \
        }
    #else
        #define FSF_RAM_CON_vSetBoolean(module, blockName, elementName, bNewVal)\
        {                                                                       \
            /**< For a detailed description of the macro see
             * \ref Service_FSF_RAM_CON_vSetBoolean . */                        \
            module##_##blockName.elementName = (bNewVal);                       \
        }
    #endif
#endif

/*~+:FSF_RAM_CON_vSetUint(module, blockName, elementName, value)*/
/**
 * \brief This function sets the value of a variable of type \ref uint16_t and updates the check value related to a 'consistent memory block using the method \ref FSF_RAM_CON_DESCR_CHECK_VALUE.
 *
 * @warning This function should not be called directly from other submodules than FSF RAM CON.
 *
 * @param puiVariable The address of the variable to set.
 * @param pucCheckValue The address of the 'check value' - here the 8 bit sum of the 'consistent memory block' to update.
 * @param uiNewVal The value to be set.
 */
extern void _FSF_RAM_CON_vSetUintBySum(uint16_t *puiVariable, uint8_t *pucCheckValue, uint16_t uiNewVal);
#ifdef FSF_RAM_CON_BY_CHECK_VALUE
    #define FSF_RAM_CON_vSetUint(module, blockName, elementName, uiNewVal)      \
    {                                                                           \
        _FSF_RAM_CON_vSetUintBySum(                                             \
            (uint16_t*)((void*)&(module##_##blockName.elementName)),            \
            (uint8_t*)((void*)&(module##_ucCheckValue##blockName)),             \
            (uiNewVal));                                                        \
    }
#else
    #ifdef FSF_RAM_CON_BY_MIRROR
        #define FSF_RAM_CON_vSetUint(module, blockName, elementName, uiNewVal)  \
        {                                                                       \
            HSUP_vDisableInt();                                                 \
            module##_##blockName.elementName = (uiNewVal);                      \
            module##_##blockName##Mirror.elementName = (uiNewVal)^0xFFFFu;      \
            HSUP_vEnableInt();                                                  \
        }
    #else
        #define FSF_RAM_CON_vSetUint(module, blockName, elementName, uiNewVal)  \
        {                                                                       \
            /**< For a detailed description of the macro see
             * \ref Service_FSF_RAM_CON_vSetUint. */                            \
            module##_##blockName.elementName = (uiNewVal);                      \
        }
    #endif
#endif

/*~+:FSF_RAM_CON_vSetBit(module, blockName, bitFieldName, field, value)*/
#ifdef FSF_RAM_CON_BY_CHECK_VALUE
    #define FSF_RAM_CON_vSetBit(module, blockName, bitFieldName, bitName,       \
        ucNewVal)                                                               \
    {                                                                           \
        HSUP_vDisableInt();                                                     \
        module##_ucCheckValue##blockName -=                                     \
            *((volatile uint8_t*)(void*)&(module##_##blockName.bitFieldName));  \
                                                                                \
        module##_##blockName.bitFieldName.bitName = (ucNewVal);                 \
                                                                                \
        module##_ucCheckValue##blockName +=                                     \
            *((volatile uint8_t*)(void*)&(module##_##blockName.bitFieldName));  \
        HSUP_vEnableInt();                                                      \
    }
#else
    #ifdef FSF_RAM_CON_BY_MIRROR
        #define FSF_RAM_CON_vSetBit(module, blockName, bitFieldName, bitName,   \
            ucNewVal)                                                           \
        {                                                                       \
            HSUP_vDisableInt();                                                 \
            module##_##blockName.bitFieldName.bitName = (ucNewVal);             \
            module##_##blockName##Mirror.bitFieldName.bitName = (ucNewVal)^1u;  \
            HSUP_vEnableInt();                                                  \
        }
    #else
        #define FSF_RAM_CON_vSetBit(module, blockName, bitFieldName, bitName,   \
            ucNewVal)                                                           \
        {                                                                       \
            /**< For a detailed description of the macro see
             * \ref Service_FSF_RAM_CON_vSetBit. */                             \
            module##_##blockName.bitFieldName.bitName = (ucNewVal);             \
        }
    #endif
#endif

/*~+:FSF_RAM_CON_vSetBitUchar(module, blockName, bitFieldVar, bitMask)*/
#ifdef FSF_RAM_CON_BY_CHECK_VALUE 
    #define FSF_RAM_CON_vSetBitUchar(module, blockName, bitFieldVar, bitMask)   \
    {                                                                           \
        HSUP_vDisableInt();                                                     \
        module##_ucCheckValue##blockName -= module##_##blockName.bitFieldVar;   \
                                                                                \
        module##_##blockName.bitFieldVar |= (uint8_t)(bitMask);                 \
                                                                                \
        module##_ucCheckValue##blockName += module##_##blockName.bitFieldVar;   \
        HSUP_vEnableInt();                                                      \
    }
#else
    #ifdef FSF_RAM_CON_BY_MIRROR
        #define FSF_RAM_CON_vSetBitUchar(module, blockName, bitFieldVar, bitMask)\
        {                                                                       \
            HSUP_vDisableInt();                                                 \
                                                                                \
            module##_##blockName.bitFieldVar |= (uint8_t)(bitMask);             \
            module##_##blockName##Mirror.bitFieldVar =                          \
                ((module##_##blockName##Mirror.bitFieldVar)^0xFFu)              \
                | ((uint8_t)(bitMask));                                         \
            module##_##blockName##Mirror.bitFieldVar ^= 0xFFu;                  \
                                                                                \
            HSUP_vEnableInt();                                                  \
        }
    #else
        #define FSF_RAM_CON_vSetBitUchar(module, blockName, bitFieldVar, bitMask)\
        {                                                                       \
            /**< For a detailed description of the macro see
             * \ref Service_FSF_RAM_CON_vSetBitUchar . */                       \
            module##_##blockName.bitFieldVar |= (uint8_t)(bitMask);             \
        }
    #endif
#endif

/*~+:FSF_RAM_CON_vResBitUchar(module, blockName, bitFieldVar, bitMask)*/
#ifdef FSF_RAM_CON_BY_CHECK_VALUE 
    #define FSF_RAM_CON_vResBitUchar(module, blockName, bitFieldVar, bitMask)   \
    {                                                                           \
        HSUP_vDisableInt();                                                     \
        module##_ucCheckValue##blockName -= module##_##blockName.bitFieldVar;   \
                                                                                \
        module##_##blockName.bitFieldVar &= (uint8_t)(~((uint8_t)(bitMask)));   \
                                                                                \
        module##_ucCheckValue##blockName += module##_##blockName.bitFieldVar;   \
        HSUP_vEnableInt();                                                      \
    }
#else
    #ifdef FSF_RAM_CON_BY_MIRROR
        #define FSF_RAM_CON_vResBitUchar(module, blockName, bitFieldVar, bitMask)\
        {                                                                       \
            HSUP_vDisableInt();                                                 \
                                                                                \
            module##_##blockName.bitFieldVar &= (uint8_t)(~((uint8_t)(bitMask)));\
            module##_##blockName##Mirror.bitFieldVar =                          \
                ((module##_##blockName##Mirror.bitFieldVar)^0xFFu)              \
                & ((uint8_t)(~((uint8_t)(bitMask))));                           \
            module##_##blockName##Mirror.bitFieldVar ^= 0xFFu;                  \
            HSUP_vEnableInt();                                                  \
        }
    #else
        #define FSF_RAM_CON_vResBitUchar(module, blockName, bitFieldVar, bitMask)\
        {                                                                       \
            /**< For a detailed description of the macro see
             * \ref Service_FSF_RAM_CON_vResBitUchar. */                        \
            module##_##blockName.bitFieldVar &= (uint8_t)~((uint8_t)(bitMask)); \
        }
    #endif
#endif

/*~+:FSF_RAM_CON_vSetBitUint(module, blockName, bitFieldVar, bitMask)*/
/**
 * \brief This function sets bits in a variable of type \ref uint16_t using a bitmask 
 * and updates the check value related to a 'consistent memory block using the method \ref FSF_RAM_CON_DESCR_CHECK_VALUE.
 *
 * @warning This function should not be called directly from other submodules than FSF RAM CON.
 *
 * @param puiBitField The address of the bitfield variable to set.
 * @param pucCheckValue The address of the 'check value' - here the 8 bit sum of the 'consistent memory block' to update.
 * @param uiBitMask The bit mask, denoting the bits to set.
 */
extern void _FSF_RAM_CON_vSetBitUintBySum(uint16_t *puiBitField, uint8_t *pucCheckValue, uint16_t uiBitMask);
#ifdef FSF_RAM_CON_BY_CHECK_VALUE 
    #define FSF_RAM_CON_vSetBitUint(module, blockName, bitFieldVar, uiBitMask)  \
    {                                                                           \
        _FSF_RAM_CON_vSetBitUintBySum(                                          \
            (uint16_t*)((void*)&(module##_##blockName.bitFieldVar)),            \
            (uint8_t*)((void*)&(module##_ucCheckValue##blockName)),             \
            uiBitMask);                                                         \
    }
#else
    #ifdef FSF_RAM_CON_BY_MIRROR
        #define FSF_RAM_CON_vSetBitUint(module, blockName, bitFieldVar, bitMask)\
        {                                                                       \
            HSUP_vDisableInt();                                                 \
                                                                                \
            module##_##blockName.bitFieldVar |= (uint16_t)(bitMask);            \
            module##_##blockName##Mirror.bitFieldVar =                          \
                ((module##_##blockName##Mirror.bitFieldVar)^0xFFFFu)            \
                | ((uint16_t)(bitMask));                                        \
            module##_##blockName##Mirror.bitFieldVar ^= 0xFFFFu;                \
                                                                                \
            HSUP_vEnableInt();                                                  \
        }
    #else
        #define FSF_RAM_CON_vSetBitUint(module, blockName, bitFieldVar, bitMask)\
        {                                                                       \
            /**< For a detailed description of the macro see
             * \ref Service_FSF_RAM_CON_vSetBitUint . */                        \
            module##_##blockName.bitFieldVar |= (uint16_t)(bitMask);            \
        }
    #endif
#endif

/*~+:FSF_RAM_CON_vResBitUint(module, blockName, bitFieldVar, bitMask)*/
/**
 * \brief This function resets bits in a variable of type \ref uint16_t using a bitmask 
 * and updates the check value related to a 'consistent memory block using the method \ref FSF_RAM_CON_DESCR_CHECK_VALUE.
 *
 * @warning This function should not be called directly from other submodules than FSF RAM CON.
 *
 * @param puiBitField The address of the bitfield variable to reset.
 * @param pucCheckValue The address of the 'check value' - here the 8 bit sum of the 'consistent memory block' to update.
 * @param uiBitMask The bit mask, denoting the bits to reset.
 */
extern void _FSF_RAM_CON_vResBitUintBySum(uint16_t *puiBitField, uint8_t *pucCheckValue, uint16_t uiBitMask);
#ifdef FSF_RAM_CON_BY_CHECK_VALUE 
    #define FSF_RAM_CON_vResBitUint(module, blockName, bitFieldVar, uiBitMask)  \
    {                                                                           \
        _FSF_RAM_CON_vResBitUintBySum(                                          \
            (uint16_t*)((void*)&(module##_##blockName.bitFieldVar)),            \
            (uint8_t*)((void*)&(module##_ucCheckValue##blockName)),             \
            uiBitMask);                                                         \
    }
#else
    #ifdef FSF_RAM_CON_BY_MIRROR
        #define FSF_RAM_CON_vResBitUint(module, blockName, bitFieldVar, bitMask)\
        {                                                                       \
            HSUP_vDisableInt();                                                 \
                                                                                \
            module##_##blockName.bitFieldVar &= ~((uint16_t)(bitMask));         \
            module##_##blockName##Mirror.bitFieldVar =                          \
                ((module##_##blockName##Mirror.bitFieldVar)^0xFFFFu)            \
                & (~((uint16_t)(bitMask)));                                     \
            module##_##blockName##Mirror.bitFieldVar ^= 0xFFFFu;                \
                                                                                \
            HSUP_vEnableInt();                                                  \
        }
    #else
        #define FSF_RAM_CON_vResBitUint(module, blockName, bitFieldVar, bitMask)\
        {                                                                       \
            /**< For a detailed description of the macro see
             * \ref Service_FSF_RAM_CON_vResBitUint . */                        \
            module##_##blockName.bitFieldVar &= ~((uint16_t)(bitMask));         \
        }
    #endif
#endif

/*~+:FSF_RAM_CON_vSetBitStructUint(module, blockName, bitFieldStructVar, bitFieldBit)*/
/**
 * \brief This function sets a bit in a bitfiled variable by name
 * and updates the check value related to a 'consistent memory block.
 */
#ifdef FSF_RAM_CON_BY_CHECK_VALUE
    #define FSF_RAM_CON_vSetBitStructUint(module, blockName, bitFieldStructVar, \
        bitFieldBit)                                                            \
    {                                                                           \
        uint16_t uiTemp;                                                        \
        HSUP_vDisableInt();                                                     \
        uiTemp = *((uint16_t*)((void*)&module##_##blockName.bitFieldStructVar));\
        module##_##blockName.bitFieldStructVar.bitFieldBit = (BOOL)true;        \
        uiTemp ^= *((uint16_t*)((void*)&module##_##blockName.bitFieldStructVar));\
                                                                                \
        module##_ucCheckValue##blockName += (HSUP_HIGH_BYTE(uiTemp));           \
        module##_ucCheckValue##blockName += (HSUP_LOW_BYTE(uiTemp));            \
        HSUP_vEnableInt();                                                      \
    }
#else
    #ifdef FSF_RAM_CON_BY_MIRROR
        #define FSF_RAM_CON_vSetBitStructUint(module, blockName,                \
            bitFieldStructVar, bitFieldBit)                                     \
        {                                                                       \
            HSUP_vDisableInt();                                                 \
            module##_##blockName.bitFieldStructVar.bitFieldBit = (BOOL)true;    \
            module##_##blockName##Mirror.bitFieldStructVar.bitFieldBit =        \
                (BOOL)false;                                                    \
            HSUP_vEnableInt();                                                  \
        }
    #else
        #define FSF_RAM_CON_vSetBitStructUint(module, blockName,                \
            bitFieldStructVar, bitFieldBit)                                     \
        {                                                                       \
            module##_##blockName.bitFieldStructVar.bitFieldBit = (BOOL)true;    \
        }
    #endif
#endif

/*~+:FSF_RAM_CON_vResBitStructUint(module, blockName, bitFieldStructVar, bitFieldBit)*/
/**
 * \brief This function resets a bit in a bitfiled variable by name
 * and updates the check value related to a 'consistent memory block.
 */
#ifdef FSF_RAM_CON_BY_CHECK_VALUE
    #define FSF_RAM_CON_vResBitStructUint(module, blockName, bitFieldStructVar, \
        bitFieldBit)                                                            \
    {                                                                           \
        uint16_t uiTemp;                                                        \
        HSUP_vDisableInt();                                                     \
        uiTemp = *((uint16_t*)((void*)&module##_##blockName.bitFieldStructVar));\
        module##_##blockName.bitFieldStructVar.bitFieldBit = (BOOL)false;       \
        uiTemp ^= *((uint16_t*)((void*)&module##_##blockName.bitFieldStructVar));\
                                                                                \
        module##_ucCheckValue##blockName -= (HSUP_HIGH_BYTE(uiTemp));           \
        module##_ucCheckValue##blockName -= (HSUP_LOW_BYTE(uiTemp));            \
        HSUP_vEnableInt();                                                      \
    }
#else
    #ifdef FSF_RAM_CON_BY_MIRROR
        #define FSF_RAM_CON_vResBitStructUint(module, blockName,                \
            bitFieldStructVar, bitFieldBit)                                     \
        {                                                                       \
            HSUP_vDisableInt();                                                 \
            module##_##blockName.bitFieldStructVar.bitFieldBit = (BOOL)false;   \
            module##_##blockName##Mirror.bitFieldStructVar.bitFieldBit =        \
                (BOOL)true;                                                     \
            HSUP_vEnableInt();                                                  \
        }
    #else
        #define FSF_RAM_CON_vResBitStructUint(module, blockName,                \
            bitFieldStructVar, bitFieldBit)                                     \
        {                                                                       \
            module##_##blockName.bitFieldStructVar.bitFieldBit = (BOOL)false;   \
        }
    #endif
#endif

/*lint -restore -e9023 -e9024 -e9026*/
  
#ifdef __cplusplus
}
#endif

/*lint -restore -e9023 -e9024 */
#endif //__FSRAMCON_H

