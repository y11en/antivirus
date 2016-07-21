#include "stdafx.h"
#include "install.h"
#include <rpc.h>
#include "resource.h"

bool IsWin9x ()
{
    OSVERSIONINFO osvi;
    osvi.dwOSVersionInfoSize = sizeof ( osvi );
    GetVersionEx ( &osvi );
    
    return ( osvi.dwPlatformId == VER_PLATFORM_WIN32_WINDOWS );
}

void GetExeTempFileName(std::string& TempName)
{
	TCHAR ExePath[MAX_PATH];
	
	// {0A9B1CE4-2FF5-41fd-B057-9CD14DA43723}
	GUID guid = 
	{ 0xa9b1ce4, 0x2ff5, 0x41fd, { 0xb0, 0x57, 0x9c, 0xd1, 0x4d, 0xa4, 0x37, 0x23 } };
	UuidCreate (&guid);
	
	TCHAR TempPath[MAX_PATH] = {0};
	GetTempPath(sizeof(TempPath)/sizeof(TempPath[0]), TempPath);
	
	_stprintf(ExePath, _T("%s%08X-%04X-%04X-%02X%02X-%02X%02X%02X%02X%02X%02X.exe"),
		TempPath,
		guid.Data1, guid.Data2, guid.Data3, 
		guid.Data4[0], guid.Data4[1], guid.Data4[2], guid.Data4[3],
		guid.Data4[4], guid.Data4[5], guid.Data4[6], guid.Data4[7]);
	
	TempName = ExePath;
}

