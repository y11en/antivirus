#if !defined(AFX_INFOPANEL_H__5D452FB7_1B5E_4614_8EB1_1268C2C99CFF__INCLUDED_)
#define AFX_INFOPANEL_H__5D452FB7_1B5E_4614_8EB1_1268C2C99CFF__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// InfoPanel.h : header file
//
#include "defs.h"
#include "PngImage.h"
/////////////////////////////////////////////////////////////////////////////
// CInfoPanel window

#define INFO_PANEL_FLAG_SIMPLE_TEXT		(0x0)
#define INFO_PANEL_FLAG_ICONED		(0x1)
#define INFO_PANEL_FLAG_LARGE_ICON	(0x2)
#define INFO_PANEL_FLAG_YELLOW		(0x4)
#define INFO_PANEL_FLAG_VSCROLL		(0x8)


class COMMONEXPORT CInfoPanel : public CStatic
{
// Construction
public:
	enum StatType
	{
		Stat_Error = 0,
		Stat_Warning,
		Stat_Info,
		Stat_Ok,
		Stat_Pause,
		Stat_Last
	};

	CInfoPanel(long nFlag = INFO_PANEL_FLAG_ICONED, bool bCustomRedraw = false);

// Attributes
public:
	CWnd m_Text;


// Operations
public:
	bool SetText(LPCTSTR szText);
	bool SetPngImage (LPCTSTR lpResName, LPCTSTR lpResType);
	void SetStatus(StatType stat, LPCTSTR szText);
	DWORD GetClickedLinkID(NMHDR * pNotifyHeader);
	void SysColorChange(COLORREF bkColor);
	void SetBackColor(COLORREF bkColor);
	
	void DrawCtrl(CDC* pDC);

		
		
		
		
		
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CInfoPanel)
	protected:
	virtual void PreSubclassWindow();
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CInfoPanel();

	// Generated message map functions
protected:
	//{{AFX_MSG(CInfoPanel)
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnPaint();
	afx_msg void OnSysColorChange  ();
	//}}AFX_MSG

	afx_msg void OnLinkNotify ( NMHDR * pNotifyHeader, LRESULT * pResult );

	DECLARE_MESSAGE_MAP()

	void CalcIconRect(CRect& rcIco);
	void CalcTextRect(CRect& rcText);
	void UpdateParentRect(HDC hdc, LPCRECT pRect);

protected:
	CPNGImage m_Ico;
	HINSTANCE m_hResInst;
	HRSRC m_PngRes;
	bool m_bCustomColor;
	COLORREF m_bkColor;
	const long m_nInfoPanelFlag;
	const long m_nIconSize;
	bool m_bUpdateParent;

	bool m_bCustomRedraw;

	CFont m_IPFont;
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_INFOPANEL_H__5D452FB7_1B5E_4614_8EB1_1268C2C99CFF__INCLUDED_)
