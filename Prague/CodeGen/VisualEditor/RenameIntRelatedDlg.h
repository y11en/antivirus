#if !defined(AFX_RENAMEINTRELATEDDLG_H__AA4B1D82_2D2F_47DF_817A_CBF326924D17__INCLUDED_)
#define AFX_RENAMEINTRELATEDDLG_H__AA4B1D82_2D2F_47DF_817A_CBF326924D17__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// RenameIntRelatedDlg.h : header file
//

#include <Stuff\PArray.h>
#include <Stuff\SArray.h>

struct CRelatedIDs {
	DWORD m_dwStrID;
	BOOL  m_bValue;
};


/////////////////////////////////////////////////////////////////////////////
// CRenameIntRelatedDlg dialog

class CRenameIntRelatedDlg : public CDialog {
// Construction
public:
	CRenameIntRelatedDlg(CSArray<CRelatedIDs> &rcRelatedInfo, CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CRenameIntRelatedDlg)
	enum { IDD = IDD_RENAMEINTRELATED_DLG };
	CButton	m_RelatedCheck1Ctrl;
	CButton	m_RelatedCheck2Ctrl;
	CButton	m_RelatedCheck3Ctrl;
	//}}AFX_DATA

	CPArray<CButton>			m_RelatedButtons;
	CSArray<CRelatedIDs> &m_rcRelatedInfo;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CRenameIntRelatedDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CRenameIntRelatedDlg)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_RENAMEINTRELATEDDLG_H__AA4B1D82_2D2F_47DF_817A_CBF326924D17__INCLUDED_)
