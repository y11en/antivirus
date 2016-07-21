#ifndef __COMPATUTILS_H__
#define __COMPATUTILS_H__

#include <sys/types.h>
#include <stdarg.h>
#if defined (__cplusplus)
extern "C" {
#else
#include <wchar.h>
#endif

char* _strlwr ( char* x );

char* _itoa ( int value, char *str, int radix );
#define itoa _itoa

char* _ltoa ( long value, char *str, int radix );
#define ltoa _ltoa

wchar_t* _itow ( int value, wchar_t *str, int radix );
#define itow _itow

wchar_t* _ltow ( long value, wchar_t *str, int radix );
#define ltow _ltow

long _wtol ( const wchar_t* str  );
#define wtol _wtol

int _stricmp ( const char *str1, const char *str2 );
#define stricmp _stricmp

int _strnicmp ( const char *str1, const char *str2, size_t n );
#define strnicmp _strnicmp

int _wcsicmp ( const wchar_t *str1, const wchar_t *str2 );
#define wcsicmp _wcsicmp

int _wcsnicmp ( const wchar_t *str1, const wchar_t *str2, size_t n );
#define wcsnicmp _wcsnicmp

void* toUCS2LE(void* data, unsigned int size);

// Microsoft safe runtime emulation
#ifndef errno_t
typedef int errno_t;
#endif
#define _TRUNCATE ((size_t)-1)
#define STRUNCATE 0xFFFF
int _snprintf_s( char *buffer, size_t sizeOfBuffer, size_t count, const char *format, ... );
int sprintf_s( char *buffer, size_t sizeOfBuffer, const char *format, ... );
errno_t strcpy_s( char *strDestination, size_t numberOfElements, const char *strSource );
errno_t strcat_s( char *strDestination, size_t numberOfElements, const char *strSource );
errno_t memcpy_s( void *dest, size_t numberOfElements, const void *src, size_t count );
errno_t strncpy_s( char *strDestination, size_t numberOfElements, const char *strSource, size_t count);

errno_t mbstowcs_s(size_t *pReturnValue, wchar_t *wcstr, size_t sizeInWords, const char *mbstr, size_t count);
errno_t wcstombs_s(size_t *pReturnValue, char *mbstr, size_t sizeInBytes, const wchar_t *wcstr, size_t count);
errno_t wcscat_s(wchar_t *strDestination, size_t numberOfElements, const wchar_t *strSource);
errno_t wcscpy_s(wchar_t *strDestination, size_t numberOfElements, const wchar_t *strSource);

int vsprintf_s(char *buffer, size_t numberOfElements, const char *format, va_list argptr); 


// end of Microsoft safe runtime emulation

#if defined (__cplusplus)
}
template<typename T>
const T& min (const T& a, const T& b) { return a > b ? b : a; }
template<typename T>
const T& max (const T& a, const T& b) { return a > b ? a : b; }
#else
#if !defined (min) || !defined (max)
#	define min(a,b) ((a)>(b)?(b):(a))
#	define max(a,b) ((a)>(b)?(a):(b))
#endif
#endif

#define _snprintf  snprintf
#define _vsnprintf vsnprintf
#define _snwprintf swprintf
#define lstrcmp    strcmp
#define lstrcpy    strcpy
#define _rotl(x, n)  (((x) << (n)) | ((x) >> (32 - (n))))

// _countof helper
#if !defined(_countof)
#if !defined(__cplusplus)
#define _countof(array) (sizeof(array) / sizeof(array[0]))
#else
template <typename countoftype, size_t sizeofarray>
char (*_countof_helper(countoftype (&array)[sizeofarray]))[sizeofarray];
#define _countof(array) sizeof(*_countof_helper(array))
#endif
#endif

#endif
