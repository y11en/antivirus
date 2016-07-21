// olatestDlg.h : header file
//

#if !defined(AFX_OLATESTDLG_H__A0CAE08A_8A21_4B06_9639_576F22E59BA1__INCLUDED_)
#define AFX_OLATESTDLG_H__A0CAE08A_8A21_4B06_9639_576F22E59BA1__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <vector>
#include "HtmlCtrl.h"
#include "RequestProgress.h"
#include <OnlineActivation.h>
#include "afxcmn.h"
#include "afxwin.h"

enum
{
	WM_REQ_COMPLETED = WM_APP + 1
};

/////////////////////////////////////////////////////////////////////////////
// COlaTestDlg dialog

class COlaTestDlg : public CDialog
{
// Construction
public:
	COlaTestDlg(CWnd* pParent = NULL);	// standard constructor

	void WriteProgressMsg(LPCTSTR szProgress);
	void WriteErrorMsg(const OnlineActivation::ActivationError& error);
	void PostReply(LPCTSTR szHeaders, PBYTE pbData, DWORD dwSize);
	void HandleRequestStarted();
	void HandleRequestFinished(bool bSucceeded);
	bool GetProxyAuth(std::string& user, std::string& pass) const;

	enum { IDD = IDD_OLATEST_DIALOG };
	UINT		m_appId;
	UINT		m_suppId;
	CString		m_password;
	CString		m_actCode;
	CListCtrl	m_urls;
	CListBox	m_progress;
	CString		m_proxy;
	CString		m_user;
	CString		m_pwd;

	enum ActivationType 
	{
		ACTIVATE_UNKNOWN = -1,
		ACTIVATE_NEW, 
		ACTIVATE_EXISTING,
	};

	ActivationType m_actType;

private:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	virtual void PostNcDestroy();

private:
	HICON m_hIcon;

	// Generated message map functions
	//{{AFX_MSG(COlaTestDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnButtonSubmit();
	afx_msg void OnButtonAbort();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

	LRESULT OnRequestCompleted(WPARAM wParam, LPARAM lParam);

private:
	CHtmlCtrl					m_browser;
	RequestProgress				m_observer;

	boost::shared_ptr<OnlineActivation::Response>	m_pResponse;
public:
	afx_msg void OnBnClickedButtonReply();
	afx_msg void OnLvnKeydownListUrls(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnLvnEndlabeleditListUrls(NMHDR *pNMHDR, LRESULT *pResult);
private:
	afx_msg void OnBnClickedCheckProxy();
	afx_msg void OnBnClickedCheckIe();
	afx_msg void OnBnClickedCheckAuth();
public:
	afx_msg void OnBnClickedRadioActype();
	afx_msg void OnBnClickedRadioRegistered();
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_OLATESTDLG_H__A0CAE08A_8A21_4B06_9639_576F22E59BA1__INCLUDED_)
