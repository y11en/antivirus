#if !defined(AFX_DGENERICSETTINGS_H__BADEA672_B751_4D9D_8897_3F7279BFA388__INCLUDED_)
#define AFX_DGENERICSETTINGS_H__BADEA672_B751_4D9D_8897_3F7279BFA388__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DGenericSettings.h : header file
//

#include "AppSettings.h"

/////////////////////////////////////////////////////////////////////////////
// CDGenericSettings dialog

class CDGenericSettings : public CDialog
{
// Construction
public:
	CDGenericSettings(CGenericSettings* pGeneralSettings, CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDGenericSettings)
	enum { IDD = IDD_DIALOG_GENERIC_SETTINGS };
	CButton	m_btApply;
	BOOL	m_chProtectAutoRun;
	BOOL	m_chProtectSelf;
	CString	m_eLogFileName;
	BOOL	m_chDontCotrnolWriteBySystem;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDGenericSettings)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDGenericSettings)
	virtual void OnOK();
	afx_msg void OnButtonApply();
	virtual BOOL OnInitDialog();
	afx_msg void OnCheckProtectAutorun();
	afx_msg void OnCheckProtectself();
	afx_msg void OnButtonBrowse();
	afx_msg void OnCheckDontcontrolwriteBysystem();
	afx_msg void OnChangeEditLogfilename();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:
	CGenericSettings* m_pGeneralSettings;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DGENERICSETTINGS_H__BADEA672_B751_4D9D_8897_3F7279BFA388__INCLUDED_)
