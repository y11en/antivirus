#if defined (_WIN32)
#include <tchar.h>
#else
#include <unix/tchar.h>
#endif

#if !defined (__cdecl) && defined (__unix__)
#define __cdecl   __attribute__((cdecl))
#endif

void __cdecl parse_cmdline (_TCHAR *cmdstart, _TCHAR **argv, _TCHAR *args, int *numargs, int *numchars);
