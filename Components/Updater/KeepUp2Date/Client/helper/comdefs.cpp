#include "comdefs.h"

bool KLUPD::isVistaOrGreaterOS()
{
#ifdef WIN32
    const DWORD dwVersion = GetVersion();

    if(VER_SERVER_NT <= dwVersion)   // Windows 98
        return false;

    return (BYTE)dwVersion >= 6;
#else
    return false;
#endif  // WIN32
}

bool KLUPD::isGlobalMutexNamespaceSupported()
{
#ifdef WIN32
    const DWORD dwVersion = GetVersion();

    if(VER_SERVER_NT <= dwVersion)   // Windows 98
        return false;

    return (BYTE)dwVersion >= 5;
#else
    return false;
#endif  // WIN32
}

bool KLUPD::unicodeFileOperationsSupported()
{
#if defined(WIN32) && defined(WSTRING_SUPPORTED)
    const DWORD dwVersion = GetVersion();

    if(VER_SERVER_NT <= dwVersion)   // Windows 98
        return false;

	return (BYTE)dwVersion >= 4;
#else
    return false;
#endif  // WIN32
}

std::string KLUPD::inet_ntoaSafe(const struct in_addr &in)
{
    const char *convertedName = inet_ntoa(in);
    return convertedName ? convertedName : "!.!.!.!";
}


