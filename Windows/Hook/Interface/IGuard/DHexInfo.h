#if !defined(AFX_DHEXINFO_H__667CCF6C_9174_418A_89ED_9D6A42C5D1B3__INCLUDED_)
#define AFX_DHEXINFO_H__667CCF6C_9174_418A_89ED_9D6A42C5D1B3__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DHexInfo.h : header file
//
/////////////////////////////////////////////////////////////////////////////
// CDHexInfo dialog

class CDHexInfo : public CDialog
{
// Construction
public:
	CDHexInfo(CWnd* pParent = NULL);   // standard constructor
	void SetHexData(PVOID ValueAddress,  DWORD Size);
// Dialog Data
	//{{AFX_DATA(CDHexInfo)
	enum { IDD = IDD_DIALOG_SHOW_HEX_INFO };
	CString	m_eHex;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDHexInfo)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDHexInfo)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:
	LPBYTE m_ValueAddress;
	DWORD m_DataSize;

	CFont m_LocalFont;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DHEXINFO_H__667CCF6C_9174_418A_89ED_9D6A42C5D1B3__INCLUDED_)
