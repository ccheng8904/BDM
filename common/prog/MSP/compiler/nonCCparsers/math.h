/********************************************************************************
*   PROJECT          PP PED-Framework
*******************************************************************************/
#ifndef MATH_H
#define MATH_H


/******************************************************************************/
/* DOCUMENTATION                                                              */
/******************************************************************************/
/** \file
*
*   \ingroup  PEDF
*
*   \brief    ANSII conforming declarations of standard math library stuff.
*
*   \details  This file exists to provide code analyzer tools
*             with standard library declarations in ANSII conforming form
*             (to inhibit unnecessary  error messages).
*
*/



/******************************************************************************/
/* INCLUDES                                                                   */
/******************************************************************************/
#include <stdint.h>



extern double sin (double);
extern double cos (double);
extern double tan (double);
extern double sinh (double);
extern double cosh (double);
extern double tanh (double);
extern double asin (double);
extern double acos (double);
extern double atan (double);
extern double atan2 (double, double);
extern double exp (double);
extern double log (double);
extern double log10 (double);
extern double pow (double, double);
extern double sqrt (double);
extern double ceil (double);
extern double floor (double);
extern double fabs (double);
extern double ldexp (double, int);
extern double frexp (double, int*);
extern double modf (double, double*);
extern double fmod (double, double);



extern float sin (float);              //lint !e18 // suppress redeclaration error
extern float cos (float);              //lint !e18 // suppress redeclaration error
extern float tan (float);              //lint !e18 // suppress redeclaration error
extern float sinh (float);             //lint !e18 // suppress redeclaration error
extern float cosh (float);             //lint !e18 // suppress redeclaration error
extern float tanh (float);             //lint !e18 // suppress redeclaration error
extern float asin (float);             //lint !e18 // suppress redeclaration error
extern float acos (float);             //lint !e18 // suppress redeclaration error
extern float atan (float);             //lint !e18 // suppress redeclaration error
extern float atan2 (float, float);     //lint !e18 // suppress redeclaration error
extern float exp (float);              //lint !e18 // suppress redeclaration error
extern float log (float);              //lint !e18 // suppress redeclaration error
extern float log10 (float);            //lint !e18 // suppress redeclaration error
extern float pow (float, float);       //lint !e18 // suppress redeclaration error
extern float sqrt (float);             //lint !e18 // suppress redeclaration error
extern float ceil (float);             //lint !e18 // suppress redeclaration error
extern float floor (float);            //lint !e18 // suppress redeclaration error
extern float fabs (float);             //lint !e18 // suppress redeclaration error
extern float ldexp (float, int);       //lint !e18 // suppress redeclaration error
extern float frexp (float, int*);      //lint !e18 // suppress redeclaration error
extern float modf (float, float*);     //lint !e18 // suppress redeclaration error
extern float fmod (float, float);      //lint !e18 // suppress redeclaration error

#endif // MATH_H
