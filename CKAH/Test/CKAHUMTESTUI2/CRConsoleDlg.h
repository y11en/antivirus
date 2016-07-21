#if !defined(AFX_CRCONSOLEDLG_H__CDE5D1BA_66B6_4111_9400_AF86EE333E59__INCLUDED_)
#define AFX_CRCONSOLEDLG_H__CDE5D1BA_66B6_4111_9400_AF86EE333E59__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// CRConsoleDlg.h : header file
//

#include "CRConsoleEdit.h"
#include <winsock.h>
#include "RdrExcl.h"

#include <CKAH/ckahum.h>

/////////////////////////////////////////////////////////////////////////////
// CCRConsoleDlg dialog

class CCRConsoleDlg : public CDialog
{
// Construction
public:
	CCRConsoleDlg(std::list<ExcludeItem>* pList, USHORT lport, USHORT rport, CWnd* pParent = NULL);   // standard constructor
    virtual ~CCRConsoleDlg();

    void SrvChar(UINT nChar);
    void CliChar(UINT nChar);

    void OutChar(UINT nChar);

    bool m_status;

    std::list<ExcludeItem>* m_pCrExludeList;

// Dialog Data
	//{{AFX_DATA(CCRConsoleDlg)
	enum { IDD = IDD_CR_CONSOLE };
	CCRConsoleEdit	m_EdtConsole;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CCRConsoleDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void PostNcDestroy();
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CCRConsoleDlg)
	virtual void OnOK();
	virtual BOOL OnInitDialog();
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

	USHORT m_rport;
	USHORT m_lport;
    bool m_connected;
    SOCKET m_lsocket;
    SOCKET m_csocket;
    SOCKET m_tsocket;
    HANDLE m_hcthread;
    HANDLE m_hsthread;

    CKAHCR::HPROXY m_hproxy;

    CString m_strText;

    static DWORD WINAPI SrvRecvThreadProc(LPVOID lpParameter);
    static DWORD WINAPI CliRecvThreadProc(LPVOID lpParameter);
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CRCONSOLEDLG_H__CDE5D1BA_66B6_4111_9400_AF86EE333E59__INCLUDED_)
