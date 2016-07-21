#if !defined(AFX_METHDLG_H__E45E1C19_6266_40DB_A5B7_62F6D93AD4CF__INCLUDED_)
#define AFX_METHDLG_H__E45E1C19_6266_40DB_A5B7_62F6D93AD4CF__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// MethDlg.h : header file
//


#include "../IfaceInfo.h"

/////////////////////////////////////////////////////////////////////////////
// CMethDlg dialog

class CMethDlg : public CDialog
{
// Construction
public:
  bool str2method();
  bool method2str();
  bool str2param();
  bool param2str();
	int  select_param();
	CMethDlg( CWnd* pParent, HDATA meth );   // standard constructor

// Dialog Data
	//{{AFX_DATA(CMethDlg)
	enum { IDD = IDD_METHOD_DESCRIPTION };
	CComboBox	m_cb_lang;
	CListCtrl	m_par_list;
	CComboBox	m_ret_val_cb;
	CString	m_comm_behav;
	CString	m_comm_large;
	CString	m_comm_short;
	CString	m_name;
	CString	m_comm_val;
	CString	m_par_comm_behav;
	CString	m_par_comm_large;
	CString	m_par_comm_short;
	CString	m_ret_def_val;
	CString	m_ret_type;
	//}}AFX_DATA

  bool update( HDATA data );
  bool update( CBaseInfo& dst, CBaseInfo& src );
    
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMethDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

  CVisualEditorDlg*  m_dad;
  CPropTypeInfo      m_prop_types;
  CObjTypeInfo       m_obj_types;
  CString            m_type;
  CMethodInfo        m_method;
  CMethodParamInfo   m_param;
  int                m_param_curr;
  int                m_param_count;

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CMethDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnItemchangedParList(NMHDR* pNMHDR, LRESULT* pResult);
  afx_msg void OnSelchangeCbLang();
  afx_msg void OnBtParAdd();
	afx_msg void OnBtParChange();
	afx_msg void OnBtParDel();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_METHDLG_H__E45E1C19_6266_40DB_A5B7_62F6D93AD4CF__INCLUDED_)
