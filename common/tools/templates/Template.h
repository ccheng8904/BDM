/*******************************************************************************
 *  Copyright (c) 2017 BSH Hausgeraete GmbH,
 *  Carl-Wery-Str. 34, 81739 Munich, Germany, www.bsh-group.de
 *
 *  All rights reserved. This program and the accompanying materials
 *  are protected by international copyright laws.
 *  Please contact copyright holder for licensing information.
 *
 *******************************************************************************
 *  PROJECT          <$PROJECT_NAME>
 ******************************************************************************/


#ifndef <$UNIT_NAME>_H
#define <$UNIT_NAME>_H

#ifdef __cplusplus
extern "C" {
#endif


/******************************************************************************/
/* DOCUMENTATION                                                              */
/******************************************************************************/
/** \file
 *
 *  \ingroup  <$DOXY_GROUP>
 *
 *  \brief    Public definitions and declarations for unit <$UnitName>.
 *
 *  \details  A detailed file description that may span several lines
 *            and that has to be terminated by an empty line.
 *
 */



/******************************************************************************/
/* INCLUDES                                                                   */
/******************************************************************************/
#include <stdint.h>


/******************************************************************************/
/*   DEPENDENCIES                                                             */
/******************************************************************************/
/** \brief    Dependencies for unit <$UnitName>
 *
 *  \details  If the unit is used in the PED Framework
 *            then the macro is needed in schedDef.c i.e.
 *            the config file of the scheduler library.
 *
 *  \attention  Add dependencies and description of dependencies!
 *              Do not forget the final comma with exception there are no dependencies!
 *              For example: #define DEP_XXX     MOD_YYY, MOD_ZZZ,
 *
 */
#define DEP_<$UNIT_PREFIX>


/******************************************************************************/
/* PUBLIC DEFINITIONS                                                         */
/******************************************************************************/
/** \brief    A brief macro description
 *            that has to be terminated by an empty line.
 *
 *  \details  A detailed  macro description that may span several lines
 *            and that has to be  terminated by an empty line.
 */
#define XXX_PUBLIC_DEF 0x55AAU


/******************************************************************************/
/* PUBLIC TYPE DEFINITIONS                                                    */
/******************************************************************************/

/** \brief    A brief struct type description
 *            that has to be terminated by an empty line.
 *
 *  \details  A detailed type description that may span several lines
 *            and that has to be  terminated by an empty line.
 */
struct XXX_PublicContext
{
    uint32_t member1; ///< short member1 description
    uint32_t member2; ///< short member2 description
};

/** \brief    A brief enum type description 
 *            that has to be terminated by an empty line.
 *
 *  \details  A detailed enum type description that may span several lines
 *            and that has to be terminated by an empty line.
 */
enum XXX_PublicStatus
{
    XXX_PublicStatus1, ///< 1st status documentation
    XXX_PublicStatus2, ///< 2nd status documentation
    XXX_PublicStatus3  ///< 3rd status documentation
};


/******************************************************************************/
/* PUBLIC FUNCTION DECLARATIONS                                               */
/******************************************************************************/

/** \brief    A brief function description
 *            that has to be terminated by an empty line.
 *
 *  \details  A detailed function description that may span several lines
 *            and that has to be terminated by an empty line.
 *
 *  \param    this:      context pointer
 *  \param    startAddr: logical start address
 *  \param    length:    number of bytes to be handled
 *
 *  \return   XXX_Status1, XXX_Status2, XXX_Status3
 */
extern enum XXX_PublicStatus XXX_doSomeStuff
(
    const struct XXX_PublicContext *this,
    uint32_t startAddr,
    uint32_t length
);

/** \brief    A brief inline function description
 *            that has to be terminated by an empty line.
 *
 *  \details  A detailed inline function description that may span several lines
 *            and that has to be terminated by an empty line.
 *
 *  \param    this:      context pointer
 *  \param    startAddr: logical start address
 *  \param    length:    number of bytes to be handled
 *
 *  \return   XXX_Status1, XXX_Status2, XXX_Status3
 */
static inline enum XXX_PublicStatus XXX_doSomeOtherStuff
(
    const struct XXX_PublicContext *this,
    uint32_t startAddr,
    uint32_t length
)
{
    return XXX_PublicStatus1;
}

/** \brief    A brief function macro description
 *            that has to be terminated by an empty line.
 *
 *  \details  A detailed function macro description that may span several lines
 *            and that has to be terminated by an empty line.
 *            Recommendation: Better define an inline function.
 *
 *  \param    A: first parameter
 *  \param    B: second parameter
 */
#define XXX_doSomeMoreStuff(A, B)

#ifdef __cplusplus
}
#endif

#endif // <$UNIT_NAME>_H
