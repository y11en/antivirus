// stdafx.h : include file for standard system include files,
//  or project specific include files that are used frequently, but
//      are changed infrequently
//

#if !defined(AFX_STDAFX_H__C19B9093_26B2_4699_8CD0_EBC7EDFCB833__INCLUDED_)
#define AFX_STDAFX_H__C19B9093_26B2_4699_8CD0_EBC7EDFCB833__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


// Insert your headers here
#define WIN32_LEAN_AND_MEAN		// Exclude rarely-used stuff from Windows headers

#include <windows.h>
#include <msi.h>
#include <msiquery.h>
#include <string>

typedef std::basic_string<TCHAR> tstring;

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_STDAFX_H__C19B9093_26B2_4699_8CD0_EBC7EDFCB833__INCLUDED_)
