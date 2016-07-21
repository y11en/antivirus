/////////////////////////////////////////////////////////////////////////////
// Copyright (C) 1997 by Joerg Koenig and the ADG mbH, Mannheim, Germany
// All rights reserved
//
// Distribute freely, except: don't remove my name from the source or
// documentation (don't take credit for my work), mark your changes (don't
// get me blamed for your possible bugs), don't alter or remove this
// notice.
// No warrantee of any kind, express or implied, is included with this
// software; use at your own risk, responsibility for damages (if any) to
// anyone resulting from the use of this software rests entirely with the
// user.
//
// Send bug reports, bug fixes, enhancements, requests, flames, etc., and
// I'll try to keep a version up to date.  I can be reached as follows:
//    J.Koenig@adg.de                 (company site)
//    Joerg.Koenig@rhein-neckar.de    (private site)
/////////////////////////////////////////////////////////////////////////////
//
// MODIFIED BY TODD C. WILSON FOR THE ROAD RUNNER NT LOGIN SERVICE.
// HOWEVER, THESE MODIFICATIONS ARE BROADER IN SCOPE AND USAGE AND CAN BE USED
// IN OTHER PROJECTS WITH NO CHANGES.
// MODIFIED LINES FLAGGED/BRACKETED BY "//!! TCW MOD"
//
/////////////////////////////////////////////////////////////////////////////


// last revised: $Date: 2003/07/23 13:15:30 $, $Revision: 1.1.1.1 $


/////////////////////////////////////////////////////////////////////////////
// Acknoledgements:
//	o	Thanks to Victor Vogelpoel (VictorV@Telic.nl) for his bug-fixes
//		and enhancements.
//	o	Thanks to Todd C. Wilson (todd@mediatec.com) for the
//		"service" on Win95
//
// Changes:
//	01/21/99
//	o	Bug fixed in "DeregisterApplicationLog()"
//		thanks to Grahame Willis (grahamew@questsoftware.com.au):
//
//	04/30/98
//	o	Added two more switches to handle command line arguments:
//		-e will force a running service to stop (corresponding
//		method in this class: virtual BOOL EndService();) and
//		-s will force the service to start (method:
//		virtual BOOL StartupService())
//
//	02/05/98
//	o	Added the methods "RegisterApplicationLog()" and
//		"DeregisterApplicationLog()" (both virtual). The first one will be
//		called from "InstallService()" and creates some registry-entries
//		for a better event-log. The second one removes these entries when
//		the service will uninstall (see "RemoveService()")
//	o	The service now obtains the security identifier of the current user
//		and uses this SID for event-logging.
//	o	The memory allocated by "CommandLineToArgvW()" will now release
//		(UNICODE version only)
//	o	The service now uses a simple message catalogue for a nicer
//		event logging
#if defined (_WIN32)
#include <windows.h>
#include <tchar.h>
#include <stdio.h>
#include <crtdbg.h>
#include <prague.h>
#include <iface/i_root.h>

#include <io.h>			//!! TCW MOD
#include <fcntl.h>		//!! TCW MOD

#include "NTService.h"
#include "NTServiceEventLogMsg.h"

#include "ver_mod.h"

#ifndef RSP_SIMPLE_SERVICE
	#define RSP_SIMPLE_SERVICE 1
#endif
#ifndef RSP_UNREGISTER_SERVICE
	#define RSP_UNREGISTER_SERVICE 0
#endif

BOOL CNTService :: m_bInstance = FALSE;

static CNTService * gpTheService = 0;			// the one and only instance

CNTService * AfxGetService() { return gpTheService; }

#define KLCS_MODULENAME L"NTService"


static LPCTSTR gszAppRegKey = TEXT("SYSTEM\\CurrentControlSet\\Services\\EventLog\\Application\\");
static LPCTSTR gszWin95ServKey=TEXT("Software\\Microsoft\\Windows\\CurrentVersion\\RunServices");	//!! TCW MOD
static LPCTSTR gszRunKey=TEXT("Software\\Microsoft\\Windows\\CurrentVersion\\Run");


/////////////////////////////////////////////////////////////////////////////
// class CNTService -- construction/destruction

CNTService :: CNTService( LPCTSTR lpServiceName, LPCTSTR lpDisplayName , LPCTSTR lpDescription)
	: m_lpServiceName(lpServiceName)
	, m_lpDisplayName(lpDisplayName ? lpDisplayName : lpServiceName)
	, m_lpDescription(lpDescription)
	, m_dwCheckPoint(0)
	, m_dwErr(0)
	, m_bDebug(FALSE)
	, m_sshStatusHandle(0)
	, m_dwControlsAccepted(SERVICE_ACCEPT_STOP)
	, m_pUserSID(0)
	, m_fConsoleReady(FALSE)
	// parameters to the "CreateService()" function:
	, m_dwDesiredAccess(SERVICE_ALL_ACCESS)
	, m_dwServiceType(SERVICE_WIN32_OWN_PROCESS)
	, m_dwStartType(SERVICE_AUTO_START)
	, m_dwErrorControl(SERVICE_ERROR_NORMAL)
	, m_pszLoadOrderGroup(0)
	, m_dwTagID(0)
	, m_pszDependencies(0)
	, m_pszStartName(0)
	, m_pszPassword(0)
	, m_bWaitConsole(FALSE)
	, m_bShowConsole(FALSE)
	, m_pszConsoleDevice(NULL)
{
	_ASSERTE( ! m_bInstance );
	InitializeCriticalSection(&m_status_cs);

	OSVERSIONINFO vi;
	vi.dwOSVersionInfoSize=sizeof(vi);  // init this.
	GetVersionEx(&vi);      //lint !e534
	m_bWinNT = (vi.dwPlatformId == VER_PLATFORM_WIN32_NT);
	m_bWinNT4 = FALSE;

	if (m_bWinNT)
	{
		if ((4 == vi.dwMajorVersion) && (0 == vi.dwMinorVersion))
			m_bWinNT4 = TRUE;
	}

	m_bInstance = TRUE;
	gpTheService = this;
	
	// SERVICE_STATUS members that rarely change
	m_ssStatus.dwServiceType = SERVICE_WIN32_OWN_PROCESS;
	m_ssStatus.dwServiceSpecificExitCode = 0;

	if( m_bWinNT ) {
		/////////////////////////////////////////////////////////////////////////
		// Providing a SID (security identifier) was contributed by Victor
		// Vogelpoel (VictorV@Telic.nl).
		// The code from Victor was slightly modified.

		// Get security information of current user
		BYTE	security_identifier_buffer[ 4096 ];
		DWORD	dwSizeSecurityIdBuffer = sizeof( security_identifier_buffer );
		PSID	user_security_identifier = NULL;

		TCHAR sUserName[ 256 ];
		DWORD dwSizeUserName  =  255;

		TCHAR sDomainName[ 256 ];
		DWORD dwSizeDomainName = 255;

		SID_NAME_USE sidTypeSecurityId;

		::ZeroMemory( sUserName, sizeof( sUserName ) );
		::ZeroMemory( sDomainName, sizeof( sDomainName ) );
		::ZeroMemory( security_identifier_buffer, dwSizeSecurityIdBuffer );

		::GetUserName( sUserName, &dwSizeUserName );

		if( ::LookupAccountName(
					0,
					sUserName,
					&security_identifier_buffer,
					&dwSizeSecurityIdBuffer,
					sDomainName,
					&dwSizeDomainName,
					&sidTypeSecurityId
				)) {
			if( ::IsValidSid( PSID(security_identifier_buffer) ) ) {
				DWORD dwSidLen = ::GetLengthSid(PSID(security_identifier_buffer));
				m_pUserSID = PSID(new BYTE [dwSidLen]);
				::CopySid(dwSidLen, m_pUserSID, security_identifier_buffer);
				_ASSERTE(::EqualSid(m_pUserSID, security_identifier_buffer));
			}
		}
	}
	
	if (!m_bWinNT || m_bWinNT4)
	{
		LoadLibrary("OLEAUT32.DLL"); // fix it in memory, overwise it caused exception if quickly loaded/freed by static linking with some other module.
	}
	/////////////////////////////////////////////////////////////////////////
}


