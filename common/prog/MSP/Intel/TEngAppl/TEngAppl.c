/*******************************************************************************
*   Copyright (c) 2016 BSH Hausgeraete GmbH,
*   Carl-Wery-Str. 34, 81739 Munich, Germany, www.bsh-group.de
*
*   All rights reserved. This program and the accompanying materials
*   are protected by international copyright laws.
*   Please contact copyright holder for licensing information.
*
********************************************************************************
*   PROJECT          PED-Framework
*******************************************************************************/
#if  !defined(__analysis__)
#include <Python.h>

/*
This function builds the main of the Python Test Engine, which can be used for PC-Testing and HW-Testing both!
*/
#define MAX_ARGC 100U

int main(int argc, char *argv[])
{
    wchar_t *argv2[MAX_ARGC];
    for (int i=0; i < argc; i++)
    {
        argv2[i] = Py_DecodeLocale(argv[i], NULL);
    }

    Py_SetProgramName(argv2[0]);  /* optional but recommended */
    Py_Initialize();

    return Py_Main(argc, argv2);
}

#endif
