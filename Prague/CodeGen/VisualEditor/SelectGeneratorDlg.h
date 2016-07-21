#if !defined(AFX_SELECTGENERATORDLG_H__3B02C901_DA30_11D3_96B1_00D0B71DDDF6__INCLUDED_)
#define AFX_SELECTGENERATORDLG_H__3B02C901_DA30_11D3_96B1_00D0B71DDDF6__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <avpcontrols/checklist.h>
#include "../pcginterface.h"
#include <stuff/fautomat.h>


// SelectGeneratorDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CSelectGeneratorDlg dialog
// ---
class CSelectGeneratorDlg : public CDialog, public CFAutoParam {
// Construction
public:
	CSelectGeneratorDlg( IPragueCodeGen *&pInterface, CLSID &rCLSID, CWnd &rcFrame, CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CSelectGeneratorDlg)
	enum { IDD = IDD_SELECTGENERATOR_DLG };
	CCheckList	m_ObjectInfo;
	CCheckList	m_Objects;
	int         m_nSelected;
	//}}AFX_DATA

	IPragueCodeGen *&m_pInterface;
	CLSID					  &m_rCLSID;
	CWnd						&m_rcFrame;

	virtual void   SetState( int s );

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSelectGeneratorDlg)
	public:
	virtual BOOL DestroyWindow();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CSelectGeneratorDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnItemChanged(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnAddGenerator();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

	void FillGeneratorsList( CCLItemsArray *pItems );
	bool CSelectGeneratorDlg::SelectGeneratorModule( CString &rcFileName );
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SELECTGENERATORDLG_H__3B02C901_DA30_11D3_96B1_00D0B71DDDF6__INCLUDED_)
