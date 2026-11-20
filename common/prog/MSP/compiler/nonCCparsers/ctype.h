/*******************************************************************************
 *  PROJECT          PP PED-Framework
 *  COMP_ABBREV      --
 *  AUTHOR           Jennifer Deepa
 *  CREATED          08-09-2017
 ******************************************************************************/
#ifndef CTYPE_H
#define CTYPE_H


/******************************************************************************/
/* DOCUMENTATION                                                              */
/******************************************************************************/
/** \file
 *     \b Note: This file exists to provide code analyser tools with standard
 *     library declarations in ANSII conforming form
 *     (to inhibit unnecessary  error messages).
 */


#define _UPPER      0x0001
#define _LOWER      0x0002
#define _DIGIT      0x0004
#define _SPACE      0x0008 /* HT  LF  VT  FF  CR  SP */
#define _PUNCT      0x0010
#define _CONTROL    0x0020
/* _BLANK is set for SP and non-ASCII horizontal space chars (eg,
   "no-break space", 0xA0, in CP1250) but not for HT.  */
#define _BLANK      0x0040
#define _HEX        0x0080
#define _LEADBYTE   0x8000

#define _ALPHA      0x0103
//lint -save -e970 usage of int outside type
int isalnum(int c);
int isalpha(int c);
int iscntrl(int c);
int isdigit(int c);
int isgraph(int c);
int islower(int c);
int isprint(int c);
int ispunct(int c);
int isspace(int c);
int isupper(int c);
int isxdigit(int c);

extern int*  _imp____mb_cur_max_dll;
#define MB_CUR_MAX (*_imp____mb_cur_max_dll)
//lint -restore -e970


#endif
