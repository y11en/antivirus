#if !defined(AFX_HYPERSTATIC_H__FC007EC3_0749_4770_908D_2FB4C1FE003E__INCLUDED_)
#define AFX_HYPERSTATIC_H__FC007EC3_0749_4770_908D_2FB4C1FE003E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// HyperStatic.h : header file
//

#include "defs.h"

/////////////////////////////////////////////////////////////////////////////
// CHyperStatic window

#define HYPER_STATIC_OPAQUE		0x00000001

class COMMONEXPORT CHyperStatic : public CStatic
{
	// Construction
public:
	explicit CHyperStatic(DWORD dwID = 0, DWORD dwFlags = 0);
	
	// Attributes
public:
	virtual void GetNeedSize ( LPRECT prectMax );

	virtual void GetLinkRect (CRect & rcLink);
		
	

	bool HitTestLink();

// Operations
public:
	DWORD	m_dwMessageID;
	BOOL	m_bUnderLine;
	COLORREF m_crLinkColour;
    COLORREF m_crOverColour;
    CFont	m_Font;

    
	

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CHyperStatic)
	protected:
	virtual void PreSubclassWindow();
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CHyperStatic();

	// Generated message map functions
protected:
	//{{AFX_MSG(CHyperStatic)
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
	afx_msg void OnPaint();
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg UINT OnGetDlgCode();
	//}}AFX_MSG

	afx_msg HBRUSH CtlColor(CDC* pDC, UINT nCtlColor); 
	LRESULT OnPrintClient (WPARAM wParam, LPARAM lParam);
	
	BOOL SendParentNotifyClick (BOOL bForceSend = FALSE);
	void Initialize();
	void SetCtrlFont(CDC *pdc = NULL);

	DECLARE_MESSAGE_MAP()

protected:
	HCURSOR	m_hLinkCursor;
	HCURSOR	m_NormalCursor;
	BOOL m_bOverControl;
	CRect m_rcFocus;
	DWORD m_dwFlags;
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_HYPERSTATIC_H__FC007EC3_0749_4770_908D_2FB4C1FE003E__INCLUDED_)
