// AvpgCfg.h : main header file for the AVPGCFG DLL
//

#if !defined(AFX_AVPGCFG_H__4361D0AB_9740_11D3_9CF4_0000E8905EBC__INCLUDED_)
#define AFX_AVPGCFG_H__4361D0AB_9740_11D3_9CF4_0000E8905EBC__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// CAvpgCfgApp
// See AvpgCfg.cpp for the implementation of this class
//

class CAvpgCfgApp : public CWinApp
{
public:
	CAvpgCfgApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAvpgCfgApp)
	public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();
	//}}AFX_VIRTUAL

	//{{AFX_MSG(CAvpgCfgApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_AVPGCFG_H__4361D0AB_9740_11D3_9CF4_0000E8905EBC__INCLUDED_)
