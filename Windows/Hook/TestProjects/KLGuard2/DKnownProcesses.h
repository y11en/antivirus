#if !defined(AFX_DKNOWNPROCESSES_H__E75F735A_18C7_4847_9B47_920817C7D206__INCLUDED_)
#define AFX_DKNOWNPROCESSES_H__E75F735A_18C7_4847_9B47_920817C7D206__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DKnownProcesses.h : header file
//

#include "WinFileInfo.h"
/////////////////////////////////////////////////////////////////////////////
// CDKnownProcesses dialog

class CDKnownProcesses : public CDialog
{
// Construction
public:
	CDKnownProcesses(CKnownProcessList* pKnownList, CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDKnownProcesses)
	enum { IDD = IDD_DIALOG_KNOWN_PROCESSES };
	CButton	m_btRemove;
	CListCtrl	m_lKnownProcesses;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDKnownProcesses)
	public:
	virtual BOOL DestroyWindow();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDKnownProcesses)
	virtual BOOL OnInitDialog();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnGetMinMaxInfo(MINMAXINFO FAR* lpMMI);
	afx_msg void OnItemchangedListKnownlist(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnButtonRemove();
	afx_msg void OnButtonAddnew();
	afx_msg void OnColumnclickListKnownlist(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDeleteitemListKnownlist(NMHDR* pNMHDR, LRESULT* pResult);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:
	void ReloadList();

	CKnownProcessList* m_pKnownList;
	CWinFileInfo m_WinFileInfo;

private:
	void SavePosition();
	void RestorePosition();

	bool m_bSortAscending;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DKNOWNPROCESSES_H__E75F735A_18C7_4847_9B47_920817C7D206__INCLUDED_)
