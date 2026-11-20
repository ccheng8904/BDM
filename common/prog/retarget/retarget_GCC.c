/*******************************************************************************
 *  Copyright (c) 2017 BSH Hausgeraete GmbH,
 *  Carl-Wery-Str. 34, 81739 Munich, Germany, www.bsh-group.de
 *
 *  All rights reserved. This program and the accompanying materials
 *  are protected by international copyright laws.
 *  Please contact copyright holder for licensing information.
 *
 *******************************************************************************
 *  PROJECT          Generic SW
 ******************************************************************************/

#if defined(__GNUC__) && !defined(__NOT_CC__)

/******************************************************************************/
/* DOCUMENTATION                                                              */
/******************************************************************************/
/** \file
 *
 *  \ingroup  retarget
 *
 *  \brief    Function definitions for GCC system calls retargeting.
 *
 *  \details  
 */


/******************************************************************************/
/* INCLUDES                                                                   */
/******************************************************************************/

#include "retarget.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/fcntl.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <sys/wait.h>

#undef errno
extern int errno;

extern const uint32_t end;
extern const uint32_t __HeapLimit;


/******************************************************************************/
/* RETARGET FUNCTION DEFINITONS                                               */
/******************************************************************************/


__attribute__((weak)) int _write(int fd, const char *buffer, int length)
{
    int index = 0;
    (void)fd;

    if (buffer != NULL)
    {
        for (; index < length; index++)
        {
            RTG_writeChar(buffer[index]);
        }
    }
    return (index);
}


__attribute__((weak)) int _read(int fd, char *buffer, int length)
{
    int index = 0;
    (void)fd;

    if (buffer != NULL)
    {
        for(; index < length; ++index)
        {
            *buffer = (char)RTG_readChar();
            
            if(*buffer == '\n')
            {
                break;
            }
            
            buffer++;
        }
    }
    return (index);
}


__attribute__((weak)) void __assert_func(const char *expr, int line, const char *file)
{
    RTG_assert(expr, file, line);
    while(1);
}


__attribute__((weak)) caddr_t _sbrk(int incr) 
{
    static char* heapEnd;
    char* heapEndOld;

    if (heapEnd == 0) {
        heapEnd = (char*)(&end);
    }

    heapEndOld = heapEnd;

    if(((int32_t)heapEnd + incr) > (int32_t)(&__HeapLimit))
    {
        errno = ENOMEM;
        return (caddr_t) -1;
    }

    heapEnd += incr;

    return (caddr_t) heapEndOld;
}


__attribute__((weak)) void initialise_monitor_handles() 
{
}


__attribute__((weak)) int _getpid(void) {
    return 1;
}


__attribute__((weak)) int _kill(int pid, int sig) 
{
    (void)pid;
    (void)sig;
    errno = EINVAL;
    return -1;
}


__attribute__((weak)) void _exit (int status) 
{
    _kill(status, -1);
    while(1)
    {
        /* endless loop */
    }
}


__attribute__((weak)) int _close(int file) 
{
    (void)file;
    return -1;
}


__attribute__((weak)) int _fstat(int file, struct stat* st) 
{
    (void)file;
    st->st_mode = S_IFCHR;
    return 0;
}


__attribute__((weak)) int _isatty(int file)
{
    (void)file;
    return 1;
}


__attribute__((weak)) int _lseek(int file, int ptr, int dir)
{
    (void)file;
    (void)dir;
    return 0;
}


__attribute__((weak)) int _open(char* path, int flags, ...)
{
    (void)path;
    (void)flags;
    return -1;
}


__attribute__((weak)) int _wait(int* status)
{
    (void)status;
    errno = ECHILD;
    return -1;
}


__attribute__((weak)) int _unlink(char* name)
{
    (void)name;
    errno = ENOENT;
    return -1;
}


__attribute__((weak)) int _stat(char* file, struct stat* st)
{
    (void)file;
    st->st_mode = S_IFCHR;
    return 0;
}


__attribute__((weak)) int _link(char* old, char* new)
{
    (void)old;
    (void)new;
    errno = EMLINK;
    return -1;
}


__attribute__((weak)) int _fork(void)
{
    errno = EAGAIN;
    return -1;
}


__attribute__((weak)) int _execve(char* name, char** argv, char** env)
{
    errno = ENOMEM;
    return -1;
}

#endif // defined(__GNUC__)
