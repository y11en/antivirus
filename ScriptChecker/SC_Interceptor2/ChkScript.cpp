#include <windows.h>
#include <version/ver_product.h>

#include "debug.h"

#include "CommonStruct.h"
#include "scr_inst.h"
#include "calcsum.h"
#include "scripting/activscp.h"
#include "resource.h"

#include "siteinfo.h"
#include "statusbaricon.h"

#include "sign\sign.h"

#include "scplugins.h"
#include "decode.h"

#include "IeStatusBar.h"

#define PLACE_POPUP_ICON 3

extern HINSTANCE g_hInstance;

// -----------------------------------------------------------------------------------------
HRESULT	CheckScriptText2(SCRIPT_ENGINE_INSTANCE_DATA* pScriptEngineInstanceData, WCHAR* pwcsScriptText, WCHAR* pwcsURL)
{
	tERROR error = errOK;
	HRESULT hResult = S_OK;
	WCHAR* pwcsLanguageName = L"<unknown>";
	DWORD dwSCPFlags = 0;
	IActiveScript* pActiveScript = NULL; 

	if (pwcsScriptText == NULL || pwcsScriptText[0] == 0)
	{
		ODS(("CheckScriptText2 script empty, pass\n"));
		return S_OK;
	}
	
	if (NULL == pwcsURL)
	{
		pwcsURL = L"<unknown>";
	}
	
	if (pScriptEngineInstanceData!=NULL)
	{
		pActiveScript = pScriptEngineInstanceData->pActiveScript;

		if (pScriptEngineInstanceData->pwcsLanguageName!=NULL)
			pwcsLanguageName = pScriptEngineInstanceData->pwcsLanguageName;

		// Check for encoded script
		if (pScriptEngineInstanceData->bEncoded 
			|| (pwcsScriptText[0] == '#' 
				&& pwcsScriptText[1] == '@' 
				&& pwcsScriptText[2] == '~' 
				&& pwcsScriptText[3] == '^'))
		{
			DWORD dwAllocSize = (DWORD)(wcslen(pwcsScriptText) + 1) * sizeof(WCHAR);
			WCHAR* pwcsDecodedScriptText = (WCHAR*)HeapAlloc(GetProcessHeap(), 0, dwAllocSize);
			if (pwcsDecodedScriptText == NULL)
				return E_OUTOFMEMORY;
			ODS(("Language: %S. Decoding script source...", pwcsLanguageName));
			DecodeScript(pwcsScriptText, pwcsDecodedScriptText, dwAllocSize);
			pwcsScriptText = pwcsDecodedScriptText;
		}
	}


	ScPluginsProcessScript(pwcsScriptText, pwcsLanguageName, pwcsURL, pActiveScript, &dwSCPFlags);
	if (dwSCPFlags & SCP_BLOCK)
	{
		hResult = E_ACCESSDENIED;
	}
	if (dwSCPFlags & SCP_BLOCK_FURTHER)
	{
		if (pScriptEngineInstanceData != NULL)
			pScriptEngineInstanceData->bDenyExecution = TRUE;
	}

	return hResult;
}