CNTService :: ~CNTService() {
	_ASSERTE( m_bInstance );
	delete [] LPBYTE(m_pUserSID);
	m_bInstance = FALSE;
	gpTheService = 0;
	DeleteCriticalSection(&m_status_cs);
}



/////////////////////////////////////////////////////////////////////////////
// class CNTService -- overridables

#define NEXT_ARG ((((*Argv)[2])==TEXT('\0'))?(--Argc,*++Argv):(*Argv)+2)


BOOL CNTService :: RegisterService( int argc, TCHAR ** argv ) {
	BOOL (CNTService::* fnc)() = &CNTService::StartDispatcher;
    DWORD Argc;
    LPTSTR * Argv;

	Argc = (DWORD) argc;
	Argv = argv;

    while( ++Argv, --Argc ) {
		if( Argv[0][0] == TEXT('-')  || Argv[0][0] == TEXT('/')) {
			switch( Argv[0][1] ) {
				case TEXT('i'):	// install the service
					fnc = &CNTService::InstallService;
					break;
				case TEXT('l'):	// login-account (only useful with -i)
					m_pszStartName = NEXT_ARG;
					break;
				case TEXT('p'):	// password (only useful with -i)
					m_pszPassword = NEXT_ARG;
					break;
//				case TEXT('u'):	// uninstall the service
//					fnc = &CNTService::RemoveService;
//					break;
				case TEXT('s'):	// start the service
					fnc = &CNTService::StartupService;
					break;
//				case TEXT('e'):	// end the service
//					fnc = &CNTService::EndService;
//					break;
				case TEXT('d'):	// debug the service
				case TEXT('f'):	//!! TCW MOD faceless non-service (Win95) mode
					m_bDebug = TRUE;
					// pass original parameters to DebugService()
					return DebugService(argc, argv,(Argv[0][1]==TEXT('f'))); //!! TCW MOD faceless non-service (Win95) mode
			}
		}
	}

	//!! TCW MOD START - if Win95, run as faceless app.
	if( fnc == &CNTService::StartDispatcher && OsIsWin95() ) {
		// act as if -f was passed anyways.
		m_bDebug = TRUE;
		return DebugService(argc, argv, TRUE);
	}
	//!! TCW MOD END - if Win95, run as faceless app.

	return (this->*fnc)();
}



BOOL CNTService :: StartDispatcher() {
    // Default implementation creates a single threaded service.
	// Override this method and provide more table entries for
	// a multithreaded service (one entry for each thread).
	SERVICE_TABLE_ENTRY dispatchTable[] =
    {
        { LPTSTR(m_lpServiceName), (LPSERVICE_MAIN_FUNCTION)ServiceMain },
        { 0, 0 }
    };

	BOOL bRet = StartServiceCtrlDispatcher(dispatchTable);
	if( ! bRet ) {
		TCHAR szBuf[256];
        AddToMessageLog(GetLastErrorText(szBuf,255));
	}

	return bRet;
}


BOOL CNTService :: RegisterGUI (CHAR* szProductName, BOOL bRegister) 
{
	BOOL bRet = FALSE;

	HKEY hKey = 0;
	LONG lRet = ERROR_SUCCESS;
	CHAR szTranslatedProductName[30+1];
	
	if (m_bWinNT)
	{
		OSVERSIONINFO vi;
		vi.dwOSVersionInfoSize=sizeof(vi);  // init this.
		GetVersionEx(&vi);
		if ( vi.dwMajorVersion <= 4 ) // Microsoft Windows NT
		{
			int len = 0;
			CHAR c;
			while (c = *szProductName)
			{
				if ((c >= 'A' && c <='Z') || (c >= 'a' && c <='z'))
				{
					szTranslatedProductName[len] = c;
					len++;
					if (len == sizeof(szTranslatedProductName))
						break;
				}
				szProductName++;
			}
			if (!len)
				return FALSE;
			szTranslatedProductName[len] = 0;
			szProductName = szTranslatedProductName;
		}
	}
	if( ::RegCreateKey(HKEY_LOCAL_MACHINE, gszRunKey , &hKey) == ERROR_SUCCESS ) 
	{
		if (bRegister)
		{
			TCHAR szPath[1024];
			if( GetModuleFileName( 0, & szPath[1], 1000 ) == 0 ) 
			{
				TCHAR szErr[256];
				_tprintf(TEXT("Unable to (un)register %s GUI - %s\r\n"), szProductName, GetLastErrorText(szErr, 256));
				return FALSE;
			}
			szPath[0]='\"';
			_tcscat(szPath,_T("\""));
			
			lRet =	::RegSetValueEx(
						hKey,				// handle of key to set value for
						szProductName,	    // address of value to set (NAME OF PRODUCT)
						0,					// reserved
						REG_SZ,				// flag for value type
						(CONST BYTE*)szPath,// address of value data
						(_tcslen(szPath) + 1)*sizeof(*szPath)	// size of value data
					);
		}
		else
		{
			lRet = ::RegDeleteValue(hKey, szProductName);
			if (lRet == ERROR_FILE_NOT_FOUND)
				lRet = ERROR_SUCCESS;
		}
		::RegCloseKey(hKey);
		bRet = (lRet == ERROR_SUCCESS);
	}

	return bRet;
}


