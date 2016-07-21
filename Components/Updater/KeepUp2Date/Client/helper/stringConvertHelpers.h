#ifndef STRINGCONVERTHELPERS_H_INCLUDED_C4981D7C_E47F_48bd_802A_9DA9B7978FBA
#define STRINGCONVERTHELPERS_H_INCLUDED_C4981D7C_E47F_48bd_802A_9DA9B7978FBA

#if _MSC_VER > 1000
    #pragma once
#endif // _MSC_VER > 1000


#include "comdefs.h"

namespace KLUPD    {

extern std::string KAVUPDCORE_API wideCharToAscii(const wchar_t *);
extern char KAVUPDCORE_API wideCharToAscii(const wchar_t);

#ifdef WSTRING_SUPPORTED

    extern std::wstring KAVUPDCORE_API asciiToWideChar(const std::string &);
    extern wchar_t KAVUPDCORE_API asciiToWideChar(const char);

    extern std::wstring KAVUPDCORE_API tStringToWideChar(const TCHAR *);
    extern std::string KAVUPDCORE_API tStringToAscii(const TCHAR *);

    // RESTRICTIONS: you can not to use asciiToTString() and wideCharToTString() functions in non-UNICODE library
    #if (defined _UNICODE) || (defined UNICODE)
        extern std::wstring KAVUPDCORE_API asciiToTString(const std::string &);
        extern std::wstring KAVUPDCORE_API wideCharToTString(const std::wstring &);
    #else
        extern std::string KAVUPDCORE_API asciiToTString(const std::string &);
        extern std::string KAVUPDCORE_API wideCharToTString(const std::wstring &);
    #endif

#else // WSTRING_SUPPORTED

    extern std::string KAVUPDCORE_API asciiToWideChar(const std::string &);
    extern char KAVUPDCORE_API asciiToWideChar(const char);

    #if (defined _UNICODE) || (defined UNICODE)
        extern std::string KAVUPDCORE_API tStringToWideChar(const wchar_t *);
        extern std::string KAVUPDCORE_API tStringToAscii(const wchar_t *);
    #else
        extern std::string KAVUPDCORE_API tStringToWideChar(const char *);
        extern std::string KAVUPDCORE_API tStringToAscii(const char *);
    #endif

    extern std::string KAVUPDCORE_API asciiToTString(const std::string &);
    extern std::string KAVUPDCORE_API wideCharToTString(const std::string &);

#endif  // WSTRING_SUPPORTED

}   // namespace KLUPD

#endif  // STRINGCONVERTHELPERS_H_INCLUDED_C4981D7C_E47F_48bd_802A_9DA9B7978FBA

