// avp32.h : main header file for the AVP32 application
//

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols
//#define MS(x) Mess[ x -IDS_NOTHING_FILES]
#include <StuffMFC/Globals.h>
#define MS(x) LoadStr(x)

/////////////////////////////////////////////////////////////////////////////
// CAvp32App:
// See avp32.cpp for the implementation of this class
//

class CAvp32App : public CWinApp
{
public:
	CAvp32App();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAvp32App)
	public:
	virtual BOOL InitInstance();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual int ExitInstance();
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CAvp32App)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////


