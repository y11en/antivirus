#ifndef COMDEFS_INCLUDED_BB491211_0D62_4768_B39D_29D3A5A4D802
#define COMDEFS_INCLUDED_BB491211_0D62_4768_B39D_29D3A5A4D802

#if _MSC_VER > 1000
    #pragma once
#endif // _MSC_VER > 1000



#ifdef WIN32
    #define WIN32_LEAN_AND_MEAN

    #include <winsock2.h>
    #include <ws2tcpip.h>
    #include <iptypes.h>

    #include <windows.h>

    #include <io.h>
    #include <direct.h>

    #include <sys/timeb.h>

    // 4996 'function': was declared deprecated
// TODO: when all moved to VS 2005 use new secure API, and fix secure bug
    #pragma warning( disable : 4996 )

    // 4251 'identifier' : class 'type' needs to have dll-interface to be used by clients of class 'type2'
    // 4786 'identifier' : identifier was truncated to 'number' characters in the debug information
    #pragma warning( disable : 4251 4786 )
#endif


#include <string>
#include <sstream>
#include <vector>
// WARNING: do not use std::map and std::set in exported interfaces,
 // because Microsoft Visual Studio 6 implementation has static variables
#include <map>
#include <set>
#include <algorithm>
#include <stack>
#include <memory>
#include <iostream>
#include <fstream>
#include <iterator>
#include <iomanip>
#include <locale>


#include <errno.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <stdarg.h>
#include <ctype.h>
#include <time.h>


#ifdef WIN32
    #ifdef LIBS_STATIC_LINKING
        #ifndef KAVUPDCORE_API
            #define KAVUPDCORE_API
        #endif
    #else
        #ifndef KAVUPDCORE_API
            #ifdef KAVUPDCORE_EXPORTS
                #define KAVUPDCORE_API __declspec(dllexport)
            #else // KAVUPDCORE_EXPORTS
                #define KAVUPDCORE_API __declspec(dllimport)
            #endif // KAVUPDCORE_EXPORTS
        #endif // KAVUPDCORE_API
    #endif

// UNIX
#else
    #define KAVUPDCORE_API
#endif
// ===================================================================


#ifdef WIN32

    // Visual Studio 6 compiler version
    #define VISUAL_STUDIO_6_COMPILER_VERSION 1200

    #if !defined(usleep)
        #define usleep(q) Sleep(q/1000)
    #endif

    // for getsockname() compatibility with UNIX
    typedef int socklen_t;

    #define snprintf  _snprintf
    #define vsnprintf _vsnprintf

    

    /// the implementation of then strptime() function already present in UNIX in time.h header file
     /// Take date string according to a passed format line
     /// @param bufferDate [out] buffer with date
     /// @param format[in] format string
     /// @param tm [out] Taken date
    KAVUPDCORE_API const char *strptime(const char *bufferDate, const char *format, struct tm *);



#else // #ifdef WIN32

    #include <dirent.h>
    #include <stdio.h>
    #include <libgen.h>
    #include <sys/wait.h>

    #include <unistd.h>

    #include <sys/socket.h>
    #include <netinet/in.h>
    #include <netdb.h>
    #include <arpa/inet.h>
    #include <stdexcept>
    #include <strings.h>

     #ifndef _snprintf
         #define _snprintf snprintf 
     #endif
     #ifndef _vsnprintf
         #define _vsnprintf vsnprintf 
     #endif


    #ifndef stricmp
        #define stricmp strcasecmp
    #endif
    #ifndef strnicmp
        #define strnicmp strncasecmp
    #endif
    #ifndef _stricmp
        #define _stricmp strcasecmp
    #endif
    #ifndef _strnicmp
        #define _strnicmp strncasecmp
    #endif

    // UNIX version have not Administration Kit support
    #define DISABLE_AK_FILETRANSFER 1

    #if defined(__HPUX_)
        #include <sys/_wchar_t.h>
    #endif
    #if defined(__APPLE__)
        #include <wchar.h>
    #endif

    #if !defined(__HPUX_) && !defined(_GLIBCPP_USE_WCHAR_T)
        typedef std::basic_string <wchar_t> wstring;
    #endif

    #if !defined(INADDR_NONE)
        #define INADDR_NONE 0xffffffff
    #endif


    #if !defined(_XOPEN_SOURCE_EXTENDED)
	    #define _XOPEN_SOURCE_EXTENDED
    #endif

    #ifndef GetLastError
        static inline int GetLastError()
        {
            return errno;
        }
    #endif

    #ifndef WSAGetLastError
        static inline int WSAGetLastError()
        {
             return errno;
        }
    #endif

#endif //WIN32


#ifdef WIN32
    #include <tchar.h>

#else // Unix

    #if defined(UNICODE) || defined(_UNICODE)
        #define __T(x)      L ## x
        typedef wchar_t     _TCHAR;
    #else
        #define __T(x)      x
        typedef char     _TCHAR;
    #endif

    #if defined(__APPLE__)
        #ifndef TCHAR
            #define TCHAR _TCHAR
        #endif
    #else
        typedef _TCHAR      TCHAR;
    #endif

    typedef unsigned int UINT;
    #ifndef MAX_PATH
        #define MAX_PATH  PATH_MAX
    #endif
    #ifndef _T
        #define _T(x)       __T(x)
    #endif
    #ifndef _TEXT
        #define _TEXT(x)    __T(x)
    #endif
#endif // Unix

namespace KLUPD {
    extern bool KAVUPDCORE_API isVistaOrGreaterOS();
    extern bool isGlobalMutexNamespaceSupported();
    extern bool unicodeFileOperationsSupported();
    // standard inet_ntoa() may return 0, and holds result into static buffer,
     // the inet_ntoaSafe() helper API works around these issues
    extern std::string KAVUPDCORE_API inet_ntoaSafe(const struct in_addr &);


#ifndef __APPLE__
    // on some platform we use non-standard wchar_t type 2 bytes size instead and 4 bytes size.
      // e.g. on MAC platform in run-time library there is a error with e.g. std::distance() which assumes
     // that wchar_t type size is 2 bytes ignoring fact that sizeof(wchar_t) is 2 bytes.
    // All that leads to access violations, and we need own wchar_t type
    //
    // Note: WSTRING support means std::wstring,
     // anyway there must by wchar_t type on any platform
    #define WSTRING_SUPPORTED
#endif


#ifndef WSTRING_SUPPORTED
    // TODO: Administration Kit file transfer protocol works with 'std::wstring' type, that is why it is disabled
    #ifndef DISABLE_AK_FILETRANSFER
        #define DISABLE_AK_FILETRANSFER
    #endif
#endif

}   // namespace KLUPD

#include "stringConvertHelpers.h"
#include "NoCaseString.h"

// headers in this list depends only from system includes
#include "updater_settings_keywords.h"
#include "kavset_xml.h"
#include "../include/log_iface.h"
#include "localize.h"

#endif  // #ifndef COMDEFS_INCLUDED_BB491211_0D62_4768_B39D_29D3A5A4D802

