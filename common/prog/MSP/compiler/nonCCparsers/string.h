/********************************************************************************
*   PROJECT          PP PED-Framework
*   AUTHOR           Borowczak
*   CREATED          29.09.2014
*******************************************************************************/
#ifndef STRING_H
#define STRING_H

/******************************************************************************/
/* DOCUMENTATION                                                              */
/******************************************************************************/
/** \file
 *     This file exists to provide code analyser tools with standard library
 *     declarations in ANSII conforming form
 *     (to inhibit unnecessary  error messages).
 */


#ifdef __cplusplus
extern "C" {
#endif


//required for size_t
#include <stddef.h>

//lint -save -e970
extern void *memchr  (const char *buffer, int chr, size_t count);
extern int   memcmp  (const char *buf1, const char *buf2, size_t count);
extern void *memcpy  (void *dest, const void * source, size_t count);
extern void *memmove (void *dest, const void * source, size_t count);
extern void *memset  (void *buffer, int chr, size_t count);

extern size_t  strlen(const char *str);
extern char *strset  (char *str, int chr);
extern char *strcat  (char *str_d, const char *str_s);
extern char *strncat (char *str_d, const char *str_s, size_t count);
extern char *strcpy  (char *str_d, const char *str_s);
extern char *strncpy (char *str_d, const char *str_s, size_t count);
extern int   strcmp  (const char *str1, const char *str2);
extern int   strncmp (const char *str1, const char *str2, size_t count);
extern char *strchr  (const char *str, int chr);
extern char *strrchr (const char *str, int chr);
extern int  strspn   (const char *str1, const char *str2);
extern int  strcspn  (const char *str1, const char *str2);
extern char *strpbrk (const char *str1, const char *str2);
extern char *strstr  (const char *str1, const char *str2);
extern char *strtok  (char *str1, const char *str2);
extern char *strerror(int errnum);
extern int strcoll(const char *string1, const char *string2);
extern size_t strxfrm(char *strDest, const char *strSource, size_t count);
//lint -restore -e970


#ifdef __cplusplus
 }
#endif


#endif