HANDLE FindPluginDriver(LPCTSTR qualified_name)
{
	const TCHAR szName [] = _T("\\\\.\\logger");
	HANDLE hPlugin = INVALID_HANDLE_VALUE;

	if (IsWin9x())
		hPlugin = CreateFile (qualified_name, 0, 0, NULL, 0, 0, NULL);
	else
		hPlugin = CreateFile (qualified_name, 0, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

	return hPlugin;
}

bool ExtractFile(ULONG uResID, LPCTSTR szFileName)
{
	HRSRC hRes = FindResource(NULL, MAKEINTRESOURCE(uResID), _T("BINARY")); 
	if (hRes == NULL) 
	{ 
		return false; 
	} 
	
	HGLOBAL hResLoad = LoadResource(NULL, hRes); 
	if (hResLoad == NULL) 
	{ 
		return false; 
	} 
	
	LPVOID lpResLock = LockResource(hResLoad); 
	if (lpResLock == NULL) 
	{ 
		return false; 
	}
	
	DWORD rsize = SizeofResource ( NULL, hRes );
	
	
	HANDLE hFile = CreateFile(szFileName, GENERIC_WRITE, FILE_SHARE_READ, NULL, CREATE_NEW, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hFile == INVALID_HANDLE_VALUE)
	{
		if (GetLastError() == ERROR_FILE_EXISTS)
		{
			printf("File \"%s\" already exist. Don't change.\n", szFileName);
			return true;
		}
		else
		{
			printf("Cannot extract \"%s\"\n", szFileName);
			return false;
		}
	}
	
	DWORD dwWrited = 0;
	if ( !WriteFile(hFile, lpResLock, rsize, &dwWrited, NULL) || dwWrited != rsize )
	{
		printf("Write file error(%d). Binary file maybe damaged.\n", GetLastError());
		return false;
	}

	CloseHandle (hFile);

	return true;
}


bool InstallKlick()
{
	OSVERSIONINFO osvi;
	osvi.dwOSVersionInfoSize = sizeof ( osvi );
	GetVersionEx ( &osvi );
	
	if ( osvi.dwPlatformId == VER_PLATFORM_WIN32_WINDOWS )
	{
		//9x
		//		  ExtractFile(IDR_KLICK9x_BINARY, "klick.vxd");
	}
	else
	{
		TCHAR DriversPath[MAX_PATH] = {0};
		ExpandEnvironmentStrings( _T("%SystemRoot%\\system32\\drivers\\klick.sys"), DriversPath, sizeof(DriversPath)/sizeof(DriversPath[0]) );
		
		if ( !ExtractFile(osvi.dwMajorVersion == 4?IDR_KLICKNT_BINARY:IDR_KLICK2K_BINARY, DriversPath) )
			return false;
		
		HANDLE hKL1 = FindPluginDriver (_T("\\\\.\\kl1"));
		
		//install klick normally
		if (hKL1==INVALID_HANDLE_VALUE)
		{
			return InstallService(_T("klick"), osvi.dwMajorVersion == 4?1:0, DriversPath);
		}
		else
		{
			HKEY hParametersKey;
			if ( ERROR_SUCCESS == RegOpenKeyEx (
				HKEY_LOCAL_MACHINE, 
				_T("SYSTEM\\CurrentControlSet\\Services\\kl1\\Parameters"),
				0,
				KEY_ALL_ACCESS, 
				&hParametersKey) )
			{
				if ( ERROR_SUCCESS == RegSetValueEx (
					hParametersKey,    // subkey handle 
					_T("klick"),				       // value name 
					0,						       // must be zero 
					REG_SZ,						   // value type 
					(LPBYTE) DriversPath,				// pointer to value data 
					(DWORD) (_tcslen(DriversPath)+1)*sizeof(TCHAR)) // length of value data 
					)
				{
					CloseHandle (hKL1);		
					return true;
				}

				RegCloseKey (hParametersKey);
			}
			
			CloseHandle (hKL1);			  
		}
	}

	return false;
	
}

bool InstallService(LPCTSTR szServiceName, DWORD dwStartType, LPCTSTR szServiceBinary)
{
	SC_HANDLE schSCManager = OpenSCManager (NULL, NULL, SC_MANAGER_ALL_ACCESS);

	if (!schSCManager)
		return false;

	SC_HANDLE schService = NULL;
	
	for (int i = 0; i<10 && !schService; ++i)
	{
		schService = CreateService (schSCManager,		// SCManager database
			szServiceName,		// name of service
			szServiceName,		// name to display
			SERVICE_ALL_ACCESS,	// desired access
			1,		// service type
			dwStartType,		// start type
			SERVICE_ERROR_NORMAL,	// error control type
			szServiceBinary,		// service's binary
			NULL,		// ordering group
			NULL,// tag identifier
			NULL,// dependencies
			NULL,			// account
			NULL);			// password

		if (schService || 
			(!schService && GetLastError() == ERROR_SERVICE_EXISTS))
		{
			CloseServiceHandle (schSCManager);
			if (schService)
				CloseServiceHandle (schService);
			
			return true;
		}
		else
			Sleep(10);
	}

    CloseServiceHandle (schSCManager);

	return false;
}

bool StopService( LPCTSTR szServiceName )
{
	SC_HANDLE schSCManager = OpenSCManager (NULL, NULL, SC_MANAGER_ALL_ACCESS);
	
	if (!schSCManager)
		return false;
	
	SC_HANDLE schService = OpenService (schSCManager, szServiceName, SERVICE_ALL_ACCESS);
	
	if (!schService)
	{
		CloseServiceHandle (schSCManager);
		return false;
	}
	
	SERVICE_STATUS serviceStatus;
	BOOL rc = ControlService (schService, SERVICE_CONTROL_STOP, &serviceStatus);

	CloseServiceHandle (schService);
	CloseServiceHandle (schSCManager);

	return rc == TRUE;
}

bool StartService( LPCTSTR szServiceName )
{
	SC_HANDLE schSCManager = OpenSCManager (NULL, NULL, SC_MANAGER_ALL_ACCESS);
	
	if (!schSCManager)
		return false;
	
	SC_HANDLE schService = OpenService (schSCManager, szServiceName, SERVICE_ALL_ACCESS);
	
	if (!schService)
	{
		CloseServiceHandle (schSCManager);
		return false;
	}
	
	BOOL rc = StartService (schService, 0, NULL) || ERROR_SERVICE_ALREADY_RUNNING == GetLastError ();

	CloseServiceHandle (schService);
	CloseServiceHandle (schSCManager);

	return rc == TRUE;
}

bool RemoveService(LPCTSTR szServiceName)
{
	SC_HANDLE schSCManager = OpenSCManager (NULL, NULL, SC_MANAGER_ALL_ACCESS);
	
	if (!schSCManager)
		return false;

	SC_HANDLE schService = OpenService (schSCManager, szServiceName, SERVICE_ALL_ACCESS);

	if (NULL == schService)
	{
		CloseServiceHandle (schSCManager);
		return false;
	}

	BOOL rc = DeleteService (schService);
	
	CloseServiceHandle (schService);
	CloseServiceHandle (schSCManager);

	return rc == TRUE;

}


#define LOGGER_NAME _T("Logger")

UnregLogger::UnregLogger() : m_hLogger(INVALID_HANDLE_VALUE)
{
	m_szDriverPath[0] = _T('\0');
}

UnregLogger::~UnregLogger()
{
	if (m_hLogger != INVALID_HANDLE_VALUE)
		CloseHandle(m_hLogger);

	StopService (LOGGER_NAME);
	RemoveService(LOGGER_NAME);
	
	if (_tcslen(m_szDriverPath))
		DeleteFile(m_szDriverPath);
}
	
bool UnregLogger::Install()
{
	OSVERSIONINFO osvi;
	osvi.dwOSVersionInfoSize = sizeof ( osvi );
	GetVersionEx ( &osvi );
	
	if ( osvi.dwPlatformId == VER_PLATFORM_WIN32_WINDOWS )
	{
		//9x
		//		  ExtractFile(IDR_KLICK9x_BINARY, "klick.vxd");
	}
	else
	{
		ExpandEnvironmentStrings( _T("%SystemRoot%\\system32\\drivers\\Logger.sys") , m_szDriverPath, sizeof(m_szDriverPath)/sizeof(m_szDriverPath[0]) );
		
		if ( !ExtractFile(IDR_LOGGERNT_BINARY, m_szDriverPath) )
			return false;
		
		if (InstallService(LOGGER_NAME, 3, m_szDriverPath))
		{
			StartService(LOGGER_NAME);
			m_hLogger = FindPluginDriver(_T("\\\\.\\") LOGGER_NAME);
		}
	}
	
	return m_hLogger != INVALID_HANDLE_VALUE;
}

void UninstallKlick()
{
	HANDLE hKLIN = FindPluginDriver (_T("\\\\.\\klin"));
	
	if (hKLIN == INVALID_HANDLE_VALUE)
	{
		TCHAR DriversPath[MAX_PATH] = {0};
		ExpandEnvironmentStrings( _T("%SystemRoot%\\system32\\drivers\\klick.sys"), DriversPath, sizeof(DriversPath)/sizeof(DriversPath[0]) );
		
		HANDLE hKL1 = FindPluginDriver (_T("\\\\.\\kl1"));
		
		//remove klick as service
		if (hKL1==INVALID_HANDLE_VALUE)
		{
			RemoveService(_T("klick"));
			DeleteFile(DriversPath);
		}
		else
		{
			HKEY hParametersKey;
			if ( ERROR_SUCCESS == RegOpenKeyEx (
				HKEY_LOCAL_MACHINE, 
				_T("SYSTEM\\CurrentControlSet\\Services\\kl1\\Parameters"),
				0,
				KEY_ALL_ACCESS, 
				&hParametersKey) )
			{
				RegDeleteValue (
					hParametersKey,    // subkey handle 
					_T("klick"));

				RegCloseKey (hParametersKey);
			}
			
			CloseHandle (hKL1);			  
		}
		
		printf("\"klick\" removed. Need reboot.");
	}
	else
	{
		printf("\"klick\" used by another product. Remove unsafe. Don't removed");
		CloseHandle (hKLIN);
	}

}
