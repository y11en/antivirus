#if !defined(AFX_DEVENTINFO_H__6B84A946_2AC4_494D_A0B4_BE688FDB8A44__INCLUDED_)
#define AFX_DEVENTINFO_H__6B84A946_2AC4_494D_A0B4_BE688FDB8A44__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DEventInfo.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDEventInfo dialog

class CDEventInfo : public CDialog
{
// Construction
public:
	CDEventInfo(PEVENT_TRANSMIT pEvt, CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDEventInfo)
	enum { IDD = IDD_DIALOG_EVENTINFO };
	CListCtrl	m_lParams;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDEventInfo)
	public:
	virtual BOOL DestroyWindow();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDEventInfo)
	virtual BOOL OnInitDialog();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnDblclkListParams(NMHDR* pNMHDR, LRESULT* pResult);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:
	PEVENT_TRANSMIT m_pEvt;

	void StoreWindowPosition();
	void RestoreWindowPosition();
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DEVENTINFO_H__6B84A946_2AC4_494D_A0B4_BE688FDB8A44__INCLUDED_)
