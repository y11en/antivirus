// CKAHUMTESTUI2.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "CKAHUMTESTUI2.h"
#include "CKAHUMTESTUI2Dlg.h"
#include "afxsock.h"
#include <time.h>
#include <locale>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CCKAHUMTESTUI2App

BEGIN_MESSAGE_MAP(CCKAHUMTESTUI2App, CWinApp)
	//{{AFX_MSG_MAP(CCKAHUMTESTUI2App)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG
	ON_COMMAND(ID_HELP, CWinApp::OnHelp)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CCKAHUMTESTUI2App construction

#define TRACEMEMALLOC

#if defined(TRACEMEMALLOC) && defined(_DEBUG)
	#define _TRACEMEMALLOC_
#endif

#ifdef _TRACEMEMALLOC_
int AllocHook( int allocType, void *userData, size_t size, int blockType, 
              long requestNumber, const unsigned char *filename, int lineNumber)
{
    if (size == 1024)
    {
        switch (allocType)
        {
        case _HOOK_ALLOC:
            return 1;

        case _HOOK_REALLOC:
            return 1;

        case _HOOK_FREE:
            return 1;
        }
    }
   
    return 1;
}
#endif

CCKAHUMTESTUI2App::CCKAHUMTESTUI2App()
{
	int nOldState = _CrtSetDbgFlag(_CRTDBG_REPORT_FLAG);
	_CrtSetDbgFlag(nOldState | _CRTDBG_LEAK_CHECK_DF);

#ifdef _TRACEMEMALLOC_
	_CrtSetAllocHook ((_CRT_ALLOC_HOOK)AllocHook);
#endif

#ifdef _DEBUG
	HINSTANCE hMFC42d = GetModuleHandle (_T("mfc42d.dll"));
	if (hMFC42d)
	{
		void *pDestructor_AFX_DEBUG_STATE = GetProcAddress (hMFC42d, MAKEINTRESOURCEA(543));
		if (pDestructor_AFX_DEBUG_STATE)
		{
			DWORD dwOldProtection = 0;
			if (VirtualProtect (pDestructor_AFX_DEBUG_STATE, 1, PAGE_READWRITE, &dwOldProtection))
			{
				*(unsigned char *)pDestructor_AFX_DEBUG_STATE = 0xC3; // ret
			}
		}
	}
#endif
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CCKAHUMTESTUI2App object

CCKAHUMTESTUI2App theApp;

/////////////////////////////////////////////////////////////////////////////
// CCKAHUMTESTUI2App initialization

BOOL CCKAHUMTESTUI2App::InitInstance()
{
	// Standard initialization
	// If you are not using these features and wish to reduce the size
	//  of your final executable, you should remove from the following
	//  the specific initialization routines you do not need.

	// Set current program locale information
	{
		TCHAR szACP[9] = {_T('.')};
		_itot(GetACP(), szACP + 1, 10);
		setlocale (LC_ALL, szACP);
		std::locale::global(std::locale(szACP));
		_tzset(); // initialize _timezone etc. global msvcrt variables
	}

	CoInitialize (NULL);

	AfxSocketInit ();

#ifdef _AFXDLL
//	Enable3dControls();			// Call this when using MFC in a shared DLL
#else
	Enable3dControlsStatic();	// Call this when linking to MFC statically
#endif

	CCKAHUMTESTUI2Dlg dlg;
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

	// Since the dialog has been closed, return FALSE so that we exit the
	//  application, rather than start the application's message pump.
	return FALSE;
}

int CCKAHUMTESTUI2App::ExitInstance() 
{
	CoUninitialize ();
	
	return CWinApp::ExitInstance();
}
