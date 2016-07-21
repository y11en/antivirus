#ifndef NASCRT_NAS_CRT_H
#define NASCRT_NAS_CRT_H

#ifdef __cplusplus
extern "C" {
#endif

#ifdef N_PLAT_NLM 		// Novell Netware

	#include <stdio.h>
	#include <stdlib.h>
//	#include <string>
	#include <stdarg.h>
	#include <stddef.h>
	#include <wchar_t.h>
#endif

#ifdef __unix
#include <wctype.h>
#ifdef __linux__
#include <wchar.h>
#endif
#endif


//typedef int wchar_t;

#ifndef __unix

int iswspace( wchar_t c );
int iswdigit( wchar_t c );
int iswalpha( wchar_t c );
int iswalnum( wchar_t c );

wchar_t towupper (
        wchar_t c
        );
wchar_t towlower (
		wchar_t c
		);
wchar_t * wcscat(
        wchar_t * dst,
        const wchar_t * src
        );
wchar_t* initNovellPath(const wchar_t* fileName);



#if 0	
size_t wcslen (
        const wchar_t * wcs
        );
wchar_t * wcscpy(wchar_t * dst, const wchar_t * src);
wchar_t * wcsncpy (
        wchar_t * dest,
        const wchar_t * source,
        size_t count
        );
int wcscmp (
        const wchar_t * src,
        const wchar_t * dst
        );
/*wchar_t * wcscat(
        wchar_t * dst,
        const wchar_t * src
        );*/
wchar_t * wcschr (
        const wchar_t * string,
        wchar_t ch
        );
int wcsncmp (
        const wchar_t * first,
        const wchar_t * last,
        size_t count
        );
#endif
size_t  mbstowcs
        (
        wchar_t  *pwcs,
        const char *s,
        size_t n
        );

size_t wcstombs
        (
        char * s,
        const wchar_t * pwcs,
        size_t n
        );   

int mbtowc
        (
        wchar_t  *pwc,
        const char *s,
        size_t n
        );

long wcstol (
        const wchar_t *nptr,
        wchar_t **endptr,
        int ibase
        );

unsigned long wcstoul (
        const wchar_t *nptr,
        wchar_t **endptr,
        int ibase
        );        
#if 0        
wchar_t *wcsstr( 
		const wchar_t *string, 
		const wchar_t *strCharSet );
		
wchar_t *wcsrchr( 
		const wchar_t *string, 
		int c );
int wcsncnt (
        const wchar_t *string,
        int cnt
        );
#endif


size_t wcsftime (
        wchar_t *wstring,
        size_t maxsize,
        const wchar_t *wformat,
        const struct tm *timeptr
        );


int _wtoi (
        const wchar_t *nptr
        );

char * _itoa (
        int val,
        char *buf,
        int radix
        );
wchar_t * _itow (
        int val,
        wchar_t *buf,
        int radix
        );

char * _ltoa (
        long val,
        char *buf,
        int radix
        );
wchar_t * _ltow (
        long val,
        wchar_t *buf,
        int radix
        );


int	vswprintf(wchar_t *buffer, const wchar_t *format, va_list arglist);
int	swprintf(wchar_t *buffer, const wchar_t *format, ...);

int	vfwprintf(FILE *stream, const wchar_t *format, va_list arglist);
int	fwprintf(FILE *stream, const wchar_t *format, ...);

int	wprintf(const wchar_t *format, ...);

int vsnwprintf (wchar_t *str, size_t maxlen, const wchar_t *format, va_list arglist);
int snwprintf (wchar_t *str, size_t maxlen, const wchar_t *format, ...);
int _snprintf (char *str, size_t maxlen, const char *format, ...);
int iswpunct( int  c );

wchar_t * wcsncpy (
        wchar_t * dest,
        const wchar_t * source,
        size_t count
        );


// ################################################################################
// ################################################################################
/*	for  support porting  IO from windows-platform   	*/
// ################################################################################

int _vsnwprintf (wchar_t *str, size_t maxlen, const wchar_t *format, va_list arglist);
int _snwprintf (wchar_t *str, size_t maxlen, const wchar_t *format, ...);

/*	for  support porting  from windows-platform   	*/
				// for char*
#endif  // !__unix

#ifdef __unix

#ifdef __linux__
#define wcsicmp wcscasecmp
#else
  int wcsicmp (
        const wchar_t * dst,
        const wchar_t * src
        );
#endif        
  // int _vsprintf(char *buffer , const char *format, va_list argptr);
  // int _vsnprintf(char *buffer , size_t size, const char *format, va_list argptr);;

#endif





#ifdef __cplusplus
}
#endif

#endif // NASCRT_NAS_CRT_H
