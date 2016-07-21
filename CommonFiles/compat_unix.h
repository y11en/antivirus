#if !defined (__compat_unix_h__) && defined (__unix__)
#define __compat_unix_h__


/* When compiling for F-Secure, our type definitions should match those
 * in the F-Secure SDK. In particular, DWORD should be "unsigned long"
 */
#ifdef USE_FM
typedef unsigned long		DWORD;
typedef unsigned long    	ULONG;
#else
typedef unsigned int		DWORD;
typedef unsigned int    	ULONG;
#endif /* ifdef USE_FM */

typedef unsigned short		WORD;
typedef int					BOOL;
typedef unsigned int    	UINT;
typedef unsigned char   	BYTE; 

/* Damned F-Secure SDK sometimes use typedef and sometimes - #define.
 * CHAR and LONG are introduced via #define, and the compiler refused 
 * to accept "typedef char char;"
 */
#if !defined(CHAR)
typedef char            	CHAR;
#endif /* if !defined(CHAR) */

typedef int             	INT;

#if !defined(LONG)
typedef long            	LONG;
#endif /* if !defined(LONG) */

typedef void            	VOID;

typedef unsigned short		WCHAR;

typedef LONG				LRESULT; 
typedef LONG				*PLONG; 
typedef BYTE				*LPBYTE; 
typedef WORD				*LPWORD;
typedef DWORD				*LPDWORD;
typedef void				*LPVOID;
typedef const void     		*LPCVOID;

#ifdef USE_FM
typedef DWORD				HANDLE ;
#else
typedef LPVOID				HANDLE ;
#endif /* ifdef USE_FM */

#ifndef __ICC
typedef long long			__int64;
#endif //__ICC

typedef struct _OVERLAPPED 
{
    DWORD   Internal;
    DWORD   InternalHigh;
    DWORD   Offset;
    DWORD   OffsetHigh;
    HANDLE  hEvent;
} OVERLAPPED, *LPOVERLAPPED;


typedef struct
{
	WORD	wYear;
	WORD	wMonth;
	WORD	wDayOfWeek;
	WORD	wDay;
	WORD	wHour;
	WORD	wMinute;
	WORD	wSecond;
	WORD	wMilliseconds;
} SYSTEMTIME;


typedef struct
{
	WORD	wYear;
	WORD	wMonth;
	WORD	wDayOfWeek;
	WORD	wDay;
} SYSTEMDATE;

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
#ifdef  UNICODE

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

#if !defined (__TCHAR_DEFINED)
	typedef TCHAR _TCHAR;
	#define __TCHAR_DEFINED
#endif /* __TCHAR_DEFINED */	

#define TEXT(quote) __TEXT(quote)   // r_winnt

#if !defined (FALSE)
	#define FALSE (0)
#endif
	
#if !defined (TRUE)
	#define TRUE (1)
#endif

#define FILE_ATTRIBUTE_READONLY			0x00000001
#define FILE_ATTRIBUTE_DIRECTORY        0x00000010
#define FILE_ATTRIBUTE_TEMPORARY		0x00000100

#endif /* __compat_unix_h__ */