BOOL CNTService :: InstallService() 
{
	if(m_bShowConsole)
		SetupConsole();
					//!! TCW MOD - have to show the console here for the
					// diagnostic or error reason: orignal class assumed
					// that we were using _main for entry (a console app).
					// This particular usage is a Windows app (no console),
					// so we need to create it. Using SetupConsole with _main
					// is ok - does nothing, since you only get one console.

    TCHAR szPath[1024];
	if( GetModuleFileName( 0, & szPath[1], 1000 ) == 0 ) 
	{
//		TCHAR szErr[256];
//		_tprintf(TEXT("Unable to install %s - %s\r\n"), m_lpDisplayName, GetLastErrorText(szErr, 256));
		return FALSE;
	}
	szPath[0]='\"';
	_tcscat(szPath,_T("\" -r"));

	BOOL bRet = FALSE;

	if( OsIsWin95() ) {	//!! TCW MOD - code added to install as Win95 service
		// Create a key for that application and insert values for
		// "EventMessageFile" and "TypesSupported"
		HKEY hKey = 0;
		LONG lRet = ERROR_SUCCESS;
		if( ::RegCreateKey(HKEY_LOCAL_MACHINE, gszWin95ServKey , &hKey) == ERROR_SUCCESS ) {
			lRet =	::RegSetValueEx(
						hKey,				// handle of key to set value for
						m_lpServiceName,	// address of value to set (NAME OF SERVICE)
						0,					// reserved
						REG_EXPAND_SZ,		// flag for value type
						(CONST BYTE*)szPath,// address of value data
						(_tcslen(szPath) + 1)*sizeof(*szPath)	// size of value data
					);
			::RegCloseKey(hKey);
			bRet=TRUE;
		}
	} else {
		// Real NT services go here.
		SC_HANDLE schSCManager =	OpenSCManager(
										0,						// machine (NULL == local)
										0,						// database (NULL == default)
										SC_MANAGER_ALL_ACCESS	// access required
									);
		if( schSCManager ) {
			SC_HANDLE schService =	CreateService(
										schSCManager,
										m_lpServiceName,
										m_lpDisplayName,
										m_dwDesiredAccess,
										m_dwServiceType,
										m_dwStartType,
										m_dwErrorControl,
										szPath,
										m_pszLoadOrderGroup,
										((m_dwServiceType == SERVICE_KERNEL_DRIVER ||
										  m_dwServiceType == SERVICE_FILE_SYSTEM_DRIVER) &&
										 (m_dwStartType == SERVICE_BOOT_START ||
										  m_dwStartType == SERVICE_SYSTEM_START)) ?
											&m_dwTagID : 0,
										m_pszDependencies,
										m_pszStartName,
										m_pszPassword
									);

			if( schService ) {
//				_tprintf(TEXT("%s installed.\r\n"), m_lpDisplayName );
				if(m_lpDescription)
				{
					SERVICE_DESCRIPTION l_hServiceDescription;
					l_hServiceDescription.lpDescription=(LPSTR)m_lpDescription;
					HMODULE l_hAdvapi=LoadLibrary("ADVAPI32.DLL");
					if(l_hAdvapi != NULL){
						typedef BOOL (WINAPI _ChangeServiceConfig2)(SC_HANDLE,DWORD,LPVOID);
						_ChangeServiceConfig2* l_pChangeServiceConfig2;
						l_pChangeServiceConfig2 =(_ChangeServiceConfig2*)GetProcAddress(l_hAdvapi,"ChangeServiceConfig2A");
						if(l_pChangeServiceConfig2)
							(*l_pChangeServiceConfig2)(schService,SERVICE_CONFIG_DESCRIPTION,&l_hServiceDescription);
						FreeLibrary(l_hAdvapi);
					}
				}
				CloseServiceHandle(schService);
				bRet = TRUE;
			} else {
//				TCHAR szErr[256];
//				_tprintf(TEXT("CreateService failed - %s\r\n"), GetLastErrorText(szErr, 256));
			}
			CloseServiceHandle(schSCManager);
		 } else {
//			TCHAR szErr[256];
//			_tprintf(TEXT("OpenSCManager failed - %s\r\n"), GetLastErrorText(szErr,256));
		}

/*
		if( bRet ) {
			// installation succeeded. Now register the message file
			GetModuleFileName( 0, szPath, 1023 );
			RegisterApplicationLog(
				szPath,		// the path to the application itself
				EVENTLOG_ERROR_TYPE | EVENTLOG_WARNING_TYPE | EVENTLOG_INFORMATION_TYPE // supported types
			);
			AddToMessageLog(TEXT("Service installed"),EVENTLOG_INFORMATION_TYPE);
		}
*/
	}	//!! TCW MOD

	return bRet;
}

BOOL CNTService :: CheckServiceAccessRights(DWORD dwServiceAccessRights) {
	BOOL bRet = FALSE;
	if( OsIsWin95() ) {	//!! TCW MOD - code added to install as Win95 service
		HKEY hKey = 0;
		LONG lRet = ERROR_SUCCESS;
		if( ::RegCreateKey(HKEY_LOCAL_MACHINE, gszWin95ServKey , &hKey) == ERROR_SUCCESS ) {
			::RegCloseKey(hKey);
			bRet=TRUE;
		}
	} 
	else 
	{
		// Real NT services go here.
		SC_HANDLE schSCManager = OpenSCManager(
									0,						// machine (NULL == local)
									0,						// database (NULL == default)
									SC_MANAGER_ALL_ACCESS	// access required
								);
		if( schSCManager ) {
			SC_HANDLE schService =	OpenService(
										schSCManager,
										m_lpServiceName,
										dwServiceAccessRights
									);

			if( schService ) {
				bRet = TRUE;
				CloseServiceHandle(schService);
			} else {
//				TCHAR szErr[256];
//				_tprintf(TEXT("OpenService failed - %s\r\n"), GetLastErrorText(szErr,256));
			}

			  CloseServiceHandle(schSCManager);
		 } else {
//			TCHAR szErr[256];
//			_tprintf(TEXT("OpenSCManager failed - %s\r\n"), GetLastErrorText(szErr,256));
		}
	}
	return bRet;
}


BOOL CNTService :: RemoveService() {
	BOOL bRet = FALSE;

	if(m_bShowConsole)
		SetupConsole();
					//!! TCW MOD - have to show the console here for the
					// diagnostic or error reason: orignal class assumed
					// that we were using _main for entry (a console app).
					// This particular usage is a Windows app (no console),
					// so we need to create it. Using SetupConsole with _main
					// is ok - does nothing, since you only get one console.


	if( OsIsWin95() ) {	//!! TCW MOD - code added to install as Win95 service
		HKEY hKey = 0;
		LONG lRet = ERROR_SUCCESS;
		if( ::RegCreateKey(HKEY_LOCAL_MACHINE, gszWin95ServKey , &hKey) == ERROR_SUCCESS ) {
			lRet = ::RegDeleteValue(hKey, m_lpServiceName);
			::RegCloseKey(hKey);
			bRet=TRUE;
		}
	} else {
		// Real NT services go here.
		
		SC_HANDLE schSCManager = OpenSCManager(
									0,						// machine (NULL == local)
									0,						// database (NULL == default)
									SC_MANAGER_ALL_ACCESS	// access required
								);
		if( schSCManager ) {
			SC_HANDLE schService =	OpenService(
										schSCManager,
										m_lpServiceName,
										SERVICE_ALL_ACCESS
									);

			if( schService ) {
				// try to stop the service
				if( ControlService(schService, SERVICE_CONTROL_STOP, &m_ssStatus) ) {
					_tprintf(TEXT("Stopping %s."), m_lpDisplayName);
					Sleep(1000);

					while( QueryServiceStatus(schService, &m_ssStatus) ) {
						if( m_ssStatus.dwCurrentState == SERVICE_STOP_PENDING ) {
							_tprintf(TEXT("."));
							Sleep( 1000 );
						} else
							break;
					}

					if( m_ssStatus.dwCurrentState == SERVICE_STOPPED )
						_tprintf(TEXT("\n%s stopped.\r\n"), m_lpDisplayName);
					else
						_tprintf(TEXT("\n%s failed to stop.\r\n"), m_lpDisplayName);
				}

				// now remove the service
				if( DeleteService(schService) ) {
//					_tprintf(TEXT("%s removed.\r\n"), m_lpDisplayName);
					bRet = TRUE;
				} else {
//					TCHAR szErr[256];
//					_tprintf(TEXT("DeleteService failed - %s\r\n"), GetLastErrorText(szErr,256));
				}

				CloseServiceHandle(schService);
			} else {
//				TCHAR szErr[256];
//				_tprintf(TEXT("OpenService failed - %s\r\n"), GetLastErrorText(szErr,256));
			}

			  CloseServiceHandle(schSCManager);
		 } else {
//			TCHAR szErr[256];
//			_tprintf(TEXT("OpenSCManager failed - %s\r\n"), GetLastErrorText(szErr,256));
		}
/*
		if( bRet )
			DeregisterApplicationLog();
*/
	}

	return bRet;
}

