// IGuard.cpp : Defines the initialization routines for the DLL.
//

#include "stdafx.h"
#include "IGuard.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//
//	Note!
//
//		If this DLL is dynamically linked against the MFC
//		DLLs, any functions exported from this DLL which
//		call into MFC must have the AFX_MANAGE_STATE macro
//		added at the very beginning of the function.
//
//		For example:
//
//		extern "C" BOOL PASCAL EXPORT ExportedFunction()
//		{
//			AFX_MANAGE_STATE(AfxGetStaticModuleState());
//			// normal function body here
//		}
//
//		It is very important that this macro appear in each
//		function, prior to any calls into MFC.  This means that
//		it must appear as the first statement within the 
//		function, even before any object variable declarations
//		as their constructors may generate calls into the MFC
//		DLL.
//
//		Please see MFC Technical Notes 33 and 58 for additional
//		details.
//

/////////////////////////////////////////////////////////////////////////////
// CIGuardApp

BEGIN_MESSAGE_MAP(CIGuardApp, CWinApp)
	//{{AFX_MSG_MAP(CIGuardApp)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CIGuardApp construction

CIGuardApp::CIGuardApp()
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CIGuardApp object

CIGuardApp theApp;

#define _REG_PATH "Software\\KasperskyLab\\KLGuard\\"
CRegStorage g_RegStorage = CRegStorage(HKEY_CURRENT_USER, _REG_PATH);
CRegStorage* g_pRegStorage = &g_RegStorage;
OSVERSIONINFO g_OSVer;

HANDLE g_hHeap;

long g_HeapUsage4 = 0;

void* _MM_Alloc(unsigned int nSize)
{
	InterlockedIncrement(&g_HeapUsage4);
	return HeapAlloc(g_hHeap, 0, nSize);
}

void _MM_Free(void* p)
{
	if (p == NULL)
	{
/*#ifdef _DEBUG
		OutputDebugString("Try to free memory with NULL pointer\n");
#endif*/
		return;
	}
	InterlockedDecrement(&g_HeapUsage4);
	HeapFree(g_hHeap, 0, p);
}

#include "DConfig.h"
#include "DEventInfo.h"

APP_REGISTRATION AppReg;
HANDLE g_hDrvDevice = NULL;

CHAR ThisFileName[MAX_PATH * 2];

extern "C" __declspec( dllexport )int CfgProc(HINSTANCE hMainInstance, HWND hMainWnd, HANDLE hDevice, PAPP_REGISTRATION pApp)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	g_hDrvDevice = hDevice;

	g_hHeap = GetProcessHeap();

	g_OSVer.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
	if(!GetVersionEx(&g_OSVer))
		return -1;

	GetModuleFileName(AfxGetInstanceHandle(), ThisFileName, sizeof(ThisFileName));

	int _sign = sign_check_file(ThisFileName, 1, NULL, 0, 0);
	if (_sign != 0)
		return _sign;

	KLKR_Init_Library(_MM_Alloc, _MM_Free);

	CWnd Wnd;
	Wnd.Attach(hMainWnd);
	CopyMemory(&AppReg, pApp, sizeof(APP_REGISTRATION));

	//AppReg.m_AppID = AVPG_MPraguePlugin;
	//AppReg.m_AppID = PersonalFireWall;
#ifdef _DEBUG
	CString strtmp;
	strtmp.Format("AVPG Client: AppId: %d, ClientType: %s, LogFileName: '%s'\n", AppReg.m_AppID, AppReg.m_ClientFlags & _CLIENT_FLAG_POPUP_TYPE != 0 ? "ClientPopup" : "ClientNonPopup", AppReg.m_LogFileName);
	OutputDebugString(strtmp);
#endif

	// -----------------------------------------------------------------------------------------
	CDConfig DConfig(&Wnd);
	DConfig.DoModal();
	// -----------------------------------------------------------------------------------------
	Wnd.Detach();

	KLKR_Shutdown_Library();

#ifdef _DEBUG
	strtmp.Format("Heap usage count in IGuard: %d\n", g_HeapUsage4);
	OutputDebugString(strtmp);
#endif
			
	return 0;
}

// -----------------------------------------------------------------------------------------
extern "C" __declspec( dllexport )int EventInfoProc(HWND hMainWnd, PEVENT_TRANSMIT pEvt)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	
	g_hHeap = GetProcessHeap();
	
	g_OSVer.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
	if(!GetVersionEx(&g_OSVer))
		return -1;
	
	CWnd Wnd;
	Wnd.Attach(hMainWnd);

	// -----------------------------------------------------------------------------------------
	CDEventInfo DEventInfo(pEvt, &Wnd);
	DEventInfo.DoModal();
	// -----------------------------------------------------------------------------------------
	Wnd.Detach();

	return 0;
}