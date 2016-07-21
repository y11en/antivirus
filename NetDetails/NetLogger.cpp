#include "NetLogger.h"
#include "tchar.h"
#include "../windows/Hook/Plugins/Include/logger_api.h"

#define KLNET_SNIFFER_SVC_NAME_W	L"klnetinf"

CNetLogger::CNetLogger() : 
	m_hLogger(NULL), m_hDll(NULL), m_bInitialized(false)
{
	m_bInitialized = InitLibrary();
}

CNetLogger::~CNetLogger()
{
	if( m_hDll )
		::FreeLibrary(m_hDll),	m_hDll = NULL;
	
	if(m_hLogger)
		CloseHandle(m_hLogger);
}

tERROR CNetLogger::HasDriver()
{
	OSVERSIONINFO osvi;
	osvi.dwOSVersionInfoSize = sizeof ( osvi );
	GetVersionEx ( &osvi );
	
	if ( osvi.dwPlatformId == VER_PLATFORM_WIN32_WINDOWS ) 		//9x
	{
		return errNOT_SUPPORTED;
	}
	
	wchar_t wzDriverPath[2*MAX_PATH];
	ExpandEnvironmentStringsW( L"%SystemRoot%\\system32\\drivers\\klnetinf.sys" , wzDriverPath, sizeof(wzDriverPath)/sizeof(wzDriverPath[0]) );
	DWORD dwAttr = ::GetFileAttributesW(wzDriverPath);
	return ( dwAttr == INVALID_FILE_ATTRIBUTES ) ? errNOT_FOUND : errOK;
}