BOOL CNTService :: SetServiceStartType(DWORD dwStartType, DWORD dwTimeout, DWORD* pWin32Error)
{ 
//#define ReportError(err_text) { _tprintf(TEXT(err_text " failed with %s\r\n"), GetLastErrorText(szErr,256)); } 
#define ReportError(err_text)// { _tprintf(TEXT(err_text " failed with %s\r\n"), GetLastErrorText(szErr,256)); } 
	BOOL bRet = FALSE;
//	TCHAR szErr[256];
	
	if (pWin32Error)
		*pWin32Error = ERROR_SUCCESS;
	if (OsIsWin95())
	{
		switch (dwStartType)
		{
		case SERVICE_BOOT_START   :
		case SERVICE_SYSTEM_START :
		case SERVICE_AUTO_START   :
			bRet = InstallService();
			break;
		case SERVICE_DEMAND_START :
		case SERVICE_DISABLED     :
			bRet = RemoveService();
			break;
		default:
			_tprintf(TEXT("SetServiceStartType failed - unknown Start Type (%d)\r\n"), dwStartType);
			if (pWin32Error)
				*pWin32Error = ERROR_INVALID_PARAMETER;
			return FALSE;
		}
		if (pWin32Error)
			*pWin32Error = GetLastError();
		return bRet;
	}

	// WinNT+

	// first try to change it directly in registry - resolving SCM vs. SelfProtection conflict issue
	{
		HKEY hKeyServices, hKeyService;
		if (ERROR_SUCCESS == RegOpenKeyEx(HKEY_LOCAL_MACHINE, TEXT("SYSTEM\\CurrentControlSet\\Services"), NULL, KEY_ALL_ACCESS, &hKeyServices))
		{
			PR_TRACE((g_root, prtIMPORTANT, "kav\tservices key opened"));
			if (ERROR_SUCCESS == RegOpenKeyEx(hKeyServices, m_lpServiceName, NULL, KEY_ALL_ACCESS, &hKeyService))
			{
				PR_TRACE((g_root, prtIMPORTANT, "kav\tservice %s key opened", m_lpServiceName));
				if (ERROR_SUCCESS == RegSetValueEx(hKeyService, "Start", 0, REG_DWORD, (LPBYTE)&dwStartType, sizeof(dwStartType)))
				{
					DWORD dwStart;
					DWORD dwType = REG_DWORD;
					DWORD dwSize = sizeof(DWORD);
					if (ERROR_SUCCESS == RegQueryValueEx(hKeyService, "Start", 0, &dwType, (LPBYTE)&dwStart, &dwSize))
					{
						if (dwStart == dwStartType)
						{
							PR_TRACE((g_root, prtIMPORTANT, "kav\tservice start changed to %d (%s)", dwStartType,
								dwStartType == SERVICE_AUTO_START ? "auto" :
								dwStartType == SERVICE_BOOT_START ? "boot" :
								dwStartType == SERVICE_DEMAND_START ? "manual" :
								dwStartType == SERVICE_DISABLED ? "disabled" :
								dwStartType == SERVICE_SYSTEM_START ? "system" :
								"unknown"
							));
							bRet = TRUE;
						}
						else
						{
							PR_TRACE((g_root, prtERROR, "kav\tverify failed: queried value(%d) != expected(%d)", dwStart, dwStartType));
						}
					}
					else
					{
						PR_TRACE((g_root, prtERROR, "kav\tquery start failed with %d", GetLastError()));
					}
				}
				else
				{
					PR_TRACE((g_root, prtERROR, "kav\tsetvalue failed with %d", GetLastError()));
				}
				RegCloseKey(hKeyService);
			}
			else
			{
				PR_TRACE((g_root, prtERROR, "kav\tservice %s key open failed with %d", m_lpServiceName, GetLastError()));
			}
			RegCloseKey(hKeyServices);
		}
		else
		{
			PR_TRACE((g_root, prtERROR, "kav\tservices key open failed with %d", GetLastError()));
		}
	}
	if (bRet)
		return bRet;
	
	SC_HANDLE schSCManager = ::OpenSCManager(
								0,						// machine (NULL == local)
								0,						// database (NULL == default)
								SC_MANAGER_ALL_ACCESS	// access required
							);
	if( !schSCManager )
	{
		if (pWin32Error)
			*pWin32Error = GetLastError();
		ReportError("OpenSCManager");
		return FALSE;
	}
	else
	{
		SC_LOCK sclLock; 
		DWORD _time = GetTickCount();
		// Need to acquire database lock before reconfiguring. 
		while (true) {
			sclLock = ::LockServiceDatabase(schSCManager);
			if (sclLock)
				break;
			if (pWin32Error)
				*pWin32Error = GetLastError();
			if (GetTickCount() - _time > dwTimeout)
				break;
			Sleep(100);
		};
		if (sclLock) 
		{
			// Open a handle to the service. 
			SC_HANDLE schService = ::OpenService( 
				schSCManager,           // SCManager database 
				m_lpServiceName,           // name of service 
				SERVICE_CHANGE_CONFIG); // need CHANGE access 
			if (schService == NULL) 
			{
				if (pWin32Error)
					*pWin32Error = GetLastError();
				ReportError("OpenService"); 
			}
			else
			{
				// Make the changes. 
				if (! ::ChangeServiceConfig( 
					schService,        // handle of service 
					SERVICE_NO_CHANGE, // service type: no change 
					dwStartType,       // change service start type 
					SERVICE_NO_CHANGE, // error control: no change 
					NULL,              // binary path: no change 
					NULL,              // load order group: no change 
					NULL,              // tag ID: no change 
					NULL,              // dependencies: no change 
					NULL,              // account name: no change 
					NULL,              // password: no change 
					NULL) )            // display name: no change
				{
					if (pWin32Error)
						*pWin32Error = GetLastError();
					ReportError("ChangeServiceConfig"); 
				}
				else 
				{
					//printf("ChangeServiceConfig SUCCESS\r\n"); 
					bRet = TRUE;
				}

				// Close the handle to the service. 
				::CloseServiceHandle(schService); 
			}

			// Release the database lock. 
			::UnlockServiceDatabase(sclLock); 
		}
			
		// Close the handle to the service manager. 
		::CloseServiceHandle(schSCManager); 
	}


#undef ReportError
	return bRet;
}


