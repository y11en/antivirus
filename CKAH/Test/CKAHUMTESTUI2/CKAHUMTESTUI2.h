// CKAHUMTESTUI2.h : main header file for the CKAHUMTESTUI2 application
//

#if !defined(AFX_CKAHUMTESTUI2_H__C23B1AE0_064F_46C6_99F2_6FDC308B646E__INCLUDED_)
#define AFX_CKAHUMTESTUI2_H__C23B1AE0_064F_46C6_99F2_6FDC308B646E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// CCKAHUMTESTUI2App:
// See CKAHUMTESTUI2.cpp for the implementation of this class
//

class CCKAHUMTESTUI2App : public CWinApp
{
public:
	CCKAHUMTESTUI2App();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CCKAHUMTESTUI2App)
	public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CCKAHUMTESTUI2App)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CKAHUMTESTUI2_H__C23B1AE0_064F_46C6_99F2_6FDC308B646E__INCLUDED_)