tERROR CNetLogger::Start()
{
	if(false == m_bInitialized)
		m_bInitialized = InitLibrary();
	
	if(false == m_bInitialized)
		return errMODULE_NOT_FOUND;

	OSVERSIONINFO osvi;
	osvi.dwOSVersionInfoSize = sizeof ( osvi );
	GetVersionEx ( &osvi );
	
	if ( osvi.dwPlatformId == VER_PLATFORM_WIN32_WINDOWS ) 		//9x
	{
		return errNOT_SUPPORTED;
	}

	// запускаем сервис, и если надо - регистрируем его и запускаем вновь
	RunCode rcode = StartService(KLNET_SNIFFER_SVC_NAME_W);
	if( rcode == _svc_not_installed )
	{
		wchar_t wzDriverPath[2*MAX_PATH];
		ExpandEnvironmentStringsW( L"%SystemRoot%\\system32\\drivers\\klnetinf.sys" , wzDriverPath, sizeof(wzDriverPath)/sizeof(wzDriverPath[0]) );
		
		if ( false == InstallService(KLNET_SNIFFER_SVC_NAME_W, SERVICE_DEMAND_START, wzDriverPath))		{
			return errMODULE_CANNOT_BE_LOADED;
		}
		
		rcode = StartService(KLNET_SNIFFER_SVC_NAME_W);
	}

	if( rcode != _ok )	{
		return errOPERATION_CANCELED;
	}
	
	m_hLogger = CreateFile (_T("\\\\.\\") LOGGER_PLUGIN_NAME, 0, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if( m_hLogger == INVALID_HANDLE_VALUE )
		m_hLogger = NULL;
	
	return ( m_hLogger != NULL ) ? errOK : errHANDLE_INVALID;
}

void CNetLogger::Stop()
{
	StopService( KLNET_SNIFFER_SVC_NAME_W );
	
	CloseHandle( m_hLogger );
	m_hLogger = NULL;
}

bool CNetLogger::InitLibrary()
{
	if(NULL == m_hDll)
		m_hDll = ::LoadLibrary("advapi32.dll");

	if( !m_hDll )
		return false;

	bool bSucc = true;
	if( bSucc ) bSucc = GetFunc((FARPROC *)&_OpenSCManagerW,    "OpenSCManagerW");
	if( bSucc ) bSucc = GetFunc((FARPROC *)&_OpenServiceW,      "OpenServiceW");
	if( bSucc ) bSucc = GetFunc((FARPROC *)&_CreateServiceW,    "CreateServiceW");
	if( bSucc ) bSucc = GetFunc((FARPROC *)&_StartServiceW,     "StartServiceW");
	if( bSucc ) bSucc = GetFunc((FARPROC *)&_ControlService,    "ControlService");
	if( bSucc ) bSucc = GetFunc((FARPROC *)&_CloseServiceHandle,"CloseServiceHandle");

	return bSucc;
}

bool CNetLogger::GetFunc(FARPROC *pFunc, LPCSTR strFunc)
{
	*pFunc = GetProcAddress(m_hDll, strFunc);
	return *pFunc != NULL;
}


bool CNetLogger::InstallService(LPCWSTR szServiceName, DWORD dwStartType, LPCWSTR szServiceBinary)
{
	if(false == m_bInitialized)
		return false;

	SC_HANDLE schSCManager = _OpenSCManagerW (NULL, NULL, SC_MANAGER_ALL_ACCESS);

	if (!schSCManager)
		return false;

	SC_HANDLE schService = _OpenServiceW (schSCManager, szServiceName, SERVICE_ALL_ACCESS);
	
	// сервис уже зарегистрирован
	if (schService)
		return true;

	schService = NULL;

	for (int i = 0; i<10 && !schService; ++i)
	{
		schService = _CreateServiceW (
			schSCManager,			// SCManager database
			szServiceName,			// name of service
			szServiceName,			// name to display
			SERVICE_ALL_ACCESS,		// desired access
			SERVICE_KERNEL_DRIVER,	// service type
			dwStartType,			// start type
			SERVICE_ERROR_NORMAL,	// error control type
			szServiceBinary,		// service's binary
			NULL,					// ordering group
			NULL,					// tag identifier
			NULL,					// dependencies
			NULL,					// account
			NULL);					// password

		if (schService || 
			(!schService && GetLastError() == ERROR_SERVICE_EXISTS))
		{
			_CloseServiceHandle (schSCManager);
			if (schService)
				_CloseServiceHandle (schService);
			
			return true;
		}
		else
			Sleep(10);
	}

    _CloseServiceHandle (schSCManager);
	return false;
}

CNetLogger::RunCode CNetLogger::StartService( LPCWSTR szServiceName )
{
	if(false == m_bInitialized)
		return _lib_not_initialized;

	SC_HANDLE schSCManager = _OpenSCManagerW (NULL, NULL, SC_MANAGER_ALL_ACCESS);
	
	if (!schSCManager)
		return _failed_sc_manager;
	
	SC_HANDLE schService = _OpenServiceW (schSCManager, szServiceName, SERVICE_ALL_ACCESS);
	
	if (!schService)
	{
		_CloseServiceHandle (schSCManager);
		return _svc_not_installed;
	}
	
	BOOL rc = _StartServiceW(schService, 0, NULL) || ERROR_SERVICE_ALREADY_RUNNING == GetLastError();

	_CloseServiceHandle (schService);
	_CloseServiceHandle (schSCManager);
	return rc == TRUE ? _ok : _svc_not_started;
}

bool CNetLogger::StopService( LPCWSTR szServiceName )
{
	if(false == m_bInitialized)
		return false;

	SC_HANDLE schSCManager = _OpenSCManagerW (NULL, NULL, SC_MANAGER_ALL_ACCESS);
	
	if (!schSCManager)
		return false;
	
	SC_HANDLE schService = _OpenServiceW (schSCManager, szServiceName, SERVICE_ALL_ACCESS);
	
	if (!schService)
	{
		_CloseServiceHandle (schSCManager);
		return false;
	}
	
	SERVICE_STATUS serviceStatus;
	BOOL rc = _ControlService (schService, SERVICE_CONTROL_STOP, &serviceStatus);

	_CloseServiceHandle (schService);
	_CloseServiceHandle (schSCManager);
	return rc == TRUE;
}
