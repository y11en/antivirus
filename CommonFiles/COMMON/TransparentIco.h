#if !defined(AFX_TRANSPARENTICO_H__4314E4AD_F4D7_4B17_B0AC_B1750861BA54__INCLUDED_)
#define AFX_TRANSPARENTICO_H__4314E4AD_F4D7_4B17_B0AC_B1750861BA54__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// TransparentIco.h : header file
//
#include "defs.h"

/////////////////////////////////////////////////////////////////////////////
// CTransparentIco window

class COMMONEXPORT CTransparentIco : public CStatic
{
// Construction
public:
	CTransparentIco();

// Attributes
public:

// Operations
public:
	HICON SetIcon(HICON hIcon);
		

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTransparentIco)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CTransparentIco();

	// Generated message map functions
protected:
	//{{AFX_MSG(CTransparentIco)
	afx_msg void OnPaint();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	//}}AFX_MSG

	
	

	DECLARE_MESSAGE_MAP()

protected:
	HICON m_hIcon;

};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TRANSPARENTICO_H__4314E4AD_F4D7_4B17_B0AC_B1750861BA54__INCLUDED_)
