#if !defined(AFX_CSTM1DLG_H__29A10BD8_0C43_469F_AC30_3BF439171AF9__INCLUDED_)
#define AFX_CSTM1DLG_H__29A10BD8_0C43_469F_AC30_3BF439171AF9__INCLUDED_

// cstm1dlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CCustom1Dlg dialog

class CCustom1Dlg : public CAppWizStepDlg
{
public:
	CCustom1Dlg();
	virtual BOOL OnDismiss();

	//{{AFX_DATA(CCustom1Dlg)
	enum { IDD = IDD_PANE1 };
	BOOL	m_bKLICK;
	BOOL	m_bKLIN;
	CString	m_strPluginDescription;
	//}}AFX_DATA

	//{{AFX_VIRTUAL(CCustom1Dlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CCustom1Dlg)
	afx_msg void OnKlick();
	afx_msg void OnKlin();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}

#endif // !defined(AFX_CSTM1DLG_H__29A10BD8_0C43_469F_AC30_3BF439171AF9__INCLUDED_)
