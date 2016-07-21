#if !defined(AFX_DECRYPTORNTLMDIALOG_H__D1477072_4BB3_4476_8ACE_C82F2E82D67B__INCLUDED_)
#define AFX_DECRYPTORNTLMDIALOG_H__D1477072_4BB3_4476_8ACE_C82F2E82D67B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class DecryptorNTLMDialog : public CDialog
{
public:
	DecryptorNTLMDialog(CWnd *pParent = 0);

	//{{AFX_DATA(DecryptorNTLMDialog)
	enum { IDD = IDD_DECRYPTORNTLM_DIALOG };
	CString	m_input1;
	CString	m_input2;
	CString	m_input3;
	CString	m_input4;
	CString	m_output1;
	CString	m_output2;
	CString	m_output3;
	CString	m_output4;
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(DecryptorNTLMDialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

protected:
	HICON m_hIcon;

	// Generated message map functions
	//{{AFX_MSG(DecryptorNTLMDialog)
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnButtonDecrypt();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DECRYPTORNTLMDIALOG_H__D1477072_4BB3_4476_8ACE_C82F2E82D67B__INCLUDED_)
