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


/******************************************************************************/
/* DOCUMENTATION                                                              */
/******************************************************************************/
/** \file
 *
 *  \ingroup  <$DOXY_GROUP>
 *
 *  \brief    Implementation of <$UnitName>.
 *
 *  \details  A detailed file description that may span several lines and
 *            that has to be terminated by an empty line.
 */


/******************************************************************************/
/* INCLUDES                                                                   */
/******************************************************************************/
#include "bsh_stdinc.h"
#include <stdint.h>
#include "<$UnitName>.h"


/******************************************************************************/
/* PRIVATE DEFINITIONS                                                        */
/******************************************************************************/

/** \brief    A brief macro type description
 *            that has to be terminated by an empty line.
 *
 *  \details  A detailed type description that may span several lines
 *            and that has to be terminated by an empty line.
 */
#define XXX_PRIVATE_DEF 0x55AAU


/******************************************************************************/
/* PRIVATE TYPE DEFINITIONS                                                   */
/******************************************************************************/

/** \brief    A brief struct type description
 *            that has to be terminated by an empty line.
 *
 *  \details  A detailed type description that may span several lines
 *            and that has to be terminated by an empty line.
 */
struct XXX_PrivateContext
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
enum XXX_PrivateStatus
{
    XXX_PrivateStatus1, ///< 1st status documentation
    XXX_PrivateStatus2, ///< 2nd status documentation
    XXX_PrivateStatus3  ///< 3rd status documentation
};


/******************************************************************************/
/* PRIVATE FUNCTION DECLARATIONS AND PRIVATE MACRO FUNCTION DEFINITIONS       */
/******************************************************************************/

static enum XXX_PrivateStatus XXX_doThePrivateStuff
(
    const struct XXX_PrivateContext *this
);

/** \brief    A brief function macro description
 *            that has to be terminated by an empty line.
 *
 *  \details  A detailed function macro description that may span several lines
 *            and that has to be terminated by an empty line.
 *            Recommendation: Better define an inline function.
 *
 *  \param    A first parameter
 *  \param    B second parameter
 */
#define PRIVARW_FUNCTION_MACRO(A, B)





/******************************************************************************/
/* PRIVATE DATA DEFINITIONS                                                   */
/******************************************************************************/

/** \brief    A brief data description
 *            that has to be terminated by an empty line.
 *
 *  \details  A detailed date description that may span several lines
 *            and that has to be terminated by an empty line.
 */
static struct XXX_PrivateContext XXX_MyPrivate;


/******************************************************************************/
/* PUBLIC FUNCTION DEFINITIONS                                                */
/******************************************************************************/

enum XXX_PublicStatus XXX_doSomeStuff
(
    const struct XXX_PublicContext *this,
    uint32_t startAddr,
    uint32_t length
)
{
    XXX_doThePrivateStuff(&XXX_MyPrivate);

    return XXX_PublicStatus1;
}


/******************************************************************************/
/* PRIVATE FUNCTION DEFINITIONS                                               */
/******************************************************************************/

/** \brief    A brief function description
 *            that has to be terminated by an empty line.
 *
 *  \details  A detailed function description that may span several lines
 *            and that has to be terminated by an empty line.
 *
 *  \param    this      context pointer
 */
static enum XXX_PrivateStatus XXX_doThePrivateStuf
(
    const struct XXX_PrivateContext *this
)
{
    return XXX_PrivateStatus1;
}


