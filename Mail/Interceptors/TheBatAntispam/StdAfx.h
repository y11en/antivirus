// stdafx.h : include file for standard system include files,
//  or project specific include files that are used frequently, but
//      are changed infrequently
//

#if !defined(AFX_STDAFX_H__A0C1A140_0008_4D3C_A21A_291AA4DDF16A__INCLUDED_)
#define AFX_STDAFX_H__A0C1A140_0008_4D3C_A21A_291AA4DDF16A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


// Insert your headers here
#define WIN32_LEAN_AND_MEAN		// Exclude rarely-used stuff from Windows headers

#include <windows.h>
#include <tchar.h>
#include <string>
typedef std::basic_string<TCHAR> tstring;

#define MAKESTR(x)		#x
#define MAKESTR2(x)		MAKESTR(x)
#define TODO(msg) message(__FILE__"(" MAKESTR2(__LINE__) "): TODO: " msg)

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_STDAFX_H__A0C1A140_0008_4D3C_A21A_291AA4DDF16A__INCLUDED_)
