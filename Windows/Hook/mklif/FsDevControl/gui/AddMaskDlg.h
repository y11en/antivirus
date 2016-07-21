#pragma once


// CAddMaskDlg dialog

class CAddMaskDlg : public CDialog
{
	DECLARE_DYNAMIC(CAddMaskDlg)

public:
	CAddMaskDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CAddMaskDlg();

	PWCHAR pMask;
	PWCHAR pValueName;

// Dialog Data
	enum { IDD = IDD_ADDMASKDLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();
//	afx_msg void OnEnChangeEdit1();
};
