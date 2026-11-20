/*
 * stdlib.h
 *
 *  Created on: 29.09.2014
 *      Author: Borowczak
 *      This file exists to provide code analyser tools with standard library declarations in ANSII conforming form (to inhibit unnecessary  error messages).
 */

#ifndef STDLIB_H_
#define STDLIB_H_

#include <stddef.h>
//lint -save -e970 modifier or type '...' outside of a typedef : basic types must be used exactly like in compiler packages
int mblen       (const char *mbstr, size_t count);
int rand(void);
int abs (int n);

void*   calloc  (size_t nobj, size_t size);
void*   malloc  (size_t size) ;
void*   realloc (void *p, size_t size);
void    free    (void *p);
void    abort   (void);
void    exit    (int status);
int     atoi    (const char *s);  //lint !e9046 'atoi' is typographically ambiguous with respect to 'atol'
long    atol    (const char *s);  //lint !e9046 'atoi' is typographically ambiguous with respect to 'atol'
double  atof    (const char *s);
void*   bsearch (const void *key, const void *base, size_t n, size_t size, int (*cmp)(const void *keyval, const void *datum));
void qsort(void *base, size_t n, size_t size, int (*cmp)(const void *, const void *));
int     system  (const char *s);
char*   getenv  (const char *s);
double strtod   (const char *s, char **endp);

#if defined (UTEST_TYPE_SYSTEM_KEIL_ARM)


#elif defined (UTEST_TYPE_SYSTEM_Windows32)


#elif defined (UTEST_TYPE_SYSTEM_RENESAS_RX)


#endif
//lint -restore -e970
#endif /* STDLIB_H_ */
