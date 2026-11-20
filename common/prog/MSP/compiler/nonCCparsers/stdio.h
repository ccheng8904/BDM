/********************************************************************************
*   PROJECT          PP PED-Framework
*   AUTHOR           Jennifer Deepa
*   CREATED          08-09-2017
*******************************************************************************/
#ifndef STDIO_H
#define STDIO_H

/******************************************************************************/
/* DOCUMENTATION                                                              */
/******************************************************************************/
/** \file
 *     This file exists to provide code analyser tools with standard library
 *     declarations in ANSII conforming form
 *     (to inhibit unnecessary  error messages).
 */

/* Switch off:
 * Note 955: parameter XY of forward declaration of XYZ lacks a name [MISRA 2012 Rule 8.2, mandatory]
 * Note 970: use of modifier or type 'XY' outside of a typedef [MISRA 2012 Directive 4.6, required]
 * Note 9046: 'FILE' is typographically ambiguous with respect to 'file' when ignoring case [MISRA 2012 Directive 4.5, advisory]
 */
/*lint -save -e955 -e970 -e9046 */

/* Flags for the iobuf structure  */
#define _IOREAD 1 /* currently reading */
#define _IOWRT  2 /* currently writing */
#define _IORW   0x0080 /* opened as "r+w" */


/*
 * The three standard file pointers provided by the run time library.
 * NOTE: These will go to the bit-bucket silently in GUI applications!
 */
#define STDIN_FILENO    0
#define STDOUT_FILENO   1
#define STDERR_FILENO   2

/* Returned by various functions on end of file condition or error. */
#define EOF (-1)

/*
 * The maximum length of a file name. You should use GetVolumeInformation
 * instead of this constant. But hey, this works.
 * Also defined in io.h.
 */
#ifndef FILENAME_MAX
#define FILENAME_MAX    (260)
#endif

/*
 * The maximum number of files that may be open at once. I have set this to
 * a conservative number. The actual value may be higher.
 */
#define FOPEN_MAX   (20)

/* After creating this many names, tmpnam and tmpfile return NULL */
#define TMP_MAX 32767

/* Constants for nOrigin indicating the position relative to which fseek
 * sets the file position.  Defined unconditionally since ISO and POSIX
 * say they are defined here.  */
#define SEEK_SET 0
#define SEEK_CUR 1
#define SEEK_END 2

#ifndef _FILE_DEFINED
#define _FILE_DEFINED
typedef long	fpos_t;

typedef struct _iobuf
{
    char*   _ptr;
    int _cnt;
    char*   _base;
    int _flag;
    int _file;
    int _charbuf;
    int _bufsiz;
    char*   _tmpfname;
} FILE;
#endif  /* Not _FILE_DEFINED */


extern FILE (*_imp___iob)[];    /* A pointer to an array of FILE */

#define _iob    (*_imp___iob)   /* An array of FILE */



int snprintf ( char * buffer, size_t size, const char * format, ... );



/*
 * File Operations
 */
FILE* fopen (const char*, const char*);
int fclose (FILE*);

extern int (fprintf)(FILE*, const char*, ...);

/*
 * Character Input and Output Functions
 */

int     fgetc (FILE*);
char*   fgets (char*, int, FILE*);
int     fputc (int, FILE*);
int     fputs (const char*, FILE*);
char*   gets (char*);
int puts (const char*);
int     ungetc (int, FILE*);
int fgetpos(FILE *, fpos_t*);
int remove(const char *);
int rename(const char *, const char *);
int perror(const char *);
int getchar(void);
int feof (FILE*);
int fsetpos (FILE*, const fpos_t*);

/*
 * File Positioning Functions
 */

 int    fseek (FILE*, long, int);
 long   ftell (FILE*);
 void   rewind (FILE*);
#endif

/*lint -restore -e955 -e970 -e9046 */