//BOOL CNTService :: EndService() {
//	BOOL bRet = FALSE;
//
//	SC_HANDLE schSCManager = ::OpenSCManager(
//								0,						// machine (NULL == local)
//								0,						// database (NULL == default)
//								SC_MANAGER_ALL_ACCESS	// access required
//							);
//	if( schSCManager ) {
//		SC_HANDLE schService =	::OpenService(
//									schSCManager,
//									m_lpServiceName,
//									SERVICE_ALL_ACCESS
//								);
//
//		if( schService ) {
//			// try to stop the service
//			if( ::ControlService(schService, SERVICE_CONTROL_STOP, &m_ssStatus) ) {
//				_tprintf(TEXT("Stopping %s."), m_lpDisplayName);
//				::Sleep(1000);
//
//				while( ::QueryServiceStatus(schService, &m_ssStatus) ) {
//					if( m_ssStatus.dwCurrentState == SERVICE_STOP_PENDING ) {
//						_tprintf(TEXT("."));
//						::Sleep( 1000 );
//					} else
//						break;
//				}
//
//				if( m_ssStatus.dwCurrentState == SERVICE_STOPPED )
//					bRet = TRUE, _tprintf(TEXT("\n%s stopped.\r\n"), m_lpDisplayName);
//                else
//                    _tprintf(TEXT("\n%s failed to stop.\r\n"), m_lpDisplayName);
//			}
//
//			::CloseServiceHandle(schService);
//		} else {
//			TCHAR szErr[256];
//			_tprintf(TEXT("OpenService failed - %s\r\n"), GetLastErrorText(szErr,256));
//		}
//
//        ::CloseServiceHandle(schSCManager);
//    } else {
//		TCHAR szErr[256];
//		_tprintf(TEXT("OpenSCManager failed - %s\r\n"), GetLastErrorText(szErr,256));
//	}
//
//	return bRet;
//}

extern CHAR  g_sProcessName[];

BOOL CNTService :: StartupService() {
	BOOL bRet = FALSE;
	m_dwErr = NO_ERROR;

	if(OsIsWin95())
	{
		CHAR CommandLine[MAX_PATH+1];
		PROCESS_INFORMATION ProcessInfo;
		STARTUPINFO si;
		memset(&si, 0, sizeof(si));
		si.cb = sizeof(si);

		wsprintf( CommandLine,"\"%s\" -r",g_sProcessName);
		bRet=CreateProcess(NULL, CommandLine, NULL, NULL, 0, 0, NULL, NULL, &si, &ProcessInfo);
		m_dwErr = GetLastError();
	}
	else
	{
		SC_HANDLE schSCManager = ::OpenSCManager( NULL ,	// machine (NULL == local)
									NULL ,					// database (NULL == default)
									READ_CONTROL);	// access required
		if( !schSCManager ) 
			m_dwErr = GetLastError();
		else
		{
			SC_HANDLE schService =	::OpenService(schSCManager, m_lpServiceName, READ_CONTROL | SERVICE_QUERY_STATUS | SERVICE_INTERROGATE | SERVICE_START);
			if( !schService ) 
				m_dwErr = GetLastError();
			else
			{
				// try to start the service
				_tprintf(TEXT("Starting up %s."), m_lpDisplayName);
				if( !::StartService(schService, 0, 0) ) 
				{
					m_dwErr = GetLastError();
					_tprintf(TEXT("\nStartService for %s failed with %d.\r\n"), m_lpDisplayName, m_dwErr);
				}
				else
				{
					int counter = 0;
					do 
					{
						if ((++counter % 10) == 0)
							_tprintf(TEXT("."));
						Sleep(100);
						if (!::QueryServiceStatus(schService, &m_ssStatus))
							break;
					} while (m_ssStatus.dwCurrentState == SERVICE_START_PENDING);

					if( m_ssStatus.dwCurrentState != SERVICE_RUNNING )
					{
						_tprintf(TEXT("\nService %s state is %d.\r\n"), m_lpDisplayName, m_ssStatus.dwCurrentState);
						m_dwErr = GetLastError();
					}
					else
					{
						_tprintf(TEXT("\nService %s started.\r\n"), m_lpDisplayName);
						bRet = TRUE;
					}
				}
				::CloseServiceHandle(schService);
			} 
			::CloseServiceHandle(schSCManager);
		}
	}
	if (!bRet)
	{
		TCHAR szErr[256];
		_tprintf(TEXT("Start of service %s failed - %s\r\n"), m_lpDisplayName, GetLastErrorTextEx(m_dwErr,szErr,256));
	}
	return bRet;
}


////////////////////////////////////////////////////////////////////////////
//!! TCW MOD - faceless window procedure for usage within Win95 (mostly),
// but can be invoked under NT by using -f
LRESULT CALLBACK _FacelessWndProc_( HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam ) {
	if (uMsg==WM_QUERYENDSESSION || uMsg==WM_ENDSESSION || uMsg==WM_QUIT) {
		if (lParam==NULL || uMsg==WM_QUIT) {
			DestroyWindow(hwnd);	// kill me
			if (AfxGetService()!=NULL)
				AfxGetService()->Stop();	// stop me.
			return TRUE;
		}
	}
	return DefWindowProc(hwnd,uMsg,wParam,lParam);
}
////////////////////////////////////////////////////////////////////////////


BOOL CNTService :: DebugService(int argc, TCHAR ** argv, BOOL faceless) {
    DWORD dwArgc;
    LPTSTR *lpszArgv;

    dwArgc   = (DWORD) argc;
    lpszArgv = argv;

	if( !faceless ) {	//!! TCW MOD - no faceless, so give it a face.
		SetupConsole();	//!! TCW MOD - make the console for debugging
	   _tprintf(TEXT("Debugging %s.\r\n"), m_lpDisplayName);

		SetConsoleCtrlHandler(ControlHandler, TRUE);
	}

	//!! TCW MOD START - if Win95, register server
	typedef DWORD (WINAPI *fp_RegServProc)(DWORD dwProcessId,DWORD dwType);
	fp_RegServProc fncptr=NULL;

	if( faceless /*&& OsIsWin95()*/ ) {
		WNDCLASS wndclass;
		memset(&wndclass,0,sizeof(WNDCLASS));
		wndclass.lpfnWndProc = _FacelessWndProc_;
		wndclass.hInstance = HINSTANCE(::GetModuleHandle(0));
		wndclass.lpszClassName = TEXT("RRL__FacelessWndProc_");
		ATOM atom = ::RegisterClass(&wndclass);
		HWND hwnd = ::CreateWindow(wndclass.lpszClassName,TEXT(""),0,0,0,0,0,0,0,wndclass.hInstance,0);
		HMODULE hModule = ::GetModuleHandle(TEXT("kernel32.dll"));
		// punch F1 on "RegisterServiceProcess" for what it does and when to use it.
		fncptr=(fp_RegServProc)::GetProcAddress(hModule, "RegisterServiceProcess");
		if (fncptr!=NULL)
			(*fncptr)(0, RSP_SIMPLE_SERVICE);
	}
	//!! TCW MOD END - if Win95, register server


    Run(dwArgc, lpszArgv);

	if (fncptr!=NULL)		//!! TCW MOD - if it's there, remove it: our run is over
		(*fncptr)(0, RSP_UNREGISTER_SERVICE);

	if(!faceless)
		SetConsoleCtrlHandler(ControlHandler, FALSE);

	return TRUE;
}


void CNTService :: Pause() {
}


void CNTService :: Continue() {
}


void CNTService :: Shutdown() {
}


/////////////////////////////////////////////////////////////////////////////
// class CNTService -- default handlers

