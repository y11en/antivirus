////////////////////////////////////////////////////////////////////
//
//  TrackToolTip.h
//  Track position tooltip
//  AVP general purposes MFC stuff
//  Victor Matiouchenkov [victor@avp.ru], Kaspersky Labs. 2000
//  Copyright (c) Kaspersky Labs
//
////////////////////////////////////////////////////////////////////
#ifndef __TRACKTOOLTIP_H__
#define __TRACKTOOLTIP_H__

#include <afxcmn.h>

// ---
class CTitleTip : public CToolTipCtrl {// CWnd {
// Construction
public:	
	CTitleTip();
	virtual ~CTitleTip();

// Attributes
public:

// Operations
public:
// Overrides
	// ClassWizard generated virtual function overrides	
	//{{AFX_VIRTUAL(CTitleTip)
	public:	virtual BOOL PreTranslateMessage(MSG* pMsg);	
	//}}AFX_VIRTUAL

					void SetWrapLongText( BOOL bWrap ) { m_bWrapLongText = bWrap; }
					BOOL GetWrapLongText() { return m_bWrapLongText; }

// Implementation
public:
					void	Hide();
					void	Show( CRect rectTitle, LPCTSTR lpszTitleText, int xoffset = 0 );
	virtual BOOL	Create( CWnd *pParentWnd );	
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);

protected:
	CWnd *m_pParentWnd;	
	CRect m_rectTitle;	// Generated message map functions
	DWORD m_dwLastLButtonDown;
	BOOL  m_bOnShow;
	BOOL  m_nMouseMoveDelay;
	BOOL	m_bWrapLongText;
	CString m_TitleText;

protected:	
	//{{AFX_MSG(CTitleTip)
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);	
	afx_msg void OnTimer( UINT nIDEvent );
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

	BOOL ProcessMessage(MSG* pMsg);
};

#endif //__TRACKTOOLTIP_H__
