#if !defined(AFX_DBGOUTPUTDLG_H__DE2E7978_3911_40FE_A3F9_01C358DBDFD4__INCLUDED_)
#define AFX_DBGOUTPUTDLG_H__DE2E7978_3911_40FE_A3F9_01C358DBDFD4__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DbgOutputDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDbgOutputDlg dialog

class CDbgOutputDlg : public CDialog
{
// Construction
public:
	DECLARE_DYNCREATE(CDbgOutputDlg)

	CDbgOutputDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDbgOutputDlg)
	enum { IDD = IDD_DBGOUTPUT_DIALOG };
	CEdit	m_EdDbgOutput;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDbgOutputDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDbgOutputDlg)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DBGOUTPUTDLG_H__DE2E7978_3911_40FE_A3F9_01C358DBDFD4__INCLUDED_)
