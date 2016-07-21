#if !defined (__KLDTDEFS_H__)
#define __KLDTDEFS_H__

#ifndef _WINBASE_
#if !(defined (__unix__))
  #define WINAPI __stdcall
#else
  #define WINAPI
#endif

#ifdef __MWERKS__
	#ifndef DWORD
	#define DWORD unsigned int
	#endif
	
	#ifndef CHAR
	#define CHAR char
	#endif
	#ifndef BOOL
		#define BOOL int
	#endif	
	#include <nwtypes.h>
	#include <stdio.h>
	#include "myassrt.h"	
#else
typedef unsigned int		DWORD;
typedef unsigned short	WORD;
typedef int							BOOL;
typedef unsigned int    UINT;
typedef unsigned char   BYTE; 
typedef char            CHAR;
typedef int             INT;
typedef long            LONG;
typedef void            VOID;
#endif

typedef unsigned short  WCHAR;    // wc,   16-bit UNICODE character

#define FAR

#define FALSE (0) 
#define TRUE  (1) 

typedef LONG 					LRESULT; 
typedef LONG					 *PLONG; 
typedef BYTE					 *LPBYTE; 
typedef WORD					 *LPWORD;
typedef DWORD					 *LPDWORD;
typedef void					 *LPVOID;
typedef const void     *LPCVOID;
typedef LPVOID					HANDLE ;

#ifndef __OVERLAPPED_DEFINED
typedef struct _OVERLAPPED {
    DWORD   Internal;
    DWORD   InternalHigh;
    DWORD   Offset;
    DWORD   OffsetHigh;
    HANDLE 	hEvent;
} OVERLAPPED, *LPOVERLAPPED;
#define __OVERLAPPED_DEFINED
#endif

typedef DWORD WPARAM;
typedef DWORD LPARAM;

typedef WCHAR *PWCHAR;
typedef WCHAR *LPWCH, *PWCH;
typedef const WCHAR *LPCWCH, *PCWCH;
typedef WCHAR *NWPSTR;
typedef WCHAR *LPWSTR, *PWSTR;

typedef const WCHAR *LPCWSTR, *PCWSTR;

//
// ANSI (Multi-byte Character) types
//
typedef CHAR *PCHAR;
typedef CHAR *LPCH, *PCH;

typedef const CHAR *LPCCH, *PCCH;
typedef CHAR *NPSTR;
typedef CHAR *LPSTR, *PSTR;
typedef const CHAR *LPCSTR, *PCSTR;

//
// Neutral ANSI/UNICODE types and macros
//
#ifdef  UNICODE                     // r_winnt

#ifndef _TCHAR_DEFINED
typedef WCHAR TCHAR, *PTCHAR;
typedef WCHAR TBYTE , *PTBYTE ;
#define _TCHAR_DEFINED
#endif /* !_TCHAR_DEFINED */

typedef LPWSTR LPTCH, PTCH;
typedef LPWSTR PTSTR, LPTSTR;
typedef LPCWSTR LPCTSTR;
typedef LPWSTR LP;
#define __TEXT(quote) L##quote      // r_winnt

#else   /* UNICODE */               // r_winnt

#ifndef _TCHAR_DEFINED
typedef char TCHAR, *PTCHAR;
typedef unsigned char TBYTE , *PTBYTE ;
#define _TCHAR_DEFINED
#endif /* !_TCHAR_DEFINED */

typedef LPSTR LPTCH, PTCH;
typedef LPSTR PTSTR, LPTSTR;
typedef LPCSTR LPCTSTR;
#define __TEXT(quote) quote         // r_winnt

#endif /* UNICODE */                // r_winnt
#define TEXT(quote) __TEXT(quote)   // r_winnt



#endif /*_WINBASE_*/

#endif
