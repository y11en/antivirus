/*-----------------02.05.99 09:31-------------------
 * Project Prague                                   *
 * Subproject compiler dependent macro definitions *
 * Author Andrew Andy Petrovitch                    *
 * Copyright (c) Kaspersky Lab                      *
 * Purpose Prague API                               *
 *                                                  *
----------------------------------------------------*/

#ifndef __PR_COMPL_H
#define __PR_COMPL_H

#ifdef EXTERN_C
	#undef EXTERN_C
#endif

#if defined( __cplusplus )
  #define EXTERN_C      extern "C"
  #define EXTERN_C_BEG  extern "C" {
  #define EXTERN_C_END  }
#else
  #define EXTERN_C      extern
  #define EXTERN_C_BEG
  #define EXTERN_C_END
#endif

#if defined( __cplusplus )
  #if defined( _MSC_VER )
    #define pr_abstract  __declspec(novtable)
    #define pr_novtable  
  #else
    #define pr_abstract
    #define pr_novtable
  #endif
#else
  #define pr_abstract
  #define pr_novtable
#endif

#define VA_LIST_ADDR(a) (a)

#if ( (defined (__MC68K__))) 	// Palm OS
  #define pr_call
  #define __cdecl
  #define __stdcall
  #define __int64 long
  #undef _PRAGUE_TRACE_
  #define DO_NOT_USE_MEMUTIL
  #define PR_DO_NOT_USE_TRY
  #define cHANDLE_POOL     16
  #define cHANDLE_ENTRIES 64
  #define cINTERFACE_POOL  8
  #define cINTERFACE_ENTRIES 32

#elif defined (__unix__)		// most UNIXs
  #if defined (HAVE_CONFIG_H)
    #include <config-unix.h>
  #endif

  #define _GNU_SOURCE 1

 #if defined (__GNUC__) || defined (__GNUG__) 
/* #define memset __builtin_memset */
/* #define snprintf __builtin_snprintf */
/* #define vscanf  __builtin_vscanf  */
/* #define vsnprintf  __builtin_vsnprintf  */
/* #define vsscanf __builtin_vsscanf */
/* #define sqrtf __builtin_sqrtf */
/* #define sqrtl __builtin_sqrtl */
/* #define abs  __builtin_abs  */
/* #define exp  __builtin_exp  */
/* #define fabs  __builtin_fabs  */
/* #define fprintf __builtin_fprintf */
/* #define fputs  __builtin_fputs  */
/* #define labs  __builtin_labs  */
/* #define log  __builtin_log  */
/* #define memcmp  __builtin_memcmp  */
/* #define memcpy  __builtin_memcpy  */
/* #define memset  __builtin_memset  */
/* #define printf __builtin_printf */
/* #define putchar  __builtin_putchar  */
/* #define puts  __builtin_puts  */
/* #define scanf  __builtin_scanf  */
/* #define sin  __builtin_sin  */
/* #define snprintf  __builtin_snprintf  */
/* #define sprintf  __builtin_sprintf  */
/* #define sqrt __builtin_sqrt */
/* #define sscanf  __builtin_sscanf  */
/* #define strcat  __builtin_strcat  */
/* #define strchr  __builtin_strchr  */
/* #define strcmp  __builtin_strcmp  */
/* #define strcpy  __builtin_strcpy  */
/* #define strcspn  __builtin_strcspn  */
/* #define strlen __builtin_strlen */
/* #define strncat  __builtin_strncat  */
/* #define strncmp  __builtin_strncmp  */
/* #define strncpy  __builtin_strncpy  */
/* #define strpbrk  __builtin_strpbrk  */
/* #define strrchr  __builtin_strrchr  */
/* #define strspn __builtin_strspn */
/* #define strstr  __builtin_strstr  */
/* #define vprintf __builtin_vprintf */
/* #define vsprintf __builtin_vsprintf */
 #endif // __GNUC__ || __GNUG__ 

#include <unix/compatutils/compatutils.h>

#define PLUGIN_EXT "ppl"
#if defined (__APPLE__)
#define SHARED_EXT "dylib"
#else
#define SHARED_EXT "so"
#endif

#if defined (__i386__)
  #define pr_call __attribute__((cdecl))

  #ifndef __cdecl
    #define __cdecl    __attribute__((cdecl))
  #endif
  #ifndef __stdcall
    #define __stdcall  __attribute__((stdcall))
  #endif
  #ifndef __fastcall
    #define __fastcall 
  #endif
#else
  #define pr_call 
  #ifndef __cdecl
    #define __cdecl 
  #endif
  #ifndef __stdcall
    #define __stdcall
  #endif
  #ifndef __fastcall
    #define __fastcall 
  #endif
#endif

  #if !defined (MAX_PATH)
    #if defined (PATH_MAX)
      #define MAX_PATH  PATH_MAX
    #else
      #define MAX_PATH	8192	/* at least for ext2fs */
    #endif
  #endif

  // Need an additional check: on 64bit UNIXs "long" is already 64bit
  #define _INTEGRAL_MAX_BITS	64	/* kind of hack */
