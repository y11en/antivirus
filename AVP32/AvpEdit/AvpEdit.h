// AvpEdit.h : main header file for the AVPEDIT application
//

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"       // main symbols

/////////////////////////////////////////////////////////////////////////////
// CAvpEditApp:
// See AvpEdit.cpp for the implementation of this class
//

class CAvpEditApp : public CWinApp
{
public:
	CAvpEditApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAvpEditApp)
	public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CAvpEditApp)
	afx_msg void OnAppAbout();
	afx_msg void OnFileOpen();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
protected:
	HANDLE h_dllMethod;
};


/////////////////////////////////////////////////////////////////////////////
