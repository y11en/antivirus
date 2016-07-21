/*
*/

#if !defined(__MSISUPPORT_CA_MISC_H)
#define __MSISUPPORT_CA_MISC_H

//////////////////////////////////////////////////////////////////////////
#include <string>
#include <vector>
#include "ca_logging.h"

//////////////////////////////////////////////////////////////////////////
namespace MsiSupport
{
namespace Utility
{

inline bool CAGetData(MSIHANDLE hInstall, std::basic_string<TCHAR>& dest)
{
	CALogWriter log(hInstall);
    DWORD cchValueBuf = 0;
    UINT uiStat =  ::MsiGetProperty(hInstall, TEXT("CustomActionData"), TEXT(""), &cchValueBuf);
    if (uiStat == ERROR_MORE_DATA)
    {
        ++cchValueBuf; // on output does not include terminating null, so add 1
		std::vector<TCHAR> buf(cchValueBuf, TEXT('\0'));
        uiStat = ::MsiGetProperty(hInstall, TEXT("CustomActionData"), &buf[0], &cchValueBuf);
		if (uiStat == ERROR_SUCCESS)
		{
			std::basic_string<TCHAR> sData(&buf[0], &buf[0] + cchValueBuf);
			std::basic_string<TCHAR> sMessage(TEXT("CAGetData: custom action data is '"));
			sMessage.append(sData).append(TEXT("'."));
			log.Write(sMessage);
			sData.swap(dest);
			return true;
		}
    }
	else
	{
		log.Write(TEXT("CAGetData: unable to get custom action data size."));
	}
    return false;
}

inline bool CAGetProperty(MSIHANDLE hInstall, LPCTSTR szProperty, std::basic_string<TCHAR>& dest)
{
//	CALogWriter log(hInstall);
    DWORD cchValueBuf = 0;
    UINT uiStat =  ::MsiGetProperty(hInstall, szProperty, TEXT(""), &cchValueBuf);
    if (uiStat == ERROR_MORE_DATA)
    {
        ++cchValueBuf; // on output does not include terminating null, so add 1
		std::vector<TCHAR> buf(cchValueBuf, TEXT('\0'));
        uiStat = ::MsiGetProperty(hInstall, szProperty, &buf[0], &cchValueBuf);
		if (uiStat == ERROR_SUCCESS)
		{
			std::basic_string<TCHAR> sData(&buf[0], &buf[0] + cchValueBuf);
//			std::basic_string<TCHAR> sMessage(TEXT("CAGetProperty: property is '"));
//			sMessage.append(sData).append(TEXT("'."));
//			log.Write(sMessage);
			sData.swap(dest);
			return true;
		}
    }
	else
	{
//		log.Write(TEXT("CAGetProperty: unable to get property size."));
	}
    return false;
}

}  // namespace Utility
}  // namespace MsiSupport

#endif  // !defined(__MSISUPPORT_CA_MISC_H)
