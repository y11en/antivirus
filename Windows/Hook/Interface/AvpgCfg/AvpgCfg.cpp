// AvpgCfg.cpp : Defines the initialization routines for the DLL.
//

#include "stdafx.h"
#include "AvpgCfg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


CReestr *g_pReestr = NULL;
CLocalize* g_pLocalize = NULL;

OSVERSIONINFO g_OSVer;
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
// CAvpgCfgApp

BEGIN_MESSAGE_MAP(CAvpgCfgApp, CWinApp)
	//{{AFX_MSG_MAP(CAvpgCfgApp)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CAvpgCfgApp construction

CAvpgCfgApp::CAvpgCfgApp()
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CAvpgCfgApp object

CAvpgCfgApp theApp;

#include "DConfig.h"
#include "DTreeMode.h"

APP_REGISTRATION AppReg;

extern "C" __declspec( dllexport )int CfgProc(HINSTANCE hMainInstance, HWND hMainWnd, HANDLE hDevice, PAPP_REGISTRATION pApp)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	int Result = -1;

	g_OSVer.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
	if(!GetVersionEx(&g_OSVer))
		return Result;

	if (hDevice != NULL)
	{

		g_pReestr = new CReestr(_AVP_GUARD);
		if (g_pReestr == NULL)
			return Result;

		g_pLocalize = new CLocalize("avpg_lng.*", g_pReestr, AfxGetInstanceHandle());

		HINSTANCE hInstance = AfxGetInstanceHandle();
		if (hInstance != NULL)
		{
			CWnd Wnd;
			Wnd.Attach(hMainWnd);
			CopyMemory(&AppReg, pApp, sizeof(APP_REGISTRATION));
#ifdef _DEBUG
			CString strtmp;
			strtmp.Format("AVPG Client: AppId: %d, ClientType: %s, LogFileName: '%s'\n", AppReg.m_AppID, AppReg.m_ClientFlags & _CLIENT_FLAG_POPUP_TYPE != 0 ? "ClientPopup" : "ClientNonPopup", AppReg.m_LogFileName);
			OutputDebugString(strtmp);
#endif
	// -----------------------------------------------------------------------------------------
			CDConfig DConfig(&Wnd, hDevice);
			/*Result = */DConfig.DoModal();
			Result = 0;

	// -----------------------------------------------------------------------------------------
			Wnd.Detach();
		}
		delete g_pLocalize;
		delete g_pReestr;
	}
	return Result;
}



BOOL CAvpgCfgApp::InitInstance() 
{
	// TODO: Add your specialized code here and/or call the base class
	AfxEnableControlContainer();
	CoInitialize(NULL);
	
	return CWinApp::InitInstance();
}

int CAvpgCfgApp::ExitInstance() 
{
	// TODO: Add your specialized code here and/or call the base class
	CoUninitialize();	
	return CWinApp::ExitInstance();
}
