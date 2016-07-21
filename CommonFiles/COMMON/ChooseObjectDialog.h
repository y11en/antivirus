// ChooseObjectDialog.h: interface for the CCChooseObjectDialog class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CHOOSEOBJECTDIALOG_H__05A090A3_104C_4D61_B416_8B53EE4A0B73__INCLUDED_)
#define AFX_CHOOSEOBJECTDIALOG_H__05A090A3_104C_4D61_B416_8B53EE4A0B73__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "defs.h"
#include "WhiteBarDlg.h"
#include "CRestrictEdit.h"

void NormalizeFolderPathA ( LPSTR szFolder, int char_buffsize );
void NormalizeFolderPathW ( LPWSTR szFolder, int char_buffsize );
void NormalizeFolderPath (CString &szFolder);

class CBrowseDirectoryNT4;

class CBrowseDirectoryNT4Wnd : public CCWhiteBarDlg
{
// Construction
public:
	CBrowseDirectoryNT4Wnd (CBrowseDirectoryNT4 *pFileDlg, UINT nHelpId);

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CBrowseDirectoryNT4Wnd)
	public:
	protected:
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CBrowseDirectoryNT4Wnd();

	// Generated message map functions
protected:
	//{{AFX_MSG(CBrowseDirectoryNT4Wnd)
	afx_msg void OnEditChange();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

	CBrowseDirectoryNT4 *m_pFileDlg;
};

class CBrowseDirectoryNT4 : public CFileDialog
{
public:
	CBrowseDirectoryNT4 (UINT nHelpId, CWnd* pParentWnd = NULL, CInputRestriction *pRestriction = NULL, LPCTSTR szInitialStr = NULL);

	CString m_StrFolderName;

public:
	CString m_szNote;
	bool m_bInitialEmpty;

protected:
	void OnInitDone();
	void OnFolderChange();
	BOOL OnFileNameOK();
	void OnFileNameChange();
	void OnLBSelChangedNotify(UINT nIDBox, UINT iCurSel, UINT nCode);

	void OnEditChangeReflect ();
	void OnSizeReflect (UINT nType, int cx, int cy);
	BOOL OnSelOKReflect ();

	virtual void PreSubclassWindow();

	CString CBrowseDirectoryNT4::GetPathName() const;
	CString m_szInitialStr;
	void DoInitialUpdate ();

	CCRestrictEdit m_FolderName;
	CStatic m_Label;

	CBrowseDirectoryNT4Wnd m_ParentBrowseDirWnd;

	BOOL m_bFirstUpdate;
	CInputRestriction *m_pRestriction;

	//{{AFX_MSG(CBrowseDirectoryNT4)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

	friend class CBrowseDirectoryNT4Wnd;
};

class CCChooseObjectDialog : protected CCWhiteBarDlg
{
public:
	CCChooseObjectDialog (UINT nHelpId);
	virtual ~CCChooseObjectDialog();
	
	bool Browse(CWnd *pParentWnd, long nType = 0);
	virtual bool OnValidate(LPCSTR lpszObject);
	virtual bool OnValidate(LPCWSTR lpszObject);
		
	void GetObjectPath(CStdStringW& szOutPath);
	void SetObjectPath(CStdStringW& szPath);

public:
	CCRestrictEditEx m_Edit;
	CString m_szTitle;
	CString m_szNote;

protected:
	virtual void OnOK();
	virtual void OnCancel() { Default (); }

	static int CALLBACK CCChooseObjectDialog::BrowseFolderCallbackProc(
		HWND hwnd, 
		UINT uMsg, 
		LPARAM lParam, 
		LPARAM lpData
		);

//	void NormalizeFolderPath(LPTSTR szFolder, int char_buffsize);
		

protected:
	bool m_bInit;
	BYTE	m_szObjectPath[2*MAX_PATH];
	bool m_bValidated;	
	bool m_bInitialEmpty;
	long m_nBrowseType;
};

class CSBHelper
{
public:
	static CSBHelper& Instance();
	~CSBHelper();

	// Get the target folder for a folder PIDL. This deals with cases where a folder
	// is an alias to a real folder, folder shortcuts, the My Documents folder, etc.
	HRESULT GetTargetFolderPathFromIDListA(LPCITEMIDLIST pidlFolder, LPSTR pszPath);
	
	// Get the target folder for a folder PIDL. This deals with cases where a folder
	// is an alias to a real folder, folder shortcuts, the My Documents folder, etc.
	HRESULT GetTargetFolderPathFromIDListW(LPCITEMIDLIST pidlFolder, LPWSTR pszPath);

private:
	CSBHelper();

	HMODULE m_hShellLib;

	HRESULT SHGetUIObjectFromFullPIDL(LPCITEMIDLIST pidl, HWND hwnd, REFIID riid, void **ppv);
	HRESULT SHILClone(LPCITEMIDLIST pidl, LPITEMIDLIST *ppidl);
	HRESULT SHGetTargetFolderIDList(LPCITEMIDLIST pidlFolder, LPITEMIDLIST *ppidl);

     typedef HRESULT (WINAPI * fnSHBindToParent) (LPCITEMIDLIST pidl, REFIID riid, VOID **ppv, LPCITEMIDLIST *ppidlLast);
     fnSHBindToParent m_SHBindToParent;
};

//////////////////////////////////////////////////////////////////////////

#endif // !defined(AFX_CHOOSEOBJECTDIALOG_H__05A090A3_104C_4D61_B416_8B53EE4A0B73__INCLUDED_)
/////////////////////////////////////////////////////////////////////////////
// CBrowseDirectoryNT4Wnd window

/////////////////////////////////////////////////////////////////////////////
