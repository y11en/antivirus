//-----------------------------------------------------
// gina.cpp : Defines the entry point for the DLL application.
//

#include <windows.h>
#include <winwlx.h>
#include <commctrl.h>
#include "resource.h"
#include "..\..\ProductCore\basegui\png\pngimg.h"
#include "service\sa.h"
#include "ver_mod.h"

HINSTANCE   g_hModule;

#ifdef _WIN64
	#define _KEY_WOW64_32KEY KEY_WOW64_32KEY
#else
	#define _KEY_WOW64_32KEY 0
#endif

//-----------------------------------------------------

class KLAnimation : public CImagePng
{
public:
	KLAnimation() :
		m_hDesktop(NULL),
		m_nTimeout(10),
		m_nCurrFrame(0),
		m_nCounter(0),
		m_nTimerID(-1),
		m_hWindow(NULL),
		m_hThread(NULL),
		m_hWndBack(NULL),
		m_bLoaded(false),
		m_bShow(false),
		m_hFile(NULL)
	{
		if( GetSystemMetrics(SM_CLEANBOOT) )
			return;

		DWORD nThreadId;
		m_hThread = CreateThread(NULL, 0, _MainLoop, this, 0, &nThreadId);
		m_hSecure = SA_Create(SA_AUTHORITY_EVERYONE, SA_ACCESS_MASK_ALL);
//		m_hFile = fopen("c:\\klogon.log", "w");
		Trace("start tracing\n");
	}

	void Trace(LPCTSTR strFmt, ...)
	{
		if( !m_hFile )
			return;

		va_list argptr;
		va_start(argptr, strFmt);
		vfprintf(m_hFile, strFmt, argptr);
		fflush(m_hFile);
		va_end(argptr);
	}

	~KLAnimation()
	{
		if( m_hSecure )
			SA_Destroy(m_hSecure);

		if( m_hThread )
			CloseHandle(m_hThread);

		if( m_hFile )
			fclose(m_hFile);
	}

	static ULONG WINAPI     _MainLoop(PVOID pArg){ return ((KLAnimation*)pArg)->MainLoop(); }

	static LRESULT CALLBACK _WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
	{
		KLAnimation *pThis = (KLAnimation*)GetWindowLong(hWnd, GWLP_USERDATA);
		switch( message )
		{
		case WM_PAINT: pThis->Draw(); break;
		case WM_TIMER:
			if( wParam == 1 )
			{
				if( !((++pThis->m_nCounter)%33) )
					pThis->UpdateShowStatus();
				pThis->CheckPlacement();
			}
			break;
		case WM_DESTROY:
			PostThreadMessage(GetCurrentThreadId(), WM_QUIT, 0, 0);
			break;

		case WM_ACTIVATEAPP: return TRUE;
		case WM_MOUSEACTIVATE: return MA_NOACTIVATEANDEAT;
		}
		return DefWindowProc(hWnd, message, wParam, lParam);
	}

	bool MainLoop()
	{
		if( !LoadImage() )
			return false;

		HDESK hDesktop = NULL;
		MSG msg;
		bool bChangingDesktop = false;

	_ChangeDesktop:
		if( hDesktop = m_hDesktop )
			SetThreadDesktop(hDesktop);

		PrepareWindow();
		UpdateShowStatus();
		CheckPlacement();
		SetTimer(m_hWindow, 1, 30, NULL);

	_MessageLoop:
		while( GetMessage(&msg, NULL, 0, 0) )
		{
			bChangingDesktop = false;
			TranslateMessage(&msg);
			DispatchMessage(&msg);

			if( hDesktop != m_hDesktop )
			{
				Trace("change desktop\n");
				bChangingDesktop = true;
				goto _ChangeDesktop;
			}
		}
		if( msg.message == WM_QUIT && bChangingDesktop )
		{
			bChangingDesktop = false;
			goto _MessageLoop;
		}
		return true;
	}

	void SetDesktop(HDESK hDesktop)
	{
		m_hDesktop = hDesktop;
	}

