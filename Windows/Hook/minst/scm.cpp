// scm.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#pragma warning(disable:4996)

void ShowHelp(LPCTSTR szOwnName)
{
	_tprintf(_T("%s needs some parameters\r\n"), szOwnName);
	_tprintf(_T("For service installing use next command line:\r\n"), szOwnName);
	_tprintf(_T("\t -i"));
		_tprintf(_T(" ServiceName"));
		_tprintf(_T(" DisplayName"));
		_tprintf(_T(" ServiceType"));
		_tprintf(_T(" StartType"));
		_tprintf(_T(" ErrorControl"));
		_tprintf(_T(" szPath"));
		_tprintf(_T(" pszLoadOrderGroup"));
		_tprintf(_T(" Dependencies"));
		_tprintf(_T(" Description"));
		_tprintf(_T(" DefaultInstance"));
		_tprintf(_T(" DefaultInstanceAltitude"));
		_tprintf(_T(" DefaultInstanceFlags"));
		_tprintf(_T(" CanUnload"));
		_tprintf(_T(" \r\n"));
	_tprintf(_T("For service uninstalling use another command line:\r\n"), szOwnName);
	_tprintf(_T("\t -u"));
		_tprintf(_T(" ServiceName"));
		_tprintf(_T(" \r\n"));

	_tprintf(_T("Example:\r\n"));
	_tprintf(_T("\t%s \r\n"), szOwnName);
		_tprintf(_T(" MyDriver"));
		_tprintf(_T(" \"MyDriver display name\""));
		_tprintf(_T(" SERVICE_FILE_SYSTEM_DRIVER"));
		_tprintf(_T(" SERVICE_AUTO_START"));
		_tprintf(_T(" SERVICE_ERROR_NORMAL"));
		_tprintf(_T(" \"system32\\DRIVERS\\MyDriver.sys\""));
		_tprintf(_T(" \"FSFilter Activity Monitor\""));
		_tprintf(_T(" FltMgr"));
		_tprintf(_T(" \"Hello world!\""));
		_tprintf(_T(" MyDriver"));
		_tprintf(_T(" 123456"));
		_tprintf(_T(" 0x0"));
		_tprintf(_T(" 0x1"));
		_tprintf(_T(" \r\n"));
}

#define check_macro(_macro_, _define_)	if ( 0 == _tcscmp(_macro_, szMacro) ) return _define_;
DWORD Macro2Dword(LPCTSTR szMacro)
{
	check_macro(_T("SERVICE_CHANGE_CONFIG"),		SERVICE_CHANGE_CONFIG); 
	check_macro(_T("SERVICE_ENUMERATE_DEPENDENTS"), SERVICE_ENUMERATE_DEPENDENTS); 
	check_macro(_T("SERVICE_INTERROGATE"),			SERVICE_INTERROGATE); 
	check_macro(_T("SERVICE_PAUSE_CONTINUE"),		SERVICE_PAUSE_CONTINUE); 
	check_macro(_T("SERVICE_QUERY_CONFIG"),			SERVICE_QUERY_CONFIG); 
	check_macro(_T("SERVICE_QUERY_STATUS"),			SERVICE_QUERY_STATUS); 
	check_macro(_T("SERVICE_START"),				SERVICE_START); 
	check_macro(_T("SERVICE_STOP"),					SERVICE_STOP); 
	check_macro(_T("SERVICE_USER_DEFINED_CONTROL"), SERVICE_USER_DEFINED_CONTROL); 
	check_macro(_T("SERVICE_ALL_ACCESS"),			SERVICE_ALL_ACCESS); 

	check_macro(_T("SERVICE_FILE_SYSTEM_DRIVER"),	SERVICE_FILE_SYSTEM_DRIVER); 
	check_macro(_T("SERVICE_KERNEL_DRIVER"),		SERVICE_KERNEL_DRIVER); 
	check_macro(_T("SERVICE_WIN32_OWN_PROCESS"),	SERVICE_WIN32_OWN_PROCESS); 
	check_macro(_T("SERVICE_WIN32_SHARE_PROCESS"),	SERVICE_WIN32_SHARE_PROCESS); 
	check_macro(_T("SERVICE_INTERACTIVE_PROCESS"),	SERVICE_INTERACTIVE_PROCESS); 
	
	check_macro(_T("SERVICE_AUTO_START"),			SERVICE_AUTO_START); 
	check_macro(_T("SERVICE_BOOT_START"),			SERVICE_BOOT_START); 
	check_macro(_T("SERVICE_DEMAND_START"),			SERVICE_DEMAND_START); 
	check_macro(_T("SERVICE_DISABLED"),				SERVICE_DISABLED); 
	check_macro(_T("SERVICE_SYSTEM_START"),			SERVICE_SYSTEM_START); 
	
	check_macro(_T("SERVICE_ERROR_IGNORE"),			SERVICE_ERROR_IGNORE); 
	check_macro(_T("SERVICE_ERROR_NORMAL"),			SERVICE_ERROR_NORMAL); 
	check_macro(_T("SERVICE_ERROR_SEVERE"),			SERVICE_ERROR_SEVERE); 
	check_macro(_T("SERVICE_ERROR_CRITICAL"),		SERVICE_ERROR_CRITICAL); 
	
	return 0;
}

