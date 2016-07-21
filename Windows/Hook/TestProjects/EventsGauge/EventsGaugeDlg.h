// EventsGaugeDlg.h : header file
//

#if !defined(AFX_EVENTSGAUGEDLG_H__72DB630E_A7EA_4CB2_94A8_E5BCFBF69602__INCLUDED_)
#define AFX_EVENTSGAUGEDLG_H__72DB630E_A7EA_4CB2_94A8_E5BCFBF69602__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/////////////////////////////////////////////////////////////////////////////
// CEventsGaugeDlg dialog

class CEventsGaugeDlg : public CDialog
{
// Construction
public:
	CEventsGaugeDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	//{{AFX_DATA(CEventsGaugeDlg)
	enum { IDD = IDD_EVENTSGAUGE_DIALOG };
	UINT	m_eAppID;
	UINT	m_eQueueLen;
	UINT	m_eMarkedQueueLen;
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CEventsGaugeDlg)
	public:
	virtual BOOL DestroyWindow();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	//{{AFX_MSG(CEventsGaugeDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnButtonGo();
	afx_msg void OnTimer(UINT nIDEvent);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_EVENTSGAUGEDLG_H__72DB630E_A7EA_4CB2_94A8_E5BCFBF69602__INCLUDED_)