	bool PrepareWindow()
	{
		if( IsWindow(m_hWindow) )
		{
			DestroyWindow(m_hWindow);
			m_hWindow = NULL;
			m_hWndBack = NULL;
			m_bShow = false;
		}

		WNDCLASSEX wcex;
		memset(&wcex, 0, sizeof(WNDCLASSEX));
		wcex.cbSize			= sizeof(WNDCLASSEX); 
		wcex.hInstance		= g_hModule;
		wcex.style			= CS_HREDRAW | CS_VREDRAW;
		wcex.lpfnWndProc	= (WNDPROC)DefWindowProc;
		wcex.hbrBackground  = NULL;
		wcex.hCursor        = LoadCursor(NULL, IDC_ARROW);// predefined arrow 
		wcex.lpfnWndProc    = _WndProc;
		wcex.lpszClassName	= "KLPlayWindow";

		RegisterClassEx(&wcex);

		HWND hActiveWindow = GetForegroundWindow();

		m_hWindow = CreateWindowEx(WS_EX_TOOLWINDOW|0x08000000, "KLPlayWindow", NULL, WS_POPUP,
			m_offs.cx, m_offs.cy, m_szSize.cx, m_szSize.cy,
			GetDesktopWindow(), NULL, g_hModule, NULL);

		if( !m_hWindow )
			return false;

		SetWindowLong(m_hWindow, GWLP_USERDATA, (LONG)this);
		SetWindowPos(m_hWindow, HWND_BOTTOM, 0, 0, 0, 0, SWP_NOMOVE|SWP_NOSIZE);

		if( hActiveWindow && hActiveWindow != GetForegroundWindow() )
			SetForegroundWindow(hActiveWindow);
		return true;
	}

	bool UpdateShowStatus()
	{
		HANDLE hBlMutex = ::OpenMutex(MUTEX_ALL_ACCESS, FALSE, "Global\\AVP.Mutex." VER_PRODUCTNAME_STR ".BL");
		bool bShow = !!hBlMutex;
		if( hBlMutex )
			CloseHandle(hBlMutex);

		Trace("UpdateShowStatus (%d)\n", (int)bShow);

		if( bShow )
		{
			HKEY hKey = NULL;
			RegOpenKeyEx(HKEY_LOCAL_MACHINE, VER_PRODUCT_REGISTRY_PATH "\\settings", 0, KEY_READ | _KEY_WOW64_32KEY, &hKey);
			
			if( hKey )
			{
				DWORD bLoginShow = 1, nSize = sizeof(bLoginShow);
				if( RegQueryValueEx(hKey, "EnableLoginShow", NULL, NULL, (PBYTE)&bLoginShow, &nSize) == ERROR_SUCCESS )
					bShow = !!bLoginShow;
				RegCloseKey(hKey);
			}
		}

		if( m_bShow != bShow )
			ShowWindow(m_hWindow, (m_bShow = bShow) ? SW_SHOW : SW_HIDE);

		return m_bShow;
	}

	void CheckPlacement()
	{
		if( !m_bShow )
			return;

		BOOL bActive = FALSE;
		if( SystemParametersInfo(114/*SPI_GETSCREENSAVERRUNNING*/, 0, &bActive, 0) && bActive )
			return;

		if( GetSystemMetrics(SM_CXFULLSCREEN) != m_offs.cx + m_szSize.cx )
		{
			m_offs.cx = GetSystemMetrics(SM_CXFULLSCREEN)-m_szSize.cx;
			SetWindowPos(m_hWindow, NULL, m_offs.cx, m_offs.cy, 0, 0, SWP_NOSIZE|SWP_NOOWNERZORDER|SWP_NOZORDER);
		}

		POINT pt = {m_offs.cx + 1, m_offs.cy + m_szSize.cy - 1};
		HWND hWnd = WindowFromPoint(pt);

		if( hWnd && hWnd != m_hWndBack && (hWnd != m_hWindow || !m_hWndBack) )
		{
			RECT rc; GetWindowRect(hWnd, &rc);
			if( rc.left <= 2 )
			{
				m_hWndBack = hWnd;
				ShowWindow(m_hWindow, SW_HIDE);
//				SetWindowPos(m_hWindow, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE|SWP_NOSIZE);
			}
		}

		if( !m_hWndBack )
			SetWindowPos(m_hWindow, HWND_BOTTOM, 0, 0, 0, 0, SWP_NOMOVE|SWP_NOSIZE);
		else
			CheckBackground();
	}

