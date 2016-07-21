#if !defined(AFX_RDREXCL_H__E0183663_3B8C_41EE_A4B9_0E362DDFF83E__INCLUDED_)
#define AFX_RDREXCL_H__E0183663_3B8C_41EE_A4B9_0E362DDFF83E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// RdrExcl.h : header file
//

#include <list>
struct ExcludeItem {
    PVOID       m_Handle;
    wchar_t     m_Path[MAX_PATH];
    ULONG       m_RemoteHost;
    USHORT      m_RemotePort;
};

/////////////////////////////////////////////////////////////////////////////
// CRdrExcl dialog

class CRdrExcl : public CDialog
{
// Construction
public:
	CRdrExcl(CWnd* pParent = NULL);   // standard constructor
    CRdrExcl(std::list<ExcludeItem>* pList, CWnd* pParent = NULL);

// Dialog Data
	//{{AFX_DATA(CRdrExcl)
	enum { IDD = IDD_RedirectorExcludes };
	CButton	m_REMOVE_EXCLUDE;
	CButton	m_ADD_EXCLUDE;
	CButton	m_CANCEL;
	CButton	m_OK;
	CListCtrl	m_List;
	CString	m_Host;
	CString	m_Port;
	CString	m_Path;
	//}}AFX_DATA

    std::list<ExcludeItem>* m_pCrExludeList;    

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CRdrExcl)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CRdrExcl)
	virtual BOOL OnInitDialog();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnAddExclude();
	afx_msg void OnRemoveExclude();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_RDREXCL_H__E0183663_3B8C_41EE_A4B9_0E362DDFF83E__INCLUDED_)

