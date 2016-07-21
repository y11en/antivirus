#if !defined(AFX_BASEPROPERTYSHEET_H__C5836431_A051_4528_90FA_3BB37A4E3BE4__INCLUDED_)
#define AFX_BASEPROPERTYSHEET_H__C5836431_A051_4528_90FA_3BB37A4E3BE4__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// BasePropertySheet.h : header file
//

#include "defs.h"

/////////////////////////////////////////////////////////////////////////////
// CBasePropertySheet

class COMMONEXPORT CBasePropertySheet : public CPropertySheet
{
	DECLARE_DYNAMIC(CBasePropertySheet)

	CBasePropertySheet();
	CBasePropertySheet(UINT nIDCaption, CWnd* pParentWnd = NULL, UINT iSelectPage = 0);
	CBasePropertySheet(LPCTSTR pszCaption, CWnd* pParentWnd = NULL, UINT iSelectPage = 0);
	virtual ~CBasePropertySheet();

	// подменяем, чтобы зухучить подмену фонтов
	virtual void BuildPropPageArray();
	BOOL Create(CWnd* pParentWnd = NULL, DWORD dwStyle = (DWORD)-1, DWORD dwExStyle = 0);
	void AddPage(CPropertyPage* pPage);

protected:
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_BASEPROPERTYSHEET_H__C5836431_A051_4528_90FA_3BB37A4E3BE4__INCLUDED_)
