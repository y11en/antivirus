/*!
 * (C) 2002 "Kaspersky Lab"
 *
 * \file kldefs.h
 * \author Андрей Казачков
 * \date 2002
 * \brief Переносимые макросы
 *
 */



#ifndef KLDEFS_H_AAE8D350_6234_4a8a_A525_56B7D38B5A65
#define KLDEFS_H_AAE8D350_6234_4a8a_A525_56B7D38B5A65


#if defined(__unix) || defined(N_PLAT_NLM) 
#include "std/conv/wcharcrt.h"
#define _wcsicmp wcsicmp
#endif

#ifdef _WIN32
#include <malloc.h>
#endif
#include <stdlib.h>
#include <string.h>

#include <avp/avp_data.h>

#ifdef _WIN32
# include <crtdbg.h>
# define KLSTD_ASSERT(x)		_ASSERTE(x)
#else
# include <assert.h>
# define KLSTD_ASSERT(x)		assert(x)
#endif

#define KLSTD_DECLARE_HANDLE(name)	\
		typedef struct name##__{int unused; }*name;

/*!
	\brief Optimization macroses
*/

#ifdef _MSC_VER
# define KLSTD_NOTHROW		__declspec(nothrow)
# define KLSTD_INLINEONLY	__forceinline
# define KLSTD_FASTCALL		__fastcall
# define KLSTD_NOVTABLE		__declspec(novtable)
# define KLSTD_NORETURN		__declspec(noreturn)
#else
# define KLSTD_NOTHROW
# define KLSTD_INLINEONLY	inline
# define KLSTD_FASTCALL
# define KLSTD_NOVTABLE
# define KLSTD_NORETURN
#endif

// local function declaration
#define KLCS_DECL

#if defined(_MSC_VER) && !defined(KLCSC_STATIC)
# define KLSTD_DLLIMPORT		__declspec(dllimport)
# define KLSTD_DLLEXPORT		__declspec(dllexport)
#else
# define KLSTD_DLLIMPORT
# define KLSTD_DLLEXPORT
#endif

#if defined(_MSC_VER)
    #define KLSTD_TYPENAME
#else
    #define KLSTD_TYPENAME typename
#endif

/*!
	\brief Declarations specifiers for DLL's
*/ 

#if defined(KLCSC_EXPORTS)
# define KLCSC_DECL KLSTD_DLLEXPORT
#else
# define KLCSC_DECL KLSTD_DLLIMPORT
#endif

#if defined(KLCSP_EXPORTS)
# define KLCSP_DECL KLSTD_DLLEXPORT
#else
# define KLCSP_DECL KLSTD_DLLIMPORT
#endif

#ifdef KLCSA_EXPORTS
# ifndef _LIB
#  define KLCSA_DECL KLSTD_DLLEXPORT
# else
#  define KLCSA_DECL
# endif
#else
# ifndef KLCSA_STATIC
#  define KLCSA_DECL KLSTD_DLLIMPORT
# else
#  define KLCSA_DECL
# endif
#endif

	// Чтобы можно было включать в другие проекты по файлам
#ifdef KLCSC_STATIC
# undef KLCSC_DECL
# define KLCSC_DECL
#endif

#ifdef KLCSC_STATIC
# undef KLCSA_DECL
# define KLCSA_DECL
#endif


#ifdef KLCSS_EXPORTS
# define KLCSS_DECL KLSTD_DLLEXPORT
#else
# define KLCSS_DECL KLSTD_DLLIMPORT
#endif

#ifdef KLCSTR_EXPORTS
# define KLCSTR_DECL KLSTD_DLLEXPORT
#else
# define KLCSTR_DECL KLSTD_DLLIMPORT
#endif

#ifdef KLCSKCA_EXPORTS
# define KLCSKCA_DECL KLSTD_DLLEXPORT
#else
# define KLCSKCA_DECL KLSTD_DLLIMPORT
#endif

#ifdef KLCSNAGT_EXPORTS
# define KLCSNAGT_DECL KLSTD_DLLEXPORT
#else
# define KLCSNAGT_DECL KLSTD_DLLIMPORT
#endif

#ifdef KLCSSRVP_EXPORTS
# define KLCSSRVP_DECL KLSTD_DLLEXPORT
#else
# define KLCSSRVP_DECL KLSTD_DLLIMPORT
#endif


#ifdef KLCSSRV_EXPORTS
# define KLCSSRV_DECL KLSTD_DLLEXPORT
#else
# define KLCSSRV_DECL KLSTD_DLLIMPORT
#endif

/*!
	\brief Useful macroses
*/ 

	/*! \brief Считает длину статического невнешенго (не extern) массива*/ 
	#define KLSTD_COUNTOF(_arr) (sizeof((_arr))/sizeof((_arr)[0]))
	
	#define KLSTD_ZEROSTRUCT(_struct) memset(&(_struct), 0, sizeof(_struct))

	// int KLSTD_VSWPRINTF(wchar_t *buffer, size_t count, const wchar_t *format, va_list argptr);
	// int KLSTD_SWPRINTF( wchar_t *buffer, size_t count, const wchar_t *format ... );
	// int KLSTD_SPRINTF(char *, size_t count, const char * format ...);

	#ifdef _WIN32 
		#define KLSTD_VSWPRINTF	_vsnwprintf
		#define KLSTD_SWPRINTF	_snwprintf
		#define KLSTD_SPRINTF	_snprintf
	#else
 	 #ifdef N_PLAT_NLM 
		#define KLSTD_VSWPRINTF	_vsnwprintf
		#define KLSTD_SWPRINTF	snwprintf
		#define KLSTD_SPRINTF	_snprintf
	 #else	
                #define KLSTD_VSWPRINTF	vswprintf
		#define KLSTD_SWPRINTF	swprintf
		#define KLSTD_SPRINTF	snprintf		
	 #endif
	#endif

	#if defined(__unix) || defined(N_PLAT_NLM)  

		wchar_t *_ltow( long value, wchar_t *string, int radix );
		char *_ltoa( long value, char *string, int radix );

		long _wtoi64(const wchar_t * psz);
	#endif
	

namespace KLSTD
{
   
    inline const wchar_t* FixNullString(const wchar_t* p)
    {
        return p ? p : L"";
    };

    inline const char* FixNullString(const char* p)
    {
        return p ? p : "";
    };
}


#endif //  KLDEFS_H_AAE8D350_6234_4a8a_A525_56B7D38B5A65
