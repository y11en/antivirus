#include "stdafx.h"
#include "DecryptorNTLM.h"
#include "DecryptorNTLMDialog.h"

#ifdef _DEBUG
    #define new DEBUG_NEW
    #undef THIS_FILE
    static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// DecryptorNTLMApplication

BEGIN_MESSAGE_MAP(DecryptorNTLMApplication, CWinApp)
	//{{AFX_MSG_MAP(DecryptorNTLMApplication)
	//}}AFX_MSG
	ON_COMMAND(ID_HELP, CWinApp::OnHelp)
END_MESSAGE_MAP()

DecryptorNTLMApplication::DecryptorNTLMApplication()
{
}

// The one and only DecryptorNTLMApplication object
DecryptorNTLMApplication theApp;


BOOL DecryptorNTLMApplication::InitInstance()
{
	DecryptorNTLMDialog dlg;
	m_pMainWnd = &dlg;
	dlg.DoModal();
	return FALSE;
}
