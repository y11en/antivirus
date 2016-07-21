#include "scplugins.h"
#include "CCriticalSection.h"
#include "debug.h"
#include "signchk.h"

CCriticalSection g_cPluginListCriticalSection;

typedef struct tag_SC_PLUGIN SC_PLUGIN;

typedef struct tag_SC_PLUGIN {
	CHAR szName[MAX_PATH];
	CHAR szModulePathName[MAX_PATH];
	DWORD dwPriority;
	void* hModule;
	tIsEnabled IsEnabled;
	tProcessScript ProcessScript;
	tCheckPopupURL CheckPopupURL;
	tIsDispatchEnabled IsDispatchEnabled;
	tDispatch Dispatch;
	SC_PLUGIN* pNextPlugin;
} SC_PLUGIN;

static SC_PLUGIN* g_pPluginsList = NULL;

BOOL ScPluginsLoad()
{
	BOOL bRes = FALSE;
	HKEY hKey = NULL;
	DWORD dwIndex = 0;
	SC_PLUGIN sc_plugin;

	g_cPluginListCriticalSection.Enter();

	if (g_pPluginsList)
		ScPluginsUnload();

	if(ERROR_SUCCESS == RegOpenKeyEx(HKEY_LOCAL_MACHINE, SC_REG_PLUGINS, 0, KEY_READ, &hKey))
	{
		while (ERROR_SUCCESS == RegEnumKey(hKey, dwIndex++, sc_plugin.szName, sizeof(sc_plugin.szName)))
		{
			HKEY hPluginKey;
			if(ERROR_SUCCESS == RegOpenKeyEx(hKey, sc_plugin.szName, 0, KEY_READ, &hPluginKey))
			{
				DWORD dwSize;
				BOOL  bEnabled = TRUE;
				DWORD dwType;
				
				dwType = REG_DWORD;
				dwSize = sizeof(bEnabled);
				RegQueryValueEx(hPluginKey, SC_REGVAL_ENABLED, NULL, &dwType, (LPBYTE)&bEnabled, &dwSize);
				
				if (bEnabled)
				{
					dwType = REG_DWORD;
					sc_plugin.dwPriority = 5;
					dwSize = sizeof(sc_plugin.dwPriority);
					RegQueryValueEx(hPluginKey, SC_REGVAL_PRIORITY, NULL, &dwType, (LPBYTE)&sc_plugin.dwPriority, &dwSize);
					
					dwSize = sizeof(sc_plugin.szModulePathName);
					if (ERROR_SUCCESS == RegQueryValue(hPluginKey, NULL, &sc_plugin.szModulePathName[0], (LPLONG)&dwSize))
					{
#if !defined(_DEBUG)
//						if (SIGN_OK == _sign_check_file(&sc_plugin.szModulePathName[0]))
#endif
						{
							sc_plugin.hModule = LoadLibraryEx(&sc_plugin.szModulePathName[0], NULL, LOAD_WITH_ALTERED_SEARCH_PATH);
							if (sc_plugin.hModule != NULL)
							{
								sc_plugin.IsEnabled = (tIsEnabled) GetProcAddress((HINSTANCE)sc_plugin.hModule, "IsEnabled");
								sc_plugin.ProcessScript = (tProcessScript) GetProcAddress((HINSTANCE)sc_plugin.hModule, "ProcessScript");
								sc_plugin.CheckPopupURL = (tCheckPopupURL) GetProcAddress((HINSTANCE)sc_plugin.hModule, "CheckPopupURL");
								sc_plugin.IsDispatchEnabled = (tIsDispatchEnabled) GetProcAddress((HINSTANCE)sc_plugin.hModule, "IsDispatchEnabled");
								sc_plugin.Dispatch = (tDispatch) GetProcAddress((HINSTANCE)sc_plugin.hModule, "Dispatch");
								if (sc_plugin.IsEnabled==NULL && sc_plugin.IsDispatchEnabled)
								{
									FreeLibrary((HINSTANCE)sc_plugin.hModule);
								}
								else
								{
									SC_PLUGIN** ppInsertPoint = &g_pPluginsList;
									SC_PLUGIN* pNewPlugin = NULL;

									while (*ppInsertPoint)
									{
										if ((*ppInsertPoint)->dwPriority >= sc_plugin.dwPriority)
											break;
										ppInsertPoint = &((*ppInsertPoint)->pNextPlugin);
									}
									sc_plugin.pNextPlugin = *ppInsertPoint;
									pNewPlugin = (SC_PLUGIN*)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, sizeof(SC_PLUGIN));
									if (pNewPlugin)
									{
										memcpy(pNewPlugin, &sc_plugin, sizeof(SC_PLUGIN));
										*ppInsertPoint = pNewPlugin;
										bRes = TRUE;
									}
								}
							}
						}
					}
				}
				RegCloseKey(hPluginKey);
			}
		}
		RegCloseKey(hKey);
	}
	
	g_cPluginListCriticalSection.Leave();
	return bRes;
}

