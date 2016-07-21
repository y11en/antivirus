// stdafx.h : include file for standard system include files,
//  or project specific include files that are used frequently, but
//      are changed infrequently
//

#if !defined(AFX_STDAFX_H__F3D6386B_03E1_4597_8338_364DDC9A1842__INCLUDED_)
#define AFX_STDAFX_H__F3D6386B_03E1_4597_8338_364DDC9A1842__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define VC_EXTRALEAN		// Exclude rarely-used stuff from Windows headers

#include <afxwin.h>         // MFC core and standard components
#include <afxext.h>         // MFC extensions

#ifndef _AFX_NO_OLE_SUPPORT
#include <afxole.h>         // MFC OLE classes
#include <afxodlgs.h>       // MFC OLE dialog classes
#include <afxdisp.h>        // MFC Automation classes
#endif // _AFX_NO_OLE_SUPPORT


#ifndef _AFX_NO_DB_SUPPORT
#include <afxdb.h>			// MFC ODBC database classes
#endif // _AFX_NO_DB_SUPPORT

#ifndef _AFX_NO_DAO_SUPPORT
#include <afxdao.h>			// MFC DAO database classes
#endif // _AFX_NO_DAO_SUPPORT

#include <afxdtctl.h>		// MFC support for Internet Explorer 4 Common Controls
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>			// MFC support for Windows Common Controls
#endif // _AFX_NO_AFXCMN_SUPPORT


// -----------------------------------------------------------------------------------------
#include <winioctl.h>

#include "..\..\hook\avpgcom.h"
#include "..\..\hook\IDriver.h"
#include "..\..\hook\funcs.h"
#include "..\..\hook\HookSpec.h"
#include "..\..\hook\NETID.h"

// -----------------------------------------------------------------------------------------
#include "Stuff\Layout.h"
#include <Stuff\RegStorage.h>
extern CRegStorage* g_pRegStorage;

// -----------------------------------------------------------------------------------------

extern APP_REGISTRATION AppReg;
extern HANDLE g_hDrvDevice;
extern OSVERSIONINFO g_OSVer;

#include "EvCache.h"

extern HANDLE g_hHeap;
extern void* _MM_Alloc(unsigned int nSize);
extern void _MM_Free(void* p);

#define SIZEOF(a) sizeof(a)/sizeof(CHAR*)
#include "FiltersStorage.h"

#define FILTER_BUFFER_SIZE 4096*2

#include "DFilters.h"
#include "OwnTabCtrl.h"

#include <Sign\Sign.h>
#include <KLKeyRec.h>
#include <AVPComID.h>

#include <AVPControls\CheckList.h>
#include "Params.h"

extern DWORD g_FuncLevel;

typedef BOOL (WINAPI *_tlstrlenW)(PWCHAR);
typedef BOOL (WINAPI *_tlstrcpyW)(PWCHAR, PWCHAR);
typedef int (WINAPI *_tlstrcmpW)(PWCHAR, PWCHAR);

extern _tlstrlenW _plstrlenW;
extern _tlstrcpyW _plstrcpyW;
extern _tlstrcmpW _plstrcmpW;


//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_STDAFX_H__F3D6386B_03E1_4597_8338_364DDC9A1842__INCLUDED_)
