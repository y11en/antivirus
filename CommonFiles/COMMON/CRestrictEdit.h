#if !defined(AFX_CRESTRICTEDIT_H__D26110A5_8FB5_4381_B709_FDD5E57AB11D__INCLUDED_)
#define AFX_CRESTRICTEDIT_H__D26110A5_8FB5_4381_B709_FDD5E57AB11D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// CRestrictEdit.h : header file
//
#include "defs.h"
#include "XPThemeHelper.h"
#include "TrackTooltip.h"

class COMMONEXPORT CInputRestriction
{
public:
	virtual ~CInputRestriction(){}
	virtual bool IsSymbolOK( LPCTSTR szTargetString, UINT nOffset, CString& szErrorTitle, CString& szErrorText) = 0;
	virtual bool IsSymbolOK( LPCWSTR szTargetString, UINT nOffset, CString& szErrorTitle, CString& szErrorText) = 0;
		
	virtual bool Finalize(LPCTSTR szTargetString, CString& szErrorTitle, CString& szErrorText) = 0;
	virtual bool Finalize(LPCWSTR szTargetString, CString& szErrorTitle, CString& szErrorText) = 0;
	
};

class COMMONEXPORT CRestrictionNumberOnly : public CInputRestriction
{
public:
	virtual bool IsSymbolOK( LPCTSTR szTargetString, UINT nOffset, CString& szErrorTitle, CString& szErrorText);
	virtual bool IsSymbolOK( LPCWSTR szTargetString, UINT nOffset, CString& szErrorTitle, CString& szErrorText);
	virtual bool Finalize(LPCTSTR szTargetString, CString& szErrorTitle, CString& szErrorText);
	virtual bool Finalize(LPCWSTR szTargetString, CString& szErrorTitle, CString& szErrorText);
	
};

class COMMONEXPORT CRestrictSubstrings : public CInputRestriction
{
public:
	struct IgnoreEntry
	{
		CString m_szSeq;
		CString m_szTitle;
		CString m_szText;
	};
	
public:
	void SetIgnoreSequences(std::vector<IgnoreEntry> & ignore );
	
	// Overrides
	virtual bool IsSymbolOK(LPCTSTR szTargetString, UINT nOffset, CString& szErrorTitle, CString& szErrorText);
	virtual bool IsSymbolOK(LPCWSTR szTargetString, UINT nOffset, CString& szErrorTitle, CString& szErrorText);
	virtual bool Finalize(LPCTSTR szTargetString, CString& szErrorTitle, CString& szErrorText);
	virtual bool Finalize(LPCWSTR szTargetString, CString& szErrorTitle, CString& szErrorText);
	

protected:
	bool IsStringOK(LPCTSTR szTargetString, CString& szErrorTitle, CString& szErrorText);
	bool IsStringOK(LPCWSTR szTargetString, CString& szErrorTitle, CString& szErrorText);
	
protected:
	std::vector<IgnoreEntry> m_IgnoreSeq;
};




class COMMONEXPORT CCBalloon
{
public:
	CCBalloon();
	virtual ~CCBalloon();
	BOOL Create(HWND hWndParent);
	
	BOOL ShowBalloon(CPoint& pt, int nIconType, LPCTSTR strTitle, LPCTSTR strText, DWORD dwTimeOut = 7000 /* 0 - infinite*/, DWORD dwMaxWidth = 0);
	void HideBalloon(bool bForceHide = true );
	HWND GetSafeHwnd  (){ return this?m_hWnd:NULL; }

	
protected:
	static LRESULT CALLBACK KeyboardHookProc( int code, WPARAM wParam, LPARAM lParam );
	static LRESULT CALLBACK MouseHookProc( int code, WPARAM wParam, LPARAM lParam );
	static LRESULT CALLBACK WndProc (HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

	static HHOOK m_hKbrdMsgHook;
	static HHOOK m_hMouseMsgHook;
	static std::list < CCBalloon * > m_pBalloonMsgWnd;
	
	HWND m_hWnd;
	bool m_bShow;
	WNDPROC m_lpfnToolTipWndProc;
	
};

/////////////////////////////////////////////////////////////////////////////
// CCRestrictEdit window

class COMMONEXPORT CCRestrictEdit : public CEdit
{
public:
	explicit CCRestrictEdit(CInputRestriction* pRestriction = NULL, bool bShowXPBallon = true);
	void SetRestriction(CInputRestriction* pRestriction)
	{	
		m_pRestriction = pRestriction;
	}

	CInputRestriction* GetRestriction()
	{
		return m_pRestriction;
	}

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CCRestrictEdit)
	//}}AFX_VIRTUAL

public:
	virtual ~CCRestrictEdit();

	// Generated message map functions
protected:
	//{{AFX_MSG(CCRestrictEdit)
	afx_msg void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()

protected:
	CInputRestriction*	m_pRestriction;
	bool				m_bShowBaloon;
};

class COMMONEXPORT CCRestrictEditEx
{
public:
	explicit CCRestrictEditEx(HWND hDualWnd = NULL, CInputRestriction* pRestriction = NULL, bool bShowXPBallon = true);
	void SetRestriction(CInputRestriction* pRestriction)
	{	
		m_pRestriction = pRestriction;
	}
	
	CInputRestriction* GetRestriction()
	{
		return m_pRestriction;
	}

	virtual ~CCRestrictEditEx(){}
	
	HWND GetSafeHwnd(){return this == NULL ? NULL : m_hWnd;}
	virtual BOOL Create(CWnd* pParent, CRect& rcWnd, DWORD dwStyle = 0x54010088, DWORD dwExStyle = 0x204);

protected:
	static LRESULT CALLBACK WindowProc (HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

protected:
	CInputRestriction*	m_pRestriction;
	bool				m_bShowBaloon;
	HWND				m_hWnd;
	WNDPROC				m_fnWndProc;
	HTHEME				m_hTheme;
	CCBalloon			m_Balloon;
};




/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CRESTRICTEDIT_H__D26110A5_8FB5_4381_B709_FDD5E57AB11D__INCLUDED_)
