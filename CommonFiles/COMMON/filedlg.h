//==============================================================================
// PRODUCT: GUIDLGS
//==============================================================================
#ifndef __GUIDLGS_FILEDLG_H
#define __GUIDLGS_FILEDLG_H

#include "defs.h"

#if _MSC_VER >= 1300

class COMMONEXPORT CCFileDialog : public CFileDialog
{
public:
	// Constructors
	explicit CCFileDialog(BOOL bOpenFileDialog, // TRUE for FileOpen, FALSE for FileSaveAs
		LPCTSTR lpszDefExt = NULL,
		LPCTSTR lpszFileName = NULL,
		DWORD dwFlags = OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
		LPCTSTR lpszFilter = NULL,
		CWnd* pParentWnd = NULL,
		DWORD dwSize = 0);

	void SetDefExts(LPCTSTR* lpDefExts, int nCount);

	virtual INT_PTR DoModal();
	virtual void OnTypeChange();

protected:
	CStringArray m_aDefExts;
};

#else
class CCFileDialogImpl;
//******************************************************************************
// 
//******************************************************************************
class COMMONEXPORT CCFileDialog : public CCommonDialog
{
	DECLARE_DYNAMIC(CCFileDialog)

public:
	OPENFILENAME m_ofn; // open file parameter block

	CCFileDialog ( 
          BOOL bOpenFileDialog, // TRUE for FileOpen, FALSE for FileSaveAs
		LPCTSTR lpszDefExt = NULL,
		LPCTSTR lpszFileName = NULL,
		DWORD dwFlags = OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
		LPCTSTR lpszFilter = NULL,
		CWnd * pParentWnd = NULL);

	virtual ~CCFileDialog();

	virtual int DoModal ();

	// Helpers for parsing file name after successful return
	// or during Overridable callbacks if OFN_EXPLORER is set
	CString GetPathName () const;  // return full path and filename
	CString GetFileName () const;  // return only filename
	CString GetFileExt () const;   // return only ext
	CString GetFileTitle () const; // return file title

     // return TRUE if readonly checked
     BOOL GetReadOnlyPref () const
     {
          return m_ofn.Flags & OFN_READONLY ? TRUE : FALSE;
     }
     
	// Enumerating multiple file selections
	POSITION GetStartPosition () const;

     CString GetNextPathName(POSITION& pos) const;

	// Helpers for custom templates
	void SetTemplate(UINT nWin3ID, UINT nWin4ID)
     {
          SetTemplate ( MAKEINTRESOURCE ( nWin3ID ), MAKEINTRESOURCE ( nWin4ID ) );
     }
     
     void SetTemplate(LPCTSTR lpWin3ID, LPCTSTR lpWin4ID);

	// Other operations available while the dialog is visible
	CString GetFolderPath() const; // return full path
	void SetControlText(int nID, LPCSTR lpsz);
	void HideControl(int nID);
	void SetDefExt(LPCSTR lpsz);
	void SetDefExts(LPCTSTR* lpDefExts, int nCount);

protected:
	void GetImplOFN();
	void SetImplOFN();
		
	CCFileDialogImpl* pImpl;
};

#endif // _MSC_VER >= 1300

#endif // __GUIDLGS_FILEDLG_H

//==============================================================================
