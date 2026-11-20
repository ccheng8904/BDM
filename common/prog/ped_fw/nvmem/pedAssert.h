/*******************************************************************************
 *  Copyright (c) 2017 BSH Hausgeraete GmbH,
 *  Carl-Wery-Str. 34, 81739 Munich, Germany, www.bsh-group.de
 *
 *  All rights reserved. This program and the accompanying materials
 *  are protected by international copyright laws.
 *  Please contact copyright holder for licensing information.
 *
 *******************************************************************************
 *  PROJECT          ASSERT_LIB
 ******************************************************************************/

#ifndef ASSERT_H
#define ASSERT_H

#ifdef __cplusplus
extern "C" {
#endif

/******************************************************************************/
/* DOCUMENTATION                                                              */
/******************************************************************************/
/**
 *  \if Assert
 *  \file 
 *  Public declarations and definitions of Assert library
 *
 *  \mainpage Common used Functions and Definitions
 *  Only the macro \ref ASSERT_EXPR is exported by the assert library. Nevertheless, a set of configurations
 *  should be done by the user according to his needs before using the assert library. The following sections give more details  on how the assert library works and how to use it.
 *  \else
 *  \file 
 *  Public declarations and definitions of Assert library
 *  Only the macro \ref ASSERT_EXPR is exported by the assert library. Nevertheless, a set of configurations
 *  should be done by the user according to his needs before using the assert library. The following sections give more details  on how the assert library works and how to use it.
 *  \endif
 *  
 *  \section intro 1. Introduction
 *  The assert library is intended to be used in productive code with regard to following aspects:
 *     - During debugging, the assert functionality is switched on. Therefore all expressions ranges are checked and
 *       assertion failures are dumped.
 *     - For the release code, the assert functionality is switched off and the code size is less big.
 *  
 *  The assert library is provided as source code which should be compiled together with the program code intended to be debugged and productive.
 *  
 *  \section Interface 2. Interface of Assert Library
 *  The assert library only provides an assert macro \ref ASSERT_EXPR which checks the expression given and by default dumps file name and line number into a ring buffer in case the assertion fails.
 *  
 *  The user needs to set the following defines before compiling his project code using the assert macros:
 *     - \ref ASSERT_LOG_NUM
 *     - \ref ASSERT_MAXLEN_FILENAME
 *  
 *  Following (ring) buffers are used to buffer assert information in case of an assert:
 *     - \ref ASSERT_ucLogFileName [<index>][<filename>] for the file name where the asserts occured,
 *     - \ref ASSERT_u16LogLineNo  [<index>] for the line number within the previous file for the same index.\n\n
 *  \remark
 *     - These buffers must be controlled by the developer when debug is activated and assert is used into the code. 
 *     - The buffers are accessible also from a host PC via the TCL Suite (DBUS connection, test message server, set of DLLs), 
 *     - The code is considered 'assert clean' ONLY if the both buffers are EMPTY. Otherwise, developer must take appropriate 
 *       action to have the assert buffers empty.
 *  
 *  
 *  \section Usage 3. Usage of Assert Library
 *  Once the indicated settings in the previous section are done, the user must :
 *  
 *     - adapt the function \ref _ASSERT_vOnFailure in case it isn't suiting his needs\n
 *       (e.g set a led instead or what ever). By default a ring buffer is used.
 *  
 *     - include the file assert.h to your project.
 *     - make sure to add -DASSERT_INCLUDED to his compiler flags in the make file. No doing solets the assert macro expands in nothing, what is suitable for productive code.
 *     - call the assert macro anywhere in code where a critical expression needs a check.
 *     - ... and therefore reduce the number of control checks within the code, what saves memory :-)
 *  
 *  
 *  \section Make 4. How to adapt your make files and your make processes
 *  The concept of using the assert library is to provide a way to generate alway two ouput files:
 *     - a debug output containing the assert expressions
 *     - a release output free from assert expressions and thus reducing the code memory needed.\n\n
 *  
 *  To adapt your make file and upgrade your make process, followings must be done
 *  
 *  \subsection file 4.1. Makefile
 *  
 *  Before the including of Mac.inc, add or adapt followings at suitable places:
 *  
 *  \b Project \b Name
 *  
 *  \code
 *  
 *  PROJECT_ = <YouProjectName>
 *  
 *  .IF $(DEBUG) == 1
 *  PROJECT =$(PROJECT_)Debug
 *  .ELSE
 *  PROJECT =$(PROJECT_)
 *  .ENDIF
 *  \endcode
 *  
 *  \b Assert \b Paths
 *  
 *  \code
 *  ASSERTXS_PATH     = $(PROJECT_PATH)\..\Assert\Libxs
 *  ASSERT_PATH       = $(PROJECT_PATH)\..\Assert\Lib
 *  
 *  # when DERIVPATH is defined, Mac.inc sets SEARCH_PATH_ new, so fake.
 *  .IF $(DERIPATH) != $(NULL)
 *  PROG_PATH_COMMON += ;$(ASSERTXS_PATH);$(ASSERT_PATH)
 *  .ELSE
 *  SEARCH_PATH_ += ;$(ASSERTXS_PATH);$(ASSERT_PATH)
 *  .ENDIF
 *  \endcode
 *  
 *  \b Assert \b Compiler \b Switch
 *  
 *  \code
 *  # assert module (add compiler switch before including "Mak.inc")
 *  # **************************************************************
 *  .IF $(DEBUG) == 1
 *  DEFINES += -DASSERT_INCLUDED
 *  .ENDIF
 *  \endcode
 *  
 *  
 *  After including Mac.inc, add or adapt followings at suitable places:\n\n
 *  
 *     1- add a debug target to your make target list :
 *  
 *  \code
 *  default:  lib lint
 *  all:      cleanup dep lib lint 
 *  debug:    cleanup dep lib lint
 *  s19:      $(PROJECT).s19
 *  lint:    "$(PROG_PATH_DV)\lin$(PROJECT).lin"
 *  dep:     "$(PROG_PATH_DV)\$(PROJECT).dep"
 *  lib:     "$(LIB_PATH_DV)\$(PROJECT).lib"
 *  \endcode
 *  
 *     2- If the assert macro is used in application code using a debug library, link the debug and the release library files:
 *  
 *  \code
 *  .IF $(DEBUG) == 1
 *  LIB= $(LIB_PATH)$(LIB)Debug.lib
 *  .ELSE
 *  LIB= $(LIB_PATH)$(LIB).lib
 *  .ENDIF
 *  \endcode
 *  
 *     3- If the assert macro is used in library code or in application code not using a debug library, compile the user config file assertxs.c with the library or the application code:
 *  
 *  \code
 *  .IF $(DEBUG) == 1
 *  OBJ += $(ASSERTXS_PATH)\AssertXS.obj
 *  .ENDIF
 *  \endcode
 *  
 *  \subsection process 4.2. Make Processes (batch files)
 *  
 *     1- To compile with debug on command line, use : 
 *  
 *  \code
 *  mk32 -f <yourMakeFile> debug DEBUG=1
 *  \endcode
 *  
 *     2- Adapt the contents your makeAll.bat as follows:
 *  
 *  \code
 *  call make.bat clean
 *  call make.bat debug
 *  call make.bat clean
 *  call make.bat dep lib lint
 *  \endcode
 *  
    *  3- Adapt the contents of your make.bat as follows (R8C example is given, extend this to all groups):\n\n
 *  
 *  \code
 *  echo ************ make R8c (R8c1B variant) *********************
 *  cd R8C\R8C1B
 *  IF %1 == debug (
 *  echo ****************** debug **********************************
 *  mk32 -f %makefile%.mak %1 DEBUG=1
 *  ) ELSE (
 *  echo ****************** release ********************************
 *  call mk -f %makefile%.mak %1 %2 %3 %4
 *  )
 *  cd..\..
 *  \endcode
 *  
 *  \section Mixing 5. How to manage debug and released modules in a project (Proposal)
 *  
 *  Any project should have the freedom to decide which debug and release modules are to be used.\n\n
 *  One effective method to realise this would be to do a specific segragation in the project makefile
 *  as follows:
 *  
 *  1- Define 2 groups of object codes and libraries: one for debug, one for release
 *  \code
 *  # For object code to be generated
 *  OBJ_DEBUG   = dfile_1.dobj dfile_2.dobj dfile_n.dobj
 *  OBJ_RELEASE = rfile_1.obj rfile_2.obj rfile_n.obj
 *  OBJ         = $(OBJ_DEBUG) $(OBJ_RELEASE)
 *  
 *  # For provided libraries to be linked to the code
 *  
 *  LIB_DEBUG   = libdebug_1.lib libdebug_2.lib libdebug_n.lib
 *  LIB_RELEASE = librel_1.lib librel_2.lib librel_n.lib
 *  LIB         = $(LIB_DEBUG) $(LIB_RELEASE)
 *  \endcode
 *  
 *  2- Define a rule for debug object code and for release object code at compilation
 *  
 *  \code
 *  # Compiler switches for release code
 *  CDEF_REL += -DRELDEF_1 -DRELDEF_2 -DRELDEF_N
 *  
 *  # Compiler switches for debug code
 *  CDEF_DEBUG = $(CDEF_REL) -DDEBUGDEF_1 -DDEBUGDEF_2 -DDEBUGDEF_N 
 *  
 *  # Rule for debug code (as example)
 *  %.dobj: %.c
 *  +@%$(CC) $(CDEF_DEBUG) $< -o $(@:b).dobj
 *  
 *  # Rule for release code (as example)
 *  %.obj: %.c
 *  +@%$(CC) $(CDEF_REL) $< -o $(@:b).obj
 *  \endcode
 *  
 *  These two rules on makefile level allow the programmer to flexibly handle his release and debug modules
 *  without touching the code.
 */

/******************************************************************************/
/* INCLUDES                                                                   */
/******************************************************************************/
#ifdef ASSERT_INCLUDED  
#include "assertxs.h"
#endif

/******************************************************************************/
/* PUBLIC DEFINITIONS                                                         */
/******************************************************************************/

#ifdef ASSERT_INCLUDED  

/** Assert macro calling the \ref _ASSERT_vOnFailure function to log file name and line number for wrong assertions.
 *  \param bTrueExpression - boolean expression which should be verified.
 *  \remarks
 *     - -DASSERT_INCLUDED must be added to compiler switches to have the macro working properly in debug mode.
 *     - For productive code, -DASSERT_INCLUDED must be removed from the       
 *       compiler switches. The macros then expands in nothing.\n
 *  
 *  \n\b Example:
 *  
 *  Use following runtime debug check
 *  
 *  \code
 *  1)
 *    Call assert for following check:
 *    The valid range is always ucWeekDay <= 7, so in case ucWeekDay > 7,
 *    assertion will fail.
 *   ASSERT_EXPR(ucWeekDay <= 7); 
 *  \endcode
 *  
 *  instead of
 *  
 *  \code
 *  2)
 *  if (ucWeekDay > 7)
 *     return error_code;
 *  \endcode
 *  
 *  At debug time, you can check and eliminate causes leading to ucWeekDay > 7 and be aware of the coding in 2), what helps you save memory.
 */
#define ASSERT_vEXPR(param) ASSERT_EXPR((param))

#define ASSERT_EXPR(bTrueExpression) do {\
   if(!(bTrueExpression)) { \
      _ASSERT_vOnFailure ((const uint8_t*)(const void *)(__FILE__), __LINE__); \
   }} while (0)

