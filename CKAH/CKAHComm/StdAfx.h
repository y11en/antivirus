// stdafx.h : include file for standard system include files,
//  or project specific include files that are used frequently, but
//      are changed infrequently
//

#if !defined(AFX_STDAFX_H__75B1F5D7_3B1F_4AF9_8501_22A46E8B871D__INCLUDED_)
#define AFX_STDAFX_H__75B1F5D7_3B1F_4AF9_8501_22A46E8B871D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


// Insert your headers here
#define WIN32_LEAN_AND_MEAN		// Exclude rarely-used stuff from Windows headers
#define _SCL_SECURE_NO_DEPRECATE

#pragma warning (disable : 4786)

#include <windows.h>
#include <assert.h>
#include <tchar.h>
#include <rpc.h>

#include <string>
#include <vector>
#include <list>
#include <map>

#include "ProgramExecutionLog.h"
extern CProgramExecutionLog log;

#include <stuffio/ioutil.h>

#include "../ckahum/stringdefs.h"

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_STDAFX_H__75B1F5D7_3B1F_4AF9_8501_22A46E8B871D__INCLUDED_)