BOOL InstallService(
					LPCTSTR lpServiceName,
					LPCTSTR lpDisplayName,
					DWORD dwServiceType, 
					DWORD dwStartType,
					DWORD dwErrorControl,
					LPCTSTR szPath,
					LPCTSTR pszLoadOrderGroup,
					LPCTSTR pszDependencies,
					LPCTSTR lpDescription,
					PTCHAR tcDefaultInstance,
					PCTSTR tcDefaultInstanceAltitude,
					PCTSTR tcDefaultInstanceFlags,
					PCTSTR tcCanUnload
					)
{
	BOOL bRet = FALSE;
	SC_HANDLE schSCManager = OpenSCManager(
		0,							// machine (NULL == local)
		0,							// database (NULL == default)
		SC_MANAGER_ALL_ACCESS	 	// access required
		);
	if( schSCManager ) 
	{
		// удаляем, если есть что..
		SC_HANDLE hOldService = OpenService(schSCManager, lpServiceName, DELETE | SERVICE_CONTROL_STOP);
		if (hOldService)
		{
			DeleteService(hOldService);
			CloseServiceHandle(hOldService);
		}

		TCHAR szzMultiDependencies[0x200];
		ZeroMemory(szzMultiDependencies, sizeof(szzMultiDependencies));
		_tcscpy(szzMultiDependencies, pszDependencies);
		szzMultiDependencies[_countof(szzMultiDependencies)-1] = 0;

		SC_HANDLE schService =	CreateService(
									schSCManager,
									lpServiceName,
									lpDisplayName,
									SERVICE_ALL_ACCESS,
									dwServiceType,
									dwStartType,
									dwErrorControl,
									szPath,
									pszLoadOrderGroup,
								    NULL,
									szzMultiDependencies,
									NULL,
									_TEXT("")
									);

		if( !schService )
			_tprintf(
			_T("failed to CreateService %s, LastError = %d\n"),
			lpServiceName,
			GetLastError()
			);
		else
		{
			bRet = TRUE;

			if( lpDescription )
			{
				SERVICE_DESCRIPTION l_hServiceDescription;
				l_hServiceDescription.lpDescription=(LPTSTR)lpDescription;
				HMODULE l_hAdvapi=LoadLibrary(_T("ADVAPI32.DLL"));
				if(l_hAdvapi != NULL){
					typedef BOOL (WINAPI _ChangeServiceConfig2)(SC_HANDLE,DWORD,LPVOID);
					_ChangeServiceConfig2* l_pChangeServiceConfig2;
#ifdef _UNICODE
#define CHG_SRV_CONFIG	"ChangeServiceConfig2W"
#else
#define CHG_SRV_CONFIG	"ChangeServiceConfig2A"
#endif
					l_pChangeServiceConfig2 =(_ChangeServiceConfig2*)GetProcAddress(l_hAdvapi, CHG_SRV_CONFIG);
					if(l_pChangeServiceConfig2)
						(*l_pChangeServiceConfig2)(schService,SERVICE_CONFIG_DESCRIPTION,&l_hServiceDescription);

					FreeLibrary(l_hAdvapi);
				}
			}

			if (tcDefaultInstance)
			{
				bRet = FALSE;

				DWORD dwDefaultInstanceFlags = 0;
				int iResult = 0;

				if (tcDefaultInstanceFlags)
					iResult = _stscanf(tcDefaultInstanceFlags, _T("%x"), &dwDefaultInstanceFlags);
				if (iResult && tcDefaultInstanceAltitude)
				{
					LONG lResult;
					TCHAR tcServiceKey[0x200];
					HKEY hServiceKey;

					_tcscpy(tcServiceKey, _T("system\\currentcontrolset\\services\\"));
					_tcscat(tcServiceKey, lpServiceName);
					lResult = RegOpenKeyEx(					
						HKEY_LOCAL_MACHINE,
						tcServiceKey,
						0,
						KEY_WOW64_64KEY | KEY_CREATE_SUB_KEY,
						&hServiceKey);
					if ( ERROR_SUCCESS == lResult )
					{
						HKEY hInstancesKey;
						DWORD dwDisposition;

						lResult = RegCreateKeyEx(
							hServiceKey,
							_T("Instances"),
							0,
							NULL,
							REG_OPTION_NON_VOLATILE,
							KEY_WOW64_64KEY | KEY_CREATE_SUB_KEY | KEY_SET_VALUE,
							NULL,
							&hInstancesKey,
							&dwDisposition
							);
						if ( ERROR_SUCCESS == lResult )
						{
							lResult = RegSetValueEx(
								hInstancesKey,
								_T("DefaultInstance"),
								0,
								REG_SZ,
								(PBYTE)tcDefaultInstance,
								(DWORD) (sizeof(TCHAR) * (_tcslen(tcDefaultInstance) + 1)) );
							if ( ERROR_SUCCESS == lResult)
							{
								HKEY hDefaultInstanceKey;
								lResult = RegCreateKeyEx(
									hInstancesKey,
									tcDefaultInstance,
									0,
									NULL,
									REG_OPTION_NON_VOLATILE,
									KEY_WOW64_64KEY | KEY_SET_VALUE,
									NULL,
									&hDefaultInstanceKey,
									&dwDisposition
									);
								if ( ERROR_SUCCESS == lResult)
								{
									lResult = RegSetValueEx(
										hDefaultInstanceKey,
										_T("Altitude"),
										0,
										REG_SZ,
										(PBYTE)tcDefaultInstanceAltitude,
										(DWORD) (sizeof(TCHAR) * (_tcslen(tcDefaultInstanceAltitude) + 1)) );
									if (ERROR_SUCCESS == lResult)
									{
										lResult = RegSetValueEx(
											hDefaultInstanceKey,
											_T("Flags"),
											0,
											REG_DWORD,
											(PBYTE)&dwDefaultInstanceFlags,
											(DWORD)sizeof(DWORD));
										if ( ERROR_SUCCESS == lResult )
										{
											bRet = TRUE;
										}
									}

									RegCloseKey(hDefaultInstanceKey);
								}
							}

							RegCloseKey(hInstancesKey);
						}

						RegCloseKey(hServiceKey);
					}
				}
			}

			if ( tcCanUnload )
			{
				LONG lResult;
				TCHAR tcParametersKey[0x200];
				HKEY hParametersKey;
				DWORD dwDisposition;

				_tcscpy(tcParametersKey, _T("system\\currentcontrolset\\services\\"));
				_tcscat(tcParametersKey, lpServiceName);
				_tcscat(tcParametersKey, _T("\\parameters"));
				lResult = RegCreateKeyEx(					
					HKEY_LOCAL_MACHINE,
					tcParametersKey,
					0,
					NULL,
					REG_OPTION_NON_VOLATILE,
					KEY_WOW64_64KEY | KEY_SET_VALUE,
					NULL, 
					&hParametersKey,
					&dwDisposition
					);
				if ( ERROR_SUCCESS == lResult )
				{
					DWORD dwUA = 0;
					
					_stscanf(tcCanUnload, _T("%d"), &dwUA);

					lResult = RegSetValueEx(
						hParametersKey,
						_T("UA"),
						0,
						REG_DWORD,
						(PBYTE)&dwUA,
						sizeof(dwUA)
						);
					if ( ERROR_SUCCESS != lResult)
						_tprintf(_T("failed to set ...\\%s\\parameters\\UA value\n"), lpServiceName);

					RegCloseKey(hParametersKey);
				}
				else
					_tprintf(_T("failed to create ...\\%s\\parameters key\n"), lpServiceName);
			}

			CloseServiceHandle(schService);
		} 

		CloseServiceHandle(schSCManager);
	}

	return bRet;
}

