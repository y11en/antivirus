// stdafx.h : include file for standard system include files,
//  or project specific include files that are used frequently, but
//      are changed infrequently
//

#if !defined(AFX_STDAFX_H__0E30868D_F10F_40D1_A8CD_7E8E3E6EF656__INCLUDED_)
#define AFX_STDAFX_H__0E30868D_F10F_40D1_A8CD_7E8E3E6EF656__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


// Insert your headers here
#define WIN32_LEAN_AND_MEAN		// Exclude rarely-used stuff from Windows headers

#pragma warning (disable : 4786)

#include <windows.h>
#include <assert.h>
#include <tchar.h>
#include <rpc.h>

#include <string>
#include <map>
#include <vector>
#include <list>

#include "../ckahcomm/ckahcomm.h"
#include "../ckahum/ckahmain.h"

extern CProgramExecutionLog log;
extern bool g_bIsNT;

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_STDAFX_H__0E30868D_F10F_40D1_A8CD_7E8E3E6EF656__INCLUDED_)
