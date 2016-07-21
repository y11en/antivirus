#if !defined(AFX_CAPTIONPANE_H__1E7497B4_268F_4A1E_8284_5759B9071A06__INCLUDED_)
#define AFX_CAPTIONPANE_H__1E7497B4_268F_4A1E_8284_5759B9071A06__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// CaptionPane.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CCaptionPane window

class CCaptionPane : public CStatic
{
// Construction
public:
	CCaptionPane();
	virtual ~CCaptionPane();

// Attributes
public:
	COLORREF m_crTextColor, m_crBgColor;
	CString m_sFontName;
	DWORD m_dwFontStyle;
	DWORD m_dwFontHeight;
	CString m_sText;

// Operations
public:
	virtual void SetText(CString sText);
	virtual void SetTextColor(COLORREF crColor);
	virtual void SetBgColor(COLORREF crColor);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CCaptionPane)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual void SetTextStyle(LPCTSTR pszFontName, DWORD dwStyle, DWORD dwHeight);

	// Generated message map functions
protected:
	//{{AFX_MSG(CCaptionPane)
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnPaint();
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CAPTIONPANE_H__1E7497B4_268F_4A1E_8284_5759B9071A06__INCLUDED_)
