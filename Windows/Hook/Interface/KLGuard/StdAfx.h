// stdafx.h : include file for standard system include files,
//  or project specific include files that are used frequently, but
//      are changed infrequently
//

#if !defined(AFX_STDAFX_H__A9DB83DB_A9FD_11D0_BFD1_444553540000__INCLUDED_)
#define AFX_STDAFX_H__A9DB83DB_A9FD_11D0_BFD1_444553540000__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define WIN32_LEAN_AND_MEAN		// Exclude rarely-used stuff from Windows headers

#include <windows.h>
#include <windowsx.h>
#include <commctrl.h>
#include <winioctl.h>
#include <excpt.h>
#include <shellapi.h>
#include <stdlib.h>

#include "resource.h"

#include "..\..\hook/funcs.h"
#include "..\..\hook/avpgcom.h"
#include "..\..\hook/HookSpec.h"
#include "..\..\hook/idriver.h"
#include "..\..\hook\NETID.h"
#include "..\..\hook\klpf_api.h"
#include "..\..\plugins\include\fw_ev_api.h"

#include <list>
using namespace std;
#include "..\..\hook\PFFilterID.h"

#include "EvThread.h"
#include "DgsThrd.h"

#define __BUFFER	4096

extern HINSTANCE g_hInst;

#define IDC_ICONNOT	100
#define WM_NOTIFYICON		(WM_APP+100)

// TODO: reference additional headers your program requires here
typedef int (WINAPI *_tMultiByteToWideChar)(UINT, DWORD, LPCSTR, int, LPWSTR, int);
extern _tMultiByteToWideChar _pMultiByteToWideChar;
typedef int (WINAPI *_tWideCharToMultiByte)(UINT, DWORD, LPCWSTR, int, LPSTR, int, LPCSTR, LPBOOL);
extern _tWideCharToMultiByte _pWideCharToMultiByte;

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_STDAFX_H__A9DB83DB_A9FD_11D0_BFD1_444553540000__INCLUDED_)
