// stdafx.h : include file for standard system include files,
//  or project specific include files that are used frequently, but
//      are changed infrequently
//

#if !defined(AFX_STDAFX_H__28A18CBF_020A_43B5_B64D_E128D1327CDD__INCLUDED_)
#define AFX_STDAFX_H__28A18CBF_020A_43B5_B64D_E128D1327CDD__INCLUDED_

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

#include "debug.h"
#include "Layout.h"

//+ ------------------------------------------------------------------------------------------

#define _TRAY_ICON_ID	100
#define _tray_blink_count 3
BOOL TrayMessage(HWND hDlg, UINT uFlags, DWORD dwMessage, UINT uID, HICON hIcon, TCHAR *pszTip);
void ModifyTrayIcon(HWND hWnd, DWORD idi);

// aimed to change icon in tray and so on
void GlobalAlert();
//+ ------------------------------------------------------------------------------------------

typedef int (WINAPI* _tBrowseCallbackProc)(HWND hwnd, UINT uMsg, LPARAM lParam,LPARAM lpData);
BOOL BrowseForFolder(HWND hWnd, CString& strTitle, CString& strResult, BOOL bIncludeFiles, _tBrowseCallbackProc pCallback);

//+ ------------------------------------------------------------------------------------------

#include "common/hyperlink.h"
#define WM_NOTIFYICON				WM_APP + 100
#define WU_SAND_BOX					WM_USER + 101
#define WU_KNOWNPROC				WM_USER + 102
#define WU_ABOUT					WM_USER + 103
#define WU_REQUESTUPDATE			WM_USER + 104
#define WU_GENERIC_SETTINGS			WM_USER + 105

#define _TIMER_MAINWND			1001
#define _TIMER_SAND_BOX			1002
#define _TIMER_MODIFIED_WND		1003
#define _TIMER_NEWPROCESS_WND	1004
#define _TIMER_TRAYBLINK		1005
#define _TIMER_TIMERSTEP		1006
#define _TIMER_CMDREMOTE		1007

// in sec
#define _TIMEOUT_SHOWMODIFIED_WND		30
#define _TIMEOUT_SHOWNEWPROCESS_WND		60

extern UINT nHyperlinkPressed;
//+ ------------------------------------------------------------------------------------------


#include "Jobs.h"

#define _APP_NAME _T("KLGuard")

extern CProcessList* gpProcList;

//+ ------------------------------------------------------------------------------------------
#define _AVP_GUARD_V2	"KL Guard v2"
#include "Reestr.h"

extern CReestr *g_pReestr;

extern void CurrentTimeToString(CString* pstrtime);


//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_STDAFX_H__28A18CBF_020A_43B5_B64D_E128D1327CDD__INCLUDED_)
