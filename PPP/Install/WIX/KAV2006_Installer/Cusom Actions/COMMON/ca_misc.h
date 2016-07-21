/*
*/

#if !defined(__MSISUPPORT_CA_MISC_H)
#define __MSISUPPORT_CA_MISC_H

//////////////////////////////////////////////////////////////////////////
#include <string>

//////////////////////////////////////////////////////////////////////////
namespace MsiSupport
{
namespace Utility
{

inline bool CAGetData(MSIHANDLE hInstall, std::basic_string<char>& dest)
{
    char* szValueBuf = NULL;
    DWORD cchValueBuf = 0;
    UINT uiStat =  ::MsiGetPropertyA(hInstall, "CustomActionData", "", &cchValueBuf);
    if (uiStat == ERROR_MORE_DATA)
    {
        ++cchValueBuf; // on output does not include terminating null, so add 1
        szValueBuf = new char[cchValueBuf];
        if (szValueBuf)
        {
            uiStat = MsiGetPropertyA(hInstall, "CustomActionData", szValueBuf, &cchValueBuf);
			if (uiStat == ERROR_SUCCESS)
				std::basic_string<char>(szValueBuf, szValueBuf + cchValueBuf).swap(dest);
			delete [] szValueBuf;
			return (uiStat == ERROR_SUCCESS);
        }
    }
    return false;
}

inline bool CAGetDataW(MSIHANDLE hInstall, std::basic_string<wchar_t>& dest)
{
    wchar_t* szValueBuf = NULL;
    DWORD cchValueBuf = 0;
    UINT uiStat =  ::MsiGetPropertyW(hInstall, L"CustomActionData", L"", &cchValueBuf);
    if (uiStat == ERROR_MORE_DATA)
    {
        ++cchValueBuf; // on output does not include terminating null, so add 1
        szValueBuf = new wchar_t[cchValueBuf];
        if (szValueBuf)
        {
            uiStat = MsiGetPropertyW(hInstall, L"CustomActionData", szValueBuf, &cchValueBuf);
			if (uiStat == ERROR_SUCCESS)
				std::basic_string<wchar_t>(szValueBuf, szValueBuf + cchValueBuf).swap(dest);
			delete [] szValueBuf;
			return (uiStat == ERROR_SUCCESS);
        }
    }
    return false;
}

inline bool CAGetProperty(MSIHANDLE hInstall, LPCSTR szProperty, std::basic_string<char>& dest)
{
    char* szValueBuf = NULL;
    DWORD cchValueBuf = 0;
    UINT uiStat =  ::MsiGetPropertyA(hInstall, szProperty, "", &cchValueBuf);
    if (uiStat == ERROR_MORE_DATA)
    {
        ++cchValueBuf; // on output does not include terminating null, so add 1
        szValueBuf = new char[cchValueBuf];
        if (szValueBuf)
        {
            uiStat = MsiGetPropertyA(hInstall, szProperty, szValueBuf, &cchValueBuf);
			if (uiStat == ERROR_SUCCESS)
				std::basic_string<char>(szValueBuf, szValueBuf + cchValueBuf).swap(dest);
			delete [] szValueBuf;
			return (uiStat == ERROR_SUCCESS);
        }
    }
    return false;
}

inline bool CAGetPropertyW(MSIHANDLE hInstall, LPCWSTR szProperty, std::basic_string<wchar_t>& dest)
{
    wchar_t* szValueBuf = NULL;
    DWORD cchValueBuf = 0;
    UINT uiStat =  ::MsiGetPropertyW(hInstall, szProperty, L"", &cchValueBuf);
    if (uiStat == ERROR_MORE_DATA)
    {
        ++cchValueBuf; // on output does not include terminating null, so add 1
        szValueBuf = new wchar_t[cchValueBuf];
        if (szValueBuf)
        {
            uiStat = MsiGetPropertyW(hInstall, szProperty, szValueBuf, &cchValueBuf);
			if (uiStat == ERROR_SUCCESS)
				std::basic_string<wchar_t>(szValueBuf, szValueBuf + cchValueBuf).swap(dest);
			delete [] szValueBuf;
			return (uiStat == ERROR_SUCCESS);
        }
    }
    return false;
}

}  // namespace Utility
}  // namespace MsiSupport

#endif  // !defined(__MSISUPPORT_CA_MISC_H)
