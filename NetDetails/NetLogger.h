// --------------------------------------------------------------------------------
#ifndef NET_LOGGER_H_INCLUDED
#define NET_LOGGER_H_INCLUDED

#include <windows.h>
#include <pr_err.h>

class CNetLogger
{
public:
	CNetLogger();
	~CNetLogger();

	static tERROR HasDriver();

	tERROR	Start();
	void	Stop();
	
	HANDLE m_hLogger;
private:
	HMODULE	m_hDll;
	bool	m_bInitialized;

	enum RunCode {	_ok = 0, _lib_not_initialized,	_failed_sc_manager,	_svc_not_installed,	_svc_not_started };

	SC_HANDLE	(WINAPI *_OpenSCManagerW)(LPCWSTR lpMachineName, LPCWSTR lpDatabaseName, DWORD dwDesiredAccess);
	SC_HANDLE	(WINAPI *_OpenServiceW)(SC_HANDLE hSCManager, LPCWSTR lpServiceName, DWORD dwDesiredAccess);
	SC_HANDLE	(WINAPI *_CreateServiceW)(SC_HANDLE hSCManager, LPCWSTR lpServiceName, LPCWSTR lpDisplayName, DWORD dwDesiredAccess, DWORD dwServiceType, DWORD dwStartType, DWORD dwErrorControl, LPCWSTR lpBinaryPathName, LPCWSTR lpLoadOrderGroup, LPDWORD lpdwTagId, LPCWSTR lpDependencies, LPCWSTR lpServiceStartName, LPCWSTR lpPassword);
	BOOL		(WINAPI *_StartServiceW)(SC_HANDLE hService, DWORD dwNumServiceArgs, LPCWSTR *lpServiceArgVectors);
	BOOL		(WINAPI *_ControlService)(SC_HANDLE hService, DWORD dwControl, LPSERVICE_STATUS lpServiceStatus);
	BOOL		(WINAPI *_CloseServiceHandle)(SC_HANDLE hSCObject);

	bool InitLibrary();
	bool GetFunc(FARPROC *pFunc, LPCSTR strFunc);

	bool	InstallService(LPCWSTR szServiceName, DWORD dwStartType, LPCWSTR szServiceBinary);
	RunCode	StartService( LPCWSTR szServiceName );
	bool	StopService( LPCWSTR szServiceName );
};

#endif // NET_LOGGER_H_INCLUDED