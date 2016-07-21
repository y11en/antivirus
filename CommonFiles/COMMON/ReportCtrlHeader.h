#if !defined(AFX_REPORTCTRLHEADER_H__CBE96267_A7AA_4D2D_9827_A51CC74565EF__INCLUDED_)
#define AFX_REPORTCTRLHEADER_H__CBE96267_A7AA_4D2D_9827_A51CC74565EF__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ReportCtrlHeader.h : header file
//
#include "defs.h"
/////////////////////////////////////////////////////////////////////////////
// CReportCtrlHeader window

class COMMONEXPORT CReportCtrlHeader : public CHeaderCtrl
{
// Construction
public:
	CReportCtrlHeader();

// Attributes
public:
	std::list<int> m_NonResizableHdrIndices;
	std::list<int> m_DisabledSortHdrIndices;

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CReportCtrlHeader)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CReportCtrlHeader();

	// Generated message map functions
protected:
	//{{AFX_MSG(CReportCtrlHeader)
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	//}}AFX_MSG

	bool IsOnHeader(POINT pt);
	bool IsOnNonResizableDivider(POINT pt);
	bool IsOnNonResizableHeader(POINT pt);
	bool IsSortEnabled(POINT pt);
	int  GetLastColumnIndex();

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_REPORTCTRLHEADER_H__CBE96267_A7AA_4D2D_9827_A51CC74565EF__INCLUDED_)
