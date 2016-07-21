// olatest.h : main header file for the OLATEST application
//

#if !defined(AFX_OLATEST_H__090668F2_4697_4DDA_8EE7_B123C01B96C5__INCLUDED_)
#define AFX_OLATEST_H__090668F2_4697_4DDA_8EE7_B123C01B96C5__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols
//#include "Olatest_i.h"

/////////////////////////////////////////////////////////////////////////////
// COlaTestApp:
// See olatest.cpp for the implementation of this class
//

class COlaTestApp : public CWinApp
{
public:
	COlaTestApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(COlaTestApp)
	public:
	virtual BOOL InitInstance();
		virtual int ExitInstance();
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(COlaTestApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:
	BOOL m_bATLInited;
private:
	BOOL InitATL();
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_OLATEST_H__090668F2_4697_4DDA_8EE7_B123C01B96C5__INCLUDED_)
