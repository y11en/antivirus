#if !defined(_INSTALL_H_)
#define _INSTALL_H_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

bool IsWin9x ();
void GetExeTempFileName(std::string& TempName);
HANDLE FindPluginDriver(LPCTSTR qualified_name);
bool ExtractFile(ULONG uResID, LPCTSTR szFileName);
bool InstallService(LPCTSTR szServiceName, DWORD dwStartType, LPCTSTR szServiceBinary);
bool StopService( LPCTSTR szServiceName );
bool StartService( LPCTSTR szServiceName );
bool RemoveService(LPCTSTR szServiceName);

bool InstallKlick();
void UninstallKlick();


#define LOGGER_NAME _T("Logger")

struct UnregLogger
{
	UnregLogger();
	~UnregLogger();
	
	bool Install();
	
	HANDLE m_hLogger;
	TCHAR m_szDriverPath[MAX_PATH];
};

#endif //_INSTALL_H_
