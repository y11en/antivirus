#if !defined(AFX_CLISTCTRL_H__7C9DCEDD_8D6B_4D66_A7A5_24A50D0107A3__INCLUDED_)
#define AFX_CLISTCTRL_H__7C9DCEDD_8D6B_4D66_A7A5_24A50D0107A3__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// CListCtrl.h : header file
//
#include "defs.h"
#include "ReportCtrlHeader.h"
#include "ListCtrl.h"

#define UM_PF2_QUERY_GRAY_TEXT	(WM_USER + 177)

/////////////////////////////////////////////////////////////////////////////
// CGrayListCtrl window

struct COMMONEXPORT GrayListLockedInfo
{
	bool m_bIsLocked;
};

#define WU_DELAYEDFIT (WM_USER+0x132)

class COMMONEXPORT CGrayListCtrl : public CCRoboListCtrl
{
// Construction
public:
	CGrayListCtrl(bool bCheckSensitive = true, bool bGrayIcon = true);

// Attributes
public:

	CReportCtrlHeader m_HeaderCtrl;

// Operations
public:
	void DitherBlt (const HDC hdcDest, const int nXDest, const int nYDest, const int nWidth, 
		const int nHeight, const HICON hIcon, const int nXSrc, const int nYSrc, bool bLocked) const;


	void FitLastListCtrlColumn ();
	void InvalidateRow(int iRow);
		
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CGrayListCtrl)
	public:
	protected:
	virtual void PreSubclassWindow();
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CGrayListCtrl();

	// Generated message map functions
protected:
	//{{AFX_MSG(CGrayListCtrl)
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnPaint();
	afx_msg void OnHdrEnddrag(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDelayedFit();
	//}}AFX_MSG
	afx_msg void OnCustomDraw(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnHdrChanging(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnHdrChanged(NMHDR* pNMHDR, LRESULT* pResult);

	bool m_bDisableErasing;
	const bool m_bCheckSensitive;
	const bool m_bGrayIcon;

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CLISTCTRL_H__7C9DCEDD_8D6B_4D66_A7A5_24A50D0107A3__INCLUDED_)
