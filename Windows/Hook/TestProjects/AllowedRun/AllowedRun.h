// AllowedRun.h : main header file for the ALLOWEDRUN application
//

#if !defined(AFX_ALLOWEDRUN_H__0E757058_364E_4084_AE31_B0FD8762F79A__INCLUDED_)
#define AFX_ALLOWEDRUN_H__0E757058_364E_4084_AE31_B0FD8762F79A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// CAllowedRunApp:
// See AllowedRun.cpp for the implementation of this class
//

class CAllowedRunApp : public CWinApp
{
public:
	CAllowedRunApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAllowedRunApp)
	public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CAllowedRunApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ALLOWEDRUN_H__0E757058_364E_4084_AE31_B0FD8762F79A__INCLUDED_)
