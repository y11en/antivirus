// stdafx.h : include file for standard system include files,
//      or project specific include files that are used frequently,
//      but are changed infrequently

#if !defined(AFX_STDAFX_H__BC8D35C2_2D0A_492B_8161_5D2B838C7598__INCLUDED_)
#define AFX_STDAFX_H__BC8D35C2_2D0A_492B_8161_5D2B838C7598__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define ISOLATION_AWARE_ENABLED 1

#ifndef _WIN32_WINNT
#define _WIN32_WINNT  0x0501
#endif
#define WINVER        0x0501
#define _WIN32_IE     0x0501

#define _CONVERSION_USES_THREAD_LOCALE
#define _USES_CONVERSION(__nCodePage) int _convert; _convert; \
	UINT _acp = __nCodePage; _acp; LPCWSTR _lpw; _lpw; LPCSTR _lpa; _lpa

#include <atlbase.h>

extern CComModule _Module;

#include <atlcom.h>
#include <atlwin.H>
#include <atlhost.h>
#include <windowsx.h>
#include <commctrl.h>
#include <uxtheme.h>

extern HINSTANCE    g_hInstance;

#include "resource.h"
#include <ProductCore/basegui.h>
#include <ProductCore/itemprops.h>
#include <ProductCore/binding.h>
#include <ProductCore/gridview.h>
#include "winutils.h"
#include "winroot.h"
#include "winctrls.h"
#include "vlistctrl.h"

extern SIZE    GetCheckImageSize(bool bIsThemed, bool bRadio);

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_STDAFX_H__BC8D35C2_2D0A_492B_8161_5D2B838C7598__INCLUDED)
