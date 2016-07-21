// CheckFSLib.h : main header file for the CHECKFSLIB application
//

#if !defined(AFX_CHECKFSLIB_H__E6B7B0AD_F14F_41D0_BA2B_6C8CEC45F9A4__INCLUDED_)
#define AFX_CHECKFSLIB_H__E6B7B0AD_F14F_41D0_BA2B_6C8CEC45F9A4__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// CCheckFSLibApp:
// See CheckFSLib.cpp for the implementation of this class
//

class CCheckFSLibApp : public CWinApp
{
public:
	CCheckFSLibApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CCheckFSLibApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CCheckFSLibApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CHECKFSLIB_H__E6B7B0AD_F14F_41D0_BA2B_6C8CEC45F9A4__INCLUDED_)
