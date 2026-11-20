/********************************************************************************
*   PROJECT          PP PED-Framework
*******************************************************************************/
#ifndef STDINT_H
#define STDINT_H


/******************************************************************************/
/* DOCUMENTATION                                                              */
/******************************************************************************/
/** \file stdint.h
 *     Standard C99 data types, independent of processor/compiler.
 *     This is defined for compilers without C99 support
 *     \b Note: Due to portability reasons it is not allowed to use C base types.
 */

/* Note 9024: pasting operator used in definition of function-like macro 'INT64_C' [MISRA 2012 Rule 20.10, advisory] */
/* Note 9026: function-like macro, 'INT8_C', defined [MISRA 2012 Directive 4.9, advisory] */
/*lint -esym(9024,*INT*_C) */
/*lint -esym(9026,*INT*_C) */

#if defined (UTEST_TYPE_SYSTEM_KEIL_ARM)

typedef unsigned char       uint8_t;
typedef unsigned short int  uint16_t;
typedef unsigned int        uint32_t;
typedef unsigned long long  uint64_t;

typedef signed char         int8_t;
typedef signed short int    int16_t;
typedef signed int          int32_t;
typedef signed long long    int64_t;

typedef signed char         int_least8_t;
typedef signed short        int_least16_t;
typedef signed int          int_least32_t;
typedef signed long long    int_least64_t;
typedef unsigned char       uint_least8_t;
typedef unsigned short      uint_least16_t;
typedef unsigned int        uint_least32_t;
typedef unsigned long long  uint_least64_t;

typedef signed int          int_fast8_t;
typedef signed int          int_fast16_t;
typedef signed int          int_fast32_t;
typedef signed long long    int_fast64_t;
typedef unsigned int        uint_fast8_t;
typedef unsigned int        uint_fast16_t;
typedef unsigned int        uint_fast32_t;
typedef unsigned long long  uint_fast64_t;


typedef  int32_t            intptr_t;
typedef uint32_t            uintptr_t;

#define INT8_C(x)   (x)
#define INT16_C(x)  (x)
#define INT32_C(x)  (x)
#define INT64_C(x)  (x ## LL)

#define UINT8_C(x)  (x ## U)
#define UINT16_C(x) (x ## U)
#define UINT32_C(x) (x ## U)
#define UINT64_C(x) (x ## ULL)

#define SIZE_MAX    UINT32_MAX 

#elif defined (UTEST_TYPE_SYSTEM_Windows32)

typedef signed char int8_t;
typedef unsigned char   uint8_t;
typedef short  int16_t;
typedef unsigned short  uint16_t;
typedef int  int32_t;
typedef unsigned   uint32_t;
typedef long long  int64_t;
typedef unsigned long long   uint64_t;

typedef signed char         int_least8_t;
typedef signed short        int_least16_t;
typedef signed int          int_least32_t;
typedef signed long long    int_least64_t;
typedef unsigned char       uint_least8_t;
typedef unsigned short      uint_least16_t;
typedef unsigned int        uint_least32_t;
typedef unsigned long long  uint_least64_t;

typedef signed int          int_fast8_t;
typedef signed int          int_fast16_t;
typedef signed int          int_fast32_t;
typedef signed long long    int_fast64_t;
typedef unsigned int        uint_fast8_t;
typedef unsigned int        uint_fast16_t;
typedef unsigned int        uint_fast32_t;
typedef unsigned long long  uint_fast64_t;

typedef signed int intptr_t;
typedef unsigned int uintptr_t;

#define INT8_C(x)   (x)
#define INT16_C(x)  (x)
#define INT32_C(x)  (x)
#define INT64_C(x)  (x ## LL)

#define UINT8_C(x)  (x ## U)
#define UINT16_C(x) (x ## U)
#define UINT32_C(x) (x ## U)
#define UINT64_C(x) (x ## ULL)

#define SIZE_MAX    UINT32_MAX

#elif defined (UTEST_TYPE_SYSTEM_RENESAS_RX)

typedef unsigned char       uint8_t;
typedef unsigned short int  uint16_t;
typedef unsigned int        uint32_t;
typedef unsigned long long  uint64_t;

typedef signed char         int8_t;
typedef signed short int    int16_t;
typedef signed int          int32_t;
typedef signed long long    int64_t;

typedef signed int          int_least8_t;
typedef signed int          int_least16_t;
typedef signed int          int_least32_t;
typedef signed long long    int_least64_t;
typedef unsigned int        uint_least8_t;
typedef unsigned int        uint_least16_t;
typedef unsigned int        uint_least32_t;
typedef unsigned long long  uint_least64_t;

typedef signed int          int_fast8_t;
typedef signed int          int_fast16_t;
typedef signed int          int_fast32_t;
typedef signed long long    int_fast64_t;
typedef unsigned int        uint_fast8_t;
typedef unsigned int        uint_fast16_t;
typedef unsigned int        uint_fast32_t;
typedef unsigned long long  uint_fast64_t;

typedef signed long int intptr_t;
typedef unsigned long int uintptr_t;

#define INT8_C(x)   (x)
#define INT16_C(x)  (x)
#define INT32_C(x)  (x ## L)
#define INT64_C(x)  (x ## LL)

#define UINT8_C(x)  (x ## U)
#define UINT16_C(x) (x ## U)
#define UINT32_C(x) (x ## UL)
#define UINT64_C(x) (x ## ULL)

#define SIZE_MAX    UINT32_MAX

#endif



#define INT8_MAX   127
#define INT8_MIN  (-INT8_MAX -1)
#define UINT8_MAX  255U

#define INT16_MAX   32767
#define INT16_MIN  (-INT16_MAX -1)
#define UINT16_MAX  65535U

#define INT32_MAX    2147483647L
#define INT32_MIN   (-INT32_MAX -1L)
#define UINT32_MAX   4294967295UL


#endif  // STDINT_H
