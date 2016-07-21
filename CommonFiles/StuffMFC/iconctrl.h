#if !defined(AFX_ICONCTRL_H__4A277949_B00F_4783_A2ED_8BBADF62D051__INCLUDED_)
#define AFX_ICONCTRL_H__4A277949_B00F_4783_A2ED_8BBADF62D051__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// iconctrl.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CIconCtrl window

class CIconCtrl : public CStatic
{
// Construction
public:
	CIconCtrl();
	virtual ~CIconCtrl();

// Attributes
public:
	CImageList* m_pilIcons;

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CIconCtrl)
	//}}AFX_VIRTUAL

// Implementation
public:
	BOOL SetIcon(HINSTANCE hInstance, LPCTSTR lpIconName, DWORD dwCX = 16, DWORD dwCY = 16, BOOL bRedraw = FALSE);
	BOOL SetIcon(HINSTANCE hInstance, int nID, DWORD dwCX = 16, DWORD dwCY = 16, BOOL bRedraw = FALSE);
	void Redraw();

	// Generated message map functions
protected:
	//{{AFX_MSG(CIconCtrl)
	afx_msg void OnPaint();
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ICONCTRL_H__4A277949_B00F_4783_A2ED_8BBADF62D051__INCLUDED_)
