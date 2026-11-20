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
 ******************************************************************************/

#if defined(__ICCARM__) && !defined(__NOT_CC__)

/******************************************************************************/
/* DOCUMENTATION                                                              */
/******************************************************************************/
/** \file
 *
 *  \ingroup  retarget
 *
 *  \brief    Function definitions for IAR system calls retargeting.
 *
 *  \details  
 */


/******************************************************************************/
/* INCLUDES                                                                   */
/******************************************************************************/

#include "retarget.h"
//#include <yfuns.h>


/******************************************************************************/
/* PRIVATE DATA DEFINITIONS                                                   */
/******************************************************************************/


/******************************************************************************/
/* RETARGET FUNCTION DEFINITONS                                               */
/******************************************************************************/


__weak size_t __write(int handle, const unsigned char * buffer, size_t size)
{
    int index = 0;
    (void)handle;

    if (buffer != NULL)
    {
        for (; index < size; ++index)
        {
            RTG_writeChar(buffer[index]);
        }
    }
    return (index);
}

__weak size_t __read(int handle, unsigned char * buffer, size_t size)
{
    (void)size;
    *buffer = (size_t)RTG_readChar();
    return 1;
}

#endif // defined(__ICCARM__)
