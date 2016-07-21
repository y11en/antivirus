// kl_tchar.h
//

#ifndef kl_tchar_h_INCLUDED
#define kl_tchar_h_INCLUDED

#include <kl_types.h>

// wide character type wrappers

#ifndef KL_UNICODE
# ifdef UNICODE
#  define KL_UNICODE 1
# endif
#endif

#if KL_UNICODE

typedef wchar_t kl_tchar_t;

#define KL_TEXT(str) L##str

#else // KL_UNICODE

typedef char kl_tchar_t;

#define KL_TEXT(str) str

#endif // KL_UNICODE

// string function wrappers

#if KL_UNICODE

#define kl_tcscat   wcscat
#define kl_tcschr   wcschr
#define kl_tcscpy   wcscpy
#define kl_tcscspn  wcscspn
#define kl_tcslen   wcslen
#define kl_tcsncat  wcsncat
#define kl_tcsncpy  wcsncpy
#define kl_tcsrchr  wcsrchr
#define kl_tcsspn   wcsspn
#define kl_tcsstr   wcsstr
#define kl_tcstok   wcstok
#define kl_tcscmp   wcscmp
#define kl_tcsnccmp wcsncmp
#define kl_tcsncmp  wcsncmp

#define kl_tcstol   wcstol
#define kl_tcstoul  wcstoul

#define kl_tprintf   wprintf
#define kl_vtprintf  vwprintf
#define kl_ftprintf  fwprintf
#define kl_vftprintf vfwprintf
#define kl_stprintf  swprintf
#define kl_sntprintf snwprintf
#define kl_vstprintf  vswprintf
#define kl_vsntprintf vsnwprintf

#define kl_tfopen _wfopen

#define klav_tstring klav_wstring

#define kl_main wmain

#ifdef _MSC_VER
# ifndef snwprintf
#  define snwprintf _snwprintf
# endif // vsnwprintf
# ifndef vsnwprintf
#  define vsnwprintf _vsnwprintf
# endif // vsnwprintf
#endif // _MSC_VER

#else // KL_UNICODE

#define kl_tcscat   strcat
#define kl_tcschr   strchr
#define kl_tcscpy   strcpy
#define kl_tcscspn  strcspn
#define kl_tcslen   strlen
#define kl_tcsncat  strncat
#define kl_tcsncpy  strncpy
#define kl_tcsrchr  strrchr
#define kl_tcsspn   strspn
#define kl_tcsstr   strstr
#define kl_tcstok   strtok
#define kl_tcscmp   strcmp
#define kl_tcsnccmp strncmp
#define kl_tcsncmp  strncmp

#define kl_tcstol   strtol
#define kl_tcstoul  strtoul

#define kl_tprintf   printf
#define kl_vtprintf  vprintf
#define kl_ftprintf  fprintf
#define kl_vftprintf vfprintf
#define kl_stprintf  sprintf
#define kl_sntprintf snprintf
#define kl_vstprintf  vsprintf
#define kl_vsntprintf vsnprintf

#define kl_tfopen fopen

#define klav_tstring klav_string

#define kl_main main

#endif // KL_UNICODE


#endif // kl_tchar_h_DEFINED