BOOL ScPluginsReloadIfNeeded()
{
	static BOOL  g_bReloadPlugins = TRUE;
	static DWORD g_dwLastReloadPluginsCheck = 0;

	HKEY hKey;
	if (g_bReloadPlugins == FALSE)
	{
		DWORD dwTicks = GetTickCount();
		if (g_dwLastReloadPluginsCheck + 1000 < dwTicks)
		{
			if(ERROR_SUCCESS == RegOpenKeyEx(HKEY_LOCAL_MACHINE, SC_REG_PLUGINS, 0, KEY_READ, &hKey))
			{
				DWORD dwType = REG_DWORD;
				DWORD dwSize = sizeof(g_bReloadPlugins);
				RegQueryValueEx(hKey, SC_REGVAL_RELOAD, NULL, &dwType, (LPBYTE)&g_bReloadPlugins, &dwSize);
				RegCloseKey(hKey);
			}
		}
		g_dwLastReloadPluginsCheck = dwTicks;
	}
	
	if (g_bReloadPlugins == TRUE)
	{
		g_bReloadPlugins = FALSE;
		return ScPluginsLoad();
	}

	return FALSE;
}

void ScPluginsUnload()
{
	SC_PLUGIN* pPlugin;

	g_cPluginListCriticalSection.Enter();

	pPlugin = g_pPluginsList;

	while (pPlugin)
	{
		SC_PLUGIN* pNextPlugin = pPlugin->pNextPlugin;
		FreeLibrary((HINSTANCE)pPlugin->hModule);
		HeapFree(GetProcessHeap(),0,pPlugin);
		pPlugin = pNextPlugin;
	}

	g_pPluginsList = NULL;

	g_cPluginListCriticalSection.Leave();

	return;
}

BOOL ScPluginsIsEnabled()
{
	BOOL bIsEnabled = FALSE;
	SC_PLUGIN* pPluginsList;

	ScPluginsReloadIfNeeded();

	if (g_pPluginsList == NULL)
		return FALSE;

	g_cPluginListCriticalSection.Enter();
	pPluginsList = g_pPluginsList;
	while (pPluginsList)
	{
		if (pPluginsList->IsEnabled())
		{
			bIsEnabled = TRUE;
			break;
		}
		pPluginsList = pPluginsList->pNextPlugin;
	}
	g_cPluginListCriticalSection.Leave();
	return bIsEnabled;
}

BOOL ScPluginsProcessScript(WCHAR* pwcsScript, WCHAR* pwcsLanguage, WCHAR* pwcsURL, IActiveScript* pActiveScript, DWORD* pdwFlags)
{
	BOOL bResult = FALSE;
	DWORD dwFlags;
	SC_PLUGIN* pPluginsList;

	ScPluginsReloadIfNeeded();

	if (g_pPluginsList == NULL)
		return FALSE;

	g_cPluginListCriticalSection.Enter();
	if (pdwFlags)
		*pdwFlags = 0;
	pPluginsList = g_pPluginsList;
	while (pPluginsList)
	{
		if (pPluginsList->ProcessScript)
		{
			dwFlags = 0;
			if (pPluginsList->ProcessScript(pwcsScript, pwcsLanguage, pwcsURL, pActiveScript, &dwFlags))
			{
				bResult = TRUE;
				if (pdwFlags)
					*pdwFlags |= dwFlags;
				if (dwFlags & SCP_BLOCK)
					break;
			}
		}
		pPluginsList = pPluginsList->pNextPlugin;
	}
	g_cPluginListCriticalSection.Leave();
	return bResult;
}


