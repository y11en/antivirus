#include "stdafx.h"
#include "UnicodeConv.h"
#include "SubstHelper.h"
#include <StuffIO\ioutil.h>

namespace KLUTIL
{
	bool Subst2Orig( LPCWSTR szwSubstPath, OUT std::wstring& orig ) 
	{
		if (::IOSIsSubstDriveW( *szwSubstPath ) == IOS_ERR_OK)
		{
			WCHAR device_name[MAX_PATH], drive [ _MAX_DRIVE ], dir [ _MAX_DIR ], process[_MAX_FNAME], ext[_MAX_EXT];
			_wsplitpath ( szwSubstPath, drive, dir, process, ext );

			HMODULE hMod = GetModuleHandle("kernel32.dll");

			bool bIsNT = !(0x80000000 & GetVersion());
			if (bIsNT)
			{
				DWORD   (WINAPI* fnQueryDosDeviceW)(LPCWSTR, LPWSTR, DWORD);
				(void*&)fnQueryDosDeviceW = hMod ? GetProcAddress(hMod, "QueryDosDeviceW") : NULL;

				WCHAR pTargetPath[MAX_PATH];
				const WCHAR* szwDrivePrefix = L"\\??\\";
				if (fnQueryDosDeviceW && fnQueryDosDeviceW(drive, pTargetPath, sizeof(pTargetPath)/sizeof(pTargetPath[0])))
				{
					if ( _wcsnicmp(pTargetPath, szwDrivePrefix, wcslen(szwDrivePrefix)) == 0 )
						wcscpy( device_name, pTargetPath + wcslen(szwDrivePrefix) );
					else
						wcscpy( device_name, pTargetPath);

					_wcsupr( device_name );
				}
				else
					return false;
			}
			else
			{
				DWORD   (WINAPI* fnQueryDosDeviceA)( LPCSTR, LPSTR, DWORD);
				(void*&)fnQueryDosDeviceA = hMod ? GetProcAddress(hMod, "QueryDosDeviceA") : NULL;

				CHAR pTargetPath[MAX_PATH];
				const CHAR* szDrivePrefix = "\\??\\";
				if (fnQueryDosDeviceA && fnQueryDosDeviceA(__LPCSTR(drive), pTargetPath, sizeof(pTargetPath)/sizeof(pTargetPath[0])))
				{
					if ( _strnicmp(pTargetPath, szDrivePrefix, strlen(szDrivePrefix)) == 0 )
						wcscpy( device_name, __LPCWSTR(pTargetPath + strlen(szDrivePrefix)) );
					else
						wcscpy( device_name, __LPCWSTR(pTargetPath) );
					
					_wcsupr( device_name );
				}
				else
					return false;

			}

			WCHAR output_name[MAX_PATH];
			_wmakepath(output_name, NULL, dir, process, ext);

			int device_name_length = wcslen(device_name);
			if (output_name[0] == L'\\' && device_name_length && device_name[device_name_length-1] == L'\\')
				device_name[device_name_length-1] = L'\0';

			wcscat ( device_name, output_name );
			orig = device_name;

		}
		else
			orig = szwSubstPath;

		return true;
	}
	
	bool LocalPathToUNC( const TCHAR * szLocalPath, OUT std::basic_string<TCHAR>& szUNCPath )
	{
		TCHAR szBuff[4096];
		IOS_ERROR_CODE err = ::IOSLocalPathToUNC(szLocalPath, szBuff, sizeof(szBuff)/sizeof(szBuff[0]));
		szUNCPath = szBuff;
		return err==IOS_ERR_OK;
	}
};