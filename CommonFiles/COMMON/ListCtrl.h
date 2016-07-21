//==============================================================================
// PRODUCT: COMMON
//==============================================================================
#ifndef __COMMON_LISTCTRL_H
#define __COMMON_LISTCTRL_H

#include "defs.h"
#include "BaseListCtrl.h"

void COMMONEXPORT GetSelectedItems ( CListCtrl & ListCtrl, std::list < int > & List );

int COMMONEXPORT GetFocusedItem ( CListCtrl & ListCtrl );

void COMMONEXPORT SelectItem ( CListCtrl & ListCtrl, int Item );

void COMMONEXPORT SelectOneItem ( CListCtrl & ListCtrl, int Item );

void COMMONEXPORT ClearSelection ( CListCtrl & ListCtrl );

//******************************************************************************
//
//******************************************************************************
class CCRoboListCtrl : public CListCtrl
{
public:
	CCRoboListCtrl ();
    virtual ~CCRoboListCtrl ();

	//{{AFX_VIRTUAL(CCRoboListCtrl)
	//}}AFX_VIRTUAL

protected:
	//{{AFX_MSG(CCRoboListCtrl)
    LRESULT AFX_MSG_CALL OnGetListItems ( WPARAM wParam, LPARAM lParam );
    LRESULT AFX_MSG_CALL OnSetHWND ( WPARAM wParam, LPARAM lParam );
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()

private:
    HWND m_hHideWnd;
};

//******************************************************************************
//
//******************************************************************************
class COMMONEXPORT CCListCtrlEx : public CCRoboListCtrl
{
public:
     CCListCtrlEx ();
     virtual ~CCListCtrlEx ();

     void FitLastListCtrlColumn ();
          
     //{{AFX_VIRTUAL(CCListCtrlEx)
	protected:
	virtual void PreSubclassWindow ();
	//}}AFX_VIRTUAL
     
protected:
     //{{AFX_MSG(CCListCtrlEx)
	afx_msg void OnHdrChanged ( NMHDR * pNMHDR, LRESULT * pResult );
	afx_msg void OnSize ( UINT nType, int cx, int cy );
	afx_msg BOOL OnEraseBkgnd ( CDC * pDC );
	//}}AFX_MSG
     
    DECLARE_MESSAGE_MAP()

private:
    bool m_bDisableErasing;
};

#endif // __COMMON_LISTCTRL_H

//==============================================================================
