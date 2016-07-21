#include "windowsRegistryWrapper.h"

ProxyDetectorNamespace::WindowsRegistryWrapper::AutoCloseHandle::AutoCloseHandle(HANDLE handle)
 : m_handle(handle)
{
}
ProxyDetectorNamespace::WindowsRegistryWrapper::AutoCloseHandle::~AutoCloseHandle()
{
    if(m_handle)
        CloseHandle(m_handle);
}

bool ProxyDetectorNamespace::WindowsRegistryWrapper::readRegistryBinary(const HKEY key,
                const std::string &valueName, std::vector<unsigned char> &value, KLUPD::Log *pLog)
{
    DWORD size = 0;
    // result is not checked because failure is expected with not enough buffer result
    const LONG unUsedQueryResult = RegQueryValueExA(key, valueName.c_str(), 0, 0, 0, &size);
    if(size)
    {
        value.resize(size);
        if(RegQueryValueExA(key, valueName.c_str(), 0, 0, &value[0], &size) == ERROR_SUCCESS)
            return true;
    }

    const int lastError = GetLastError();
    TRACE_MESSAGE4("Failed to read registry key '%s', size %d, last error %S",
        valueName.c_str(), size, KLUPD::errnoToString(lastError, KLUPD::windowsStyle).toWideChar());
    return false;
}

bool ProxyDetectorNamespace::WindowsRegistryWrapper::readRegistryString(const HKEY key,
                                        const std::string &valueName, std::string &value, KLUPD::Log *pLog)
{
    std::vector<unsigned char> buffer;
    if(!readRegistryBinary(key, valueName, buffer, pLog))
        return false;
    value.resize(buffer.size());
    std::copy(buffer.begin(), buffer.end(), value.begin());
    return true;
}


bool ProxyDetectorNamespace::WindowsRegistryWrapper::readRegistryDword(const HKEY key,
                                                                       const std::string &valueName,
                                                                       DWORD &value,
                                                                       KLUPD::Log *pLog)
{
    DWORD size = 0;
    // result is not checked because failure is expected with not enough buffer result
    const LONG unUsedQueryResult = RegQueryValueExA(key, valueName.c_str(), 0, 0, 0, &size);
    if(size != sizeof(value))
    {
        const int lastError = GetLastError();
        TRACE_MESSAGE4("Failed to read registry DWORD '%s', invalid size %d, last error %S",
            valueName.c_str(), size, KLUPD::errnoToString(lastError, KLUPD::windowsStyle).toWideChar());
        return false;
    }

    if(RegQueryValueExA(key, valueName.c_str(), 0, 0,
        reinterpret_cast<unsigned char *>(&value), &size) == ERROR_SUCCESS)
    {
        return true;
    }

    const int lastError = GetLastError();
    TRACE_MESSAGE3("Failed to read registry DWORD '%s', last error %S",
        valueName.c_str(), KLUPD::errnoToString(lastError, KLUPD::windowsStyle).toWideChar());
    return false;
}

bool ProxyDetectorNamespace::WindowsRegistryWrapper::openCurrentThreadUserRegistryKey(const char *subKey, HKEY &keyResult, KLUPD::Log *pLog)
{
    std::string sidString;
    KLUPD::NoCaseString getSidStringResultDescription;
    KLUPD::NoCaseString openRegistryBySidResultDescription;
    if(getSidStringForCurrentThread(sidString, getSidStringResultDescription))
    {
        if(RegOpenKeyExA(HKEY_USERS, (sidString + "\\" + subKey).c_str(), 0, KEY_READ, &keyResult) == ERROR_SUCCESS)
        {
            TRACE_MESSAGE3("Successfully opened registry key '%s\\%s'", sidString.c_str(), subKey);
            return true;
        }

        const int lastError = GetLastError();
        openRegistryBySidResultDescription = KLUPD::NoCaseString(L"open registry by SID string result ")
            + KLUPD::errnoToString(lastError, KLUPD::windowsStyle);
    }

    KLUPD::NoCaseString composedResultForTrace;
    if(!getSidStringResultDescription.empty())
        composedResultForTrace = KLUPD::NoCaseString(L"(get thread sid string result: ") + getSidStringResultDescription + L")";
    if(!openRegistryBySidResultDescription.empty())
        composedResultForTrace = KLUPD::NoCaseString(L"(open registry with thread token: ") + openRegistryBySidResultDescription + L")";

    if(RegOpenKeyExA(HKEY_CURRENT_USER, subKey, 0, KEY_READ, &keyResult)
        == ERROR_SUCCESS)
    {
        TRACE_MESSAGE3("Registry key '%s' opened without thread impersonation %S", subKey, composedResultForTrace.toWideChar());
        return true;
    }

    const int lastError = GetLastError();
    TRACE_MESSAGE4("Failed to open registry key '%s', last error %S  %S",
        subKey, KLUPD::errnoToString(lastError, KLUPD::windowsStyle).toWideChar(), composedResultForTrace.toWideChar());
    return false;
}

bool ProxyDetectorNamespace::WindowsRegistryWrapper::getSidStringForCurrentThread(std::string &sidString, KLUPD::NoCaseString &resultDescription)
{
    AutoCloseHandle threadToken;
    if(!OpenThreadToken(GetCurrentThread(), TOKEN_QUERY | TOKEN_IMPERSONATE | TOKEN_DUPLICATE, FALSE, &threadToken.m_handle))
    {
        const int lastError = GetLastError();
        resultDescription = KLUPD::NoCaseString(L"open thread token result ") + KLUPD::errnoToString(lastError, KLUPD::windowsStyle);
        return false;
    }

    DWORD tokenInformationLength = 0;
    // result is not checked, because too small buffer is passed and it is expected to fail
    const BOOL uncheckedResult = GetTokenInformation(threadToken.m_handle, TokenUser, 0, 0, &tokenInformationLength);
    if(!tokenInformationLength)
    {
        const int lastError = GetLastError();
        resultDescription = KLUPD::NoCaseString(L"get token information size result ")
            + KLUPD::errnoToString(lastError, KLUPD::windowsStyle);
        return false;
    }

    std::vector<unsigned char> tokenInformation(tokenInformationLength, 0);
    PTOKEN_USER tokenUser = reinterpret_cast<PTOKEN_USER>(&tokenInformation[0]);
    if(!GetTokenInformation(threadToken.m_handle, TokenUser, tokenUser, tokenInformationLength, &tokenInformationLength))
    {
        const int lastError = GetLastError();
        resultDescription = KLUPD::NoCaseString(L"get token information result ")
            + KLUPD::errnoToString(lastError, KLUPD::windowsStyle);
        return false;
    }

    std::ostringstream sidStream;
    sidStream.imbue(std::locale::classic());
	SID *sid = reinterpret_cast<SID *>(tokenUser->User.Sid);
    sidStream << "S"
        << "-" << static_cast<size_t>(sid->Revision)
        << "-" << static_cast<size_t>(sid->SubAuthorityCount);
    for(size_t authorityIndex = 0; authorityIndex < sid->SubAuthorityCount; ++authorityIndex)
    {
        if(!sid->SubAuthority[authorityIndex])
            break;
        sidStream << "-" << sid->SubAuthority[authorityIndex];
    }

    sidString = sidStream.str();
    return true;
}
