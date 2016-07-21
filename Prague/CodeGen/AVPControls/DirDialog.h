#if !defined(DIRDIALOG_INCLUDED_)
#define DIRDIALOG_INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// DirDialog.h - header for CDirDialog class

class CDirDialog;

// CDlgWnd - intercepts messages from child controls
class CDlgWnd : public CWnd
{
		CWnd			 *m_pLocaleWnd;
		CDirDialog *m_pHost;
    CString			m_strPath;                  // Current directory
public:
		CDlgWnd(CWnd *pLocWnd, CDirDialog *pHost) : m_pLocaleWnd(pLocWnd), m_pHost(pHost) {}
    void CheckDir(const CString &ss);   // Display directory contents

protected:

	//{{AFX_MSG(CDlgWnd)
	afx_msg void OnSize(UINT nType, int cx, int cy);
  afx_msg void OnOpen();              // Open button clicked
	afx_msg LRESULT OnSetFolderPath( WPARAM wParam, LPARAM lParam );
		//}}AFX_MSG
    DECLARE_MESSAGE_MAP()
};

// CDirEdit - edit control class
class CDirEdit : public CEdit
{
		CWnd *m_pLocaleWnd;
		CDirDialog *m_pHost;
    CString m_strPath;                  // Current directory
public:
		CDirEdit(CWnd *pLocWnd, CDirDialog *pHost) : m_pLocaleWnd(pLocWnd), m_pHost(pHost) {}
protected:
    afx_msg void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);
    afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
    afx_msg UINT OnGetDlgCode();
		afx_msg LRESULT OnSetFolderPath( WPARAM wParam, LPARAM lParam );
		afx_msg LRESULT OnPaste( WPARAM wParam, LPARAM lParam );
			
    DECLARE_MESSAGE_MAP()
};

// CDirDialog - directory selection dialog
class CDirDialog : public CFileDialog
{
	DECLARE_DYNAMIC(CDirDialog)

public:
    CDirDialog(LPCTSTR initial = NULL,
               LPCTSTR filter = NULL, CWnd* pParentWnd = NULL);

    CString GetPath() { return m_strPath; }

    // Overriden members of CFileDialog
    virtual void OnInitDone();
    virtual void OnFolderChange();

    // Disallow selection of files (since we're only selecting directories)
    virtual BOOL OnFileNameOK() { return TRUE; }

		virtual BOOL CheckFolderName( LPCTSTR pFolderName ) { return TRUE; }

private:
    CString m_strPath;                  // Current directory
    CString m_strFilter;                // The current file filters string (used for string storage
                                        // for internal use of File Open dialog)
		CString m_strTitle;									// The dialog title (used for string storage
                                        // for internal use of File Open dialog)
    CDlgWnd m_DlgWnd;                   // Subclassed dialog window (parent of CDirDialog window)
    CDirEdit m_Edit;                    // Edit control where you can type in a dir. name
    CButton m_Open;                     // "Open" button (replaces OK button)

protected :
	//{{AFX_MSG(CDirDialog)
	//}}AFX_MSG
    DECLARE_MESSAGE_MAP()
};

#endif // DIRDIALOG_INCLUDED_
