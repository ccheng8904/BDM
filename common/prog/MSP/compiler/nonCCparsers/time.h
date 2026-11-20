/*******************************************************************************
 *  PROJECT          PP PED-Framework
 *  COMP_ABBREV      --
 *  AUTHOR           Jennifer Deepa
 *  CREATED          08-09-2017
 ******************************************************************************/
#ifndef TIME_H
#define TIME_H


/******************************************************************************/
/* DOCUMENTATION                                                              */
/******************************************************************************/
/** \file
 *     \b Note: This file exists to provide code analyser tools with standard
 *     library declarations in ANSII conforming form
 *     (to inhibit unnecessary  error messages).
 */


#define CLOCKS_PER_SEC  ((clock_t)1000)

/*
 * A type for measuring processor time (in clock ticks).
 */
#ifndef _CLOCK_T_DEFINED
typedef long    clock_t;
#define _CLOCK_T_DEFINED
#endif

#ifndef _TM_DEFINED
/*
 * A structure for storing all kinds of useful information about the
 * current (or another) time.
 */
struct tm
{
    int tm_sec;     /* Seconds: 0-59 (K&R says 0-61?) */
    int tm_min;     /* Minutes: 0-59 */
    int tm_hour;    /* Hours since midnight: 0-23 */
    int tm_mday;    /* Day of the month: 1-31 */
    int tm_mon;     /* Months *since* january: 0-11 */
    int tm_year;    /* Years since 1900 */
    int tm_wday;    /* Days since Sunday (0-6) */
    int tm_yday;    /* Days since Jan. 1: 0-365 */
    int tm_isdst;   /* +1 Daylight Savings Time, 0 No DST,
                 * -1 don't know */
};
#define _TM_DEFINED
#endif

typedef unsigned int time_t; /* date/time in unix secs past 1-Jan-70 */


clock_t clock (void);
time_t  time (time_t*);
char* asctime (const struct tm*);
struct tm*  localtime (const time_t*);
char* ctime (const time_t*);
size_t strftime (char*, size_t, const char*, const struct tm*);
struct tm* gmtime (const time_t*);
double difftime(time_t time2, time_t timel);
time_t mktime(struct tm *tp);


#endif
