////////////////////////////////////////////////////////////////////
//
//  RichEditCtrlEx.h
//  RichEditCtrl20 definition file
//  AVP general purpose stuff
//  Victor Matiouchenkov [victor@avp.ru], Kaspersky Labs. 1999
//  Copyright (c) Kaspersky Labs
//
////////////////////////////////////////////////////////////////////

#if !defined(AFX_RICHEDITCTRLEX_H__4A2039C4_7655_11D3_96B0_00104B5B66AA__INCLUDED_)
#define AFX_RICHEDITCTRLEX_H__4A2039C4_7655_11D3_96B0_00104B5B66AA__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
//

#include <afxcmn.h>

/////////////////////////////////////////////////////////////////////////////
// CRichEditCtrlEx window
// ---
class CRichEditCtrlEx : public CRichEditCtrl {
// Construction
public:
	CRichEditCtrlEx();

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CRichEditCtrlEx)
	public:
	virtual BOOL Create( DWORD in_dwStyle, const RECT& in_rcRect, CWnd* in_pParentWnd, UINT in_nID ) ;
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CRichEditCtrlEx();

					BOOL AutoURLDetect( BOOL bEnable ) ;

	// Generated message map functions
protected:
	//{{AFX_MSG(CRichEditCtrlEx)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

class _AFX_RICHEDITEX_STATE {
public:

			 _AFX_RICHEDITEX_STATE();
	virtual ~_AFX_RICHEDITEX_STATE();

public:
	HINSTANCE m_hInstRichEdit20;

	static _AFX_RICHEDITEX_STATE &GetState();

	bool Is20Activated() { return !!m_hInstRichEdit20; } 
};


BOOL PASCAL AfxInitRichEditEx() ;


//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_RICHEDITCTRLEX_H__4A2039C4_7655_11D3_96B0_00104B5B66AA__INCLUDED_)