void WINAPI CNTService :: ServiceMain(DWORD dwArgc, LPTSTR *lpszArgv) {
	_ASSERTE( gpTheService != 0 );

	// register our service control handler:
	gpTheService->m_sshStatusHandle =	RegisterServiceCtrlHandler(
											gpTheService->m_lpServiceName,
											CNTService::ServiceCtrl
										);

	if( gpTheService->m_sshStatusHandle )
	{
		if( gpTheService->ReportStatus(SERVICE_START_PENDING) ){
			gpTheService->Run( dwArgc, lpszArgv );}
		else
			gpTheService->ReportStatus(SERVICE_STOPPED);
	}
}


void WINAPI CNTService :: ServiceCtrl(DWORD dwCtrlCode) {
	_ASSERTE( gpTheService != 0 );

	// Handle the requested control code.
	switch( dwCtrlCode ) {
		case SERVICE_CONTROL_STOP:
			// Stop the service.
			gpTheService->m_ssStatus.dwCurrentState = SERVICE_STOP_PENDING;
			gpTheService->Stop();
			break;

		case SERVICE_CONTROL_PAUSE:
			gpTheService->m_ssStatus.dwCurrentState = SERVICE_PAUSE_PENDING;
			gpTheService->Pause();
			break;

		case SERVICE_CONTROL_CONTINUE:
			gpTheService->m_ssStatus.dwCurrentState = SERVICE_CONTINUE_PENDING;
			gpTheService->Continue();
			break;

		case SERVICE_CONTROL_SHUTDOWN:
			gpTheService->Shutdown();
			break;

		case SERVICE_CONTROL_INTERROGATE:
			// Update the service status.
			gpTheService->ReportStatus(gpTheService->m_ssStatus.dwCurrentState);
			break;

		default:
			// invalid control code
			break;
	}

}


BOOL WINAPI CNTService :: ControlHandler(DWORD dwCtrlType) {
	_ASSERTE(gpTheService != 0);
	switch( dwCtrlType ) {
		case CTRL_BREAK_EVENT:  // use Ctrl+C or Ctrl+Break to simulate
		case CTRL_C_EVENT:      // SERVICE_CONTROL_STOP in debug mode
		case CTRL_CLOSE_EVENT:      // SERVICE_CONTROL_STOP in debug mode
			_tprintf(TEXT("\nStopping %s.\r\n"), gpTheService->m_lpDisplayName);
			gpTheService->Stop();
			return TRUE;
	}
	return FALSE;
}


/////////////////////////////////////////////////////////////////////////////
// class CNTService -- helpers

//!! TCW MOD - added DWORD dwErrExit for error exit value. Defaults to zero
BOOL CNTService :: ReportStatus(
						DWORD dwCurrentState,
						DWORD dwWaitHint,
						DWORD dwErrExit,
						BOOL  bAcceptControls) {
	BOOL fResult = TRUE;
	
	::EnterCriticalSection(&m_status_cs);

	if(dwCurrentState == -1)
	{
		switch(m_ssStatus.dwCurrentState)
		{

		case SERVICE_RUNNING:
        case SERVICE_STOPPED:
			if (m_ssStatus.dwControlsAccepted != m_dwControlsAccepted)
				break;
			::LeaveCriticalSection(&m_status_cs);
			return TRUE;
		default:
			break;
		}
		dwCurrentState = m_ssStatus.dwCurrentState;
	}

	if( !m_bDebug && m_sshStatusHandle ) { // when debugging we don't report to the SCM
        if( bAcceptControls == FALSE || dwCurrentState == SERVICE_START_PENDING)
            m_ssStatus.dwControlsAccepted = 0;
        else
            m_ssStatus.dwControlsAccepted = m_dwControlsAccepted;

        m_ssStatus.dwCurrentState = dwCurrentState;
        m_ssStatus.dwWin32ExitCode = NO_ERROR;
        m_ssStatus.dwWaitHint = dwWaitHint;

			//!! TCW MOD START - added code to support error exiting
			m_ssStatus.dwServiceSpecificExitCode = dwErrExit;

			if (dwErrExit!=0)
			{
				m_ssStatus.dwWin32ExitCode = ERROR_SERVICE_SPECIFIC_ERROR;
	            m_ssStatus.dwControlsAccepted &= ~SERVICE_ACCEPT_STOP;
			}
			//!! TCW MOD END - added code to support error exiting

        if( dwCurrentState == SERVICE_RUNNING ||
            dwCurrentState == SERVICE_STOPPED )
            m_ssStatus.dwCheckPoint = 0;
        else
            m_ssStatus.dwCheckPoint = ++m_dwCheckPoint;

        // Report the status of the service to the service control manager.
        if (!(fResult = SetServiceStatus( m_sshStatusHandle, &m_ssStatus))) {
            AddToMessageLog(TEXT("SetServiceStatus() failed"));
        }
    }

	::LeaveCriticalSection(&m_status_cs);
    return fResult;
}


void CNTService :: AddToMessageLog(LPCTSTR lpszMsg, WORD wEventType, DWORD dwEventID) {
/*
	m_dwErr = GetLastError();

	// use default message-IDs
	if( dwEventID == DWORD(-1) ) {
		switch( wEventType ) {
			case EVENTLOG_ERROR_TYPE:
				dwEventID = MSG_ERROR_1;
				break;
			case EVENTLOG_WARNING_TYPE:
				dwEventID = MSG_WARNING_1;
				break;
			case EVENTLOG_INFORMATION_TYPE:
				dwEventID = MSG_INFO_1;
				break;
			case EVENTLOG_AUDIT_SUCCESS:
				dwEventID = MSG_INFO_1;
				break;
			case EVENTLOG_AUDIT_FAILURE:
				dwEventID = MSG_INFO_1;
				break;
			default:
				dwEventID = MSG_INFO_1;
				break;
		}
	}

	// Use event logging to log the error.
	HANDLE hEventSource = RegisterEventSource(0, m_lpServiceName);

	if( hEventSource != 0 ) {
		LPCTSTR lpszMessage = lpszMsg;

		ReportEvent(
			hEventSource,	// handle of event source
			wEventType,		// event type
			0,				// event category
			dwEventID,		// event ID
			m_pUserSID,		// current user's SID
			1,				// strings in lpszStrings
			0,				// no bytes of raw data
			&lpszMessage,	// array of error strings
			0				// no raw data
		);

		::DeregisterEventSource(hEventSource);
    }
*/
}

LPTSTR CNTService :: GetLastErrorTextEx( DWORD error, LPTSTR lpszBuf, DWORD dwSize ) {
    LPTSTR lpszTemp = 0;

    DWORD dwRet =	::FormatMessage(
						FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM |FORMAT_MESSAGE_ARGUMENT_ARRAY,
						0,
						error,
						LANG_NEUTRAL,
						(LPTSTR)&lpszTemp,
						0,
						0
					);

    if( !dwRet || (dwSize < dwRet+14) )
        lpszBuf[0] = TEXT('\0');
    else {
        lpszTemp[_tcsclen(lpszTemp)-2] = TEXT('\0');  //remove cr/nl characters
        _tcscpy(lpszBuf, lpszTemp);
    }

    if( lpszTemp )
        LocalFree(HLOCAL(lpszTemp));

    return lpszBuf;
}

LPTSTR CNTService :: GetLastErrorText( LPTSTR lpszBuf, DWORD dwSize ) {
	return GetLastErrorTextEx(GetLastError(), lpszBuf, dwSize);
}

