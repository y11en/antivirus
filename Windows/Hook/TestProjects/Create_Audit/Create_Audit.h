// Create_Audit.h : main header file for the CREATE_AUDIT application
//

#if !defined(AFX_CREATE_AUDIT_H__E1B3456F_1564_4F6D_B2E8_14E0E27CC232__INCLUDED_)
#define AFX_CREATE_AUDIT_H__E1B3456F_1564_4F6D_B2E8_14E0E27CC232__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// CCreate_AuditApp:
// See Create_Audit.cpp for the implementation of this class
//

class CCreate_AuditApp : public CWinApp
{
public:
	CCreate_AuditApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CCreate_AuditApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CCreate_AuditApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CREATE_AUDIT_H__E1B3456F_1564_4F6D_B2E8_14E0E27CC232__INCLUDED_)
