#ifndef __port_unix_h_
#define __port_unix_h_

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

#endif