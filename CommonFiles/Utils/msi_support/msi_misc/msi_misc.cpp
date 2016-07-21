// wix_misc.cpp : Defines the entry point for the DLL application.
//

#include "stdafx.h"
#include "msi_misc.h"
#pragma warning(disable : 4786)
#include "../common/ca_misc.h"
#include "../common/ca_data_parser.h"
#include <utils/osdetect.h>

//////////////////////////////////////////////////////////////////////////
BOOL APIENTRY DllMain(HANDLE hModule, DWORD ul_reason_for_call, LPVOID lpReserved)
{
    return TRUE;
}

//////////////////////////////////////////////////////////////////////////
MSI_MISC_API UINT __stdcall ReturnUserExit(MSIHANDLE hInstall)
{
	return ERROR_INSTALL_USEREXIT;
}

//////////////////////////////////////////////////////////////////////////
MSI_MISC_API UINT __stdcall ReturnError(MSIHANDLE hInstall)
{
	return ERROR_INSTALL_FAILURE;
}

//////////////////////////////////////////////////////////////////////////
MSI_MISC_API UINT __stdcall RemoveFileOnReboot(MSIHANDLE hInstall)
{
	using namespace MsiSupport::Utility;
	UINT res = ERROR_INSTALL_FAILURE;
	tstring sData;
	if (CAGetData(hInstall, sData))
	{
		CADataParser<TCHAR> parser;
		if (parser.parse(sData))
		{
			const tstring& sPath = parser.value(TEXT("Path"));
			if (!sPath.empty())
			{
				Utility::OSDetect os_detect;
				if (os_detect.IsValid())
				{
					if (os_detect.IsPlatform9x())
					{
						::WritePrivateProfileString(TEXT("rename"), TEXT("NUL"), sPath.c_str(), TEXT("wininit.ini"));
						res = ERROR_SUCCESS;
					}
					else
					{
						if (::MoveFileEx(sPath.c_str(), NULL, MOVEFILE_DELAY_UNTIL_REBOOT))
							res = ERROR_SUCCESS;
					}
				}
			}
		}
	}
	return res;
}