// -----------------------------------------------------------------------------------------
HRESULT	CheckScriptText(LPVOID pCurEngine, WCHAR* pwcScriptText)
{
	HRESULT hCheckResult = S_OK;
	HWND hWndStatusBar = NULL;
	WCHAR* pwcsLanguage = L"<unknown>";
	SCRIPT_ENGINE_INSTANCE_DATA* pScriptEngineInstanceData = NULL;
	BOOL  bCheckerEnabled;
//	RECT bmrect;
	WCHAR* pURL = NULL;
	DWORD dwUrlN = 0;

#define _TIMING_
#if defined(_TIMING_) && defined(_DEBUG)
	// Init timing
	__int64 t;
	__int64 t1;
	static __int64 t2=0;
	static DWORD __cnt=0;
	QueryPerformanceCounter((LARGE_INTEGER*)&t);
#endif


	pScriptEngineInstanceData = GetScriptEngineInstanceData(pCurEngine);
	
	bCheckerEnabled = ScPluginsIsEnabled();

	if (pScriptEngineInstanceData == NULL)
	{
		ODS(("CheckScriptText: ERROR: GetScriptEngineInstanceData failed - instance not registered? (_this=%08X)", pCurEngine));
	}
	else
	{
		bool bDrawIcon = FALSE;
		HKEY settings_key = NULL;
		
		if (pScriptEngineInstanceData->bDenyExecution)
		{
			// script execution is denied
			ODS(("CheckScriptText: script blocked - scripts are denied for this script engine"));
			return E_ACCESSDENIED;
		}
		
		if (pScriptEngineInstanceData->pwcsLanguageName)
			pwcsLanguage = pScriptEngineInstanceData->pwcsLanguageName;
		
		if (ERROR_SUCCESS == RegOpenKey(HKEY_LOCAL_MACHINE, VER_PRODUCT_REGISTRY_PATH "\\profiles\\Script_Monitoring\\settings", &settings_key))
		{
			DWORD dwType;
			DWORD dwData;
			DWORD dwSize = sizeof(dwData);
			if (ERROR_SUCCESS == RegQueryValueEx(settings_key, "DrawIcon", NULL, &dwType, (LPBYTE)&dwData, &dwSize))
				bDrawIcon = (dwData != 0);
			RegCloseKey(settings_key);
		}
		
		if (bDrawIcon)
		{
			if (pScriptEngineInstanceData->hWndStatusBar == NULL)
			{
				pScriptEngineInstanceData->hWndStatusBar = GetExplorerStatusBarHWnd(pScriptEngineInstanceData->pActiveScriptSite, NULL);
				
				hWndStatusBar = pScriptEngineInstanceData->hWndStatusBar;
				if(hWndStatusBar)
					IeStatusBarSetState(hWndStatusBar, PLACE_POPUP_ICON, bCheckerEnabled ? SCSTATE_ACTIVE : SCSTATE_INACTIVE, NULL, NULL);
			}
		}

		pScriptEngineInstanceData->dwScriptsCount++;
		dwUrlN = pScriptEngineInstanceData->dwScriptsCount;
		if (pScriptEngineInstanceData->wcsURL == NULL)
			pScriptEngineInstanceData->wcsURL = GetSourceURL(pScriptEngineInstanceData->pActiveScriptSite);
		pURL = pScriptEngineInstanceData->wcsURL;
	}

	if(bCheckerEnabled == FALSE)
	{
		ODS(("Script checker disabled, script execution passed\n"));
		hCheckResult = S_OK;
	}
	else
	{
		WCHAR  pUrlStack[0x100];
		WCHAR* pUrlAlloc = pUrlStack;
		if(pURL!=NULL && dwUrlN!=0)
		{
			if (wcslen(pURL) + 20 > countof(pUrlStack))
				pUrlAlloc = (WCHAR*)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, (wcslen(pURL) + 20) * sizeof(WCHAR));
			if (pUrlAlloc)
			{
				WCHAR wcsNum[20];
				wcscpy(pUrlAlloc, pURL);
				wcscat(pUrlAlloc, L"[");
				_itow(dwUrlN, wcsNum, 10);
				wcscat(pUrlAlloc, wcsNum);
				wcscat(pUrlAlloc, L"]");
				pURL = pUrlAlloc;
			}
		}

		if (FAILED(hCheckResult = CheckScriptText2(pScriptEngineInstanceData, pwcScriptText, pURL)))
		{
			ODS(("CheckScriptText: script blocked (script code check failed)."));
		}
// commented 31.01.2006 by Mike
// rescanning of pEngineScriptStr make huge overhits in same cases, but actually never help

//		else
//		{
//			ODS(("Script checked successfully\n"));
//
//			BOOL bCheckEngineScriptStr = FALSE;
//			WCHAR* pEngineScriptStr;
//
//			bCheckEngineScriptStr = (GetEngineScriptStr(pCurEngine) != NULL);
//			pEngineScriptStr = AddEngineScriptStr(pCurEngine, pwcScriptText);
//			if (bCheckEngineScriptStr) 
//			{
//				if (FAILED(hCheckResult = CheckScriptText2(pScriptEngineInstanceData, pEngineScriptStr, pURL)))
//				{
//					ODS(("CheckScriptText: script blocked (JOINED script code check failed)."));
//				}
//			}
//		}
		if (pUrlAlloc != pUrlStack)
			HeapFree(GetProcessHeap(), 0, pUrlAlloc);
	}

	// erase icon in status bar
	if(hWndStatusBar)
		IeStatusBarSetState(hWndStatusBar, PLACE_POPUP_ICON, SCSTATE_IDLE, NULL, NULL);
	
#if defined(_TIMING_) && defined(_DEBUG)
	// Calculate timing
	QueryPerformanceCounter((LARGE_INTEGER*)&t1);
	t=t1-t;
	QueryPerformanceFrequency((LARGE_INTEGER*)&t1);
	t2+=t;
	ODS(("CheckScriptText time=%d.%06ds, total time=%d.%06ds, count=%d", (DWORD)(t/t1), (DWORD)((t%t1)*1000000/t1), (DWORD)(t2/t1), (DWORD)((t2%t1)*1000000/t1), ++__cnt));	
	t1=t2;
#endif 
	
	if (FAILED(hCheckResult))
		hCheckResult = E_ACCESSDENIED;

	return hCheckResult;
}

// -----------------------------------------------------------------------------------------
/*
void Error()
{
#ifdef _DEBUG
	DWORD LastError = GetLastError();
	if (LastError != 0)
	{
		LPVOID lpMsgBuf;
		CHAR Tmp[1024];
		FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
			NULL,LastError,MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),(LPTSTR) &lpMsgBuf,0,NULL);
		wsprintf(Tmp,"KAVSC: Last error was: %s",LastError ? lpMsgBuf : "");
		LocalFree(lpMsgBuf);
		ODS((Tmp));
	}
#endif
}
*/