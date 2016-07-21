// stdafx.h : include file for standard system include files,
//  or project specific include files that are used frequently, but
//      are changed infrequently
//

#if !defined(AFX_STDAFX_H__CAA6BB28_C2D6_41EB_AA20_5BAB0850C90D__INCLUDED_)
#define AFX_STDAFX_H__CAA6BB28_C2D6_41EB_AA20_5BAB0850C90D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#pragma once

// Modify the following defines if you have to target a platform prior to the ones specified below.
// Refer to MSDN for the latest info on corresponding values for different platforms.
#ifndef WINVER				// Allow use of features specific to Windows 98 or later.
#define WINVER 0x0410		// Change this to the appropriate value to target other versions of Windows.
#endif

#ifndef _WIN32_WINNT		// Allow use of features specific to Windows 98 or later.                   
#define _WIN32_WINNT 0x0410	// Change this to the appropriate value to target other versions of Windows.
#endif						

#ifndef _WIN32_WINDOWS		// Allow use of features specific to Windows 98 or later.
#define _WIN32_WINDOWS 0x0410 // Change this to the appropriate value to target Windows Me or later.
#endif

#ifndef _WIN32_IE			// Allow use of features specific to IE 3.0 or later.
#define _WIN32_IE 0x0300	// Change this to the appropriate value to target other versions of IE.
#endif

#define VC_EXTRALEAN		// Exclude rarely-used stuff from Windows headers

#include <windows.h>
#include <wininet.h>
#include <crtdbg.h>
#include <tchar.h>

#endif // !defined(AFX_STDAFX_H__CAA6BB28_C2D6_41EB_AA20_5BAB0850C90D__INCLUDED_)
