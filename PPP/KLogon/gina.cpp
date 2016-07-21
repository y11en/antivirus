//-----------------------------------------------------

#include <windows.h>
#include <winwlx.h>

//-----------------------------------------------------

static char *ImportFunctions[]=
{
	"WlxActivateUserShell",
	"WlxDisplayLockedNotice",
	"WlxDisplaySASNotice",
	"WlxDisplayStatusMessage",
	"WlxGetStatusMessage",
	"WlxInitialize",
	"WlxIsLockOk",
	"WlxIsLogoffOk",
	"WlxLoggedOnSAS",
	"WlxLoggedOutSAS",
	"WlxLogoff",
	"WlxNegotiate",
	"WlxNetworkProviderLoad",
	"WlxRemoveStatusMessage",
	"WlxScreenSaverNotify",
	"WlxShutdown",
	"WlxStartApplication",
	"WlxWkstaLockedSAS",
	"WlxGetConsoleSwitchCredentials",
	"WlxReconnectNotify",
	"WlxDisconnectNotify",
	NULL
};

struct MSWlxAPI
{
	BOOL (WINAPI *ActivateUserShell)(PVOID pWlxContext,PWSTR pszDesktopName,PWSTR pszMprLogonScript,PVOID pEnvironment);
	VOID (WINAPI *DisplayLockedNotice)(PVOID pWlxContext);
	VOID (WINAPI *DisplaySASNotice)(PVOID pWlxContext);
	BOOL (WINAPI *DisplayStatusMessage)(PVOID pWlxContext,HDESK hDesktop,DWORD dwOptions,PWSTR pTitle,PWSTR pMessage);
	BOOL (WINAPI *GetStatusMessage)(PVOID pWlxContext,DWORD *pdwOptions,PWSTR pMessage,DWORD dwBufferSize);
	BOOL (WINAPI *Initialize)(LPWSTR lpWinsta,HANDLE hWlx,PVOID pvReserved,PVOID pWinlogonFunctions,PVOID *pWlxContext);
	BOOL (WINAPI *IsLockOk)(PVOID pWlxContext);
	BOOL (WINAPI *IsLogoffOk)(PVOID pWlxContext);
	int  (WINAPI *LoggedOnSAS)(PVOID pWlxContext,DWORD dwSasType,PVOID pReserved);
	int  (WINAPI *LoggedOutSAS)(PVOID pWlxContext,DWORD dwSasType,PLUID pAuthenticationId,PSID pLogonSid,PDWORD pdwOptions,PHANDLE phToken,PWLX_MPR_NOTIFY_INFO pNprNotifyInfo,PVOID *pProfile);
	VOID (WINAPI *Logoff)(PVOID pWlxContext);
	BOOL (WINAPI *Negotiate)(DWORD dwWinLogonVersion,PDWORD pdwDllVersion);
	BOOL (WINAPI *NetworkProviderLoad)(PVOID pWlxContext,PWLX_MPR_NOTIFY_INFO pNprNotifyInfo);
	BOOL (WINAPI *RemoveStatusMessage)(PVOID pWlxContext);
	BOOL (WINAPI *ScreenSaverNotify)(PVOID pWlxContext,BOOL *pSecure);
	VOID (WINAPI *Shutdown)(PVOID pWlxContext,DWORD ShutdownType);
	BOOL (WINAPI *StartApplication)(PVOID pWlxContext,PWSTR pszDesktopName,PVOID pEnvironment,PWSTR pszCmdLine);
	int  (WINAPI *WkstaLockedSAS)(PVOID pWlxContext,DWORD dwSasType);
	BOOL (WINAPI *GetConsoleSwitchCredentials)(PVOID pWlxContext, PVOID pCredInfo);
	VOID (WINAPI *ReconnectNotify)(PVOID pWlxContext);
	VOID (WINAPI *DisconnectNotify)(PVOID pWlxContext);

	MSWlxAPI()
	{
		HMODULE hGina = LoadLibrary("msgina.dll");
		if( !hGina )
			return;

		void **fAddr = (void**)this;
		for(char **fName = ImportFunctions; *fName; fName++, fAddr++)
			*fAddr = GetProcAddress(hGina, *fName);
	}
};

MSWlxAPI g_WlxAPI;

//-----------------------------------------------------

BOOL WINAPI WlxActivateUserShell(PVOID pWlxContext,PWSTR pszDesktopName,PWSTR pszMprLogonScript,PVOID pEnvironment)
{
	if( g_WlxAPI.ActivateUserShell )
		return g_WlxAPI.ActivateUserShell(pWlxContext,pszDesktopName,pszMprLogonScript,pEnvironment);
	return FALSE;
}

VOID WINAPI WlxDisplayLockedNotice(PVOID pWlxContext)
{
	if( g_WlxAPI.DisplayLockedNotice )
		g_WlxAPI.DisplayLockedNotice(pWlxContext);
}

VOID WINAPI WlxDisplaySASNotice(PVOID pWlxContext)
{
	if( g_WlxAPI.DisplaySASNotice )
		g_WlxAPI.DisplaySASNotice(pWlxContext);
}

BOOL WINAPI WlxDisplayStatusMessage(PVOID pWlxContext,HDESK hDesktop,DWORD dwOptions,PWSTR pTitle,PWSTR pMessage)
{
	if( g_WlxAPI.DisplayStatusMessage )
		return g_WlxAPI.DisplayStatusMessage(pWlxContext,hDesktop,dwOptions,pTitle,pMessage);
	return FALSE;
}

