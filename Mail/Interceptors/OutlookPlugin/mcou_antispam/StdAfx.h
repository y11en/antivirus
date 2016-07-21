// stdafx.h : include file for standard system include files,
//      or project specific include files that are used frequently,
//      but are changed infrequently

#if !defined(AFX_STDAFX_H__6A4E951E_A471_4D04_95D2_73B98E912FF3__INCLUDED_)
#define AFX_STDAFX_H__6A4E951E_A471_4D04_95D2_73B98E912FF3__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define STRICT
#ifndef _WIN32_WINNT
#define _WIN32_WINNT 0x0400
#endif

#define _ATL_APARTMENT_THREADED
#define _ATL_STATIC_REGISTRY

#if _MSC_VER >= 1400
	#include <atlbase.h>
#else
	#include "atlbase_StatregPatch.h"
#endif
//You may derive a class from CComModule and use it if you want to override
//something, but do not change the name of _Module
extern ATL::CComModule _Module;
#include <atlcom.h>
#include <atlhost.h>
#include <atlctl.h>
using namespace ATL;

#include "AntispamOUSupportClass.h"
using namespace AntispamOUSupport;

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#pragma warning(disable: 4146) 
#import "dll\mso9.dll" rename_namespace("Office"), named_guids, no_implementation rename("RGB", "msoRGB") rename("DocumentProperties", "msoDocumentProperties")
using namespace Office;

#import "dll\MSOUTL9.OLB" rename_namespace("Outlook"), named_guids, raw_interfaces_only, no_implementation rename("RGB", "msoRGB")
using namespace Outlook;


#endif // !defined(AFX_STDAFX_H__6A4E951E_A471_4D04_95D2_73B98E912FF3__INCLUDED)
