//==============================================================================
// PRODUCT: GUIDLGS
//==============================================================================
#ifndef __COMMON_FILEDLGIMPL_H
#define __COMMON_FILEDLGIMPL_H

#if _MSC_VER < 1300
//******************************************************************************
// 
//******************************************************************************
class CCFileDialogImpl : public CCommonDialog
{
	DECLARE_DYNAMIC(CCFileDialogImpl)

public:
	OPENFILENAME m_ofn; // open file parameter block

	static CCFileDialogImpl* CreateImpl(BOOL bOpenFileDialog,
		LPCTSTR lpszDefExt, LPCTSTR lpszFileName, DWORD dwFlags,
		LPCTSTR lpszFilter, CWnd* pParentWnd);

	static void DeleteImpl  (CCFileDialogImpl* pImpl);
		
		
	virtual int DoModal ();

	// Helpers for parsing file name after successful return
	// or during Overridable callbacks if OFN_EXPLORER is set
	CString GetPathName () const;  // return full path and filename
	CString GetFileName () const;  // return only filename
	CString GetFileExt () const;   // return only ext
	CString GetFileTitle () const; // return file title

     // return TRUE if readonly checked
     BOOL GetReadOnlyPref () const;
     
	// Enumerating multiple file selections
	POSITION GetStartPosition () const;

     CString GetNextPathName(POSITION& pos) const;
	 

	// Helpers for custom templates
	void SetTemplate(UINT nWin3ID, UINT nWin4ID);
     
     void SetTemplate(LPCTSTR lpWin3ID, LPCTSTR lpWin4ID);

	// Other operations available while the dialog is visible
	CString GetFolderPath() const; // return full path
	void SetControlText(int nID, LPCSTR lpsz);
	void HideControl(int nID);
	void SetDefExt(LPCSTR lpsz);
	void SetDefExts(LPCTSTR* lpDefExts, int nCount);

// Overridable callbacks
protected:
	friend UINT CALLBACK _AfxMFCCommDlgProc(HWND, UINT, WPARAM, LPARAM);
	virtual UINT OnShareViolation(LPCTSTR lpszPathName);
	virtual BOOL OnFileNameOK();
	virtual void OnLBSelChangedNotify(UINT nIDBox, UINT iCurSel, UINT nCode);

	// only called back if OFN_EXPLORER is set
	virtual void OnInitDone();
	virtual void OnFileNameChange();
	virtual void OnFolderChange();
	virtual void OnTypeChange();

	CString GetNextFileName(POSITION& pos) const;
	CString GetNextPathNameSingleSelect(POSITION& pos) const;
	CString GetNextPathNameMultiSelect  (POSITION& pos) const;
	
	static LRESULT CALLBACK fnWndProcFilter (HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

protected:
	BOOL m_bOpenFileDialog;       // TRUE for file open, FALSE for file save
	CString m_strFilter;          // filter string
						// separate fields with '|', terminate with '||\0'
	TCHAR m_szFileTitle[64];       // contains file title after return
	TCHAR m_szFileName[_MAX_PATH]; // contains full path name after return

	OPENFILENAME*  m_pofnTemp;

	virtual BOOL OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult);

protected:
	CCFileDialogImpl ( 
		BOOL bOpenFileDialog, // TRUE for FileOpen, FALSE for FileSaveAs
		LPCTSTR lpszDefExt,
		LPCTSTR lpszFileName,
		DWORD dwFlags,
		LPCTSTR lpszFilter,
		CWnd * pParentWnd);
	
	CString m_szFiles;
	CString m_szFolderPath;

	WNDPROC m_fnProcOrig;

	CStringArray m_aDefExts;

	friend class CCFileDialog;
};

#endif // _MSC_VER >= 1300

#endif // __COMMON_FILEDLGIMPL_H