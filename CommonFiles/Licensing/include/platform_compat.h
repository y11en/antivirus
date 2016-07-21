#if !defined (__PLATFORM_COMPAT_H__)
#define __PLATFORM_COMPAT_H__

#if defined (_WIN32)

#include <tchar.h>

#if defined (_MSC_VER)
  #pragma once
  #pragma warning (push, 4)
#endif

#if defined (_MSC_VER)
  #pragma warning (pop)
#endif

#else

  // Here is the place for UNIX  and Novell Netware declarations

#if defined (__unix__)

#undef _UNICODE
#undef UNICODE



#define __T(x)          x
#define _T(x)       __T(x)
#define _TEXT(x)    __T(x)

typedef char  _TCHAR;
typedef _TCHAR TCHAR;

#if !defined (_topen)
  #define _topen open
  #define O_BINARY 0
#endif

#if !defined (_read)
  #define _read read
#endif

#if !defined (_write)
  #define _write write
#endif

#if !defined (_lseek)
  #define _lseek lseek 
#endif

#if !defined (_close)
  #define _close close
#endif

#if !defined (_ttoi)
  #define _ttoi atoi
#endif

#if !defined (_tcscmp)
  #define _tcscmp strcmp
#endif


#if !defined (_tmakepath)
  #define _tmakepath _makepath
#endif

#if !defined (_tsplitpath)
  #define _tsplitpath _splitpath
#endif

#if !defined (_tcscpy)
  #define _tcscpy strcpy
#endif

// Novell defines

#else

#if defined (N_PLAT_NLM)

#undef _UNICODE
#undef UNICODE

#define __T(x)          x
#define _T(x)       __T(x)
#define _TEXT(x)    __T(x)

typedef char  _TCHAR;
typedef _TCHAR TCHAR;

#if !defined (_topen)
  #define _topen open
#endif

#if !defined (_read)
  #define _read read
#endif

#if !defined (_write)
  #define _write write
#endif

#if !defined (_lseek)
  #define _lseek lseek 
#endif

#if !defined (_close)
  #define _close close
#endif

#if !defined (_ttoi)
  #define _ttoi atoi
#endif

#if !defined (_tcscmp)
  #define _tcscmp strcmp
#endif


#if !defined (_tmakepath)
  #define _tmakepath _makepath
#endif

#if !defined (_tsplitpath)
  #define _tsplitpath _splitpath
#endif

#if !defined (_tcscpy)
  #define _tcscpy strcpy
#endif

#endif // Novell

#endif

#endif // UNIX

#endif
