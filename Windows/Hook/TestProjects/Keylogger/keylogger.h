// keylogger.h : main header file for the KEYLOGGER application
//

#if !defined(AFX_KEYLOGGER_H__A3F87241_65C7_4ECE_8EE6_3797F36192D4__INCLUDED_)
#define AFX_KEYLOGGER_H__A3F87241_65C7_4ECE_8EE6_3797F36192D4__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// CKeyloggerApp:
// See keylogger.cpp for the implementation of this class
//

class CKeyloggerApp : public CWinApp
{
public:
	CKeyloggerApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CKeyloggerApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CKeyloggerApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_KEYLOGGER_H__A3F87241_65C7_4ECE_8EE6_3797F36192D4__INCLUDED_)
