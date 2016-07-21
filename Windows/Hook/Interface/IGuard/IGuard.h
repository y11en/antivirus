// IGuard.h : main header file for the IGUARD DLL
//

#if !defined(AFX_IGUARD_H__551AF612_5DE3_4C31_B1D6_651D0D090EFD__INCLUDED_)
#define AFX_IGUARD_H__551AF612_5DE3_4C31_B1D6_651D0D090EFD__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// CIGuardApp
// See IGuard.cpp for the implementation of this class
//

class CIGuardApp : public CWinApp
{
public:
	CIGuardApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CIGuardApp)
	//}}AFX_VIRTUAL

	//{{AFX_MSG(CIGuardApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_IGUARD_H__551AF612_5DE3_4C31_B1D6_651D0D090EFD__INCLUDED_)
