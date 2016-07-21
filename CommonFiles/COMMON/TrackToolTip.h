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

#include "defs.h"

//******************************************************************************
//
//******************************************************************************
class COMMONEXPORT CTitleTip : public CToolTipCtrl
{
public:	
	CTitleTip ();
	virtual ~CTitleTip ();

public:
	//{{AFX_VIRTUAL(CTitleTip)
	public:	virtual BOOL PreTranslateMessage ( MSG * pMsg );
	//}}AFX_VIRTUAL

    void SetWrapLongText ( BOOL bWrap )
    {
        m_bWrapLongText = bWrap;
    }

    BOOL ShowState () const
    {
        return m_bOnShow;
    }

    BOOL GetWrapLongText ()
    {
        return m_bWrapLongText;
    }

	void SetAutoHide(bool bAutoHide)
	{
		m_bAutoHide = bAutoHide;
	}

	void SetAbsolutePositionFlag(bool bAbsPosFlag)
	{
		m_bAbsPosFlag = bAbsPosFlag;
	}


public:
	void Hide ();
	
    // bShowInAnyCase - параметр показа tooltip(а), если равен true, то 
    // tooltip показывается в любом случае, если равен false, то tooltip 
    // показывается только когда задаваемый текст не умещается в rectTitle
    void Show ( CRect rectTitle, LPCTSTR lpszTitleText, bool bShowInAnyCase = false );

    virtual BOOL Create ( CWnd * pParentWnd );
	virtual LRESULT WindowProc ( UINT message, WPARAM wParam, LPARAM lParam );

public:
	int m_nDelayConst;

protected:
	CWnd * m_pParentWnd;	
	CRect m_rectTitle;
	DWORD  m_dwLastLButtonDown;
	BOOL m_bOnShow;
	BOOL m_nMouseMoveDelay;
	BOOL m_bWrapLongText;
	CString m_TitleText;

	bool m_bAutoHide;
	bool m_bAbsPosFlag;

protected:	
	//{{AFX_MSG(CTitleTip)
	afx_msg void OnMouseMove ( UINT nFlags, CPoint point );
	afx_msg void OnTimer ( UINT nIDEvent );
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

	BOOL ProcessMessage ( MSG * pMsg );


};

#endif //__TRACKTOOLTIP_H__
