//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//

#ifndef AFX_REGEDIT_H__F9157575_8FA9_4FA9_A525_7D47141BF401__INCLUDED_
#define AFX_REGEDIT_H__F9157575_8FA9_4FA9_A525_7D47141BF401__INCLUDED_

//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"       // main symbols

//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//

struct cRegistry;

//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//

class CRegEditApp : public CWinApp
{
	//--------------------------------------------//
	HMODULE		m_hPragueLoader;
	cRoot*		m_pPragueRoot;
	bool      m_bFile;

	//--------------------------------------------//
	CString MakePragueLoaderParhname(LPCTSTR i_strPath);
	BOOL	PragueLoad	(LPCTSTR i_strPathname); 
	void	PragueUnload(); 

	//--------------------------------------------//
	void	LoadParams();
	void	SaveParams();

	// data integer definitions are in the Persistent.h
	CStringArray	m_arrPersistentData;

public:
	CRegEditApp();

	CString GetPersistentData(int i_nDataCode);
	void SetPersistentData(int i_nDataCode, CString& i_strData);
	void ReportPragueError(LPCTSTR i_strMsg, tERROR i_nErr);

	cRegistry* OpenRegistry(LPCTSTR i_strRegKey, BOOL& o_bWinReg, BOOL& o_bReadOnly);

	UINT AppGetFileName(LPCTSTR lpszPathName, LPTSTR lpszTitle, UINT nMax);
	void AppAbbreviateName(LPTSTR lpszCanon, int cchMax, BOOL bAtLeastName);
	void CheckInputParameter(CString& io_strKey);	
	bool IsFile() const { return m_bFile; }

	//{{AFX_VIRTUAL(CRegEditApp)
	public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();
	virtual CDocument* OpenDocumentFile(LPCTSTR lpszFileName);
	//}}AFX_VIRTUAL

	//{{AFX_MSG(CRegEditApp)
	afx_msg void OnAppAbout();
	//}}AFX_MSG

	afx_msg void OnFileNew();
	afx_msg void OnFileOpen();
	afx_msg void OnNewKey();

	DECLARE_MESSAGE_MAP()
};

//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//

CRegEditApp* GlobalGetApp();

//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//

//{{AFX_INSERT_LOCATION}}

#endif // 
