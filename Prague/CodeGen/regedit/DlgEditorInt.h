//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//

#ifndef AFX_DLGEDITORINT_H__A534E55C_9227_4144_95E7_EA4098E6AB48__INCLUDED_
#define AFX_DLGEDITORINT_H__A534E55C_9227_4144_95E7_EA4098E6AB48__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//
class CDlgEditorInt : public CDialog
{
public:
	CDlgEditorInt(CWnd* pParent = NULL);   // standard constructor

	//{{AFX_DATA(CDlgEditorInt)
	enum { IDD = IDD_EDITOR_INT };
	CString	m_strValueName;
	//}}AFX_DATA

	int			m_nDec;
	tTYPE_ID	m_nType;
	BYTE*		m_pBuf;
	BOOL		m_bModified;
	BYTE		m_arrBuf[16];

	//{{AFX_VIRTUAL(CDlgEditorInt)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

protected:
	virtual void OnOK();

	//{{AFX_MSG(CDlgEditorInt)
	virtual BOOL OnInitDialog();
	afx_msg void OnDec();
	afx_msg void OnHex();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//

#endif // 