BOOL ScPluginsCheckPopupURL(DWORD dwAction, WCHAR* pwcsSourceURL, WCHAR* pwcsDestinationURL, DWORD* pdwFlags, tCheckPopupURLCallbackFn pCallbackFn, LPVOID pCallbackCtx)
{
	BOOL bResult = FALSE;
	DWORD dwFlags;
	SC_PLUGIN* pPluginsList;

	ScPluginsReloadIfNeeded();

	if (g_pPluginsList == NULL)
		return FALSE;

	if (pwcsSourceURL == NULL)
		pwcsSourceURL = L"<unknown>";
	if (pwcsDestinationURL == NULL)
		pwcsDestinationURL = L"<unknown>";

	g_cPluginListCriticalSection.Enter();
	if (pdwFlags)
		*pdwFlags = 0;
	pPluginsList = g_pPluginsList;
	while (pPluginsList)
	{
		if (pPluginsList->CheckPopupURL)
		{
			dwFlags = 0;
			if (pPluginsList->CheckPopupURL(dwAction, pwcsSourceURL, pwcsDestinationURL, &dwFlags, pCallbackFn, pCallbackCtx))
			{
				bResult = TRUE;
				if (pdwFlags)
					*pdwFlags |= dwFlags;
				if (dwFlags & SCP_BLOCK)
					break;
			}
		}
		pPluginsList = pPluginsList->pNextPlugin;
	}
	g_cPluginListCriticalSection.Leave();
	return bResult;
}

BOOL ScPluginsIsDispatchEnabled(DWORD dwActionId)
{
	BOOL bResult = FALSE;
	SC_PLUGIN* pPluginsList;

	ScPluginsReloadIfNeeded();

	if (g_pPluginsList == NULL)
		return FALSE;

	g_cPluginListCriticalSection.Enter();
	pPluginsList = g_pPluginsList;
	while (pPluginsList)
	{
		if (pPluginsList->IsDispatchEnabled)
		{
			if (pPluginsList->IsDispatchEnabled(dwActionId))
			{
				bResult = TRUE;
				break;
			}
		}
		else if (pPluginsList->Dispatch)
		{
			bResult = TRUE;
			break;
		}
		pPluginsList = pPluginsList->pNextPlugin;
	}
	g_cPluginListCriticalSection.Leave();
	return bResult;
}

BOOL ScPluginsDispatch(DWORD dwActionId, void* pActionDataStruct, DWORD dwActionDataSize, DWORD* pdwFlags)
{
	BOOL bResult = FALSE;
	SC_PLUGIN* pPluginsList;
	DWORD dwFlags = 0;

	ScPluginsReloadIfNeeded();

	if (g_pPluginsList == NULL)
		return FALSE;

	g_cPluginListCriticalSection.Enter();
	if (pdwFlags)
		*pdwFlags = 0;
	pPluginsList = g_pPluginsList;
	while (pPluginsList)
	{
		if (pPluginsList->Dispatch)
		{
			HRESULT hres;
			dwFlags = 0;
			hres = pPluginsList->Dispatch(dwActionId, pActionDataStruct, dwActionDataSize, &dwFlags);
			if (hres == E_ACCESSDENIED)
			{
				dwFlags |= SCP_BLOCK;
				hres = S_OK;
			}
			if (SUCCEEDED(hres))
			{
				bResult = TRUE;
				if (pdwFlags)
					*pdwFlags |= dwFlags;
				if (dwFlags & (SCP_BLOCK | SCP_WHITELISTED))
					break;
			}
		}
		pPluginsList = pPluginsList->pNextPlugin;
	}
	g_cPluginListCriticalSection.Leave();
	return bResult;
}

