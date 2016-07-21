// stdafx.h : include file for standard system include files,
//  or project specific include files that are used frequently, but
//      are changed infrequently
//

#if !defined(AFX_STDAFX_H__291B24BB_EB9A_419C_9176_37BF4A1DB284__INCLUDED_)
#define AFX_STDAFX_H__291B24BB_EB9A_419C_9176_37BF4A1DB284__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


// Insert your headers here
#define WIN32_LEAN_AND_MEAN		// Exclude rarely-used stuff from Windows headers
#define _SCL_SECURE_NO_DEPRECATE
#define _USE_32BIT_TIME_T

#include <windows.h>

#include "../../../../CommonFiles/KLUtil/Log.h"
#include "../../../../Windows/Personal 4.6/release_type.h"

extern KLUTIL::CLog log;

/*
#if defined(_DEBUG) && defined(__PERSONAL_PRO)
	#define __FIREWALL
#endif
*/
//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_STDAFX_H__291B24BB_EB9A_419C_9176_37BF4A1DB284__INCLUDED_)
