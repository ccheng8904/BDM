/*
File Name        : vcast_test_support.h
Date of creation : 06.11.2020
Author Name/Dept : UAT Team, RBEI
Description      :VectorCast tool related file;Used to replace the const variable with null for Vectorcast testing
*/

#ifndef VCAST_TEST_SUPPORT_H
#define VCAST_TEST_SUPPORT_H

#ifndef VCAST_TEST
#define  T_CONST  const    
#else
#define  T_CONST        
#endif

#endif
