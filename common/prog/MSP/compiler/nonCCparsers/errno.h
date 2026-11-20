/*******************************************************************************
 *  PROJECT          PP PED-Framework
 *  COMP_ABBREV      --
 *  AUTHOR           Ulrike Brueggemann
 *  CREATED          2017-01-26
 ******************************************************************************/
#ifndef ERRNO_H
#define ERRNO_H

/******************************************************************************/
/* DOCUMENTATION                                                              */
/******************************************************************************/
/** \file
 *     \b Note: This file exists to provide code analyser tools with standard
 *     library declarations in ANSII conforming form
 *     (to inhibit unnecessary  error messages).
 */

extern int errno;

#define EDOM    1
#define ERANGE  2
#define EILSEQ  4
#define ESIGNUM 3
#define EINVAL  5
#define ENOMEM  6 

#endif
