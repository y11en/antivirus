
#if !defined(__AVPPort_h__) 
#define __AVPPort_h__

#ifdef _WIN32_WCE	// Pocket PC
	#define _ASSERT(a)
	#define _ASSERTE(a)
	#define _RPT0(a, b)
	#define _RPT1(a, b, c)
	#define _RPT2(a, b, c, d)
	#define _RPT3(a, b, c, d, e)
	#define _RPT4(a, b, c, d, e, f)
	//
#endif


#ifndef _WCHAR_T_DEFINED
	#define _WCHAR_T_DEFINED
	#if defined (__unix__) && defined (__cplusplus) && defined (HAS_BUILTIN_WCHAR_T)
		// GCC 3.x has built-in wchar_t type in C++ mode (but not in C mode).
		// So we have to declare wchar_t while in C mode, but not in C++.
	#else
		typedef unsigned short wchar_t;
	#endif /* HAS_BUILTIN_WCHAR_T */
#endif /* _WCHAR_T_DEFINED */


#if !defined (_SIZE_T_DEFINED_) && !defined (__unix__)
	#define _SIZE_T_DEFINED_
		
	#if !defined(_SIZE_T)
		#define _SIZE_T
		typedef unsigned size_t;
	#endif
#endif


#ifndef USHRT_MAX
	#define USHRT_MAX 0xffff
#endif


#ifdef __MC68K__	// Palm OS
	#define PILOT_PRECOMPILED_HEADERS_OFF
	#include <pilot.h>
	#define __cdecl
	typedef unsigned int size_t;
	#define _RPT0(a, b)
	#define _RPT1(a, b, c)
	#define _RPT2(a, b, c, d)
	#define _RPT3(a, b, c, d, e)
	#define _RPT4(a, b, c, d, e, f)

	#define _ASSERT(a)
	#define _ASSERTE(a)

	#define memcpy MemMove
	#define memset MemSet
	#define strlen StrLen
	#define strcpy StrCopy

#elif defined (__unix__)

  	#include <sys/types.h>
  	#include <string.h>

	#ifdef __cplusplus

		extern "C" {
	#endif
	
	// Declare unicode function prototypes (defined in libproperty)
	size_t wcslen (const wchar_t *string);
	wchar_t * wcscpy (wchar_t *strDestination, const wchar_t *strSource);
	int wcscmp (const wchar_t *string1, const wchar_t *string2);
 	
	#ifdef __cplusplus
		}
	#endif

	// TCHAR stuff
	#if !defined (_TCHAR_DEFINED)
		#define _TCHAR_DEFINED
		typedef char TCHAR;
	#endif

	#define _tcscat     	strcat
	#define _tcscpy     	strcpy
	#define _tcsdup     	strdup
	#define _tcsclen    	strlen
	#define	_tcsinc(p)		(p+1)
	#define	_tcsnextc(p)	((unsigned int ) p[1])	
	

	/* Hide all calling conversion macroses */
	#undef _cdecl
	#undef __cdecl
	#undef _stdcall
	#undef __stdcall
	#undef _fastcall
	#undef __fastcall
	#undef pascal
	#undef WINAPI

	#define _cdecl
	#define __cdecl
	#define _stdcall
	#define __stdcall
	#define _fastcall
	#define __fastcall
	#define pascal
	#define WINAPI

	/* Hide all pointer-size macroses */
	#undef	NEAR
	#undef	FAR
	#undef	far
	#undef	near

	#define	NEAR
	#define	FAR
	#define	far
	#define	near
	
#elif defined( M_UTILS )
	#include <m_utils.h>
#else

//  [7/5/2002] SHOULD BE REVISED Petrovitch, Graf.
/*
	#include <string.h>

	#ifdef __cplusplus
		extern "C" {
	#endif
    	size_t wcslen( const wchar_t *string );
     	wchar_t *wcscpy( wchar_t *strDestination, const wchar_t *strSource );
	#ifdef __cplusplus
		}
	#endif
*/ 
#endif


#if !((defined(__WATCOMC__) && (defined(__DOS__)||defined(__OS2__))) || (defined (__unix__))) && !defined(__MWERKS__)

	#ifndef _WIN32_WCE	// Pocket PC
		#include <crtdbg.h>
		#include <minmax.h>
	#endif
   
#else /* !((defined(__WATCOMC__) && (defined(__DOS__)||defined(__OS2__))) || (defined (__unix__))) && !defined(__MWERKS__) */

	#define _RPT0(a, b)
	#define _RPT1(a, b, c)
	#define _RPT2(a, b, c, d)
	#define _RPT3(a, b, c, d, e)
	#define _RPT4(a, b, c, d, e, f)

	#define _ASSERT(a)
	#define _ASSERTE(a)

	#define max(a,b)            (((a) > (b)) ? (a) : (b))
	#define min(a,b)            (((a) < (b)) ? (a) : (b))

	/*
	   Section 3:  Other constants
	   Section 3a: NULL
	*/

	#if !defined(NULL) && defined(__cplusplus)
		#define NULL   0
	#endif

	#if defined( N_RC_INVOKED )
		#define NULL   0
	#endif

	#if !defined NULL
		#if defined N_MSC
			#define NULL ((void *) 0)
	#elif defined(M_I86S) || defined(M_I86SM) || defined(M_I86C) || \
          defined(M_I86CM) || \
          defined(__TINY__) || defined(__SMALL__) || defined(__MEDIUM__) || \
          defined(N_PLAT_NLM)
         #define NULL   0
      #else
         #define NULL   0L
      #endif
	#endif /* NULL */
#endif //!((defined(__WATCOMC__) && (defined(__DOS__)||defined(__OS2__))) || (defined (__unix__)))
#endif /* __AVPPort_h__ */





