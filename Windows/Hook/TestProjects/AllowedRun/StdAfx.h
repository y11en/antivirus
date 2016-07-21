// stdafx.h : include file for standard system include files,
//  or project specific include files that are used frequently, but
//      are changed infrequently
//

#if !defined(AFX_STDAFX_H__D2B513AB_6356_4F1C_B809_EA37A3E56B16__INCLUDED_)
#define AFX_STDAFX_H__D2B513AB_6356_4F1C_B809_EA37A3E56B16__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define VC_EXTRALEAN		// Exclude rarely-used stuff from Windows headers

#include <afxwin.h>         // MFC core and standard components
#include <afxext.h>         // MFC extensions
#include <afxdtctl.h>		// MFC support for Internet Explorer 4 Common Controls
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>			// MFC support for Windows Common Controls
#endif // _AFX_NO_AFXCMN_SUPPORT

#include "Stuff\Layout.h"

//+ ----------------------------------------------------------------------------------------
//+ driver
#include "../../hook/avpgcom.h"
#include "../../hook/idriver.h"
#include "../../hook/fsdrvlib.h"
#include "../../hook/hookspec.h"
#include "../../hook/funcs.h"
//- ----------------------------------------------------------------------------------------

#define _AppName			_T("AllowedRun")
#define _IniFileName		_T("AllowedRun.ini")

#define _Section_Folders	_T("Folders")
#define _Section_Apps		_T("Applications")
#define _Section_Actions	_T("Actions")

#define _Ini_SaveFolters	_T("AutoSaveFilters")

#include "FiltersStorage.h"
#include "evcache.h"

extern void* _MM_Alloc(unsigned int nSize);
extern void _MM_Free(void* p);
extern int _MM_Compare(void* p1, void* p2, int len);

extern BOOL BrowseForFolder(HWND hWnd, CString& strTitle, CString& strResult);
extern void ReparsePath(WCHAR* path, DWORD pathlen);
extern BOOL ExistNameInListCtrl(CListCtrl* pList, CString* pStrObject);

extern CFiltersStorage* gpFilters;
extern void AddAppFilter(CString& szProcess);
extern void AddFolderFilter(CString& strFolder);

#define WU_ADDNEW	WM_USER + 1001
#define WU_DELETE	WM_USER + 1002
#define WU_EDIT		WM_USER + 1003


//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_STDAFX_H__D2B513AB_6356_4F1C_B809_EA37A3E56B16__INCLUDED_)
