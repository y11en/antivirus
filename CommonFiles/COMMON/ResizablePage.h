#if !defined(AFX_RESIZABLEPAGE_H__INCLUDED_)
#define AFX_RESIZABLEPAGE_H__INCLUDED_

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
#include "layout.h"
#include "defs.h"
#include "dialog.h"

struct CCMapWrapper;
//******************************************************************************
//
//******************************************************************************
class COMMONEXPORT CResizablePage : public CPropertyPage//, public CResizableLayout
{
	DECLARE_DYNCREATE(CResizablePage)

public:
	CResizablePage ()
    {
        Construct ();
    }

    CResizablePage ( UINT nIDTemplate, UINT nIDCaption = 0 )
    :   CPropertyPage ( nIDTemplate, nIDCaption )
    {
        Construct ();
    }

    CResizablePage ( LPCTSTR lpszTemplateName, UINT nIDCaption = 0 )
    :   CPropertyPage ( lpszTemplateName, nIDCaption )
    {
        Construct ();
    }

    virtual ~CResizablePage ();
    
    DECLARE_DIALOG_BASED()
        
    void SetLayout ( RULE * pLayoutRule )
    {
        m_pLayoutRule = pLayoutRule;
    }

    void ComputeLayoutEx ();

	void SetCtrlTooltip(int nID, LPCTSTR szTooltip);
	void SetCtrlTooltip(HWND hWndCtrl, LPCTSTR szTooltip);
	
	BOOL IsInitDone()
	{
		return m_bInitDone;
	}

/*	void SetCtrlLockedState(int nID, bool bAlignTop = false, bool bLocked = true);
	void SetCtrlLockedState(HWND hWnd, bool bAlignTop = false, bool bLocked = true);
*/

    //{{AFX_VIRTUAL(CResizablePage)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	//}}AFX_VIRTUAL
    
private:
	void Construct ();

    BOOL m_bInitDone;			// if all internal vars initialized
    RULE * m_pLayoutRule;

protected:
	virtual CWnd * GetResizableWnd ()
	{
		// make the layout know its parent window
		return this;
	};
	
    //{{AFX_MSG(CResizablePage)
	virtual BOOL OnInitDialog ();
	afx_msg void OnSize ( UINT nType, int cx, int cy );
	afx_msg void OnLButtonDown ( UINT nFlags, CPoint point );
	afx_msg void OnTimer(UINT nIDEvent);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
		
protected:

//tooltips
	CCMapWrapper* m_pTooltipMap;
	
	CTitleTip* m_pTitleTip;
	CPoint m_ptMouseClientPos;
	HWND m_hwndChild;
	UINT TOOLTIP_TIMER_ID;
	std::basic_string<TCHAR> m_szTooltipNext;

//lock icon
/*	
	typedef std::map<HWND, bool> HWND2RECT_SET;
	HWND2RECT_SET m_LockCtrlsSet;
*/
	CImageList m_ImageList;
	int m_nLockedIconIndex;


};

#endif // !defined(AFX_RESIZABLEPAGE_H__INCLUDED_)
