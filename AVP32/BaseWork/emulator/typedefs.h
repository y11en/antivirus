/*
 *  typedefs.h,v 1.2 2002/10/17 09:55:03 tim 
 */

#ifndef TYPEDEFS_H
#define TYPEDEFS_H

#include <string>
#include <vector>
#include <map>

#include <ctype.h>
#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <limits.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <dlfcn.h>
#include <signal.h>
#include <time.h>

#include <avpport.h>
#include <compat_unix.h>
#include <byteorder.h>


/* 
 * Additional type definitions for cross-platform compatibility
 */
typedef signed int		SDWORD;
typedef signed short 	SWORD;
typedef signed char 	SBYTE;

#if defined (WIN32) 
	#define vsnprintf 			_vsnprintf
	typedef unsigned __int64	QWORD;
	typedef signed __int64		SQWORD;
#elif SIZEOFLONG == 8
	typedef unsigned long		QWORD;
	typedef signed long			SQWORD;
#else
	typedef unsigned long long	QWORD;
	typedef signed long long	SQWORD;
#endif

/*


	#define vsnprintf _vsnprintf
	#if !defined (SKIP_ENGINE_PARTS)
		typedef unsigned long 		DWORD;	
		typedef unsigned short 		WORD;
		typedef unsigned char 		BYTE;
		typedef unsigned int		BOOL;
	#endif


#elif defined (hpux) || defined (sparc)

	#if !defined (SKIP_ENGINE_PARTS)
		typedef unsigned int 	DWORD;	
		typedef unsigned short 	WORD;
		typedef unsigned char 	BYTE;
		typedef unsigned int	BOOL;
	#endif


	typedef signed int			SDWORD;	
	typedef signed short 		SWORD;
	typedef signed char 		SBYTE;

#else
	#error Unsupported OS! Please define your OS as -D<OSNAME>
#endif
*/


#if defined (__cplusplus)
inline void SET_BIT (DWORD& value, DWORD flag, int is_set )
{
	if ( is_set )
		value |= flag;
	else
		value &= (~flag);
}


inline int IS_SET (DWORD value, DWORD flag)
{
	return (value&flag);
}


inline void SET_BIT (WORD& value, WORD flag, int is_set )
{
	if ( is_set )
		value |= flag;
	else
		value &= (~flag);
}


inline int IS_SET (WORD value, WORD flag)
{
	return (value&flag);
}
#endif /* __cplusplus */

/*
#if !defined (MACHINE_IS_LITTLE_ENDIAN) && !defined (MACHINE_IS_BIG_ENDIAN)
#error Unknown byte order! Use MACHINE_IS_BIG_ENDIAN or MACHINE_IS_LITTLE_ENDIAN
#endif

#if defined (MACHINE_IS_LITTLE_ENDIAN) && defined (MACHINE_IS_BIG_ENDIAN)
#error Ambiguous byte order, both MACHINE_IS_BIG_ENDIAN and MACHINE_IS_LITTLE_ENDIAN are defined!
#endif

#if defined (MACHINE_IS_LITTLE_ENDIAN)
	#include "byteorder-little-endian.h"
#else
	#include "byteorder-big-endian.h"
#endif
*/


#define READ_X86_BYTE(ptr)			*(ptr)
#define READ_X86_WORD(ptr)			ReadWordPtr(ptr)
#define READ_X86_DWORD(ptr)			ReadDWordPtr(ptr)

#define WRITE_X86_BYTE(ptr,data)	*(ptr) = data
#define WRITE_X86_WORD(ptr,data)	WriteWordPtr(ptr, data)
#define WRITE_X86_DWORD(ptr,data)	WriteDWordPtr(ptr, data)


inline BYTE		READ_X86_DATA (BYTE *ptr)					{	return READ_X86_BYTE(ptr);	}
inline WORD		READ_X86_DATA (WORD *ptr)					{	return READ_X86_WORD(ptr);	}
inline DWORD	READ_X86_DATA (DWORD *ptr)					{	return READ_X86_DWORD(ptr);	}
inline SBYTE	READ_X86_SIGNED_DATA (BYTE *ptr)			{	return (SBYTE) READ_X86_BYTE(ptr);		}
inline SWORD	READ_X86_SIGNED_DATA (WORD *ptr)			{	return (SWORD) READ_X86_WORD(ptr);		}
inline SDWORD	READ_X86_SIGNED_DATA (DWORD *ptr)			{	return (SDWORD) READ_X86_DWORD(ptr);	}
inline void		WRITE_X86_DATA (BYTE *ptr, BYTE value)		{	WRITE_X86_BYTE(ptr, value);				}
inline void		WRITE_X86_DATA (WORD *ptr, WORD value)		{	WRITE_X86_WORD(ptr, value);				}
inline void		WRITE_X86_DATA (DWORD *ptr, DWORD value)	{	WRITE_X86_DWORD(ptr, value);			}


#include "wrappers.h"
#include "i386disasm.h"
#include "i386regs.h"
#include "i386cpu.h"
#include "i386exception.h"
#include "cmd_stack.h"
#include "symboltable.h"
#include "coffexception.h"
#include "emulator.h"
#include "symboltable.h"

#include "elfbaseformat.h"
#include "elf-format.h"
#include "elfparser.h"
#include "elfbases.h"

#endif /* TYPEDEFS_H */