/////////////////////////////////////////////////////////////////////////////
// class CNTService -- implementation
/*
void CNTService :: RegisterApplicationLog( LPCTSTR lpszFileName, DWORD dwTypes ) {
	TCHAR szKey[256];
	_tcscpy(szKey, gszAppRegKey);
	_tcscat(szKey, m_lpServiceName);
	HKEY hKey = 0;
	LONG lRet = ERROR_SUCCESS;
	
	// Create a key for that application and insert values for
	// "EventMessageFile" and "TypesSupported"
	if( ::RegCreateKey(HKEY_LOCAL_MACHINE, szKey, &hKey) == ERROR_SUCCESS ) {
		lRet =	::RegSetValueEx(
					hKey,						// handle of key to set value for
					TEXT("EventMessageFile"),	// address of value to set
					0,							// reserved
					REG_EXPAND_SZ,				// flag for value type
					(CONST BYTE*)lpszFileName,	// address of value data
					(_tcslen(lpszFileName) + 1)*sizeof(*lpszFileName)	// size of value data
				);

		// Set the supported types flags.
		lRet =	::RegSetValueEx(
					hKey,					// handle of key to set value for
					TEXT("TypesSupported"),	// address of value to set
					0,						// reserved
					REG_DWORD,				// flag for value type
					(CONST BYTE*)&dwTypes,	// address of value data
					sizeof(DWORD)			// size of value data
				);
		::RegCloseKey(hKey);
	}

	// Add the service to the "Sources" value

	lRet =	::RegOpenKeyEx( 
				HKEY_LOCAL_MACHINE,	// handle of open key 
				gszAppRegKey,		// address of name of subkey to open 
				0,					// reserved 
				KEY_ALL_ACCESS,		// security access mask 
				&hKey				// address of handle of open key 
			);
	if( lRet == ERROR_SUCCESS ) {
		DWORD dwSize;

		// retrieve the size of the needed value
		lRet =	::RegQueryValueEx(
					hKey,			// handle of key to query 
					TEXT("Sources"),// address of name of value to query 
					0,				// reserved 
					0,				// address of buffer for value type 
					0,				// address of data buffer 
					&dwSize			// address of data buffer size 
				);

 		if( lRet == ERROR_SUCCESS ) {
			DWORD dwType;
			DWORD dwNewSize = dwSize+_tcslen(m_lpServiceName)+1;
			LPBYTE Buffer = LPBYTE(::GlobalAlloc(GPTR, dwNewSize));

			lRet =	::RegQueryValueEx(
						hKey,			// handle of key to query 
						TEXT("Sources"),// address of name of value to query 
						0,				// reserved 
						&dwType,		// address of buffer for value type 
						Buffer,			// address of data buffer 
						&dwSize			// address of data buffer size 
					);
			if( lRet == ERROR_SUCCESS ) {
				_ASSERTE(dwType == REG_MULTI_SZ);

				// check whether this service is already a known source
				register LPTSTR p = LPTSTR(Buffer);
				for(; *p; p += _tcslen(p)+1 ) {
					if( _tcscmp(p, m_lpServiceName) == 0 )
						break;
				}
				if( ! * p ) {
					// We're standing at the end of the stringarray
					// and the service does still not exist in the "Sources".
					// Now insert it at this point.
					// Note that we have already enough memory allocated
					// (see GlobalAlloc() above). We also don't need to append
					// an additional '\0'. This is done in GlobalAlloc() above
					// too.
					_tcscpy(p, m_lpServiceName);

					// OK - now store the modified value back into the
					// registry.
					lRet =	::RegSetValueEx(
								hKey,			// handle of key to set value for
								TEXT("Sources"),// address of value to set
								0,				// reserved
								dwType,			// flag for value type
								Buffer,			// address of value data
								dwNewSize		// size of value data
							);
				}
			}

			::GlobalFree(HGLOBAL(Buffer));
		}

		::RegCloseKey(hKey);
	}
}

void CNTService :: DeregisterApplicationLog() {
	TCHAR szKey[256];
	_tcscpy(szKey, gszAppRegKey);
	_tcscat(szKey, m_lpServiceName);
	HKEY hKey = 0;
	LONG lRet = ERROR_SUCCESS;

	lRet = ::RegDeleteKey(HKEY_LOCAL_MACHINE, szKey);

	// now we have to delete the application from the "Sources" value too.
	lRet =	::RegOpenKeyEx( 
				HKEY_LOCAL_MACHINE,	// handle of open key 
				gszAppRegKey,		// address of name of subkey to open 
				0,					// reserved 
				KEY_ALL_ACCESS,		// security access mask 
				&hKey				// address of handle of open key 
			);
	if( lRet == ERROR_SUCCESS ) {
		DWORD dwSize;

		// retrieve the size of the needed value
		lRet =	::RegQueryValueEx(
					hKey,			// handle of key to query 
					TEXT("Sources"),// address of name of value to query 
					0,				// reserved 
					0,				// address of buffer for value type 
					0,				// address of data buffer 
					&dwSize			// address of data buffer size 
				);

 		if( lRet == ERROR_SUCCESS ) {
			DWORD dwType;
			LPBYTE Buffer = LPBYTE(::GlobalAlloc(GPTR, dwSize));
			LPBYTE NewBuffer = LPBYTE(::GlobalAlloc(GPTR, dwSize));

			lRet =	::RegQueryValueEx(
						hKey,			// handle of key to query 
						TEXT("Sources"),// address of name of value to query 
						0,				// reserved 
						&dwType,		// address of buffer for value type 
						Buffer,			// address of data buffer 
						&dwSize			// address of data buffer size 
					);
			if( lRet == ERROR_SUCCESS ) {
				_ASSERTE(dwType == REG_MULTI_SZ);

				// check whether this service is already a known source
				register LPTSTR p = LPTSTR(Buffer);
				register LPTSTR pNew = LPTSTR(NewBuffer);
				BOOL bNeedSave = FALSE;	// assume the value is already correct
				for(; *p; p += _tcslen(p)+1) {
					// except ourself: copy the source string into the destination
					if( _tcscmp(p, m_lpServiceName) != 0 ) {
						_tcscpy(pNew, p);
						pNew += _tcslen(pNew)+1;
					} else {
						bNeedSave = TRUE;		// *this* application found
						dwSize -= _tcslen(p)+1;	// new size of value
					}
				}
				if( bNeedSave ) {
					// OK - now store the modified value back into the
					// registry.
					lRet =	::RegSetValueEx(
								hKey,			// handle of key to set value for
								TEXT("Sources"),// address of value to set
								0,				// reserved
								dwType,			// flag for value type
								NewBuffer,		// address of value data
								dwSize			// size of value data
							);
				}
			}

			::GlobalFree(HGLOBAL(Buffer));
			::GlobalFree(HGLOBAL(NewBuffer));
		}

		::RegCloseKey(hKey);
	}
}
*/
////////////////////////////////////////////////////////


//#include <c:\program files\microsoft visual studio\vc98/crt/src/internal.h>

