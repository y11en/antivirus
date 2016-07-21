#ifndef _m_utils_h
#define _m_utils_h

#if defined (__unix__)

	// Never, NEVER declare system function prototypes manually under UNIX!
	// Every UNIX has its own prototypes in their header files, and such 
	// declaration will screw the compatibility completely.
	#include <stdlib.h>
	#include <string.h>
#else

#include <AVPPort.h>
	

//#define M_UTILS VIK 06.02.02

#ifdef __cplusplus
extern "C" {
#endif
	
// sets pointer for dbg_report function
// 
// NOTE: 
//   Some MS code uses ASSERT... defines
//   also debug compilled modules often contain code checks stack after any function call
//   (depends on compiller option)
// 
// this function is called on ASSERTION FAILED or esp_check failed events
//  !!! if this function returns nonzero value it initiates ----==== INT3 ====---- call !!!
void set_dbg_report_func( int (*check_func)(const char*) );
	
#ifdef __cplusplus
}
#endif


#ifdef _CHECK_INCLUDES
  #ifdef _INC_MEMORY
  #error Cannot be compiled simultaneously with "memory.h"
  #endif

  #ifdef _INC_STRING
  #error Cannot be compiled simultaneously with "string.h"
  #endif

  #ifdef _INC_WCHAR
  #error Cannot be compiled simultaneously with "wchar.h"
  #endif

  #ifdef _INC_STDLIB
  #error Cannot be compiled simultaneously with "stdlib.h"
  #endif

#endif

#if defined( NON_STANDART_NAMES )
  #define __F__(a) my_##a
#else
  #define __F__(a) a
#endif

#ifdef __cplusplus
extern "C" {
#endif

	#if defined( _CRTIMP ) && !defined( NON_STANDART_NAMES )
		#if defined( __cplusplus ) && defined(_M_AMD64)
			#define _MY_CRTIMP
			#define _MY_CRTIMP2  _CRTIMP
		#else
			#define _MY_CRTIMP   _CRTIMP
			#define _MY_CRTIMP2  _CRTIMP
		#endif
	#else
		#define _MY_CRTIMP
		#define _MY_CRTIMP2
	#endif

  // memory manipulations
             void* __cdecl __F__(memcpy)( void* dst, const void* src, size_t count );
             void* __cdecl __F__(memset)( void* dst, int fill, size_t count );
             int   __cdecl __F__(memcmp)( const void* m1, const void* m2, size_t count );
  _MY_CRTIMP void* __cdecl __F__(memmove)( void* dst, const void* src, size_t count );


  // string manipulations
             size_t __cdecl __F__(strlen)( const char* src );
             char*  __cdecl __F__(strcpy)( char* dst, const char* src );
  _MY_CRTIMP char*  __cdecl __F__(strncpy)( char* dst, const char* src, size_t max_len );
             char*  __cdecl __F__(strcat)( char* dst, const char* src );
             int    __cdecl __F__(strcmp)( const char* s1, const char* s2 );
  
  // wide string manipulation
  _MY_CRTIMP2 size_t   __cdecl __F__(wcslen)( const wchar_t* wcs );
  _MY_CRTIMP2 wchar_t* __cdecl __F__(wcscpy)( wchar_t* dst, const wchar_t* src );
  _MY_CRTIMP2 wchar_t* __cdecl __F__(wcscat)( wchar_t* dst, const wchar_t* src );
  _MY_CRTIMP2 int      __cdecl __F__(wcscmp)( const wchar_t* s1, const wchar_t* s2 );


  // itoa functions
  _MY_CRTIMP char * __cdecl __F__(itoa)( int val, char *buf, int radix );
  _MY_CRTIMP char * __cdecl __F__(ltoa)( long val, char *buf, int radix );
  _MY_CRTIMP char * __cdecl __F__(ultoa)( unsigned long val, char *buf, int radix );
  #ifndef _NO_INT64
    _MY_CRTIMP char * __cdecl __F__(i64toa)( __int64 val, char *buf, int radix );
    _MY_CRTIMP char * __cdecl __F__(ui64toa)( unsigned __int64 val, char *buf, int radix );
  #endif

#ifdef __cplusplus
}
#endif

#endif /* defined (__unix__) */

#endif