#else
/** Assert macro empty when ASSERT_INCLUDED compiler switch is not defined */
#define ASSERT_EXPR(bTrueExpression) do {} while (0)
#endif

/******************************************************************************/
/* PUBLIC TYPE DEFINITIONS                                                    */
/******************************************************************************/
#ifdef ASSERT_INCLUDED
/** \brief 'Ring' Buffer to store the filename where assert occured.
 *  
 *  \param index_1 in range [0 - \ref ASSERT_LOG_NUM - 1] : Number of elements in the buffer.
 *  \param index_2 in range [0 - \ref ASSERT_MAXLEN_FILENAME] : String index of file name.\n\n
 *  \remark \ref ASSERT_LOG_NUM and \ref ASSERT_MAXLEN_FILENAME are user defined.
 */
extern uint8_t  ASSERT_ucLogFileName [ASSERT_LOG_NUM][ASSERT_MAXLEN_FILENAME];


/** \brief 'Ring' Buffer to store the line number where assert occured.
 *  
 *  \param index_1 in range 0 - \ref ASSERT_LOG_NUM - 1 : Number of elements in the buffer.
 *  
 *  \remark \ref ASSERT_LOG_NUM is user defined.
 */
extern uint16_t ASSERT_u16LogLineNo [ASSERT_LOG_NUM];
#endif


/******************************************************************************/
/* PUBLIC FUNCTION DECLARATIONS                                               */
/******************************************************************************/

/** \brief Callback function on assert expression error to log 
 *         information about failed assertion.
 *  
 *  \param pucFileName - Name of file in which expression failed.
 *  \param u16LineNo   - Line containing failing expression,
 *                       range from 0 to 0xFFFF. 
 *  \return
 *     none
 *  \remarks
 *     - Allthough a default implementation is provided, this function
 *       must be implemented in a project suitable way in AssertXS.c file.
 *     - This function must not be called by application code.
 *     - To avoid recursivity, function shouldn't call any other function. 
 */
extern void _ASSERT_vOnFailure (const uint8_t *pucFileName, uint16_t u16LineNo);

#ifdef __cplusplus
}
#endif

#endif
