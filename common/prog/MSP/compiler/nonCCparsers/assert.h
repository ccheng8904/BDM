/********************************************************************************
*   PROJECT          Generic SW
*******************************************************************************/
#ifndef ASSERT_H
#define ASSERT_H

/*lint -save -e9024 stringize operator used in definition of function-like macro */
/******************************************************************************/
/* DOCUMENTATION                                                              */
/******************************************************************************/
/** \file
 *     This file exists to provide code analyser tools with standard library
 *     declarations in ANSII conforming form
 *     (to inhibit unnecessary  error messages).
 */

/*lint -save -e9024 -e9026 */
#ifdef NDEBUG           /* required by ANSI standard */
#define assert(__e) ((void)0)

#else
#define assert(__e) ((__e) ? ((void)0) : __assert_func(__FILE__, __LINE__, #__e))

#endif /* NDEBUG */
/*lint -restore -e9024 -e9026 */
extern void __assert_func(const char *file, int line, const char *s);//lint !e970 use of modifier or type 'int' outside of a typedef [MISRA 2012 Directive 4.6, required]

/*lint -restore -e9024 */
#endif // ASSERT_H