BOOL WINAPI WlxGetStatusMessage(PVOID pWlxContext,DWORD *pdwOptions,PWSTR pMessage,DWORD dwBufferSize)
{
	if( g_WlxAPI.GetStatusMessage )
		return g_WlxAPI.GetStatusMessage(pWlxContext,pdwOptions,pMessage,dwBufferSize);
	return FALSE;
}

BOOL WINAPI WlxInitialize(LPWSTR lpWinsta,HANDLE hWlx,PVOID pvReserved,PVOID pWinlogonFunctions,PVOID *pWlxContext)
{
	if( g_WlxAPI.Initialize )
		return g_WlxAPI.Initialize(lpWinsta,hWlx,pvReserved,pWinlogonFunctions,pWlxContext);
	return FALSE;
}

BOOL WINAPI WlxIsLockOk(PVOID pWlxContext)
{
	if( g_WlxAPI.IsLockOk )
		return g_WlxAPI.IsLockOk(pWlxContext);
	return FALSE;
}

BOOL WINAPI WlxIsLogoffOk(PVOID pWlxContext)
{
	if( g_WlxAPI.IsLogoffOk )
		return g_WlxAPI.IsLogoffOk(pWlxContext);
	return FALSE;
}

int WINAPI WlxLoggedOnSAS(PVOID pWlxContext,DWORD dwSasType,PVOID pReserved)
{
	if( g_WlxAPI.LoggedOnSAS )
		return g_WlxAPI.LoggedOnSAS(pWlxContext,dwSasType,pReserved);
	return 0;
}

int WINAPI WlxLoggedOutSAS(PVOID pWlxContext,DWORD dwSasType,PLUID pAuthenticationId,PSID pLogonSid,PDWORD pdwOptions,PHANDLE phToken,PWLX_MPR_NOTIFY_INFO pNprNotifyInfo,PVOID *pProfile)
{
	int nResult = 0;
	if( g_WlxAPI.LoggedOutSAS )
		nResult = g_WlxAPI.LoggedOutSAS(pWlxContext,dwSasType,pAuthenticationId,pLogonSid,pdwOptions,phToken,pNprNotifyInfo,pProfile);

	return nResult;
}

VOID WINAPI WlxLogoff(PVOID pWlxContext)
{
	if( g_WlxAPI.Logoff )
		g_WlxAPI.Logoff(pWlxContext);
}

BOOL WINAPI WlxNegotiate(DWORD dwWinLogonVersion,PDWORD pdwDllVersion)
{
	if( g_WlxAPI.Negotiate )
		return g_WlxAPI.Negotiate(dwWinLogonVersion,pdwDllVersion);
	return FALSE;
}

BOOL WINAPI WlxNetworkProviderLoad(PVOID pWlxContext,PWLX_MPR_NOTIFY_INFO pNprNotifyInfo)
{
	if( g_WlxAPI.NetworkProviderLoad )
		return g_WlxAPI.NetworkProviderLoad(pWlxContext,pNprNotifyInfo);
	return FALSE;
}

BOOL WINAPI WlxRemoveStatusMessage(PVOID pWlxContext)
{
	if( g_WlxAPI.RemoveStatusMessage )
		return g_WlxAPI.RemoveStatusMessage(pWlxContext);
	return FALSE;
}

BOOL WINAPI WlxScreenSaverNotify(PVOID pWlxContext,BOOL *pSecure)
{
	if( g_WlxAPI.ScreenSaverNotify )
		return g_WlxAPI.ScreenSaverNotify(pWlxContext,pSecure);
	return FALSE;
}

VOID WINAPI WlxShutdown(PVOID pWlxContext,DWORD ShutdownType)
{
	if( g_WlxAPI.Shutdown )
		g_WlxAPI.Shutdown(pWlxContext,ShutdownType);
}

BOOL WINAPI WlxStartApplication(PVOID pWlxContext,PWSTR pszDesktopName,PVOID pEnvironment,PWSTR pszCmdLine)
{
	if( g_WlxAPI.StartApplication )
		return g_WlxAPI.StartApplication(pWlxContext,pszDesktopName,pEnvironment,pszCmdLine);
	return FALSE;
}

int WINAPI WlxWkstaLockedSAS(PVOID pWlxContext,DWORD dwSasType)
{
	if( g_WlxAPI.WkstaLockedSAS )
		return g_WlxAPI.WkstaLockedSAS(pWlxContext,dwSasType);
	return 0;
}

BOOL WINAPI WlxGetConsoleSwitchCredentials (PVOID pWlxContext, PVOID pCredInfo)
{
	if( g_WlxAPI.GetConsoleSwitchCredentials )
		return g_WlxAPI.GetConsoleSwitchCredentials(pWlxContext, pCredInfo);
	return FALSE;
}

VOID WINAPI WlxReconnectNotify(PVOID pWlxContext)
{
	if( g_WlxAPI.ReconnectNotify )
		g_WlxAPI.ReconnectNotify(pWlxContext);
}

VOID WINAPI WlxDisconnectNotify (PVOID pWlxContext)
{
	if( g_WlxAPI.DisconnectNotify )
		g_WlxAPI.DisconnectNotify(pWlxContext);
}

//-----------------------------------------------------
