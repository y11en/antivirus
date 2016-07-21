//==============================================================================
// PRODUCT: COMMON
//==============================================================================
#ifndef __COMMON_STDAFX_H
#define __COMMON_STDAFX_H

#pragma  warning( disable : 4786 )

#define VC_EXTRALEAN		// Exclude rarely-used stuff from Windows headers

#define _SCL_SECURE_NO_DEPRECATE
#define _USE_32BIT_TIME_T

// OS and components minimum versions
#if !defined(_WIN32_IE)
	#define _WIN32_IE     0x0501 //comctl32 ver 4.72
#endif //_WIN32_IE

#if !defined(_WIN32_WINNT)
	#define _WIN32_WINNT  0x0400 //NT 4.0
#endif //_WIN32_WINNT

#if !defined(WINVER)
	#if defined _WIN32_WINNT
		#define WINVER _WIN32_WINNT
	#else
		#define WINVER 0x0400
	#endif
#endif

#include <afxwin.h>         // MFC core and standard components
#include <afxext.h>         // MFC extensions

#if _MSC_VER < 1300
	#include <..\src\afximpl.h>
#endif

#include <afxdtctl.h>		// MFC support for Internet Explorer 4 Common Controls

#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>			// MFC support for Windows Common Controls
#endif // _AFX_NO_AFXCMN_SUPPORT

#ifndef _AFX_NO_OLE_SUPPORT
#include <afxole.h>         // MFC OLE classes
#include <afxodlgs.h>       // MFC OLE dialog classes
#include <afxdisp.h>        // MFC OLE automation classes
#endif // _AFX_NO_OLE_SUPPORT

#ifndef _AFX_NO_DB_SUPPORT
#include <afxdb.h>			// MFC ODBC database classes
#endif // _AFX_NO_DB_SUPPORT

#ifndef _AFX_NO_DAO_SUPPORT
#include <afxdao.h>			// MFC DAO database classes
#endif // _AFX_NO_DAO_SUPPORT

#include <afxtempl.h>

#include <afxpriv.h>

#include <afxcoll.h>

#include <shlwapi.h>

#include <tchar.h>

#include <commctrl.h>

#include <vector>
#include <list>
#include <map>
#include <string>

#include <process.h>

#include "..\..\Windows\Workstation5.0\CEP\Constants.h"
#include "..\..\Windows\Workstation5.0\CEP\Serialize.h"

#define SS_NO_CONVERSION
#define SS_NO_BSTR
#include "..\Stuff\StdString.h"

#endif // __COMMON_STDAFX_H

//==============================================================================