BOOL RemoveService(LPCTSTR szServiceName) 
{
	BOOL bRet = FALSE;

	SC_HANDLE schSCManager = OpenSCManager(
		0,                                                                                          
		0,                                                                                          
		SC_MANAGER_ALL_ACCESS     // access required
		);
	if( !schSCManager ) 
		_tprintf(TEXT("Cannot open Service Control manager."));
	else
	{
		SC_HANDLE schService = OpenService(
			schSCManager,
			szServiceName,
			SERVICE_ALL_ACCESS
			);
		if( !schService ) 
			_tprintf(TEXT("Cannot open %s."), szServiceName);
		else
		{
			SERVICE_STATUS ssStatus;
			// try to stop the service
			if( !ControlService(schService, SERVICE_CONTROL_STOP, &ssStatus) ) 
				_tprintf(TEXT("Cannot control %s."), szServiceName);
			else
			{
				_tprintf(TEXT("Stopping %s."), szServiceName);
				Sleep(1000);

				while( QueryServiceStatus(schService, &ssStatus) ) 
				{
					if( ssStatus.dwCurrentState == SERVICE_STOP_PENDING ) 
					{
						_tprintf(TEXT("."));
						Sleep( 1000 );
					} 
					else
						break;
				}

				if( ssStatus.dwCurrentState == SERVICE_STOPPED )
					_tprintf(TEXT("\n%s stopped.\r\n"), szServiceName);
				else
					_tprintf(TEXT("\n%s failed to stop.\r\n"), szServiceName);
			}

			// now remove the service
			if( !DeleteService(schService) )
				_tprintf(TEXT("Cannot delete %s."), szServiceName);
			else
				bRet = TRUE;

			CloseServiceHandle(schService);
		}
		CloseServiceHandle(schSCManager);
	}
	return bRet;
}

int _tmain(int argc, _TCHAR* argv[])
{
	if ( argc > 1 )
	{
		if ( 0 == _tcscmp(argv[1], _T("-i")) )
		{
			// Install service
			if ( argc >= 11 )
				return InstallService(
					argv[2],
					argv[3],
					Macro2Dword(argv[4]),
					Macro2Dword(argv[5]),
					Macro2Dword(argv[6]),
					argv[7],
					argv[8],
					argv[9],
					argv[10],
					argc >= 12? argv[11] : NULL,
					argc >= 13? argv[12] : NULL,
					argc >= 14? argv[13] : NULL,
					argc >= 15? argv[14] : NULL
					);
		}
		else if ( 0 == _tcscmp(argv[1], _T("-u"))  && argc >= 3)
		{
			// Uninstall service
			return RemoveService(argv[2]);
		}
	}

	ShowHelp(argv[0]);
	return FALSE;
}

