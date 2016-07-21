#if !defined(AFX_WIZARDHEADERCTRL_H__5DFB5EA1_A730_11D3_83C8_0000E816586C__INCLUDED_)
#define AFX_WIZARDHEADERCTRL_H__5DFB5EA1_A730_11D3_83C8_0000E816586C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// WizardHeaderCtrl.h : header file
// 

/////////////////////////////////////////////////////////////////////////////

#define BA_LEFT			0
#define BA_RIGHT		1
#define BA_STRETCH		2
#define BA_HCENTRE		3

#define BA_VCENTRE		4
#define BA_TOP			5
#define BA_BOTTOM		6

#define TEXT_MARGIN		5
#define TEXT_V_MARGIN	2

#define BA_H_MARGIN		0
#define BA_V_MARGIN		0

/////////////////////////////////////////////////////////////////////////////
// CWizardHeaderCtrl window

class CWizardHeaderCtrl : public CStatic
{
// Construction
public:
	CWizardHeaderCtrl();

// Attributes
public:
	virtual void SetPicture(CBitmap* pPicture, BOOL bRedraw = FALSE);
	virtual void SetIcon(HICON hIcon, BOOL bRedraw = FALSE);
	virtual void SetAlignment(DWORD dwHAligh, DWORD dwVAligh, BOOL bRedraw = FALSE);
	virtual void SetTitleFontHeight(DWORD dwHeight, BOOL bRedraw = FALSE);
	virtual void SetTextColor(COLORREF col, BOOL bRedraw = FALSE);
	virtual void SetBgColor(COLORREF col, BOOL bRedraw = FALSE);
	virtual void SetText(CString sText, BOOL bRedraw = FALSE);
	virtual void SetTitle(CString sTitle, BOOL bRedraw = FALSE);
	virtual void SetTitleFontFace(CString sFontFace, BOOL bRedraw = FALSE);
	virtual void SetMargins(int nPictureCX, int nPictureCY, int nTextCX,  int nTextCY, BOOL bRedraw = FALSE);
	virtual void SetDrawBorder(BOOL bDraw, BOOL bRedraw = FALSE);

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CWizardHeaderCtrl)
	//}}AFX_VIRTUAL

// Implementation
protected:
	BOOL m_bDrawBorder;
	CString m_sFontFaceName;
	DWORD m_dwTitleFontHeight, m_dwTextFontHeight;
	DWORD m_dwHBitmapAlignment;
	DWORD m_dwVBitmapAlignment;
	
	CString m_sTitle, m_sText;
	COLORREF m_crBackground, m_crText;
	HICON m_hIcon;
	CBitmap* m_pPicture;
	
	int m_nPictureCX, m_nPictureCY, 
		m_nTextCX, m_nTextCY;
	
public:
	virtual void DrawBitmap(CDC *pDC, CRect R, UINT nFlags, CBitmap *pBitmap);
	virtual void Redraw();
	
	// Generated message map functions
public:

	//{{AFX_MSG(CWizardHeaderCtrl)
	afx_msg void OnPaint();
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_WIZARDHEADERCTRL_H__5DFB5EA1_A730_11D3_83C8_0000E816586C__INCLUDED_)
