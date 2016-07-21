// AvpBase.h : main header file for the AVPBASE DLL
//

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// CAvpBaseApp
// See AvpBase.cpp for the implementation of this class
//

class CAvpBaseApp : public CWinApp
{
public:
	CAvpBaseApp();
	~CAvpBaseApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAvpBaseApp)
	//}}AFX_VIRTUAL

	//{{AFX_MSG(CAvpBaseApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////
