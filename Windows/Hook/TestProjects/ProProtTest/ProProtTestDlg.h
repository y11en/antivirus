// ProProtTestDlg.h : header file
//

#if !defined(AFX_PROPROTTESTDLG_H__9394E4B7_0F43_4F3C_9E1D_63B86ACE68B6__INCLUDED_)
#define AFX_PROPROTTESTDLG_H__9394E4B7_0F43_4F3C_9E1D_63B86ACE68B6__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/////////////////////////////////////////////////////////////////////////////
// CProProtTestDlg dialog

class CProProtTestDlg : public CDialog
{
// Construction
public:
	CProProtTestDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	//{{AFX_DATA(CProProtTestDlg)
	enum { IDD = IDD_PROPROTTEST_DIALOG };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CProProtTestDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	//{{AFX_MSG(CProProtTestDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnButtonAddprot();
	afx_msg void OnButtonDelrot();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:

	PVOID m_dwContext;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PROPROTTESTDLG_H__9394E4B7_0F43_4F3C_9E1D_63B86ACE68B6__INCLUDED_)
