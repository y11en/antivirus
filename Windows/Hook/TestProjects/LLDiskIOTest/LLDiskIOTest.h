// LLDiskIOTest.h : main header file for the LLDISKIOTEST application
//

#if !defined(AFX_LLDISKIOTEST_H__2AAF91E2_9EB5_4453_91DA_F110D4E5CA5F__INCLUDED_)
#define AFX_LLDISKIOTEST_H__2AAF91E2_9EB5_4453_91DA_F110D4E5CA5F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// CLLDiskIOTestApp:
// See LLDiskIOTest.cpp for the implementation of this class
//

class CLLDiskIOTestApp : public CWinApp
{
public:
	CLLDiskIOTestApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CLLDiskIOTestApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CLLDiskIOTestApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_LLDISKIOTEST_H__2AAF91E2_9EB5_4453_91DA_F110D4E5CA5F__INCLUDED_)
