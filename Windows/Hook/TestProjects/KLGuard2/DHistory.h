#if !defined(AFX_DHISTORY_H__0EC22EE2_5BC0_4EB0_932C_7F3BDD6BB405__INCLUDED_)
#define AFX_DHISTORY_H__0EC22EE2_5BC0_4EB0_932C_7F3BDD6BB405__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DHistory.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDHistory dialog

class CDHistory : public CDialog
{
// Construction
public:
	CDHistory(ULONG ProcessId, CString* pCaption, CString* pImagePath, _StrHistory* pReg, _StrHistory* pFile, _StrHistory* pFileSelf, CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDHistory)
	enum { IDD = IDD_DIALOG_PROCESS_INFO };
	CStatic	m_stIcon;
	CListBox	m_lHistory;
	CString	m_eInfo;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDHistory)
	public:
	virtual BOOL DestroyWindow();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDHistory)
	virtual BOOL OnInitDialog();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnButtonClear();
	afx_msg void OnGetMinMaxInfo(MINMAXINFO FAR* lpMMI);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:
	ULONG m_ProcessId;
	CString* m_pCaption;
	CString* m_pImagePath;
	_StrHistory* m_pReg;
	_StrHistory* m_pFile;
	_StrHistory* m_pFileSelf;

	CWinFileInfo m_FileInfo;
private:
	void RestorePosition();
	void SavePosition();
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DHISTORY_H__0EC22EE2_5BC0_4EB0_932C_7F3BDD6BB405__INCLUDED_)