	void CheckBackground()
	{
		if( !m_hWndBack )
			return;

		HDC dc = GetDC(NULL);

		for(int i = 0; i < 5; i++)
		{
			if( m_bgPix != -1 )
			{	
				COLORREF bgPix = GetPixel(dc, m_offs.cx + m_bgPt.x, m_offs.cy + m_bgPt.y);

				if( bgPix == CLR_INVALID || bgPix == m_bgPix )
					break;
			}

			Trace("draw picture\n");

			RECT rc = {m_offs.cx, m_offs.cy, m_offs.cx+m_szSize.cx, m_offs.cy+m_szSize.cy};
			CImagePng::Draw(dc, rc, NULL, 0);
			m_bgPix = GetPixel(dc, m_offs.cx + m_bgPt.x, m_offs.cy + m_bgPt.y);

			Sleep(10);
		}
		ReleaseDC(NULL, dc);
	}

	bool LoadImage()
	{
		Trace("load image\n");

		CHAR strPath[MAX_PATH];
		ULONG nSize = sizeof(strPath); ULONG nType;

		HKEY hKey = NULL;
		RegOpenKeyEx(HKEY_LOCAL_MACHINE, VER_PRODUCT_REGISTRY_PATH "\\settings", 0, KEY_READ | _KEY_WOW64_32KEY, &hKey);

		if( !hKey || RegQueryValueEx(hKey, "LoginImage", 0, &nType, (PBYTE)strPath, &nSize) != ERROR_SUCCESS )
		{
			GetModuleFileName((HINSTANCE)g_hModule, strPath, sizeof(strPath));

			char* szName = strrchr(strPath, '\\');
			if( !szName )
				return false;

			strcpy(++szName, "logo.png");
		}

		if( hKey )
			RegCloseKey(hKey);

		BYTE* pData = NULL;
		DWORD nDataSize= 0, nReadBytes = 0;

		Trace("image path(%s)\n", strPath);

		HANDLE hFile = CreateFile(strPath, GENERIC_READ, FILE_SHARE_READ,
			NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

		if( hFile != INVALID_HANDLE_VALUE )
		{
			nDataSize = GetFileSize(hFile, NULL);
			pData = new BYTE[nDataSize];

			ReadFile(hFile, pData, nDataSize, &nReadBytes, NULL);
			CloseHandle(hFile);
		}
		else
			Trace("CreateFile failed\n");

		bool bResult = pData ? !!CImagePng::Init(pData, nDataSize) : false;

		if( !bResult )
			Trace("CImagePng::Init\n");

		if( pData )
			delete[] pData;

		m_offs.cx = GetSystemMetrics(SM_CXFULLSCREEN)-m_szSize.cx;
		m_offs.cy = 0;//GetSystemMetrics(SM_CYFULLSCREEN)-m_szSize.cy;
		return bResult;
	}

	void Draw()
	{
		PAINTSTRUCT ps;
		HDC dc = BeginPaint(m_hWindow, &ps);

		if( !m_hWndBack )
		{
			PaintDesktop(dc);
			RECT rc = {0, 0, m_szSize.cx, m_szSize.cy};
			CImagePng::Draw(dc, rc, NULL, 0);
		}

		EndPaint(m_hWindow, &ps); 
	}

private:
	HSA         m_hSecure;
	HANDLE      m_hThread;
	HDESK       m_hDesktop;
	HWND        m_hWindow;
	HWND        m_hWndBack;
	DWORD       m_nTimeout;
	DWORD       m_nCounter;
	UINT        m_nCurrFrame;
	DWORD       m_nTimerID;
	SIZE        m_offs;
	bool        m_bLoaded;
	bool        m_bShow;
	FILE*       m_hFile;
};

KLAnimation g_pAnime;

//-----------------------------------------------------

VOID WLEventStart(PWLX_NOTIFICATION_INFO pInfo)
{
	g_pAnime.SetDesktop(pInfo ? pInfo->hDesktop : NULL);
}

VOID WLEventStop(PWLX_NOTIFICATION_INFO pInfo)
{
}

//-----------------------------------------------------

BOOL APIENTRY DllMain(HANDLE hModule, DWORD ul_reason_for_call, LPVOID lpReserved)
{
    switch (ul_reason_for_call)
	{
		case DLL_PROCESS_ATTACH:
			g_hModule = (HMODULE)hModule;
			break;
		case DLL_THREAD_ATTACH:
		case DLL_THREAD_DETACH:
		case DLL_PROCESS_DETACH:
			break;
    }
    return TRUE;
}

//-----------------------------------------------------
