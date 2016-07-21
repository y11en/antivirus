#if !defined(AFX_RESIZABLESHEET_H__INCLUDED_)
#define AFX_RESIZABLESHEET_H__INCLUDED_

/////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2000-2001 by Paolo Messina
// (http://www.geocities.com/ppescher - ppescher@yahoo.com)
//
// The contents of this file are subject to the Artistic License (the "License").
// You may not use this file except in compliance with the License. 
// You may obtain a copy of the License at:
// http://www.opensource.org/licenses/artistic-license.html
//
// If you find this code useful, credits would be nice!
//
/////////////////////////////////////////////////////////////////////////////

#include "ResizableLayout.h"
#include "DragWnd.h"
#include "htmlhelpsup.h"
#include "defs.h"
#include "BasePropertySheet.h"

//******************************************************************************
//
//******************************************************************************

#define UM_QUERY_HELP_CONTEXT_FROM_PROP_SHEET (WM_USER + 0x221)

class COMMONEXPORT CResizableSheet : public CBasePropertySheet, public CResizableLayout
{
	DECLARE_DYNAMIC(CResizableSheet)

public:
	CResizableSheet ()
    {
        Construct ();
    }

    CResizableSheet ( UINT nIDCaption, CWnd * pParentWnd = NULL, UINT iSelectPage = 0 )
    :   CBasePropertySheet ( nIDCaption, pParentWnd, iSelectPage )
    {
        Construct ();
    }

    CResizableSheet ( LPCTSTR pszCaption, CWnd * pParentWnd = NULL, UINT iSelectPage = 0 )
    :   CBasePropertySheet ( pszCaption, pParentWnd, iSelectPage )
    {
        Construct ();
    }

    virtual ~CResizableSheet ()
    {
    }

private:
	// flags
	BOOL m_bEnableSaveRestore;
	BOOL m_bRectOnly;
	BOOL m_bSavePage;

	// layout vars
	CSize m_sizePageTL, m_sizePageBR;

	// internal status
	CString m_sSection;			// section name (identifies a parent window)
	BOOL m_bInitDone;			// if all internal vars initialized
	
	CFont m_font;

public:
	//{{AFX_VIRTUAL(CResizableSheet)
	public:
	virtual BOOL OnInitDialog();
	//}}AFX_VIRTUAL

private:
	void PresetLayout();

    void Construct ()
    {
        m_bInitDone = FALSE;
        m_bEnableSaveRestore = FALSE;
        m_bSavePage = FALSE;
    }

    void SavePage();
	void LoadPage();

	BOOL IsWizard() { return (m_psh.dwFlags & PSH_WIZARD); }

// callable from derived classes
protected:
	// section to use in app's profile
	void EnableSaveRestore(LPCTSTR pszSection, BOOL bRectOnly = FALSE,
		BOOL bWithPage = FALSE);
	int GetMinWidth();	// minimum width to display all buttons


	virtual CWnd* GetResizableWnd()
	{
		// make the layout know its parent window
		return this;
	};

// Generated message map functions
protected:
	virtual BOOL ArrangeLayoutCallback(LayoutInfo& layout);
	//{{AFX_MSG(CResizableSheet)
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnDestroy();
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	//}}AFX_MSG
	afx_msg BOOL OnPageChanging(NMHDR* pNotifyStruct, LRESULT* pResult);
	DECLARE_MESSAGE_MAP()
	
	DECLARE_USE_WND_HTMLHELP()
};

//******************************************************************************
//
//******************************************************************************
class COMMONEXPORT CCDragResizableSheet : public CResizableSheet
{
public:
    CCDragResizableSheet ()
    {
    }
    
    CCDragResizableSheet ( UINT nIDCaption, CWnd * pParentWnd = NULL, UINT iSelectPage = 0 )
    :   CResizableSheet ( nIDCaption, pParentWnd, iSelectPage )
    {
    }
    
    CCDragResizableSheet ( LPCTSTR pszCaption, CWnd * pParentWnd = NULL, UINT iSelectPage = 0 )
    :   CResizableSheet ( pszCaption, pParentWnd, iSelectPage )
    {
    }

protected:
    virtual LRESULT WindowProc ( UINT message, WPARAM wParam, LPARAM lParam )
    {
        if  ( ( message == WM_LBUTTONDOWN ) && ( LButtonDragWnd ( GetSafeHwnd (), lParam ) == 0 ) )
        {
            return 0;
        }
        return CResizableSheet::WindowProc ( message, wParam, lParam );
    }
};


#endif	// AFX_RESIZABLESHEET_H__INCLUDED_
