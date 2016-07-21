#if !defined(AFX_PROPMEMBERDLG_H__C3498806_DFBF_11D3_96B1_00D0B71DDDF6__INCLUDED_)
#define AFX_PROPMEMBERDLG_H__C3498806_DFBF_11D3_96B1_00D0B71DDDF6__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "resource.h"
#include <avpcontrols/controltree.h>
#include <property/property.h>
#include <stuff/parray.h>

// PropMemberDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CPropMemberDlg dialog
class CPropMemberDlg : public CDialog {
// Construction
public:
	CPropMemberDlg(CString &rcMemberText, HDATA hStructTree, CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CPropMemberDlg)
	enum { IDD = IDD_PROPMEMBER_DLG };
	CControlTree	m_ControlTree;
	HDATA         m_hStructTree;
	//}}AFX_DATA

	CString      &m_rcMemberText;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPropMemberDlg)
	public:
	virtual BOOL DestroyWindow();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CPropMemberDlg)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	afx_msg void OnCleanUp();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

	void	LoadInterfaceDataStructureMemberShortCommentStuff( HDATA hData, HPROP hProp, CCTItemsArray &newItems, int nFirstItemLevel );
	void	LoadInterfaceDataStructureMemberTypeStuff( HDATA hData, HPROP hProp, CCTItemsArray &newItems, int nFirstItemLevel );
	void	LoadInterfaceDataStructureMemberStuff( HDATA hData, HPROP hProp, CCTItemsArray &newItems, int nFirstItemLevel );
	void	LoadOneInterfaceDataStructureMemberStuff(HDATA hDataStructureMemberData, CCTItemsArray &newItems, int nFirstItemLevel);
	void	LoadOneInterfaceDataStructureStuff(HDATA hDataStructureData, CCTItemsArray &newItems, int nFirstItemLevel);

	int		FindStructureMember( CPArray<char> &pNames, int nNameIndex, int nItemIndex );
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PROPMEMBERDLG_H__C3498806_DFBF_11D3_96B1_00D0B71DDDF6__INCLUDED_)
