#if !defined(AFX_REPORTLISTCTRL_H__3220724A_0172_4FB9_A714_372BB4DA0787__INCLUDED_)
#define AFX_REPORTLISTCTRL_H__3220724A_0172_4FB9_A714_372BB4DA0787__INCLUDED_

#include "..\..\Windows\Personal 4.6\RPT\rptdb.h"
 
//******************************************************************************
//
//******************************************************************************
class COMMONEXPORT CReportListCtrl : public CCRoboListCtrl
{
public:
	enum ESortDirection
	{
		eSortNone,
		eSortSmallToBig,
		eSortBigToSmall,
		eSortAuto
	};
	
	CReportListCtrl ();

    virtual ~CReportListCtrl ();

	CReportListCtrl::ESortDirection SetSortMark ( int nHdrItem, ESortDirection eSort = eSortAuto );
	ESortDirection GetSortDirection ();
	int GetSortColumn ();

	void FitLastListCtrlColumn ();
	
public:
	//{{AFX_VIRTUAL(CReportListCtrl)
	protected:
	virtual void PreSubclassWindow ();
	//}}AFX_VIRTUAL
	
protected:
	ESortDirection NextSort ( ESortDirection eCurrSort );
	void SetColumnMark ( int nSortItem, ESortDirection eSort );
	virtual void OnReportContextMenu(CPoint& pt);
	virtual bool IsRButtonSelected();

		
	int m_nSortItem;
	ESortDirection m_eSortDirection;
	CBitmap m_bmSortDown;
	CBitmap m_bmSortUp;
	
	bool m_bXP;
    bool m_bDisableErasing;

    //{{AFX_MSG(CReportListCtrl)
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnHdrChanged ( NMHDR * pNMHDR, LRESULT * pResult );
	afx_msg BOOL OnEraseBkgnd ( CDC * pDC );
	//}}AFX_MSG
    
    DECLARE_MESSAGE_MAP()
};

#endif // !defined(AFX_REPORTLISTCTRL_H__3220724A_0172_4FB9_A714_372BB4DA0787__INCLUDED_)
