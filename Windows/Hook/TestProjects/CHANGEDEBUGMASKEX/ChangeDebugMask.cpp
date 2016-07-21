// ChangeDebugMask.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "ChangeDebugMask.h"
#include "ChangeDebugMaskDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CChangeDebugMaskApp

BEGIN_MESSAGE_MAP(CChangeDebugMaskApp, CWinApp)
	//{{AFX_MSG_MAP(CChangeDebugMaskApp)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG
	ON_COMMAND(ID_HELP, CWinApp::OnHelp)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CChangeDebugMaskApp construction

CChangeDebugMaskApp::CChangeDebugMaskApp()
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CChangeDebugMaskApp object

CChangeDebugMaskApp theApp;

/////////////////////////////////////////////////////////////////////////////
// CChangeDebugMaskApp initialization

BOOL CChangeDebugMaskApp::InitInstance()
{
	LPSTR cmdLine = GetCommandLine();

#ifdef _AFXDLL
	Enable3dControls();			// Call this when using MFC in a shared DLL
#else
	Enable3dControlsStatic();	// Call this when linking to MFC statically
#endif

	CChangeDebugMaskDlg dlg;
	m_pMainWnd = &dlg;
	int nResponse = dlg.DoModal();
	if (nResponse == IDOK)
	{
		// TODO: Place code here to handle when the dialog is
		//  dismissed with OK
	}
	else if (nResponse == IDCANCEL)
	{
		// TODO: Place code here to handle when the dialog is
		//  dismissed with Cancel
	}

	return FALSE;
}
