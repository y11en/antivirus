// OEAntiSpam.cpp : Implementation of DLL Exports.

#include "stdafx.h"
#include "resource.h"
#include "OEAntiSpam.h"
#include "MainWindow.h"
#include "MyComModule.h"
#include "WndInterceptor.h"
#include "Tracing.h"
#include "RebarWindow.h"
#include "Utility.h"

#include <r3hook/sputnik.h>

CMyComModule _Module;
CMainWindow g_wnd;
CRebarWindow g_rebar;

BEGIN_OBJECT_MAP(ObjectMap)
END_OBJECT_MAP()

DWORD WINAPI WaitCreationProc(LPVOID pParam);
HANDLE g_hStopEvent = 0, g_hWaitThread = 0;

HHOOK g_hHook = NULL;

/////////////////////////////////////////////////////////////////////////////
// DLL Entry Point

extern "C"
BOOL WINAPI DllMain(HINSTANCE hInstance, DWORD dwReason, LPVOID /*lpReserved*/)
{
	switch (dwReason)
	{
	case DLL_PROCESS_ATTACH:
        _Module.Init(ObjectMap, hInstance, NULL);
		_Module._WriteOEVerToLog();
        DisableThreadLibraryCalls(hInstance);

		{
			// check whether this is a msimn.exe process
			TCHAR szBuf[MAX_PATH];
			GetModuleFileName(NULL, szBuf, MAX_PATH);

			// get the module name
			TCHAR *szPos = _tcsrchr(szBuf, _T('\\'));
			tstring strModuleName = szPos ? (szPos + 1) : szBuf;

			Utility::MakeStrUpper(strModuleName);

			if (strModuleName == _T("MSIMN.EXE") || strModuleName == _T("WINMAIL.EXE"))
			{
				ASTRACE(_T(""));
				ASTRACE(_T("------------------  OutlookExpress Antispam Plugin started ----------"));
				g_hStopEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
				DWORD dwTID;
				g_hWaitThread = CreateThread(NULL, 0, WaitCreationProc, (LPVOID)g_hStopEvent, 0, &dwTID);
				if(!g_hWaitThread)
					ASTRACE(_T("Failed to start wait thread"));

				if( (BYTE)::GetVersion() > 5 )
				{
					// под Vista мы должны успеть прочитать внутренние значения из файла,
					// пока Windows Mail не успел залочить файл
					COERule::ReadInternalValuesVista();
				}
			}
			else
			{
				tstring msg(_T("An attempt to load the DLL into process "));
				msg += strModuleName;
				ASTRACE(msg.c_str());
			}
		}

		// the main module initialization is performed in CMainWindow::OnAfterSubclass()
		break;

	case DLL_PROCESS_DETACH:
		if( g_hHook )
			UnhookWindowsHookEx(g_hHook);

		if(g_hWaitThread && g_hStopEvent)
		{
			SetEvent(g_hStopEvent);
			DWORD dwThreadState;
			if(GetExitCodeThread(g_hWaitThread, &dwThreadState) && dwThreadState == STILL_ACTIVE)
				WaitForSingleObject(g_hWaitThread, INFINITE);
		}
		if(g_hStopEvent)
			CloseHandle(g_hStopEvent);
		if(g_hWaitThread)
			CloseHandle(g_hWaitThread);
		ASTRACE(_T("------------------  OutlookExpress Antispam Plugin finished ----------"));

		CWndInterceptor::GetInstance().Destroy();
        _Module.Term();
		break;
	}

    return TRUE;    // ok
}

HWND GetOEWindow(LPCTSTR szWindowClass, HWND hwndParent = NULL)
{
	HWND hWnd = hwndParent ? 
		FindWindowEx(hwndParent, NULL, szWindowClass, NULL) :
		FindWindow(szWindowClass, NULL);
	if(hWnd)
	{
		DWORD dwPID;
		GetWindowThreadProcessId(hWnd, &dwPID);
		return dwPID == GetCurrentProcessId() ? hWnd : NULL;
	}
	return NULL;
}

DWORD WINAPI WaitCreationProc(LPVOID pParam)
{
	HANDLE hEvent = (HANDLE)pParam;
	HWND hWndBrowser = NULL, hwndRebar = NULL;

	do
	{
		if(!hWndBrowser && (hWndBrowser = GetOEWindow(_T("Outlook Express Browser Class"))) != NULL)
		{
			ASTRACE(_T("Browser window found"));
			g_wnd.m_bHasRebar = false;
			BOOL bCheck = g_wnd.SubclassWindow(hWndBrowser);
			if (bCheck)
			{
				ASTRACE(_T("Posting init message"));
				g_wnd.PostMessage(WM_AFTER_SUBCLASS, 0, 0);
			}
		}
		
		if(!hwndRebar && hWndBrowser && (hwndRebar = GetOEWindow(_T("SizableRebar"), hWndBrowser)) != NULL)
		{
			ASTRACE(_T("Rebar found"));
			if (!g_wnd.m_bHasRebar)
			{
				g_rebar.Initialize();
				g_rebar.SubclassWindow(hwndRebar);
				g_wnd.m_bHasRebar = true;
			}
		}
	
	} while((!hWndBrowser || !hwndRebar) && WaitForSingleObject(hEvent, 1000) == WAIT_TIMEOUT);

	return 0;
}

LRESULT CALLBACK CBT_Proc(int nCode, WPARAM wParam, LPARAM lParam)
{
	switch (nCode)
	{
	case HCBT_CREATEWND:
		{
			HWND hWnd = reinterpret_cast<HWND>(wParam);

			TCHAR szClass[256] = {0};
			GetClassName(hWnd, szClass, sizeof(szClass) - 1);
			if (*szClass == _T('\0'))
				return CallNextHookEx(g_hHook, nCode, wParam, lParam);

			tstring strClassName(szClass);
			if (strClassName == _T("Outlook Express Browser Class"))
			{
				ASTRACE(_T("Browser window creation detected"));
				g_wnd.m_bHasRebar = false;
				BOOL bCheck = g_wnd.SubclassWindow(hWnd);
				if (bCheck)
				{
					ASTRACE(_T("Posting init message"));
					g_wnd.PostMessage(WM_AFTER_SUBCLASS, 0, 0);
				}
			}
			else if (strClassName == _T("SizableRebar"))
			{
				ASTRACE(_T("Rebar creation detected"));
				if (!g_wnd.m_bHasRebar)
				{
					g_rebar.Initialize();
					g_rebar.SubclassWindow(hWnd);
					g_wnd.m_bHasRebar = true;
				}
			}
		}
		break;
	}

	return CallNextHookEx(g_hHook, nCode, wParam, lParam);
}

STDAPI DllRegisterServer()
{
	ASTRACE(_T("Registering plugin"));

	return RegisterSputnikDllByHandle("msoe.dll", _Module.GetModuleInstance());
}

STDAPI DllUnregisterServer()
{
	ASTRACE(_T("Unregistering plugin"));

	COESettings settings;
	settings.DeleteRules();

	return UnregisterSputnikDllByHandle("msoe.dll", _Module.GetModuleInstance());
}