extern "C" {
/*
 * Control structure for lowio file handles
 */
typedef struct {
        long osfhnd;    /* underlying OS file HANDLE */
        char osfile;    /* attributes of file (e.g., open in text mode?) */
        char pipech;    /* one char buffer for handles opened on pipes */
#ifdef _MT
        int lockinitflag;
        CRITICAL_SECTION lock;
#endif  /* _MT */
    }   ioinfo;

/*
 * Definition of IOINFO_L2E, the log base 2 of the number of elements in each
 * array of ioinfo structs.
 */
#define IOINFO_L2E          5

/*
 * Definition of IOINFO_ARRAY_ELTS, the number of elements in ioinfo array
 */
#define IOINFO_ARRAY_ELTS   (1 << IOINFO_L2E)

/*
 * Definition of IOINFO_ARRAYS, maximum number of supported ioinfo arrays.
 */
#define IOINFO_ARRAYS       64

#define _NHANDLE_           (IOINFO_ARRAYS * IOINFO_ARRAY_ELTS)

/*
 * Access macros for getting at an ioinfo struct and its fields from a
 * file handle
 */
#define _pioinfo(i) ( __pioinfo[(i) >> IOINFO_L2E] + ((i) & (IOINFO_ARRAY_ELTS - \
                              1)) )
#define _osfhnd(i)  ( _pioinfo(i)->osfhnd )

#define _osfile(i)  ( _pioinfo(i)->osfile )

#define _pipech(i)  ( _pioinfo(i)->pipech )

/*
 * Safer versions of the above macros. Currently, only _osfile_safe is
 * used.
 */
#define _pioinfo_safe(i)    ( ((i) != -1) ? _pioinfo(i) : &__badioinfo )

#define _osfhnd_safe(i)     ( _pioinfo_safe(i)->osfhnd )

#define _osfile_safe(i)     ( _pioinfo_safe(i)->osfile )

#define _pipech_safe(i)     ( _pioinfo_safe(i)->pipech )

/*
 * Special, static ioinfo structure used only for more graceful handling
 * of a C file handle value of -1 (results from common errors at the stdio
 * level).
 */
extern _CRTIMP ioinfo __badioinfo;

/*
 * Array of arrays of control structures for lowio files.
 */
extern _CRTIMP ioinfo * __pioinfo[];

/*
 * Current number of allocated ioinfo structures (_NHANDLE_ is the upper
 * limit).
 */
extern int _nhandle;

/* __osfile flag values for DOS file handles */

#define FOPEN           0x01    /* file handle open */
#define FEOFLAG         0x02    /* end of file has been encountered */
#ifdef _MAC
#define FWRONLY         0x04    /* file handle associated with write only file */
#define FLOCK           0x08    /* file has been successfully locked at least once */
#else  /* _MAC */
#define FCRLF           0x04    /* CR-LF across read buffer (in text mode) */
#define FPIPE           0x08    /* file handle refers to a pipe */
#endif  /* _MAC */
#ifdef _WIN32
#define FNOINHERIT      0x10    /* file handle opened _O_NOINHERIT */
#else  /* _WIN32 */
#define FRDONLY         0x10    /* file handle associated with read only file */
#endif  /* _WIN32 */

#define FAPPEND         0x20    /* file handle opened O_APPEND */
#define FDEV            0x40    /* file handle refers to device */
#define FTEXT           0x80    /* file handle is in text mode */

}
//!! TCW MOD - function to create console for faceless apps if not already there
void CNTService::SetupConsole() {
	
	if( !m_bShowConsole ) 
		return;
	if( m_fConsoleReady ) 
		return;

	HANDLE hConDevice = INVALID_HANDLE_VALUE;
	DWORD astds[3]={STD_OUTPUT_HANDLE,STD_ERROR_HANDLE,STD_INPUT_HANDLE};
	FILE *atrgs[3]={stdout,stderr,stdin};

//	if (m_pszConsoleDevice)
//	{
//		hConDevice = CreateFile(m_pszConsoleDevice, GENERIC_READ | GENERIC_WRITE,0, 0, OPEN_EXISTING, 0, 0);
//	}

	if (GetFileType(GetStdHandle(STD_OUTPUT_HANDLE)) != FILE_TYPE_PIPE)
	{
		AllocConsole();	// you only get 1 console.
	}
	
	SetConsoleTitle(VER_PRODUCTNAME_STR);

	// lovely hack to get the standard io (printf, getc, etc) to the new console. Pretty much does what the
	// C lib does for us, but when we want it, and inside of a Window'd app.
	// The ugly look of this is due to the error checking (bad return values. Remove the if xxx checks if you like it that way.
	for( register int i=0; i<3; i++ ) {
		long hand=(long)GetStdHandle(astds[i]);
//		if (hConDevice != INVALID_HANDLE_VALUE)
//			hand = (long)hConDevice;
		if( hand!=(long)INVALID_HANDLE_VALUE ) {
			int osf=_open_osfhandle(hand, _O_BINARY);
			if( osf!=-1 ) {
				FILE *fp=_fdopen(osf,(astds[i]==STD_INPUT_HANDLE) ? "rb" : "wb");
				if( fp!=NULL ) {
					setvbuf(fp, 0, _IONBF, 0);
					*(atrgs[i])=*fp;
//					_osfile(osf) |= FDEV | FPIPE;
//					_osfile(osf) &= ~(FTEXT);
				}
			}
		}
	}

	if (GetFileType(GetStdHandle(STD_INPUT_HANDLE)) != FILE_TYPE_PIPE)
	{
		HANDLE hStdIn = GetStdHandle(STD_INPUT_HANDLE);
		HANDLE hStdOut = GetStdHandle(STD_OUTPUT_HANDLE);
		DWORD dwConsoleModeInOld;
		DWORD dwConsoleModeOutOld;
		DWORD dwConsoleModeIn;
		DWORD dwConsoleModeOut;
		GetConsoleMode(hStdIn, &dwConsoleModeIn);
		GetConsoleMode(hStdOut, &dwConsoleModeOut);

		dwConsoleModeInOld = dwConsoleModeIn;
		dwConsoleModeOutOld = dwConsoleModeOut;

//		if (GetFileType(GetStdHandle(STD_INPUT_HANDLE)) == FILE_TYPE_CHAR)
		dwConsoleModeIn &= ~(ENABLE_LINE_INPUT | ENABLE_ECHO_INPUT);
		dwConsoleModeIn &= ~ENABLE_PROCESSED_INPUT;
		SetConsoleMode(hStdIn, dwConsoleModeIn);
		SetConsoleMode(hStdOut, dwConsoleModeOut);
		
		// verify mode set
//			{
//				DWORD dwConsoleMode;
//				GetConsoleMode(hStdOut, &dwConsoleMode);
//				_tprintf("cmode out: %08X %08X %08X\r\n", dwConsoleModeOutOld, dwConsoleModeOut, dwConsoleMode);
//				GetConsoleMode(hStdIn, &dwConsoleMode);
//				_tprintf("cmode in: %08X %08X %08X\r\n", dwConsoleModeInOld, dwConsoleModeIn, dwConsoleMode);
//			}
	}

//		_tprintf("CNTService::SetupConsole()\r\n");
//		_tprintf("out type: %d\r\n", GetFileType(GetStdHandle(STD_OUTPUT_HANDLE)));
//		_tprintf("in type: %d, mode=%08X\r\n", GetFileType(GetStdHandle(STD_INPUT_HANDLE)), GetStdHandle(STD_OUTPUT_HANDLE));
	m_fConsoleReady=TRUE;
}
#endif //_WIN32
