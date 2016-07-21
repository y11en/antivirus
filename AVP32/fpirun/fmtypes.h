/****************************************************************************\
  MODULE........: FMTYPES.H | This module contains common type declarations.
  AUTHOR........: Alex Starkov (AS)
  COPYRIGHT.....: Copyright (c) 1997, Data Fellows Ltd.
  VERSION.......: 1.00
  HISTORY.......: DATE     COMMENT
                  -------- ------------------------------------------------
                  03-01-97 Created.
                  16-02-01 turbpa2001-02-16[scr-14191] - UNICODE support
                  16-07-01 turbpa2001-07-16 - remove warnings on Linux platform
                  29-10-01 turbpa2001-10-29#18488 Problematic file causes scan timeout
\****************************************************************************/

/*===========================================[ REDEFINITION DEFENCE ]=======*/
#ifndef FMTYPES_H
#define FMTYPES_H

/*===========================================[ SPECIAL ]====================*/
/*===========================================[ IMPORT DECLARATIONS ]========*/
/*===========================================[ C++ DEFENCE ]================*/
/*===========================================[ PUBLIC CONSTANTS ]===========*/

#ifdef VTOOLSD // define some types here which are otherwise missing in the VTOOLSD environment
#  define EOF (-1)
#endif //VTOOLSD

#ifndef TRUE
#  define TRUE  1
#endif

#ifndef FALSE
#  define FALSE 0
#endif

#ifndef VTOOLSD // this is defined in vtoolsc.h (without redefinition defence)
                // we get macro redefinition warnings if this is not excluded
#  ifndef NULL
#    define NULL  0
#  endif
#endif // !VTOOLSD

#if defined(UNIX) || defined(_SYS_TYPES_H)
#  include <sys/types.h>
#  include <linux/limits.h>
#  define MAX_PATH PATH_MAX
#endif

#ifndef MAX_PATH
#  if defined __TURBOC__ && !defined __WIN32__
#    define MAX_PATH  255
#  else
#    define MAX_PATH  260
#  endif
#endif

#ifdef WIN32
//turbpa2001-10-29#18488{
//#  define INBUFSIZ  65536  // this is for buffered file reading (32-bit)
#  define INBUFSIZ  16384  
//turbpa2001-10-29#18488}
#else
#  define INBUFSIZ  16384  // this is for buffered file reading (16-bit)
#endif
/* what should be the linux read size ????? */

/*===========================================[ PUBLIC TYPES ]===============*/

#ifndef OS2
typedef int BOOL;
#endif

typedef unsigned char byte;

#if !defined(UNIX) || !defined(__USE_MISC)
    typedef unsigned long ulong;
    typedef unsigned short ushort;
#endif

// @type HFMFILE | External file handle.
typedef int HFMFILE;
// @type FMCOMPONENTTYPE | Type of the component file.
typedef	unsigned int FMCOMPONENTTYPE;

//turbpa2001-02-16[scr-14191]{
// add target definintion where unicode is valid
#if defined( WIN32 ) ||  defined(NT_DRIVER)
	#define FMUNICODE_SUPPORT
#endif

#ifdef FMUNICODE_SUPPORT
// declare WCHAR if not declared
#ifndef _WCHAR_T_DEFINED
	typedef unsigned short wchar_t;
	#define _WCHAR_T_DEFINED
#endif
	// declare private WCHAR as for different env,
	// wchar_t can be defined but WCHAR not
	// it is impossible to controll this definition
	typedef wchar_t FMWCHAR;
#else
#endif
//turbpa2001-02-16[scr-14191]}
/*===========================================[ PUBLIC VARIABLES ]===========*/
/*===========================================[ PUBLIC FUNCTIONS ]===========*/
/*===========================================[ PSEUDO/INLINE FUNCTIONS ]====*/
/*===========================================[ END C++ DEFENCE ]============*/
/*===========================================[ END REDEFINITION DEFENCE ]===*/
#endif/*FMTYPES_H*/

/** (END OF FILE  : fmtypes.h) **********************************************/
