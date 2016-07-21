// stdafx.h : include file for standard system include files,
//      or project specific include files that are used frequently,
//      but are changed infrequently

#if !defined(AFX_STDAFX_H__B35254A6_163E_4812_9747_56DB83E8A82D__INCLUDED_)
#define AFX_STDAFX_H__B35254A6_163E_4812_9747_56DB83E8A82D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define STRICT
#ifndef _WIN32_WINNT
#define _WIN32_WINNT 0x0400
#endif
#define _ATL_APARTMENT_THREADED

#include <atlbase.h>
#include <tchar.h>
#include <string>
typedef std::basic_string<TCHAR> tstring;

#include "ver_mod.h"

//You may derive a class from CComModule and use it if you want to override
//something, but do not change the name of _Module
#include "MyComModule.h"
extern CMyComModule _Module;

#include <atlwin.h>
#include <atlcom.h>

#define MAKESTR(x)		#x
#define MAKESTR2(x)		MAKESTR(x)
#define TODO(msg) message(__FILE__"(" MAKESTR2(__LINE__) "): TODO: " msg)

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_STDAFX_H__B35254A6_163E_4812_9747_56DB83E8A82D__INCLUDED)