#if defined(__LP64__)
  #define __int64	long 
//  #define __time32_t	long
#else
  #define __int64	long long
//  #define __time32_t	long long
#endif
typedef int __time32_t;

  #ifndef TCHAR
    #define TCHAR char
    #define _TCHAR_DEFINED
  #endif

/*   #define _WCHAR_T_DEFINED */
/*   #include <wchar.h> */

  #define DO_NOT_USE_MEMUTIL
  #define PATH_DELIMETER "/"
  #define PATH_DELIMETER_WIDE L"/"
  #define PATH_DELIMETER_CHAR '/' 
  #define PATH_DELIMETER_WIDECHAR L'/' 
  #define PRINTF_LONGLONG "%lld"
  #define PRINTF_QWORD "%llu"
  #define PRINTF_QWORD_HEX "%llx"  
  #if defined(__LP64__)
	#define PRINTF_DATA "%llu"
  #else
	#define PRINTF_DATA "%lu"
  #endif
  #define PRINTF_QWORD_HEX_UP "%llX"
  #define DLL_IMPORT
  #define DLL_EXPORT
  #define LONG_LONG_CONST(val) (val##ll)
#elif defined(__midl)
  #define pr_call
  #define PR_RPC_DEFINE_OBJECT( type )  typedef tUINT h##type
#else
  #define pr_call __cdecl
#endif


#ifdef  __MWERKS__
  #include <string.h>
  #ifndef _WCHAR_T_DEFINED
    typedef unsigned short wchar_t;
    #define _WCHAR_T_DEFINED
  #endif

  #ifndef TCHAR
    #define TCHAR char
    #define _TCHAR_DEFINED
  #endif

  #define _asm asm
  #define _cdecl cdecl
  
  #undef  VA_LIST_ADDR
  #define VA_LIST_ADDR(a) (a[0])
  
#endif

#ifdef _MSC_VER // MS Visual C compiler
  #if !defined( _M_X64 ) && defined( _M_AMD64 ) 
       #define _M_X64
  #endif

  #if !defined( INT3 ) 
		#if defined( _DEBUG ) && !defined (_WIN32_WCE) && !defined(_M_X64) && !defined (_M_IA64)
			#define INT3 __asm int 3 
		#else
			#define INT3
		#endif
  #endif

  #define PR_EXPORT __declspec(dllexport)
  #define PATH_DELIMETER "\\"
  #define PATH_DELIMETER_WIDE L"\\"
  #define PATH_DELIMETER_CHAR '\\'
  #define PATH_DELIMETER_WIDECHAR L'\\' 
  #define PRINTF_LONGLONG "%I64i"
  #define PRINTF_QWORD "%I64u"
  #define PRINTF_QWORD_HEX "%I64x"
  #define PRINTF_QWORD_HEX_UP "%I64X"

	#if defined(_M_X64) || defined(_M_IA64)
		#define PRINTF_DATA "%I64u"
	#else
		#define PRINTF_DATA "%u"
	#endif

  #define DLL_IMPORT __declspec(dllimport)
  #define DLL_EXPORT __declspec(dllexport)
  #define LONG_LONG_CONST(val) (val)
	#if defined(__cplusplus)
		#if !defined(__PURE_CALL_DEFINED)
			#define __PURE_CALL_DEFINED
				extern "C" static int __cdecl _purecall(void)
				{
//					INT3;
					return 0x800000c2; // errOBJECT_NOT_INITIALIZED;
				}
		#endif // if !defined(__PURE_CALL_DEFINED)
	#endif // if defined(__cplusplus)
		

#else // !MS Visual C compiler
    #define PR_EXPORT
//  #define PR_EXPORT __declspec(dllexport)

#endif // end MS Visual C compiler


// kernel tuning
  // --- handle pool size
  #if !defined( cHANDLE_POOL )
    #define cHANDLE_POOL    0x80
    #define cHANDLE_ENTRIES 0x200
  #endif

  // --- interface pool size
  #if !defined( cINTERFACE_POOL )
    #define cINTERFACE_POOL    64
    #define cINTERFACE_ENTRIES 64
  #endif

#if !defined(__FILE__LINE__)
	#define _QUOTE(x) # x
	#define QUOTE(x) _QUOTE(x)

	// Use __FILE__LINE__ as a string containing "File.x(line#)" for example:
	//     #pragma message( __FILE__LINE__ "Remove this line after testing.")
	#define __FILE__LINE__ __FILE__ "(" QUOTE(__LINE__) ") : "
	// The above example will display the file name and line number in the build window
	// in such a way as to allow you to double click on it to go to the line.
#endif // __FILE__LINE__

#ifdef PUBLIC_BUILD
  #undef  INT3
#endif // PUBLIC_BUILD

#ifndef INT3
	#define INT3
#endif //INT3

#endif // __PR_COMPL_H
