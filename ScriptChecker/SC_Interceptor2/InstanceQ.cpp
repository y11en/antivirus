#include "InstanceQ.h"
#include "debug.h"

typedef struct _ENGINE_INSTANCE
{
	OLECHAR DllName[MAX_PATH];
	HINSTANCE hIns;
	struct _ENGINE_INSTANCE* pNext;
}ENGINE_INSTANCE, *PENGINE_INSTANCE;

PENGINE_INSTANCE g_EngineQueue = NULL;

void FreeEngineQueue()
{
	PENGINE_INSTANCE pNext = g_EngineQueue;
	while (pNext != NULL)
	{
		FreeLibrary(pNext->hIns);
		g_EngineQueue = pNext->pNext;
		HeapFree(GetProcessHeap(), NULL, pNext);
		pNext = g_EngineQueue;
	}
//	ODS(("KAVSC: Engines removed\n"));
}

HINSTANCE IsEngineLoaded(OLECHAR* DllName)
{
	PENGINE_INSTANCE pNext = g_EngineQueue;
	while (pNext != NULL)
	{
		if (wcscmp(pNext->DllName, DllName) == 0)
		{
//			ODS(("KAVSC: Engine already loaded\n"));
			return pNext->hIns;
		}
		pNext = pNext->pNext;
	}

	return NULL;
}

BOOL AddNewEngine(OLECHAR* DllName, HINSTANCE hIns)
{
	PENGINE_INSTANCE pNext = (PENGINE_INSTANCE) HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, sizeof(ENGINE_INSTANCE));
	if (pNext != NULL)
	{
		ODS(("KAVSC: Engine loaded"));
		pNext->pNext = g_EngineQueue;
		g_EngineQueue = pNext;
		wcscpy(pNext->DllName, DllName);
		pNext->hIns = hIns;
		
		return TRUE;
	}

	return FALSE;
}

HINSTANCE GetEngineInstance(OLECHAR* DllName, WCHAR* wcsLanguage)
{
	OSVERSIONINFO osvi;
	HINSTANCE hIns = IsEngineLoaded(DllName);
	
	if (hIns != NULL)
		return hIns;

	ZeroMemory(&osvi, sizeof(OSVERSIONINFO));
    osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
	GetVersionEx(&osvi);

	if(osvi.dwPlatformId == VER_PLATFORM_WIN32_WINDOWS)
	{
		char szDllNameA[MAX_PATH];
		wsprintf(szDllNameA, "%S", DllName);
		hIns = LoadLibraryA(szDllNameA);
	}
	else
		hIns = LoadLibraryW((LPOLESTR)DllName);
	if (hIns == NULL)
	{
		size_t enginelen = wcslen(wcsLanguage);
		if (enginelen)
		{
			PWCHAR wEngineTmp = (PWCHAR) HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, (enginelen + 1) * sizeof(WCHAR));
			if (wEngineTmp)
			{
				wcscpy(wEngineTmp, wcsLanguage);
				_wcsupr(wEngineTmp);
				if (enginelen >= wcslen(L"jscript"))
				{
					if (memcmp(wEngineTmp, L"JSCRIPT", wcslen(L"jscript")) == 0)
						hIns = LoadLibraryA("jscript.dll");
				}
				if (enginelen >= wcslen(L"vbscript"))
				{
					if (memcmp(wEngineTmp, L"VBSCRIPT", wcslen(L"vbscript")) == 0)
						hIns = LoadLibraryA("vbscript");
				}

				HeapFree(GetProcessHeap(), NULL, wEngineTmp);
			}
		}
	}
	if (hIns != NULL)
	{
		if (AddNewEngine(DllName, hIns) == FALSE)
		{
			FreeLibrary(hIns);
			hIns = NULL;
		}
	}
	return hIns;
}

typedef HRESULT (*_tDllCanUnloadNow)(void);

BOOL EngineCanUnload()
{
	BOOL bResult = TRUE;
	ODS(("EngineCanUnload..."));
	_tDllCanUnloadNow pfDll;
	PENGINE_INSTANCE pNext = g_EngineQueue;
	while (pNext != NULL)
	{
		pfDll = (_tDllCanUnloadNow) GetProcAddress(pNext->hIns, "DllCanUnloadNow");
		ODS(("Asking EngineCanUnload of %S...", pNext->DllName));
		if (pfDll != NULL)
		{
			if (pfDll() == S_OK)
			{
				ODS(("S_OK"));
			}
			else
			{
				ODS(("S_FALSE"));
				bResult = FALSE;	
			}
		}	
		else
		{
			ODS(("Cannot get address of EngineCanUnload"));
			bResult = FALSE;
		}
		pNext = pNext->pNext;
	}

	if (bResult == FALSE)
	{
		ODS(("EngineCanUnload: Can't unload. Child is alive!"));
	}

	return bResult;
}