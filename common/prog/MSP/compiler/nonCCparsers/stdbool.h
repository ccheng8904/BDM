/********************************************************************************
*   PROJECT          PP PED-Framework
*******************************************************************************/
#ifndef STDBOOL_H
#define STDBOOL_H

/******************************************************************************/
/* DOCUMENTATION                                                              */
/******************************************************************************/
/** \file
 *    Standard C99 bool data type, independent of processor/compiler.
 *    This is defined for compilers without C99 support
 *    \b Note: This file exists to provide code analyser tools with standard
 *    library declarations in ANSII conforming form
 *   (to inhibit unnecessary  error messages).
 */


#ifndef __cplusplus
typedef _Bool bool;
#define true  1
#define false 0
#endif

#endif
