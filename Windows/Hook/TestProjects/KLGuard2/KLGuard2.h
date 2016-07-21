// KLGuard2.h : main header file for the KLGUARD2 application
//

#if !defined(AFX_KLGUARD2_H__AAB5AAEA_9B5C_452A_9ADC_1B8E782AF4AE__INCLUDED_)
#define AFX_KLGUARD2_H__AAB5AAEA_9B5C_452A_9ADC_1B8E782AF4AE__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// CKLGuard2App:
// See KLGuard2.cpp for the implementation of this class
//

class CKLGuard2App : public CWinApp
{
public:
	CKLGuard2App();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CKLGuard2App)
	public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CKLGuard2App)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_KLGUARD2_H__AAB5AAEA_9B5C_452A_9ADC_1B8E782AF4AE__INCLUDED_)
