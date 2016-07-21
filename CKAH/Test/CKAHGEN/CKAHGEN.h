// CKAHGEN.h : main header file for the CKAHGEN application
//

#if !defined(AFX_CKAHGEN_H__ACDD7943_C828_4CFF_9D79_59CC1A096706__INCLUDED_)
#define AFX_CKAHGEN_H__ACDD7943_C828_4CFF_9D79_59CC1A096706__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// CCKAHGENApp:
// See CKAHGEN.cpp for the implementation of this class
//

class CCKAHGENApp : public CWinApp
{
public:
	CCKAHGENApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CCKAHGENApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CCKAHGENApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CKAHGEN_H__ACDD7943_C828_4CFF_9D79_59CC1A096706__INCLUDED_)
