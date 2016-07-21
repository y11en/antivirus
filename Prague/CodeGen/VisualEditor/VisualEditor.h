// VisualEditor.h : main header file for the VISUALEDITOR application
//

#if !defined(AFX_VISUALEDITOR_H__9D0A11D5_C8F2_11D3_96B1_00D0B71DDDF6__INCLUDED_)
#define AFX_VISUALEDITOR_H__9D0A11D5_C8F2_11D3_96B1_00D0B71DDDF6__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include <property/property.h>
#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// CVisualEditorApp:
// See VisualEditor.cpp for the implementation of this class
//

class CVisualEditorApp : public CWinApp {
public:
	CVisualEditorApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CVisualEditorApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

	void ProcessCommandLine();
// Implementation

	//{{AFX_MSG(CVisualEditorApp)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////
int		DlgMessageBox( CWnd *pWnd, DWORD dwTextID, UINT nType = MB_OK );
int		DlgMessageBox( CWnd *pWnd, LPCTSTR lpszText, UINT nType = MB_OK );
HDATA DeserializeDTree( void *pBuffer, DWORD nSize );
void *SerializeDTree( HDATA hData, DWORD &nSSize );
TCHAR *GetErrorString( HRESULT hError );
TCHAR *GetStartupDirectory();


//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_VISUALEDITOR_H__9D0A11D5_C8F2_11D3_96B1_00D0B71DDDF6__INCLUDED_)
