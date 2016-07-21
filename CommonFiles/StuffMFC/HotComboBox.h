#if !defined(AFX_HOTCOMBOBOX_H__68782B02_B925_11D2_B882_0020182B6AB8__INCLUDED_)
#define AFX_HOTCOMBOBOX_H__68782B02_B925_11D2_B882_0020182B6AB8__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// HotComboBox.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CHotComboBox window

class CHotComboBox : public CComboBox
{
protected:
	DECLARE_DYNCREATE(CHotComboBox)

// Construction
public:
	CHotComboBox();

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CHotComboBox)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CHotComboBox();

	BOOL m_bAutoComplete;

	// Generated message map functions
protected:
	virtual void DrawBorder(bool fHot = true);
	COLORREF m_clr3DHilight;
	COLORREF m_clr3DLight;
	COLORREF m_clr3DDkShadow;
	COLORREF m_clr3DShadow;
	COLORREF m_clr3DFace;
	bool m_fGotFocus;
	bool m_fTimerSet;
	//{{AFX_MSG(CHotComboBox)
	afx_msg void OnPaint();
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	afx_msg void OnKillFocus(CWnd* pNewWnd);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnNcMouseMove(UINT nHitTest, CPoint point);
	afx_msg void OnSysColorChange();
	afx_msg void OnSetfocus();
	afx_msg void OnKillfocus();
	afx_msg void OnEditUpdate();
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_HOTCOMBOBOX_H__68782B02_B925_11D2_B882_0020182B6AB8__INCLUDED_)
