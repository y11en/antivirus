// stdafx.h : include file for standard system include files,
//  or project specific include files that are used frequently, but
//      are changed infrequently
//

#if !defined(AFX_STDAFX_H__656E6110_9778_4C4C_92D9_BAA8157A5E40__INCLUDED_)
#define AFX_STDAFX_H__656E6110_9778_4C4C_92D9_BAA8157A5E40__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define WIN32_LEAN_AND_MEAN		// Exclude rarely-used stuff from Windows headers

#if !defined(_T)
	#if defined(UNICODE)
		#define _T(x) L##x
	#else
		#define _T(x) x
	#endif
#endif

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_STDAFX_H__656E6110_9778_4C4C_92D9_BAA8157A5E40__INCLUDED_)
