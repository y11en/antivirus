#if !defined(AFX_DECRYPTORNTLM_H__B5EB0B87_7B57_44E8_828A_91466A649531__INCLUDED_)
#define AFX_DECRYPTORNTLM_H__B5EB0B87_7B57_44E8_828A_91466A649531__INCLUDED_

#if _MSC_VER > 1000
    #pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

class DecryptorNTLMApplication : public CWinApp
{
public:
	DecryptorNTLMApplication();

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(DecryptorNTLMApplication)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL


	//{{AFX_MSG(DecryptorNTLMApplication)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DECRYPTORNTLM_H__B5EB0B87_7B57_44E8_828A_91466A649531__INCLUDED_)
