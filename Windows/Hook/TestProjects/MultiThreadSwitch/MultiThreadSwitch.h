// MultiThreadSwitch.h : main header file for the MULTITHREADSWITCH application
//

#if !defined(AFX_MULTITHREADSWITCH_H__4FFC7098_5BF1_44C0_8049_476E1A7B0AD0__INCLUDED_)
#define AFX_MULTITHREADSWITCH_H__4FFC7098_5BF1_44C0_8049_476E1A7B0AD0__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// CMultiThreadSwitchApp:
// See MultiThreadSwitch.cpp for the implementation of this class
//

class CMultiThreadSwitchApp : public CWinApp
{
public:
	CMultiThreadSwitchApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMultiThreadSwitchApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CMultiThreadSwitchApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MULTITHREADSWITCH_H__4FFC7098_5BF1_44C0_8049_476E1A7B0AD0__INCLUDED_)
