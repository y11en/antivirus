#include "stdinc.h"

#include "../core/updater.h"

#ifdef WIN32

    // Purpose: obtains locale settings
    // Details: the function returns 'Localion' settings from Control Panelt
    // Note:
    //   1. the function returns 'Locale' settings for current user
    //   2. the function returns 'Locale' settings for default user if run as NT Service
    // Restriction: the 'GetUserGeoID' function may be not available on Win9.x,
    //               and Windows 2000, thus the function will return false
    // Restriction: Windows Vista is not supported by this function
    // Return: true in case locale information is obtained
    static bool getGeoInfo(TCHAR *code, const size_t size, KLUPD::Log *pLog)
    {
        if(!code || (size < 2 * sizeof(TCHAR)))
        {
            TRACE_MESSAGE("Failed to get geographical information, because buffer is not supplied")
            return false;
        }

        typedef LONG (WINAPI *LPGETUSERGEOID)(DWORD);
        typedef int (WINAPI *LPGETGEOINFO)(LONG, DWORD, LPTSTR, int, WORD);

        HMODULE kernelModule = GetModuleHandle(_T("Kernel32.dll"));
        if(!kernelModule)
        {
            const int lastError = GetLastError();
            TRACE_MESSAGE2("Failed to obtain kernel.dll module handle, last error '%S'",
                KLUPD::errnoToString(lastError, KLUPD::windowsStyle).toWideChar());
		    return false;
        }

        LPGETUSERGEOID functionGetUserGeoID = reinterpret_cast<LPGETUSERGEOID>(GetProcAddress(kernelModule, "GetUserGeoID"));
        if(!functionGetUserGeoID)
        {
            const int lastError = GetLastError();
            TRACE_MESSAGE2("Failed to resolve 'GetUserGeoID' function address, last error '%S'",
                KLUPD::errnoToString(lastError, KLUPD::windowsStyle).toWideChar());
		    return false;
        }

        #if defined(UNICODE) || defined(_UNICODE)
            LPGETGEOINFO functionGetGeoInfo = reinterpret_cast<LPGETGEOINFO>(GetProcAddress(kernelModule, "GetGeoInfoW"));
        #else
            LPGETGEOINFO functionGetGeoInfo = reinterpret_cast<LPGETGEOINFO>(GetProcAddress(kernelModule, "GetGeoInfoA"));
        #endif

        if(!functionGetGeoInfo)
        {
            const int lastError = GetLastError();
            TRACE_MESSAGE2("Failed to resolve 'GetGeoInfo' function address, last error '%S'",
                KLUPD::errnoToString(lastError, KLUPD::windowsStyle).toWideChar());
		    return false;
        }

        const GEOID geoId = functionGetUserGeoID(GEOCLASS_NATION);
	    if(geoId == GEOID_NOT_AVAILABLE)
        {
            const int lastError = GetLastError();
            TRACE_MESSAGE2("Geographical identifier is not available, last error '%S'",
                KLUPD::errnoToString(lastError, KLUPD::windowsStyle).toWideChar());
		    return false;
        }

        if(!functionGetGeoInfo(geoId, GEO_ISO2, code, size, 0))
        {
            const int lastError = GetLastError();
            TRACE_MESSAGE2("Failed to obtained region with GetGeoInfo, last error '%S'",
                KLUPD::errnoToString(lastError, KLUPD::windowsStyle).toWideChar());
            return false;
        }

        return true;
    }

    // Purpose: obtains locale settings. The function present, because on Windows 9.x and Windows 2000
    //  the getGeoInfo() may fail. Also the getLocaleInfo() must be used on Windows Vista
    // 
    // Details: the function returns 'the language for non-Unicode programs' settings from Control Panelt
    // Return: true in case locale information is obtained
    static bool getLocaleInfo(TCHAR *code, const size_t size, KLUPD::Log *pLog)
    {
        if(!code || (size < 2 * sizeof(TCHAR)))
        {
            TRACE_MESSAGE("Failed to get locale information, because buffer is not supplied")
            return false;
        }

        if(!GetLocaleInfo(LOCALE_SYSTEM_DEFAULT, LOCALE_SISO3166CTRYNAME, code, size))
        {
            const int lastError = GetLastError();
            TRACE_MESSAGE2("GetLocaleInfo failed, last error '%S'",
                KLUPD::errnoToString(lastError, KLUPD::windowsStyle).toWideChar());
            return false;
        }
        return true;
    }

#endif  // WIN32



KLUPD::NoCaseString KLUPD::getPreferredRegion(Log *pLog)
{
    TCHAR result[16];
    memset(result, 0, sizeof(result));

    bool regionDetected = false;
#ifdef WIN32
    if(isVistaOrGreaterOS())
    {
        TRACE_MESSAGE("Automatic Region dectection for Vista OS");
        regionDetected = getLocaleInfo(result, sizeof(result), pLog);
    }
    else
    {
        regionDetected = getGeoInfo(result, sizeof(result), pLog)
            || getLocaleInfo(result, sizeof(result), pLog);
    }

#else
    // TODO: return 2-letter country code (ru, us, ...)
    TRACE_MESSAGE("Automatic region dectection is not implemented, region defaulted to '*'");
#endif

    if(!regionDetected)
        return L"*";

    
    // convert to lowercase, and from multibyte to char
    char code[16];
    memset(code, 0, sizeof(code));
    strncpy(code, tStringToAscii(result).c_str(), 2);
    if(code[0] >= 'A' && code[0] <= 'Z')
        code[0] = code[0] + 'a' - 'A';
    if(code[1] >= 'A' && code[1] <= 'Z')
        code[1] = code[1] + 'a' - 'A';
    return asciiToWideChar(code).c_str();
}
